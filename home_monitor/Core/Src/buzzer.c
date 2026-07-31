/**
 * @file    buzzer.c
 * @brief   有源蜂鸣器驱动实现
 */

#include "buzzer.h"
#include "stm32f1xx_hal.h"

static void delay_ms(uint32_t ms)
{
    HAL_Delay(ms);
}

void Buzzer_Init(void)
{
    GPIO_InitTypeDef gpio = {0};

    __HAL_RCC_GPIOB_CLK_ENABLE();

    gpio.Pin   = BUZZER_GPIO_PIN;
    gpio.Mode  = GPIO_MODE_OUTPUT_PP;
    gpio.Pull  = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(BUZZER_GPIO_PORT, &gpio);

    Buzzer_Off();
}

void Buzzer_On(void)
{
    HAL_GPIO_WritePin(BUZZER_GPIO_PORT, BUZZER_GPIO_PIN, GPIO_PIN_SET);
}

void Buzzer_Off(void)
{
    HAL_GPIO_WritePin(BUZZER_GPIO_PORT, BUZZER_GPIO_PIN, GPIO_PIN_RESET);
}

void Buzzer_Toggle(void)
{
    HAL_GPIO_TogglePin(BUZZER_GPIO_PORT, BUZZER_GPIO_PIN);
}

void Buzzer_Beep(uint32_t ms)
{
    Buzzer_On();
    delay_ms(ms);
    Buzzer_Off();
}
