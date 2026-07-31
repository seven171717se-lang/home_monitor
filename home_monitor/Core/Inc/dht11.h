#ifndef __DHT11_H
#define __DHT11_H

#include "stm32f1xx_hal.h"                  // HAL Device header
#include <stdint.h>

/* 可配置项：根据你的 CubeMX 引脚修改 */
#define DHT11_GPIO_PORT  GPIOA
#define DHT11_GPIO_PIN   GPIO_PIN_3
#define DHT11_GPIO_CLK   __HAL_RCC_GPIOA_CLK_ENABLE()

#define DHT11_OUT 1
#define DHT11_IN  0

#define DHT11_Low  HAL_GPIO_WritePin(DHT11_GPIO_PORT, DHT11_GPIO_PIN, GPIO_PIN_RESET)
#define DHT11_High HAL_GPIO_WritePin(DHT11_GPIO_PORT, DHT11_GPIO_PIN, GPIO_PIN_SET)

uint8_t DHT11_Init(void);
uint8_t DHT11_Read_Byte(void);
uint8_t DHT11_Read_Bit(void);
void    DHT11_Mode(uint8_t mode);
uint8_t DHT11_Check(void);
void    DHT11_Rst(void);
uint8_t DHT11_Read_Data(uint8_t *temp_int, uint8_t *temp_dec, uint8_t *humi_int, uint8_t *humi_dec);
uint8_t DHT11_Read_Buf(uint8_t *buf);

#endif
