/**
 * @file    encoder.c
 * @brief   旋转编码器驱动 — 依赖 CubeMX 初始化
 * @note    TIM3 和 GPIO 由 CubeMX 生成的 MX_TIM3_Init()/MX_GPIO_Init() 配置
 *          Encoder_Init() 只负责启动编码器
 */

#include "encoder.h"
#include "stm32f1xx_hal.h"

/* 引用 CubeMX 生成的全局句柄（在 main.c 中定义）*/
extern TIM_HandleTypeDef htim3;

/* 私有变量 ----------------------------------------------------------------*/
static volatile uint8_t btn_pressed = 0;
static int32_t last_count = 0;

/* 公有函数 ----------------------------------------------------------------*/

void Encoder_Init(void)
{
    /* 启动编码器模式计数 */
    HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);

    /* 确保 EXTI0 中断已使能（MX_GPIO_Init 已配好 PB0）*/
    HAL_NVIC_SetPriority(EXTI0_IRQn, 15, 0);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

int32_t Encoder_GetCount(void)
{
    return (int32_t)htim3.Instance->CNT;
}

EncoderDirection Encoder_GetDirection(void)
{
    int32_t current = Encoder_GetCount();
    EncoderDirection dir = ENCODER_DIR_NONE;

    if (current > last_count)
        dir = ENCODER_DIR_CW;
    else if (current < last_count)
        dir = ENCODER_DIR_CCW;

    last_count = current;
    return dir;
}

void Encoder_Reset(void)
{
    __HAL_TIM_SET_COUNTER(&htim3, 0);
    last_count = 0;
}

void Encoder_ButtonPressed(void)
{
    btn_pressed = 1;
}

uint8_t Encoder_GetButtonPressed(void)
{
    uint8_t ret = btn_pressed;
    btn_pressed = 0;
    return ret;
}
