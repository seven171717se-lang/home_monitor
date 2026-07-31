/**
 * @file    adc_pot.h
 * @brief   电位器 ADC 驱动
 */

#ifndef __ADC_POT_H
#define __ADC_POT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define ADC_POT_RESOLUTION  4096  // 12位ADC
#define ADC_POT_VREF        3300  // mV

void    ADC_Pot_Init(void);
uint32_t ADC_Pot_Read(void);           // 返回 0~4095
uint32_t ADC_Pot_ReadVoltage(void);    // 返回 mV (0~3300)
uint8_t  ADC_Pot_GetPercent(void);     // 返回 0~100

#ifdef __cplusplus
}
#endif

#endif /* __ADC_POT_H */
