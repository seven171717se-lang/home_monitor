/**
 * @file    w25qxx.h
 * @brief   W25Qxx SPI Flash 驱动
 * @note    依赖 CubeMX 初始化 SPI + CS 引脚，驱动只负责收发命令
 */

#ifndef __W25QXX_H
#define __W25QXX_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* 可配置项：根据你的 CubeMX SPI/引脚修改
 * 默认使用 SPI1：SCK=PA5 MISO=PA6 MOSI=PA7，CS=PA4
 */
#define W25Q_SPI           hspi2
#define W25Q_CS_GPIO_PORT  GPIOB
#define W25Q_CS_GPIO_PIN   GPIO_PIN_12

#define W25Q_CS_LOW()   HAL_GPIO_WritePin(W25Q_CS_GPIO_PORT, W25Q_CS_GPIO_PIN, GPIO_PIN_RESET)
#define W25Q_CS_HIGH()  HAL_GPIO_WritePin(W25Q_CS_GPIO_PORT, W25Q_CS_GPIO_PIN, GPIO_PIN_SET)

/**
 * @brief  初始化 W25Qxx
 * @note   SPI 与 CS 引脚由 CubeMX 配置，此处仅拉高 CS
 */
void W25Q_Init(void);

/**
 * @brief  读取 JEDEC ID
 * @return 24 位 ID，例如 W25Q16=0xEF4014 W25Q32=0xEF4015
 *         W25Q64=0xEF4016 W25Q128=0xEF4018，读不到返回 0xFFFFFF
 */
uint32_t W25Q_ReadID(void);

/**
 * @brief  读取数据（任意地址任意长度，跨页自动处理）
 * @param  addr: 24 位起始地址
 * @param  buf:  数据缓冲区
 * @param  len:  读取长度
 */
void W25Q_Read(uint32_t addr, uint8_t *buf, uint32_t len);

/**
 * @brief  页写（单页 ≤ 256 字节，不能跨页）
 * @param  addr: 24 位起始地址
 * @param  buf:  数据缓冲区
 * @param  len:  写入长度（1~256）
 */
void W25Q_WritePage(uint32_t addr, const uint8_t *buf, uint16_t len);

/**
 * @brief  连续写（自动跨页，最长 0xFFFFFF，需先擦除目标区域）
 * @param  addr: 24 位起始地址
 * @param  buf:  数据缓冲区
 * @param  len:  写入长度
 */
void W25Q_Write(uint32_t addr, const uint8_t *buf, uint32_t len);

/**
 * @brief  擦除 4KB 扇区（地址按 4KB 对齐）
 */
void W25Q_EraseSector(uint32_t addr);

/**
 * @brief  整片擦除（耗时较长，等待内部完成）
 */
void W25Q_EraseChip(void);

#ifdef __cplusplus
}
#endif

#endif /* __W25QXX_H */
