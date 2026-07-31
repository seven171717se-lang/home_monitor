/**
 * @file    w25qxx.c
 * @brief   W25Qxx SPI Flash 驱动实现 — 依赖 CubeMX 初始化
 * @note    SPI1 由 CubeMX 生成的 MX_SPI1_Init() 配置（模式0：CPOL=Low, CPHA=1Edge）
 *          CS(PA4) 由 MX_GPIO_Init() 配置为推挽输出，默认高电平
 */

#include "w25qxx.h"
#include "stm32f1xx_hal.h"

/* 引用 CubeMX 生成的 SPI 句柄（在 main.c 中定义）*/
extern SPI_HandleTypeDef W25Q_SPI;

/* 命令码 ----------------------------------------------------------------*/
#define W25Q_CMD_WRITE_ENABLE    0x06
#define W25Q_CMD_READ_STATUS     0x05
#define W25Q_CMD_READ_ID         0x9F
#define W25Q_CMD_READ_DATA       0x03
#define W25Q_CMD_PAGE_PROGRAM    0x02
#define W25Q_CMD_SECTOR_ERASE    0x20
#define W25Q_CMD_CHIP_ERASE      0xC7

/* 私有函数 ----------------------------------------------------------------*/

/**
 * @brief  SPI 收发单字节
 */
static uint8_t w25q_transfer(uint8_t byte)
{
    uint8_t rx = 0;
    HAL_SPI_TransmitReceive(&W25Q_SPI, &byte, &rx, 1, 100);
    return rx;
}

/**
 * @brief  写使能（所有写/擦除操作前必须调用）
 */
static void w25q_write_enable(void)
{
    W25Q_CS_LOW();
    w25q_transfer(W25Q_CMD_WRITE_ENABLE);
    W25Q_CS_HIGH();
}

/**
 * @brief  等待内部操作完成（BUSY 位清零）
 */
static void w25q_wait_busy(void)
{
    uint8_t status;

    W25Q_CS_LOW();
    w25q_transfer(W25Q_CMD_READ_STATUS);
    do {
        status = w25q_transfer(0xFF);
    } while (status & 0x01);
    W25Q_CS_HIGH();
}

/**
 * @brief  单页写（≤256 字节）
 */
static void w25q_write_page_raw(uint32_t addr, const uint8_t *buf, uint16_t len)
{
    w25q_write_enable();

    W25Q_CS_LOW();
    w25q_transfer(W25Q_CMD_PAGE_PROGRAM);
    w25q_transfer((addr >> 16) & 0xFF);
    w25q_transfer((addr >> 8) & 0xFF);
    w25q_transfer(addr & 0xFF);
    for (uint16_t i = 0; i < len; i++)
        w25q_transfer(buf[i]);
    W25Q_CS_HIGH();

    w25q_wait_busy();
}

/* 公有函数 ----------------------------------------------------------------*/

void W25Q_Init(void)
{
    W25Q_CS_HIGH();
}

uint32_t W25Q_ReadID(void)
{
    uint32_t id = 0;

    W25Q_CS_LOW();
    w25q_transfer(W25Q_CMD_READ_ID);
    id = (uint32_t)w25q_transfer(0xFF) << 16;
    id |= (uint32_t)w25q_transfer(0xFF) << 8;
    id |= (uint32_t)w25q_transfer(0xFF);
    W25Q_CS_HIGH();

    return id;
}

void W25Q_Read(uint32_t addr, uint8_t *buf, uint32_t len)
{
    W25Q_CS_LOW();
    w25q_transfer(W25Q_CMD_READ_DATA);
    w25q_transfer((addr >> 16) & 0xFF);
    w25q_transfer((addr >> 8) & 0xFF);
    w25q_transfer(addr & 0xFF);
    for (uint32_t i = 0; i < len; i++)
        buf[i] = w25q_transfer(0xFF);
    W25Q_CS_HIGH();
}

void W25Q_WritePage(uint32_t addr, const uint8_t *buf, uint16_t len)
{
    if (len == 0 || len > 256)
        return;
    w25q_write_page_raw(addr, buf, len);
}

void W25Q_Write(uint32_t addr, const uint8_t *buf, uint32_t len)
{
    while (len > 0)
    {
        uint16_t page_left = (uint16_t)(256 - (addr % 256));
        uint16_t n = (len < page_left) ? (uint16_t)len : page_left;

        w25q_write_page_raw(addr, buf, n);

        addr += n;
        buf += n;
        len -= n;
    }
}

void W25Q_EraseSector(uint32_t addr)
{
    w25q_write_enable();

    W25Q_CS_LOW();
    w25q_transfer(W25Q_CMD_SECTOR_ERASE);
    w25q_transfer((addr >> 16) & 0xFF);
    w25q_transfer((addr >> 8) & 0xFF);
    w25q_transfer(addr & 0xFF);
    W25Q_CS_HIGH();

    w25q_wait_busy();
}

void W25Q_EraseChip(void)
{
    w25q_write_enable();

    W25Q_CS_LOW();
    w25q_transfer(W25Q_CMD_CHIP_ERASE);
    W25Q_CS_HIGH();

    w25q_wait_busy();
}
