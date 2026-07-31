/**
 * @file    adc_pot.c
 * @brief   电位器 ADC 驱动实现
 */

#include "adc_pot.h"
#include "stm32f1xx_hal.h"

static ADC_HandleTypeDef hadc1;

void ADC_Pot_Init(void)
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
    sConfig.Channel = ADC_CHANNEL_0;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_71CYCLES_5;
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);
}

uint32_t ADC_Pot_Read(void)
{
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, 10);
    return HAL_ADC_GetValue(&hadc1);
}

uint32_t ADC_Pot_ReadVoltage(void)
{
    return ADC_Pot_Read() * ADC_POT_VREF / ADC_POT_RESOLUTION;
}

uint8_t ADC_Pot_GetPercent(void)
{
    return (uint8_t)(ADC_Pot_Read() * 100UL / 4095UL);
}
