/**
 * @file    light_sensor.c
 * @brief   光敏传感器 ADC 驱动实现
 * @note    AO → PA0 (ADC1_CH0)
 */

#include "light_sensor.h"
#include "stm32f1xx_hal.h"

static ADC_HandleTypeDef hadc1;

void LightSensor_Init(void)
{
    hadc1.Instance = ADC1;
    hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
    hadc1.Init.ContinuousConvMode = ENABLE;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 1;

    HAL_ADC_Init(&hadc1);

    ADC_ChannelConfTypeDef sConfig = {0};
    sConfig.Channel = ADC_CHANNEL_0;        /* PA0 */
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_71CYCLES_5;
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);
}

uint32_t LightSensor_Read(void)
{
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, 10);
    return HAL_ADC_GetValue(&hadc1);
}

uint32_t LightSensor_ReadVoltage(void)
{
    return LightSensor_Read() * LIGHT_ADC_VREF / LIGHT_ADC_RESOLUTION;
}

uint8_t LightSensor_GetPercent(void)
{
    /* 光敏模块：越亮电压越高，映射为 0~100% */
    return (uint8_t)(LightSensor_Read() * 100UL / 4095UL);
}
