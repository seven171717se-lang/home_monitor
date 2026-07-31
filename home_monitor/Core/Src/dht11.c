#include "dht11.h"
#include "stm32f1xx_hal.h"

/* DWT 精确微秒延时 */
static uint32_t us_tick = 0;

static void DWT_Init(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    us_tick = SystemCoreClock / 1000000U;
}

static void Delay_us(uint32_t us)
{
    uint32_t start = DWT->CYCCNT;
    uint32_t ticks = us * us_tick;
    while ((DWT->CYCCNT - start) < ticks);
}

static void Delay_ms(uint32_t ms)
{
    HAL_Delay(ms);
}

void DHT11_Rst(void)
{
    DHT11_Mode(DHT11_OUT);   // SET OUTPUT
    DHT11_Low;
    Delay_ms(20);
    DHT11_High;              // DQ=1
    Delay_us(13);
}

uint8_t DHT11_Check(void)
{
    uint8_t retry = 0;
    DHT11_Mode(DHT11_IN);    // SET INPUT
    while (HAL_GPIO_ReadPin(DHT11_GPIO_PORT, DHT11_GPIO_PIN) && retry < 100)  // DHT11 40~80us
    {
        retry++;
        Delay_us(1);
    }
    if (retry >= 100) return 1;
    else retry = 0;
    while (!HAL_GPIO_ReadPin(DHT11_GPIO_PORT, DHT11_GPIO_PIN) && retry < 100)  // DHT11 40~80us
    {
        retry++;
        Delay_us(1);
    }
    if (retry >= 100) return 1;
    return 0;
}

uint8_t DHT11_Read_Bit(void)
{
    uint8_t retry = 0;
    while (HAL_GPIO_ReadPin(DHT11_GPIO_PORT, DHT11_GPIO_PIN) && retry < 100)
    {
        retry++;
        Delay_us(1);
    }
    retry = 0;
    while (!HAL_GPIO_ReadPin(DHT11_GPIO_PORT, DHT11_GPIO_PIN) && retry < 100)
    {
        retry++;
        Delay_us(1);
    }
    Delay_us(40);
    if (HAL_GPIO_ReadPin(DHT11_GPIO_PORT, DHT11_GPIO_PIN)) return 1;
    else return 0;
}

uint8_t DHT11_Read_Byte(void)
{
    uint8_t i, dat;
    dat = 0;
    for (i = 0; i < 8; i++)
    {
        dat <<= 1;
        dat |= DHT11_Read_Bit();
    }
    return dat;
}

uint8_t DHT11_Read_Data(uint8_t *temp_int, uint8_t *temp_dec, uint8_t *humi_int, uint8_t *humi_dec)
{
    uint8_t buf[5];
    uint8_t i;
    DHT11_Rst();
    if (DHT11_Check() == 0)
    {
        for (i = 0; i < 5; i++)
        {
            buf[i] = DHT11_Read_Byte();
        }
        if ((uint8_t)(buf[0] + buf[1] + buf[2] + buf[3]) == buf[4])
        {
            *humi_int = buf[0];
            *humi_dec = buf[1];
            *temp_int = buf[2];
            *temp_dec = buf[3];
        }
    }
    else return 1;
    return 0;
}

uint8_t DHT11_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStructure.Pin   = DHT11_GPIO_PIN;
    GPIO_InitStructure.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(DHT11_GPIO_PORT, &GPIO_InitStructure);

    HAL_GPIO_WritePin(DHT11_GPIO_PORT, DHT11_GPIO_PIN, GPIO_PIN_SET);

    DWT_Init();

    DHT11_Rst();
    return DHT11_Check();
}

void DHT11_Mode(uint8_t mode)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    if (mode)
    {
        GPIO_InitStructure.Pin   = DHT11_GPIO_PIN;
        GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStructure.Mode  = GPIO_MODE_OUTPUT_PP;
    }
    else
    {
        GPIO_InitStructure.Pin  = DHT11_GPIO_PIN;
        GPIO_InitStructure.Mode = GPIO_MODE_INPUT;   // 浮空输入
    }
    HAL_GPIO_Init(DHT11_GPIO_PORT, &GPIO_InitStructure);
}

uint8_t DHT11_Read_Buf(uint8_t *buf)
{
    uint8_t i;
    DHT11_Rst();
    if (DHT11_Check() != 0) return 1;
    for (i = 0; i < 5; i++)
    {
        buf[i] = DHT11_Read_Byte();
    }
    return 0;
}
