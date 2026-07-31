/**
 * @file    motor_l9110s.c
 * @brief   L9110S 直流电机驱动实现 — 依赖 CubeMX 初始化
 * @note    TIM3 两路 PWM 由 CubeMX 生成的 MX_TIM3_Init() 配置
 *          正转：IA 输出 PWM、IB 为低；反转：IA 为低、IB 输出 PWM
 *          刹车：两路 100%；自由停止：两路 0%
 */

#include "motor_l9110s.h"
#include "stm32f1xx_hal.h"

/* 引用 CubeMX 生成的定时器句柄（在 main.c 中定义）*/
extern TIM_HandleTypeDef MOTOR_TIM;

/* 私有函数 ----------------------------------------------------------------*/

/**
 * @brief  设置单路 PWM 占空比
 * @param  channel: 定时器通道
 * @param  duty:    0 ~ 100
 */
static void motor_set_duty(uint32_t channel, uint16_t duty)
{
    uint32_t arr;
    uint32_t cmp;

    if (duty > 100U)
        duty = 100U;

    arr = __HAL_TIM_GET_AUTORELOAD(&MOTOR_TIM);
    cmp = (uint32_t)duty * (arr + 1U) / 100U;

    __HAL_TIM_SET_COMPARE(&MOTOR_TIM, channel, cmp);
}

/* 公有函数 ----------------------------------------------------------------*/

void Motor_Init(void)
{
    HAL_TIM_PWM_Start(&MOTOR_TIM, MOTOR_CH_FORWARD);
    HAL_TIM_PWM_Start(&MOTOR_TIM, MOTOR_CH_REVERSE);
    Motor_Coast();
}

void Motor_Forward(uint16_t duty)
{
    motor_set_duty(MOTOR_CH_FORWARD, duty);
    motor_set_duty(MOTOR_CH_REVERSE, 0U);
}

void Motor_Reverse(uint16_t duty)
{
    motor_set_duty(MOTOR_CH_FORWARD, 0U);
    motor_set_duty(MOTOR_CH_REVERSE, duty);
}

void Motor_Brake(void)
{
    motor_set_duty(MOTOR_CH_FORWARD, 100U);
    motor_set_duty(MOTOR_CH_REVERSE, 100U);
}

void Motor_Coast(void)
{
    motor_set_duty(MOTOR_CH_FORWARD, 0U);
    motor_set_duty(MOTOR_CH_REVERSE, 0U);
}

void Motor_SetSpeed(int8_t speed)
{
    uint16_t duty;

    if (speed > 100)
        speed = 100;
    else if (speed < -100)
        speed = -100;

    if (speed == 0) {
        Motor_Coast();
        return;
    }

    duty = (uint16_t)((speed > 0) ? speed : -speed);

    if (speed > 0)
        Motor_Forward(duty);
    else
        Motor_Reverse(duty);
}
