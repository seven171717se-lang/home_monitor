/**
 * @file    esp8266_at.h
 * @brief   ESP8266 WiFi 模组 AT 指令驱动
 * @note    依赖 CubeMX 初始化 USART1（115200 8N1 + 全局中断），驱动负责收发与应答检测
 */

#ifndef __ESP8266_AT_H
#define __ESP8266_AT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* 可配置项：根据你的 CubeMX 串口修改
 * 默认使用 USART1：TX=PA9, RX=PA10
 */
#define ESP8266_UART           huart1
#define ESP8266_RX_BUF_SIZE    512

/**
 * @brief  应答状态
 */
typedef enum {
    ESP8266_RESP_ERROR   = 0,  /**< 收到 ERROR */
    ESP8266_RESP_OK      = 1,  /**< 收到 OK */
    ESP8266_RESP_TIMEOUT = 2   /**< 超时无应答 */
} ESP8266_RespStatus;

/**
 * @brief  初始化 ESP8266 串口接收
 * @note   需在 MX_USART1_UART_Init() 之后调用；
 *         启动单字节中断接收，收到数据存入环形缓冲区
 */
void ESP8266_Init(void);

/**
 * @brief  发送一条 AT 指令（自动追加 \r\n）
 * @param  cmd: 指令字符串，如 "AT"、"AT+GMR"、"AT+CWMODE=1"
 */
void ESP8266_SendCmd(const char *cmd);

/**
 * @brief  等待应答
 * @param  timeout_ms: 超时时间（毫秒）
 * @return ESP8266_RESP_OK / ESP8266_RESP_ERROR / ESP8266_RESP_TIMEOUT
 */
ESP8266_RespStatus ESP8266_WaitResp(uint32_t timeout_ms);

/**
 * @brief  清空接收缓冲区
 */
void ESP8266_ClearBuf(void);

/**
 * @brief  取出接收到的数据（取走并清空）
 * @param  buf:     存放数据的缓冲区
 * @param  max_len: 缓冲区大小
 * @return 实际拷贝的字节数
 */
uint16_t ESP8266_GetRxData(char *buf, uint16_t max_len);

/**
 * @brief  测试模块是否在线（发送 "AT" 等待 "OK"）
 * @return 1 = 在线, 0 = 无响应
 */
uint8_t ESP8266_ATReady(void);

#ifdef __cplusplus
}
#endif

#endif /* __ESP8266_AT_H */
