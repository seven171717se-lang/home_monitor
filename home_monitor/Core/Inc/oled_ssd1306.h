/**
 * @file    oled_ssd1306.h
 * @brief   OLED SSD1306 驱动（I2C接口，128x64）
 */

#ifndef __OLED_SSD1306_H
#define __OLED_SSD1306_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define OLED_WIDTH   128
#define OLED_HEIGHT  64

/**
 * @brief  初始化 OLED
 * @note   调用前需确保 MX_I2C1_Init() 已执行
 */
void OLED_Init(void);

/**
 * @brief  清空显存
 */
void OLED_Clear(void);

/**
 * @brief  将显存刷新到 OLED
 */
void OLED_Refresh(void);

/**
 * @brief  画一个像素点
 * @param  x  列 (0~127)
 * @param  y  行 (0~63)
 * @param  color  1=亮, 0=灭
 */
void OLED_DrawPixel(uint8_t x, uint8_t y, uint8_t color);

/**
 * @brief  显示字符串（6x8字体）
 * @param  x  起始列
 * @param  y  起始行 (0~7，每行8像素)
 * @param  str  字符串
 */
void OLED_ShowString(uint8_t x, uint8_t y, const char *str);

/**
 * @brief  显示数字
 * @param  x  起始列
 * @param  y  起始行
 * @param  num  数字
 * @param  len  位数
 */
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len);

/**
 * @brief  显示汉字（16x16点阵）
 * @param  x  起始列
 * @param  y  起始行
 * @param  index  汉字在字库中的索引
 */

/* ── 扩展绘图 ── */
void OLED_FillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color);
void OLED_DrawHLine(uint8_t x, uint8_t y, uint8_t w, uint8_t color);
void OLED_DrawVLine(uint8_t x, uint8_t y, uint8_t h, uint8_t color);
void OLED_DrawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color);
void OLED_DrawProgressBar(uint8_t x, uint8_t y, uint8_t w, uint8_t percent);
void OLED_InvertArea(uint8_t x, uint8_t y, uint8_t w, uint8_t h);
void OLED_ShowChinese(uint8_t x, uint8_t y, uint8_t index);

#ifdef __cplusplus
}
#endif

#endif /* __OLED_SSD1306_H */


