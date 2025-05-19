/*
 * flash.c
 *
 *  Created on: May 18, 2025
 *      Author: UnikoZera
 */

#include "flash.h"
#include "spi.h"

#ifndef W25Q64_CS_GPIO_Port
#define W25Q64_CS_GPIO_Port GPIOA
#endif
#ifndef W25Q64_CS_Pin
#define W25Q64_CS_Pin GPIO_PIN_7
#endif

#define W25Q64_CS_LOW() HAL_GPIO_WritePin(W25Q64_CS_GPIO_Port, W25Q64_CS_Pin, GPIO_PIN_RESET)
#define W25Q64_CS_HIGH() HAL_GPIO_WritePin(W25Q64_CS_GPIO_Port, W25Q64_CS_Pin, GPIO_PIN_SET)

// W25Q64 命令
#define W25X_WriteEnable 0x06
#define W25X_WriteDisable 0x04
#define W25X_ReadStatusReg1 0x05
#define W25X_ReadStatusReg2 0x35
#define W25X_WriteStatusReg 0x01
#define W25X_PageProgram 0x02
#define W25X_SectorErase 0x20   // 4KB
#define W25X_BlockErase32K 0x52 // 32KB
#define W25X_BlockErase64K 0xD8 // 64KB
#define W25X_ChipErase 0xC7     // or 0x60
#define W25X_PowerDown 0xB9
#define W25X_ReleasePowerDown 0xAB
#define W25X_ReadData 0x03
#define W25X_FastReadData 0x0B
#define W25X_FastReadDual 0x3B
#define W25X_DeviceID 0xAB
#define W25X_ManufactDeviceID 0x90
#define W25X_JedecDeviceID 0x9F

#define W25Q64_DUMMY_BYTE 0xA5
#define W25Q64_TIMEOUT 1000 // SPI Timeout in ms

// 内部辅助函数
static uint8_t W25Q64_Spi_TransmitReceive(uint8_t data)
{
    uint8_t rx_data;
    HAL_SPI_TransmitReceive(&hspi2, &data, &rx_data, 1, W25Q64_TIMEOUT);
    return rx_data;
}

static void W25Q64_Spi_Transmit(uint8_t *pData, uint16_t Size)
{
    HAL_SPI_Transmit(&hspi2, pData, Size, W25Q64_TIMEOUT);
}

static void W25Q64_Spi_Receive(uint8_t *pData, uint16_t Size)
{
    HAL_SPI_Receive(&hspi2, pData, Size, W25Q64_TIMEOUT);
}

void W25Q64_Write_Enable(void)
{
    W25Q64_CS_LOW();
    uint8_t cmd = W25X_WriteEnable;
    W25Q64_Spi_Transmit(&cmd, 1);
    W25Q64_CS_HIGH();
}

void W25Q64_Write_Disable(void)
{
    W25Q64_CS_LOW();
    uint8_t cmd = W25X_WriteDisable;
    W25Q64_Spi_Transmit(&cmd, 1);
    W25Q64_CS_HIGH();
}

uint8_t W25Q64_Read_StatusRegister1(void)
{
    uint8_t status;
    W25Q64_CS_LOW();
    uint8_t cmd = W25X_ReadStatusReg1;
    W25Q64_Spi_Transmit(&cmd, 1);
    W25Q64_Spi_Receive(&status, 1);
    W25Q64_CS_HIGH();
    return status;
}

uint8_t W25Q64_Read_StatusRegister2(void)
{
    uint8_t status;
    W25Q64_CS_LOW();
    uint8_t cmd = W25X_ReadStatusReg2;
    W25Q64_Spi_Transmit(&cmd, 1);
    W25Q64_Spi_Receive(&status, 1);
    W25Q64_CS_HIGH();
    return status;
}

void W25Q64_Wait_Busy(void)
{
    uint32_t tickstart = HAL_GetTick();
    while ((W25Q64_Read_StatusRegister1() & 0x01) == 0x01)
    { // Check BUSY bit
        if ((HAL_GetTick() - tickstart) > (W25Q64_TIMEOUT * 10))
        { // Adjusted timeout logic
            // Handle timeout error if necessary
            break;
        }
    }
}

// 初始化 W25Q64
// 返回值: 0,成功; 1,失败
uint8_t W25Q64_Init(void)
{
    W25Q64_CS_HIGH(); // Ensure CS is high initially
    HAL_Delay(10);    // Short delay after power-up
    uint32_t id = W25Q64_Read_JEDEC_ID();
    // JEDEC ID for W25Q64FV/JV is EF4017h
    // Manufacturer ID: EFh (Winbond)
    // Memory Type: 40h (SPI, 25Q Series)
    // Capacity: 17h (64Mbit = 8MByte)
    if (id == 0xEF4017)
    {
        return 0; // Success
    }
    return 1; // Failure
}

// 读取芯片ID (Manufacturer/Device ID)
// 返回值: (Manufacturer ID << 8) | Device ID
uint16_t W25Q64_Read_ID(void)
{
    uint8_t cmd[4] = {W25X_ManufactDeviceID, 0x00, 0x00, 0x00};
    uint8_t id_data[2];
    uint16_t id;

    W25Q64_CS_LOW();
    W25Q64_Spi_Transmit(cmd, 4); // Send command and 3 dummy address bytes
    W25Q64_Spi_Receive(id_data, 2);
    W25Q64_CS_HIGH();

    id = (id_data[0] << 8) | id_data[1];
    return id;
}

// 读取 JEDEC ID
// 返回值: (Manufacturer ID << 16) | (Memory Type << 8) | Capacity
uint32_t W25Q64_Read_JEDEC_ID(void)
{
    uint8_t cmd = W25X_JedecDeviceID;
    uint8_t id_data[3];
    uint32_t id;

    W25Q64_CS_LOW();
    W25Q64_Spi_Transmit(&cmd, 1);
    W25Q64_Spi_Receive(id_data, 3);
    W25Q64_CS_HIGH();

    id = (id_data[0] << 16) | (id_data[1] << 8) | id_data[2];
    return id;
}

// 擦除整个芯片
// 等待时间较长
void W25Q64_Erase_Chip(void)
{
    W25Q64_Write_Enable();
    W25Q64_Wait_Busy(); // Wait for WEL bit to be set

    W25Q64_CS_LOW();
    uint8_t cmd = W25X_ChipErase;
    W25Q64_Spi_Transmit(&cmd, 1);
    W25Q64_CS_HIGH();

    W25Q64_Wait_Busy(); // Wait for erase to complete
}

// 擦除一个扇区 (4KB)
// SectorAddr: 要擦除的扇区地址 (0x000000 - 0x7FF000, 必须是4KB的倍数)
void W25Q64_Erase_Sector(uint32_t SectorAddr)
{
    W25Q64_Write_Enable();
    W25Q64_Wait_Busy(); // Wait for WEL bit to be set

    W25Q64_CS_LOW();
    uint8_t cmd[4];
    cmd[0] = W25X_SectorErase;
    cmd[1] = (SectorAddr & 0xFF0000) >> 16;
    cmd[2] = (SectorAddr & 0x00FF00) >> 8;
    cmd[3] = (SectorAddr & 0x0000FF);
    W25Q64_Spi_Transmit(cmd, 4);
    W25Q64_CS_HIGH();

    W25Q64_Wait_Busy(); // Wait for erase to complete
}

// 页编程 (写入数据，最多256字节)
// WriteAddr: 写入地址 (0x000000 - 0x7FFFFF)
// pBuffer: 要写入的数据的指针
// NumByteToWrite: 要写入的字节数 (1-256)
void W25Q64_Page_Program(uint32_t WriteAddr, uint8_t *pBuffer, uint16_t NumByteToWrite)
{
    if (NumByteToWrite > 256 || NumByteToWrite == 0)
    {
        return; // Invalid size
    }

    W25Q64_Write_Enable();
    // W25Q64_Wait_Busy(); // WEL bit should be checked by polling status reg if needed,
    // but usually a small delay or direct command is fine.
    // The busy check after programming is more critical.

    W25Q64_CS_LOW();
    uint8_t cmd[4];
    cmd[0] = W25X_PageProgram;
    cmd[1] = (WriteAddr & 0xFF0000) >> 16;
    cmd[2] = (WriteAddr & 0x00FF00) >> 8;
    cmd[3] = (WriteAddr & 0x0000FF);
    W25Q64_Spi_Transmit(cmd, 4);
    W25Q64_Spi_Transmit(pBuffer, NumByteToWrite);
    W25Q64_CS_HIGH();

    W25Q64_Wait_Busy(); // Wait for page program to complete
}

// 写入数据 (自动处理跨页和扇区擦除)
// WriteAddr: 写入地址
// pBuffer: 数据指针
// NumByteToWrite: 写入字节数
void W25Q64_Write(uint32_t WriteAddr, uint8_t *pBuffer, uint32_t NumByteToWrite)
{
    uint32_t sector_pos;
    uint16_t sector_offset;
    uint16_t sector_remain;
    uint16_t bytes_to_write_in_page;
    uint32_t current_addr = WriteAddr;
    uint8_t *current_buf = pBuffer;
    uint32_t remaining_bytes = NumByteToWrite;

    // W25Q64有8M字节 = 8 * 1024 * 1024 Bytes = 0x800000 Bytes
    // 扇区大小 4KB = 4096 Bytes = 0x1000 Bytes
    // 页大小 256 Bytes = 0x100 Bytes

    while (remaining_bytes > 0)
    {
        sector_pos = current_addr / 4096;     // 当前地址所在的扇区
        sector_offset = current_addr % 4096;  // 当前地址在扇区内的偏移
        sector_remain = 4096 - sector_offset; // 当前扇区剩余空间

        // 检查是否需要擦除扇区 (通常写入前需要确保目标区域已擦除)
        // 这里简化处理，假设用户会先擦除。如果需要自动擦除，可以在这里添加逻辑。
        // 例如: W25Q64_Erase_Sector(sector_pos * 4096);

        // 计算当前页可写入的字节数
        uint16_t page_offset = current_addr % 256;
        bytes_to_write_in_page = 256 - page_offset;

        if (bytes_to_write_in_page > remaining_bytes)
        {
            bytes_to_write_in_page = remaining_bytes;
        }
        if (bytes_to_write_in_page > sector_remain)
        { // 确保不跨扇区写入（如果扇区擦除逻辑在此处）
            bytes_to_write_in_page = sector_remain;
        }

        W25Q64_Page_Program(current_addr, current_buf, bytes_to_write_in_page);

        current_addr += bytes_to_write_in_page;
        current_buf += bytes_to_write_in_page;
        remaining_bytes -= bytes_to_write_in_page;
    }
}

// 读取数据
// ReadAddr: 读取地址
// pBuffer: 数据接收缓冲区
// NumByteToRead: 读取字节数
void W25Q64_Read(uint32_t ReadAddr, uint8_t *pBuffer, uint32_t NumByteToRead)
{
    W25Q64_CS_LOW();
    uint8_t cmd[4];
    cmd[0] = W25X_ReadData;
    cmd[1] = (ReadAddr & 0xFF0000) >> 16;
    cmd[2] = (ReadAddr & 0x00FF00) >> 8;
    cmd[3] = (ReadAddr & 0x0000FF);
    W25Q64_Spi_Transmit(cmd, 4);
    W25Q64_Spi_Receive(pBuffer, NumByteToRead);
    W25Q64_CS_HIGH();
}

// 快速读取数据 (可以使用更高的 SPI 时钟)
// ReadAddr: 读取地址
// pBuffer: 数据接收缓冲区
// NumByteToRead: 读取字节数
void W25Q64_Fast_Read(uint32_t ReadAddr, uint8_t *pBuffer, uint32_t NumByteToRead)
{
    W25Q64_CS_LOW();
    uint8_t cmd[5]; // Command (1) + Address (3) + Dummy (1)
    cmd[0] = W25X_FastReadData;
    cmd[1] = (ReadAddr & 0xFF0000) >> 16;
    cmd[2] = (ReadAddr & 0x00FF00) >> 8;
    cmd[3] = (ReadAddr & 0x0000FF);
    cmd[4] = W25Q64_DUMMY_BYTE;  // Dummy byte
    W25Q64_Spi_Transmit(cmd, 5); // Send command, address, and dummy byte
    W25Q64_Spi_Receive(pBuffer, NumByteToRead);
    W25Q64_CS_HIGH();
}

// 双线快速读取数据 (需要支持双线模式的硬件连接)
// ReadAddr: 读取地址
// pBuffer: 数据接收缓冲区
// NumByteToRead: 读取字节数
void W25Q64_Dual_Read(uint32_t ReadAddr, uint8_t *pBuffer, uint32_t NumByteToRead) //!STM32F103不支持qspi！
{
    W25Q64_CS_LOW();
    uint8_t cmd[4];
    cmd[0] = W25X_FastReadDual;
    cmd[1] = (ReadAddr & 0xFF0000) >> 16;
    cmd[2] = (ReadAddr & 0x00FF00) >> 8;
    cmd[3] = (ReadAddr & 0x0000FF);
    W25Q64_Spi_Transmit(cmd, 4);
    W25Q64_Spi_Receive(pBuffer, NumByteToRead);
    W25Q64_CS_HIGH();
}
