/**
 * @file    encoder.h
 * @brief   旋转编码器驱动（EC11 / HW-040）
 * @note    使用 TIM3 编码器模式 + PB0 EXTI 中断
 */

#ifndef __ENCODER_H
#define __ENCODER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief  旋转方向
 */
typedef enum {
    ENCODER_DIR_CW  = 1,   /**< 顺时针 */
    ENCODER_DIR_CCW = -1,  /**< 逆时针 */
    ENCODER_DIR_NONE = 0   /**< 无转动 */
} EncoderDirection;

/**
 * @brief  初始化编码器
 * @note   配置 TIM3 编码器模式 + PB0 EXTI 中断
 */
void Encoder_Init(void);

/**
 * @brief  获取当前计数值
 * @return TIM3->CNT 的值（0 ~ 65535）
 */
int32_t Encoder_GetCount(void);

/**
 * @brief  获取旋转方向
 * @return ENCODER_DIR_CW / ENCODER_DIR_CCW / ENCODER_DIR_NONE
 */
EncoderDirection Encoder_GetDirection(void);

/**
 * @brief  计数值归零
 */
void Encoder_Reset(void);

/**
 * @brief  按键中断处理函数
 * @note   用户在 HAL_GPIO_EXTI_Callback() 中调用此函数
 */
void Encoder_ButtonPressed(void);

/**
 * @brief  检测按键是否被按下
 * @return 1 = 按下, 0 = 未按下
 * @note   调用后自动清除标志位
 */
uint8_t Encoder_GetButtonPressed(void);

#ifdef __cplusplus
}
#endif

#endif /* __ENCODER_H */
