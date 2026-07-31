/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : 家庭环境监测�? �? 主程�?
  *   DHT11 温湿�? + 光敏(ADC)光照 �? OLED显示 �? 自动风扇控制
  *   �? 蜂鸣�?/LED报警 �? W25Qxx日志记录 �? 
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "oled_ssd1306.h"
#include "encoder.h"
#include "dht11.h"
#include "motor_l9110s.h"
#include "w25qxx.h"
#include "buzzer.h"
#include "light_sensor.h"
#include <string.h>
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* 系统配置 */
#define LOG_INTERVAL_S    60    /* Flash 数据记录间隔（秒�? */
#define SENSOR_READ_MS    1200  /* DHT11 读取间隔（毫秒） */
#define FLASH_CFG_ADDR    0x000000  /* 配置区（扇区0�?4KB�? */
#define FLASH_LOG_ADDR    0x001000  /* 日志区（扇区1�?4KB�? */
#define FLASH_LOG_SIZE    4096
/* 每条日志 8 字节：温�?(1) + 湿度(1) + 光照(1) + 风扇(1) + 序列�?(2) + 标志(1) + 保留(1) */
#define LOG_ENTRY_SIZE    8
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi2;

TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;

/* USER CODE BEGIN PV */
/* �?�? 传感器数�? �?�? */
uint8_t  dht_temp = 0, dht_hum = 0, dht_ok = 0;
uint8_t  light_level = 0;           /* B2K 电位�? �? 光照 0~100% */
uint32_t sensor_tick = 0;

/* �?�? 控制状�?? �?�? */
uint8_t  page = 0;                  /* 0=仪表�? 1=设置 2=日志 */
uint8_t  manual_mode = 0;           /* 0=自动 1=手动 */
uint8_t  motor_speed = 0;
uint8_t  temp_threshold = 30;       /* 温度报警阈�?? */
uint8_t  light_threshold = 80;      /* 光照报警阈�?? */

/* �?�? 报警状�?? �?�? */
uint8_t  alarm_temp = 0;            /* 温度告警 */
uint8_t  alarm_light = 0;           /* 强光告警 */
uint32_t alarm_tick = 0;
uint8_t  alarm_blink = 0;
uint32_t buzzer_tick = 0;
uint8_t  buzzer_beeping = 0;

/* �?�? Flash 日志 �?�? */
uint32_t log_count = 0;             /* 已记录条�? */
uint32_t log_tick = 0;

/* �?�? 编码器辅�? �?�? */
int32_t  enc_last = 0;
uint8_t  set_item = 0;   /* 设置�?: 0=调节 1=模式 2=保存 */
uint8_t  set_focus = 0;  /* 0=调温�? 1=调光�? */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI2_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_SPI2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
  /* �?�? 1. 显示屏启�? �?�? */
  OLED_Init();
  OLED_Clear();
  OLED_ShowString(28, 0, "Home Monitor");
  OLED_ShowString(34, 3, "Starting...");
  OLED_Refresh();
  HAL_Delay(800);

  /* �?�? 2. 初始化所有外�? �?�? */
  Buzzer_Init();
  Encoder_Init();
  Motor_Init();
  W25Q_Init();
  LightSensor_Init();
  DHT11_Init();

  /* �?�? 3. �? Flash 加载设置 �?�? */
  {
    uint8_t cfg[2] = {0};
    W25Q_Read(FLASH_CFG_ADDR, cfg, 2);
    if (cfg[0] >= 20 && cfg[0] <= 50) temp_threshold = cfg[0];
    if (cfg[1] <= 100)               light_threshold = cfg[1];
  }


  OLED_Clear();
  OLED_ShowString(28, 1, "Home Monitor");
  OLED_ShowString(28, 3, "System Ready");
  OLED_Refresh();
  HAL_Delay(1500);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

    /* ==== 传感器采�? ==== */
    if (HAL_GetTick() - sensor_tick >= SENSOR_READ_MS)
    {
        sensor_tick = HAL_GetTick();
        { uint8_t t_dec=0, h_dec=0; dht_ok = (DHT11_Read_Data(&dht_temp,&t_dec,&dht_hum,&h_dec)==0); }
        light_level = LightSensor_GetPercent();
    }

    /* ==== 自动控制 ==== */
    alarm_temp  = (dht_ok && dht_temp > temp_threshold);
    alarm_light = (light_level > light_threshold);
    if (!manual_mode)
    {
        if (alarm_temp)
        {
            uint8_t over = dht_temp - temp_threshold;
            motor_speed = (over>=5) ? 100 : (50+over*10);
            if (motor_speed>100) motor_speed=100;
            Motor_Forward(motor_speed);
        }
        else { motor_speed=0; Motor_Coast(); }
    }

    /* ==== 声光报警 ==== */
    if (alarm_temp || alarm_light)
    {
        if (!buzzer_beeping && HAL_GetTick()-buzzer_tick>=500)
        { buzzer_tick=HAL_GetTick(); buzzer_beeping=1; Buzzer_On(); }
        else if (buzzer_beeping && HAL_GetTick()-buzzer_tick>=100)
        { buzzer_tick=HAL_GetTick(); buzzer_beeping=0; Buzzer_Off(); }
        if (HAL_GetTick()-alarm_tick>=250)
        { alarm_tick=HAL_GetTick(); alarm_blink=!alarm_blink;
          HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,alarm_blink?GPIO_PIN_RESET:GPIO_PIN_SET); }
    }
    else { Buzzer_Off(); buzzer_beeping=0; HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_SET); }

    /* ==== Flash 日志 ==== */
    if (HAL_GetTick()-log_tick>=LOG_INTERVAL_S*1000UL)
    {
        log_tick=HAL_GetTick();
        uint32_t addr=FLASH_LOG_ADDR+log_count*LOG_ENTRY_SIZE;
        if (addr+LOG_ENTRY_SIZE<FLASH_LOG_ADDR+FLASH_LOG_SIZE)
        {
            uint8_t e[LOG_ENTRY_SIZE];
            e[0]=dht_ok?dht_temp:0xFF; e[1]=dht_ok?dht_hum:0xFF;
            e[2]=light_level; e[3]=motor_speed;
            e[4]=(uint8_t)(log_count>>8); e[5]=(uint8_t)log_count;
            e[6]=alarm_temp|(alarm_light<<1); e[7]=0;
            if (log_count==0) W25Q_EraseSector(FLASH_LOG_ADDR);
            W25Q_Write(addr,e,LOG_ENTRY_SIZE);
            log_count++;
        }
    }

    /* ==== 按键翻页 ==== */
    {
        uint8_t btn = Encoder_GetButtonPressed();
        if (btn)
        {
            if (page == 1)
            {
                if (set_item==0 && set_focus==0) set_focus=1;
                else { set_item=(set_item+1)%3; set_focus=0; }
            }
            else page=(page+1)%4;
            Encoder_Reset(); enc_last=Encoder_GetCount();
        }
    }

    OLED_Clear();

    /* ========== Page 0：仪表盘 ========== */
    if (page == 0)
    {
        char buf[21];

        /* 标题�? �? 反色 */
        OLED_FillRect(0, 0, 128, 10, 1);
        OLED_InvertArea(0, 0, 128, 10);
        OLED_ShowString(28, 0, "HOME MONITOR");
        OLED_DrawHLine(0, 10, 128, 1);

        /* 温湿�? */
        if (dht_ok)
            sprintf(buf, "T:%2dC  H:%2d%%", dht_temp, dht_hum);
        else
            sprintf(buf, "T:--C  H:--%%");
        OLED_ShowString(0, 2, buf);

        /* 光照 */
        sprintf(buf, "L:%3d%%", light_level);
        OLED_ShowString(0, 3, buf);

        /* 风扇 */
        sprintf(buf, "F:%3d%%", motor_speed);
        OLED_ShowString(0, 4, buf);


        /* 报警状�?? */
        if (alarm_temp || alarm_light)
        {
            sprintf(buf, "!! %s%s !!",
                    alarm_temp  ? "TEMP " : "",
                    alarm_light ? "LIGHT" : "");
            OLED_ShowString(0, 5, buf);
        }
        else
        {
            OLED_ShowString(0, 5, "Status: Normal");
        }

        /* 底栏分隔 + 状�?? */
        OLED_DrawHLine(0, 54, 128, 1);
        sprintf(buf, "Log:%lu", log_count);
        OLED_ShowString(0, 7, buf);
    }
    /* ========== Page 1：设�? ========== */
    else if (page == 1)
    {
        char buf[21];
        EncoderDirection dir = Encoder_GetDirection();

        /* 标题�? */
        OLED_FillRect(0, 0, 128, 10, 1);
        OLED_InvertArea(0, 0, 128, 10);
        OLED_ShowString(40, 0, "SETTINGS");
        OLED_DrawHLine(0, 10, 128, 1);

        switch (set_item)
        {
        case 0:
            /* 温度+光照 �? 按键切换焦点 */
            if (set_focus == 0)
            {
                if (dir == ENCODER_DIR_CW  && temp_threshold < 50) temp_threshold++;
                if (dir == ENCODER_DIR_CCW && temp_threshold > 20) temp_threshold--;
            }
            else
            {
                if (dir == ENCODER_DIR_CW  && light_threshold < 100) light_threshold += 5;
                if (dir == ENCODER_DIR_CCW && light_threshold > 10)  light_threshold -= 5;
            }
            OLED_ShowString(0, 2, set_focus ? " Temp:" : ">Temp:");
            sprintf(buf, "%d C", temp_threshold);
            OLED_ShowString(60, 2, buf);
            OLED_ShowString(0, 4, set_focus ? ">Light:" : " Light:");
            sprintf(buf, "%d %%", light_threshold);
            OLED_ShowString(60, 4, buf);
            break;
        case 1:
            /* 模式切换 */
            if (dir == ENCODER_DIR_CW || dir == ENCODER_DIR_CCW)
                manual_mode = !manual_mode;
            OLED_ShowString(0, 2, ">Control Mode");
            OLED_ShowString(0, 4, manual_mode ? " MANUAL" : " AUTO");
            break;
        case 2:
            /* 保存+返回 */
            {
                uint8_t cfg[2] = {temp_threshold, light_threshold};
                W25Q_EraseSector(FLASH_CFG_ADDR);
                W25Q_Write(FLASH_CFG_ADDR, cfg, 2);
            }
            page = 0;
            set_item = 0;
            set_focus = 0;
            set_focus = 0;
            Encoder_Reset();
            enc_last = Encoder_GetCount();
            break;
        }
        OLED_DrawHLine(0, 54, 128, 1);
        OLED_ShowString(0, 7, "Btn:next Turn:adj");
    }
    /* ========== Page 2：模式控�? ========== */
    else if (page == 2)
    {
        char buf[21];
        EncoderDirection dir = Encoder_GetDirection();

        /* 标题�? */
        OLED_FillRect(0, 0, 128, 10, 1);
        OLED_InvertArea(0, 0, 128, 10);
        OLED_ShowString(40, 0, "  MODE  ");
        OLED_DrawHLine(0, 10, 128, 1);

        OLED_ShowString(0, 2, "Control:");
        OLED_ShowString(60, 2, manual_mode ? "MANUAL" : "AUTO");

        if (dir != ENCODER_DIR_NONE && !manual_mode)
        {
            /* 自动模式下旋�? �? 切到手动 */
            manual_mode = 1;
        }

        if (manual_mode)
        {
            /* 手动模式：旋转调�? */
            OLED_ShowString(0, 4, "Fan Speed:");
            if (dir == ENCODER_DIR_CW  && motor_speed < 100) motor_speed += 5;
            if (dir == ENCODER_DIR_CCW && motor_speed > 0)   motor_speed -= 5;
            sprintf(buf, "  %3d %%", motor_speed);
            OLED_ShowString(0, 5, buf);
            if (motor_speed > 0) Motor_Forward(motor_speed);
            else Motor_Coast();
        }
        else
        {
            OLED_ShowString(0, 4, "Auto mode:");
            OLED_ShowString(0, 5, " Fan by temp");
            OLED_ShowString(0, 6, " threshold");
        }

        OLED_DrawHLine(0, 54, 128, 1);
        OLED_ShowString(0, 7, "Turn:adj Btn:next");
    }
    /* ========== Page 3：数据日�? ========== */
    else
    {
        char buf[21];

        /* 标题�? */
        OLED_FillRect(0, 0, 128, 10, 1);
        OLED_InvertArea(0, 0, 128, 10);
        OLED_ShowString(40, 0, "DATA LOG");
        OLED_DrawHLine(0, 10, 128, 1);

        sprintf(buf, "Records: %lu", log_count);
        OLED_ShowString(0, 2, buf);

        if (log_count > 0)
        {
            /* �?新一�? */
            uint32_t addr = FLASH_LOG_ADDR + (log_count - 1) * LOG_ENTRY_SIZE;
            uint8_t entry[LOG_ENTRY_SIZE];
            W25Q_Read(addr, entry, LOG_ENTRY_SIZE);

            OLED_ShowString(0, 3, "Latest:");
            sprintf(buf, "T=%dC H=%d%% L=%d%%",
                    entry[0] != 0xFF ? entry[0] : 0,
                    entry[1] != 0xFF ? entry[1] : 0,
                    entry[2]);
            OLED_ShowString(0, 4, buf);

            /* �?早一�? */
            addr = FLASH_LOG_ADDR;
            W25Q_Read(addr, entry, LOG_ENTRY_SIZE);

            OLED_ShowString(0, 5, "Oldest:");
            sprintf(buf, "T=%dC H=%d%% L=%d%%",
                    entry[0] != 0xFF ? entry[0] : 0,
                    entry[1] != 0xFF ? entry[1] : 0,
                    entry[2]);
            OLED_ShowString(0, 6, buf);
        }
        else
        {
            OLED_ShowString(16, 3, "No data yet...");
            OLED_ShowString(4, 5, "Auto-log every 60s");
        }
        OLED_DrawHLine(0, 54, 128, 1);
        OLED_ShowString(0, 7, "Btn:next page");
    }

    OLED_Refresh();
    HAL_Delay(80);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_71CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 50;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI1;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 0;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 0;
  if (HAL_TIM_Encoder_Init(&htim3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 17;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 99;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */
  HAL_TIM_MspPostInit(&htim4);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA3 PA8 */
  GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PB0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PB12 */
  GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
