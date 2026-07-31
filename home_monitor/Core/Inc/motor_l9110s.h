/**
 * @file    motor_l9110s.h
 * @brief   L9110S 直流电机驱动（双路 PWM 调速）
 * @note    依赖 CubeMX 初始化定时器两路 PWM 输出，驱动只控制占空比
 */

#ifndef __MOTOR_L9110S_H
#define __MOTOR_L9110S_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* 可配置项：根据你的 CubeMX 定时器/引脚修改
 * 默认使用 TIM3：CH1(PA6) -> IA 正转，CH2(PA7) -> IB 反转
 */
#define MOTOR_TIM           htim4
#define MOTOR_CH_FORWARD    TIM_CHANNEL_3   /* IA -> PB8 */
#define MOTOR_CH_REVERSE    TIM_CHANNEL_4   /* IB -> PB9 */

/**
 * @brief  初始化电机驱动
 * @note   启动两路 PWM 并停止电机；
 *         定时器与通道须先在 CubeMX 中配置为 PWM 输出
 */
void Motor_Init(void);

/**
 * @brief  设置电机转速
 * @param  speed: -100 ~ +100，负值为反转，0 为自由停止
 */
void Motor_SetSpeed(int8_t speed);

/**
 * @brief  正转
 * @param  duty: 占空比 0 ~ 100
 */
void Motor_Forward(uint16_t duty);

/**
 * @brief  反转
 * @param  duty: 占空比 0 ~ 100
 */
void Motor_Reverse(uint16_t duty);

/**
 * @brief  刹车（两路输出 100%，电机急停）
 */
void Motor_Brake(void);

/**
 * @brief  自由停止（两路输出 0%，电机惯性滑行）
 */
void Motor_Coast(void);

#ifdef __cplusplus
}
#endif

#endif /* __MOTOR_L9110S_H */
