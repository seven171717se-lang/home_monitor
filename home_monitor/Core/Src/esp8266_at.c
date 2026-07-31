/**
 * @file    esp8266_at.c
 * @brief   ESP8266 AT 指令驱动实现 — 依赖 CubeMX 初始化
 * @note    USART1 由 CubeMX 生成的 MX_USART1_UART_Init() 配置（115200 8N1）
 *          USART1 全局中断已使能，收到数据存入环形缓冲区
 */

#include "esp8266_at.h"
#include "stm32f1xx_hal.h"
#include <string.h>

/* 引用 CubeMX 生成的 UART 句柄（在 main.c 中定义）*/
extern UART_HandleTypeDef ESP8266_UART;

/* 环形缓冲区 ----------------------------------------------------------------*/
static volatile uint8_t rx_buf[ESP8266_RX_BUF_SIZE];
static volatile uint16_t rx_head = 0;   /* 写指针 */
static volatile uint16_t rx_tail = 0;   /* 读指针 */
static uint8_t rx_byte = 0;

/* 私有函数 ----------------------------------------------------------------*/

static void rx_put(uint8_t b)
{
    uint16_t next = (uint16_t)((rx_head + 1) % ESP8266_RX_BUF_SIZE);

    if (next != rx_tail)          /* 缓冲满则丢弃 */
    {
        rx_buf[rx_head] = b;
        rx_head = next;
    }
}

/* 公有函数 ----------------------------------------------------------------*/

void ESP8266_Init(void)
{
    ESP8266_ClearBuf();

    /* 确保 USART1 中断已使能（CubeMX 通常已配置，这里兜底）*/
    HAL_NVIC_SetPriority(USART1_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);

    /* 启动单字节中断接收 */
    HAL_UART_Receive_IT(&ESP8266_UART, &rx_byte, 1);
}

void ESP8266_SendCmd(const char *cmd)
{
    uint16_t len = (uint16_t)strlen(cmd);

    HAL_UART_Transmit(&ESP8266_UART, (uint8_t *)cmd, len, 200);
    HAL_UART_Transmit(&ESP8266_UART, (uint8_t *)"\r\n", 2, 200);
}

ESP8266_RespStatus ESP8266_WaitResp(uint32_t timeout_ms)
{
    uint32_t start = HAL_GetTick();
    uint16_t idx = rx_tail;
    uint8_t ok_match = 0;
    uint8_t err_match = 0;

    while ((HAL_GetTick() - start) < timeout_ms)
    {
        while (idx != rx_head)
        {
            uint8_t ch = rx_buf[idx];
            idx = (uint16_t)((idx + 1) % ESP8266_RX_BUF_SIZE);

            /* 匹配 "OK" */
            if (ch == 'O')
                ok_match = 1;
            else if (ok_match == 1 && ch == 'K')
                return ESP8266_RESP_OK;
            else
                ok_match = 0;

            /* 匹配 "ERROR" */
            if (ch == 'E')
                err_match = 1;
            else if (err_match == 1 && ch == 'R')
                err_match = 2;
            else if (err_match == 2 && ch == 'R')
                err_match = 3;
            else if (err_match == 3 && ch == 'O')
                err_match = 4;
            else if (err_match == 4 && ch == 'R')
                return ESP8266_RESP_ERROR;
            else
                err_match = 0;
        }
    }

    return ESP8266_RESP_TIMEOUT;
}

void ESP8266_ClearBuf(void)
{
    rx_head = 0;
    rx_tail = 0;
}

uint16_t ESP8266_GetRxData(char *buf, uint16_t max_len)
{
    uint16_t n = 0;

    while (n < max_len && rx_tail != rx_head)
    {
        buf[n++] = (char)rx_buf[rx_tail];
        rx_tail = (uint16_t)((rx_tail + 1) % ESP8266_RX_BUF_SIZE);
    }
    return n;
}

uint8_t ESP8266_ATReady(void)
{
    ESP8266_ClearBuf();
    ESP8266_SendCmd("AT");
    return (ESP8266_WaitResp(1000) == ESP8266_RESP_OK) ? 1 : 0;
}

/* 串口接收完成回调（单字节中断接收完成）*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance != ESP8266_UART.Instance)
        return;

    rx_put(rx_byte);
    HAL_UART_Receive_IT(huart, &rx_byte, 1);
}
