/**
 * @file    buzzer.h
 * @brief   有源蜂鸣器驱动（GPIO 控制）
 */

#ifndef __BUZZER_H
#define __BUZZER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* 可配置项：根据你的 CubeMX 引脚修改 */
#define BUZZER_GPIO_PORT   GPIOA
#define BUZZER_GPIO_PIN    GPIO_PIN_8

void Buzzer_Init(void);
void Buzzer_On(void);
void Buzzer_Off(void);
void Buzzer_Toggle(void);
void Buzzer_Beep(uint32_t ms);

#ifdef __cplusplus
}
#endif

#endif /* __BUZZER_H */
