/**
 * @file    light_sensor.h
 * @brief   光敏传感器 ADC 驱动（模拟 AO 输出）
 * @note    光敏模块 AO → PA0 (ADC1_CH0)，依赖 CubeMX 初始化
 */

#ifndef __LIGHT_SENSOR_H
#define __LIGHT_SENSOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define LIGHT_ADC_RESOLUTION  4096  /* 12位ADC */
#define LIGHT_ADC_VREF        3300  /* mV */

void     LightSensor_Init(void);
uint32_t LightSensor_Read(void);         /* 返回原始 ADC 0~4095 */
uint32_t LightSensor_ReadVoltage(void);  /* 返回 mV 0~3300 */
uint8_t  LightSensor_GetPercent(void);   /* 返回光照百分比 0~100（越亮越大） */

#ifdef __cplusplus
}
#endif

#endif /* __LIGHT_SENSOR_H */
