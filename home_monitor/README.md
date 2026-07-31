# Home Monitor — 家庭环境监测仪

基于 STM32F103C8T6 + HAL 库的多传感器环境监测系统，OLED 菜单交互，自动温控风扇，Flash 日志存储。

## 硬件接线

| STM32 引脚 | 外设 | 说明 |
|---|---|---|
| PA0 | 光敏模块 AO | ADC1_CH0 |
| PA3 | DHT11 DATA | OneWire |
| PA6 | 编码器 CLK | TIM3_CH1 |
| PA7 | 编码器 DT | TIM3_CH2 |
| PA8 | 有源蜂鸣器 | GPIO 输出 |
| PB0 | 编码器 BTN | EXTI0 下降沿 |
| PB6 | OLED SCL | I2C1 |
| PB7 | OLED SDA | I2C1 |
| PB8 | L9110S IA | TIM4_CH3 PWM |
| PB9 | L9110S IB | TIM4_CH4 PWM |
| PB12 | W25Q64 CS | SPI2 软件 CS |
| PB13 | W25Q64 CLK | SPI2 SCK |
| PB14 | W25Q64 DO | SPI2 MISO |
| PB15 | W25Q64 DI | SPI2 MOSI |
| PC13 | 板载 LED | 低电平亮 |

## CubeMX 关键配置

| 外设 | 配置 |
|---|---|
| I2C1 | 100kHz, PB6/PB7 |
| SPI2 | Full-Duplex Master, CPOL=Low CPHA=1Edge, NSS=Soft |
| TIM3 | Encoder Mode, Combined: TI1, Prescaler=0, Period=50 |
| TIM4 | PWM CH3+CH4, Prescaler=72-1, Period=1000-1 |
| ADC1 | CH0(PA0), Continuous Conversion, 12bit, PCLK2/6 |
| PB0 | GPIO_EXTI0, Falling edge, Pull-up |

## 菜单操作

| 按键 | 操作 |
|---|---|
| 旋转编码器 | 调节数值 / 切换选项 |
| 按下按键 | 翻页 / 切换焦点 / 确认 |

## 页面说明

| 页面 | 功能 |
|---|---|
| Page 0 — HOME MONITOR | 仪表盘：温度、湿度、光照、风扇转速、报警状态、日志条数 |
| Page 1 — SETTINGS | 温度阈值(20-50°C)、光照阈值(10-100%)、控制模式、保存到 Flash |
| Page 2 — MODE | AUTO/MANUAL 切换，手动调速 0-100% |
| Page 3 — DATA LOG | 日志条数，最新/最早记录（温度、湿度、光照） |

## 自动控制逻辑

- 温度超过阈值 → 自动启动风扇（超 1°C=50% 转速，超 5°C=100%）
- 温度或光照超标 → 蜂鸣器间歇报警 + LED 闪烁
- Flash 每 60 秒记录一条（温度+湿度+光照+风扇+标志）
- 阈值保存到 Flash 扇区 0，断电不丢失

## 编译

Keil MDK-ARM 打开 `MDK-ARM/home_monitor.uvprojx` → 编译 → ST-Link 下载。
