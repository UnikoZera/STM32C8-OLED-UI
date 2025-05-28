/*
 * oled.c
 *
 *  Created on: May 3, 2025
 *      Author: UnikoZera
 */

#include "oled.h"
#define OLED_ADDR 0x3C // I2C for SSD1315

volatile uint32_t oled_last_update_time = 0;   // 上次更新显示的时间
volatile uint8_t oled_update_flag = 0;         // 更新标志位
volatile uint8_t oled_dma_busy = 0;            // DMA传输忙标志
volatile uint8_t oled_dirty_pages[OLED_PAGES]; // 标记哪些页需要更新

// 128 (宽) x 8 (页) = 1024 字节
uint8_t OLED_BackBuffer[128 * 8];
uint8_t OLED_FrontBuffer[128 * 8 + 1]; // 额外的字节用于I2C命令

// ASCII 6x8 字体数组 (32-127字符)
const uint8_t OLED_FONT_6x8[] = {
    0x00,0x00,0x00,0x00,0x00,0x00, /*' ', 0 */
    0x00,0x00,0x9f,0x83,0x00,0x00, /*'!', 1 */
    0x00,0x07,0x00,0x00,0x07,0x00, /*'\"', 2 */
    0x20,0xfc,0x27,0xe4,0x3f,0x04, /*'#', 3 */
    0x00,0x67,0x89,0xfe,0x73,0x00, /*'$', 4 */
    0x26,0x29,0x1f,0xf8,0x94,0x64, /*'%', 5 */
    0x00,0xff,0x99,0xe1,0xf2,0x90, /*'&', 6 */
    0x00,0x00,0x07,0x00,0x00,0x00, /*''', 7 */
    0x00,0x00,0xfe,0x01,0x01,0x00, /*'(', 8 */
    0x00,0x01,0x01,0xfe,0x00,0x00, /*')', 9 */
    0x04,0x68,0x1e,0x18,0x68,0x04, /*'*', 10 */
    0x00,0x10,0x7c,0x10,0x10,0x00, /*'+', 11 */
    0x00,0x00,0xc0,0x40,0x00,0x00, /*',', 12 */
    0x00,0x10,0x10,0x10,0x10,0x00, /*'-', 13 */
    0x00,0x00,0xc0,0xc0,0x00,0x00, /*'.', 14 */
    0x00,0x80,0x70,0x0e,0x01,0x00, /*'/', 15 */
    0x00,0x7e,0xb9,0x91,0x7e,0x00, /*'0', 16 */
    0x00,0x82,0x81,0xff,0x80,0x00, /*'1', 17 */
    0x00,0xc3,0xa1,0x91,0x8e,0x00, /*'2', 18 */
    0x00,0xc1,0x8d,0x8b,0x71,0x00, /*'3', 19 */
    0x00,0x30,0x2c,0x23,0xf1,0x00, /*'4', 20 */
    0x00,0xcf,0x89,0x89,0xf9,0x00, /*'5', 21 */
    0x00,0x78,0x8e,0x89,0x70,0x00, /*'6', 22 */
    0x00,0x03,0xc1,0x39,0x07,0x00, /*'7', 23 */
    0x00,0x77,0x89,0x89,0x77,0x00, /*'8', 24 */
    0x00,0x0e,0x91,0x71,0x1e,0x00, /*'9', 25 */
    0x00,0x00,0xcc,0xcc,0x00,0x00, /*':', 26 */
    0x00,0x00,0x8c,0x4c,0x00,0x00, /*';', 27 */
    0x00,0x30,0x28,0x48,0x44,0x00, /*'<', 28 */
    0x00,0x24,0x24,0x24,0x24,0x00, /*'=', 29 */
    0x00,0x44,0x48,0x28,0x30,0x00, /*'>', 30 */
    0x00,0x01,0xb1,0x11,0x0e,0x00, /*'?', 31 */
    0xfc,0x02,0x71,0x89,0x8b,0xfc, /*'@', 32 */
    0x80,0xf8,0x27,0x27,0xf8,0x80, /*'A', 33 */
    0x00,0xff,0x91,0x91,0xee,0x00, /*'B', 34 */
    0x00,0x7e,0x81,0x81,0xc3,0x00, /*'C', 35 */
    0x00,0xff,0x81,0x81,0x7e,0x00, /*'D', 36 */
    0x00,0xff,0x91,0x91,0x91,0x00, /*'E', 37 */
    0x00,0xff,0x11,0x11,0x11,0x00, /*'F', 38 */
    0x00,0x7e,0x81,0x91,0x72,0x00, /*'G', 39 */
    0x00,0xff,0x08,0x08,0xff,0x00, /*'H', 40 */
    0x00,0x81,0xff,0x81,0x81,0x00, /*'I', 41 */
    0x40,0xc1,0x81,0x81,0x7f,0x00, /*'J', 42 */
    0x00,0xff,0x08,0x1c,0xe3,0x81, /*'K', 43 */
    0x00,0xff,0x80,0x80,0x80,0x00, /*'L', 44 */
    0xff,0x06,0x08,0x06,0xff,0x00, /*'M', 45 */
    0x00,0xff,0x0e,0x70,0xff,0x00, /*'N', 46 */
    0x00,0x7e,0x81,0x81,0x7e,0x00, /*'O', 47 */
    0x00,0xff,0x11,0x11,0x1b,0x04, /*'P', 48 */
    0x00,0x7e,0x81,0x81,0xfe,0x00, /*'Q', 49 */
    0x00,0xff,0x11,0x31,0xdf,0x00, /*'R', 50 */
    0x00,0xce,0x89,0x91,0xf3,0x00, /*'S', 51 */
    0x01,0x01,0xff,0x01,0x01,0x01, /*'T', 52 */
    0x00,0x7f,0x80,0x80,0x7f,0x00, /*'U', 53 */
    0x01,0x1f,0xe0,0xe0,0x1f,0x01, /*'V', 54 */
    0x0f,0xf0,0x3e,0x3f,0xf0,0x0f, /*'W', 55 */
    0x80,0xc3,0x3c,0x3c,0xc3,0x80, /*'X', 56 */
    0x01,0x06,0xf8,0x18,0x06,0x01, /*'Y', 57 */
    0x00,0xc1,0xb1,0x8d,0x83,0x00, /*'Z', 58 */
    0x00,0x00,0xff,0x00,0x00,0x00, /*'[', 59 */
    0x00,0x01,0x0e,0x70,0x80,0x00, /*'\\', 60 */
    0x00,0x00,0x00,0xff,0x00,0x00, /*']', 61 */
    0x00,0x0c,0x03,0x03,0x0c,0x00, /*'^', 62 */
    0x00,0x00,0x00,0x00,0x00,0x00, /*'_', 63 */
    0x00,0x00,0x00,0x01,0x00,0x00, /*'`', 64 */
    0x00,0xf4,0x94,0x94,0xfc,0x00, /*'a', 65 */
    0x00,0xff,0x84,0x84,0xfc,0x00, /*'b', 66 */
    0x00,0x78,0x84,0x84,0xcc,0x00, /*'c', 67 */
    0x00,0xfc,0x84,0x84,0xff,0x00, /*'d', 68 */
    0x00,0xfc,0x94,0x94,0xdc,0x00, /*'e', 69 */
    0x00,0x08,0xfe,0x09,0x09,0x00, /*'f', 70 */
	0x00,0xcc,0x92,0x92,0x7c,0x00, /*'g', 71 */
    0x00,0xff,0x04,0x04,0xfc,0x00, /*'h', 72 */
    0x00,0x84,0x85,0xfd,0x80,0x80, /*'i', 73 */
    0x00,0x04,0x04,0xfd,0x01,0x00, /*'j', 74 */
    0x00,0xff,0x10,0x38,0xc4,0x84, /*'k', 75 */
    0x01,0x01,0x7f,0x80,0x80,0x80, /*'l', 76 */
    0x00,0xf8,0x08,0xf8,0x08,0xf8, /*'m', 77 */
    0x00,0xfc,0x04,0x04,0xfc,0x00, /*'n', 78 */
    0x00,0x78,0x84,0x84,0xfc,0x00, /*'o', 79 */
	0x00,0xfc,0x24,0x24,0x18,0x00, /*'p', 80 */
    0x00,0xfc,0x84,0x84,0xfc,0x00, /*'q', 81 */
    0x00,0xfc,0x04,0x04,0x1c,0x00, /*'r', 82 */
    0x00,0xdc,0x94,0xa4,0xe4,0x00, /*'s', 83 */
    0x00,0x04,0x7f,0x84,0x84,0x00, /*'t', 84 */
    0x00,0x7c,0x80,0x80,0x7c,0x00, /*'u', 85 */
    0x04,0x1c,0xe0,0xe0,0x1c,0x04, /*'v', 86 */
    0x1c,0xe0,0x3c,0x78,0xf0,0x0c, /*'w', 87 */
    0x80,0xc4,0x38,0x38,0xc4,0x80, /*'x', 88 */
    0x00,0x8e,0xf0,0x30,0x0e,0x00, /*'y', 89 */
    0x00,0xc4,0xa4,0x94,0x8c,0x00, /*'z', 90 */
    0x00,0x08,0x08,0xf7,0x00,0x00, /*'{', 91 */
    0x00,0x00,0x00,0xff,0x00,0x00, /*'|', 92 */
    0x00,0x00,0xf7,0x08,0x08,0x00, /*'}', 93 */
    0x00,0x18,0x08,0x10,0x18,0x00, /*'~', 94 */
    0xfc,0x04,0xfc,0x00,0x00,0x00, /*'', 95 */

};

const uint8_t cmds[] =
    {
        0x20, 0x00,       // 水平寻址模式
        0x21, 0x00, 0x7F, // 列地址范围: 0-127
        0x22, 0x00, 0x07  // 页地址范围: 0-7
};

// 初始化缓冲区
void OLED_InitBuffer(void)
{
    // 清空缓冲区
    memset(OLED_BackBuffer, 0, sizeof(OLED_BackBuffer));
    memset(OLED_FrontBuffer, 0, sizeof(OLED_FrontBuffer));

    // 初始化第一个字节为数据控制字节
    OLED_FrontBuffer[0] = 0x40; // 数据控制字节：Co=0, D/C#=1 (数据)

    // 初始化状态变量
    oled_update_flag = OLED_READY;
    oled_dma_busy = OLED_READY;

    // 将所有页面标记为脏（需要更新）
    for (uint8_t i = 0; i < OLED_PAGES; i++)
    {
        oled_dirty_pages[i] = 1;
    }
}

// 清空缓冲区
void OLED_ClearBuffer(void)
{
    // 重置缓冲区为全0 (全黑)
    memset(OLED_BackBuffer, 0, sizeof(OLED_BackBuffer));

    // 将所有页面标记为脏（需要更新）
    for (uint8_t i = 0; i < OLED_PAGES; i++)
    {
        oled_dirty_pages[i] = 1;
    }
}

uint8_t OLED_IsBusy(void)
{
    // 如果标记为忙，检查是否已经过了足够时间
    if (oled_update_flag)
    {
        // SSD1315/SSD1306 典型帧率约为60Hz，每帧约16.7ms
        // 可以根据需要调整刷新率，减小此值可以提高帧率
        uint32_t current_time = HAL_GetTick();
        if (current_time - oled_last_update_time >= 0)
        {
            oled_update_flag = OLED_READY; // 已经过了足够时间，不再忙
        }
    }

    // 如果DMA传输正在进行，也视为忙
    return (oled_update_flag || oled_dma_busy);
}

void OLED_UpdateDisplayVSync(void)
{
    // 等待上一次更新完成
    while (OLED_IsBusy())
    {
        // 可以加入短暂延时或者让出CPU，但通常不会停留很久
    }

    oled_update_flag = OLED_BUSY;
    oled_last_update_time = HAL_GetTick();

    OLED_FrontBuffer[0] = 0x40;                                             // 数据控制字节：Co=0, D/C#=1 (数据)
    memcpy(OLED_FrontBuffer + 1, OLED_BackBuffer, OLED_WIDTH * OLED_PAGES); // 复制当前缓冲区到前缓冲区

    // 一次性发送所有命令
    for (uint8_t i = 0; i < sizeof(cmds); i++)
    {
        OLED_SendCommand(cmds[i]);
    }


    HAL_I2C_Master_Transmit(&hi2c1, OLED_ADDR << 1, OLED_FrontBuffer, OLED_WIDTH * OLED_PAGES + 1, HAL_MAX_DELAY);

    // 重置所有脏页标记
    for (uint8_t i = 0; i < OLED_PAGES; i++)
    {
        oled_dirty_pages[i] = 0;
    }
}

// 使用DMA更新显示函数
void OLED_UpdateDisplayDMA(void)
{
    // 如果DMA忙或者OLED忙，则返回
    if (oled_dma_busy || oled_update_flag)
    {
        return;
    }

    oled_update_flag = OLED_BUSY;
    oled_dma_busy = OLED_BUSY;
    oled_last_update_time = HAL_GetTick();

    // 前缓冲区的第一个字节设为数据控制字节
    OLED_FrontBuffer[0] = 0x40;                                             // 数据控制字节：Co=0, D/C#=1 (数据)
    memcpy(OLED_FrontBuffer + 1, OLED_BackBuffer, OLED_WIDTH * OLED_PAGES); // 复制当前缓冲区到前缓冲区

    // 一次性发送所有命令
    for (uint8_t i = 0; i < sizeof(cmds); i++)
    {
        OLED_SendCommand(cmds[i]);
    }

    // 使用DMA传输数据（非阻塞）
    HAL_I2C_Master_Transmit_DMA(&hi2c1, OLED_ADDR << 1, OLED_FrontBuffer, OLED_WIDTH * OLED_PAGES + 1);

    // 重置所有脏页标记
    for (uint8_t i = 0; i < OLED_PAGES; i++)
    {
        oled_dirty_pages[i] = 0;
    }
}

// 局部更新显示函数
void OLED_UpdateDisplayPartial(uint8_t startPage, uint8_t endPage)
{
    // 边界检查
    if (startPage >= OLED_PAGES || endPage >= OLED_PAGES || startPage > endPage)
        return;

    // 等待上一次更新完成
    while (OLED_IsBusy())
    {
        // 可以加入短暂延时或者让出CPU
    }

    oled_update_flag = OLED_BUSY;
    oled_last_update_time = HAL_GetTick();

    // 设置页地址范围
    OLED_SendCommand(0x22);      // 页地址设置命令
    OLED_SendCommand(startPage); // 起始页
    OLED_SendCommand(endPage);   // 结束页

    // 设置列地址范围（总是更新整行）
    OLED_SendCommand(0x21); // 列地址设置命令
    OLED_SendCommand(0x00); // 起始列
    OLED_SendCommand(0x7F); // 结束列 (127)

    // 计算需要传输的数据长度
    uint16_t dataLen = OLED_WIDTH * (endPage - startPage + 1);

    // 前缓冲区的第一个字节设为数据控制字节
    OLED_FrontBuffer[0] = 0x40; // 数据控制字节：Co=0, D/C#=1 (数据)

    // 只复制需要更新的页
    for (uint8_t page = startPage; page <= endPage; page++)
    {
        memcpy(
            OLED_FrontBuffer + 1 + (page - startPage) * OLED_WIDTH,
            OLED_BackBuffer + page * OLED_WIDTH,
            OLED_WIDTH);
        oled_dirty_pages[page] = 0; // 重置脏页标记
    }

    // 发送数据
    HAL_I2C_Master_Transmit(&hi2c1, OLED_ADDR << 1, OLED_FrontBuffer, dataLen + 1, HAL_MAX_DELAY);
}

void OLED_WritePixel(int16_t x, int16_t y, uint8_t color)
{
    // 边界检查
    if (x >= OLED_WIDTH || x < 0 || y >= OLED_HEIGHT || y < 0)
        return;

    // 计算像素所在的字节位置
    uint16_t byte_index = x + (y / 8) * OLED_WIDTH;
    uint8_t bit_position = y % 8;
    uint8_t page = y / 8; // 计算所在页

    // 记录当前像素值
    uint8_t old_value = OLED_BackBuffer[byte_index];

    if (color) // 如果需要点亮像素
        OLED_BackBuffer[byte_index] |= (1 << bit_position);
    else // 如果需要熄灭像素
        OLED_BackBuffer[byte_index] &= ~(1 << bit_position);

    // 如果像素值发生变化，标记该页为脏
    if (old_value != OLED_BackBuffer[byte_index])
    {
        oled_dirty_pages[page] = 1;
    }
}

void OLED_SendCommand(uint8_t command)
{
    uint8_t data[2];
    data[0] = 0x00; // Co = 0, D/C# = 0
    data[1] = command;
    HAL_I2C_Master_Transmit(&hi2c1, OLED_ADDR << 1, data, 2, HAL_MAX_DELAY);
}

void OLED_SendData(uint8_t data)
{
    uint8_t buffer[2];
    buffer[0] = 0x40; // Co = 0, D/C# = 1 (数据)
    buffer[1] = data;
    HAL_I2C_Master_Transmit(&hi2c1, OLED_ADDR << 1, buffer, 2, HAL_MAX_DELAY);
}

void OLED_ClearDisplay(void)
{
    OLED_ClearBuffer();
    OLED_UpdateDisplayVSync();
}

// 更新OLED初始化函数来使用双缓冲
void OLED_Init()
{
    HAL_Delay(100); // 等待OLED上电稳定

    // 初始化命令序列
    OLED_SendCommand(0xAE); // 关闭显示
    OLED_SendCommand(0xD5); // 设置显示时钟分频比/振荡器频率
    OLED_SendCommand(0x80); // 推荐值
    OLED_SendCommand(0xA8); // 设置多路复用比
    OLED_SendCommand(0x3F); // 默认 0x3F (1/64 duty)
    OLED_SendCommand(0xD3); // 设置显示偏移
    OLED_SendCommand(0x00); // 无偏移
    OLED_SendCommand(0x40); // 设置显示开始行 (0x40-0x7F)
    OLED_SendCommand(0x8D); // 电荷泵设置
    OLED_SendCommand(0x14); // 启用电荷泵
    OLED_SendCommand(0x20); // 设置内存寻址模式
    OLED_SendCommand(0x00); // 水平寻址模式
    OLED_SendCommand(0xA1); // 段重映射，列地址127映射到SEG0
    OLED_SendCommand(0xC8); // COM输出扫描方向，从COM[N-1]到COM0
    OLED_SendCommand(0xDA); // COM硬件配置
    OLED_SendCommand(0x12); // 交替COM配置，使能COM左/右重映射
    OLED_SendCommand(0x81); // 设置对比度控制
    OLED_SendCommand(0xCF); // 对比度值(0-255)
    OLED_SendCommand(0xD9); // 设置预充电周期
    OLED_SendCommand(0xF1); // 第1和第2阶段的预充电周期
    OLED_SendCommand(0xDB); // 设置VCOMH解除选择电平
    OLED_SendCommand(0x30); // 约0.83xVcc
    OLED_SendCommand(0xA4); // 显示RAM内容
    OLED_SendCommand(0xA6); // 正常显示(非反显)
    OLED_SendCommand(0x2E); // 禁用滚动

    // 初始化缓冲区
    OLED_InitBuffer();

    // 清屏
    OLED_ClearBuffer();

    // 开启显示
    OLED_SendCommand(0xAF);
}

// 区域反色功能 - 将指定矩形区域内的像素颜色反转 //! UPDATEDISPLAY REQUIRED
void OLED_InvertArea(int16_t x, int16_t y, uint8_t width, uint8_t height)
{
    if (width == 0 || height == 0)
        return;

    int16_t x_start_on_screen = x;
    int16_t y_start_on_screen = y;
    int16_t x_end_on_screen = x + width - 1;
    int16_t y_end_on_screen = y + height - 1;

    // Clip to screen boundaries
    if (x_start_on_screen < 0)
        x_start_on_screen = 0;
    if (y_start_on_screen < 0)
        y_start_on_screen = 0;
    if (x_end_on_screen >= OLED_WIDTH)
        x_end_on_screen = OLED_WIDTH - 1;
    if (y_end_on_screen >= OLED_HEIGHT)
        y_end_on_screen = OLED_HEIGHT - 1;

    // If clipped area is invalid (e.g., entirely off-screen after clipping)
    if (x_start_on_screen > x_end_on_screen || y_start_on_screen > y_end_on_screen)
        return;

    // 计算涉及的页范围
    uint8_t start_page = y_start_on_screen / 8;
    uint8_t end_page = y_end_on_screen / 8;

    // 标记受影响的页为脏页
    extern volatile uint8_t oled_dirty_pages[];
    for (uint8_t page = start_page; page <= end_page; page++)
    {
        oled_dirty_pages[page] = 1;
    }

    // 优化的像素更新逻辑 - 按行处理以提高缓存命中率
    for (int16_t j = y_start_on_screen; j <= y_end_on_screen; j++)
    {
        uint8_t page = (uint8_t)j / 8;
        uint8_t bit_position = (uint8_t)j % 8;
        uint16_t byte_index_base = page * OLED_WIDTH;

        // 处理当前行上的所有像素
        for (int16_t i = x_start_on_screen; i <= x_end_on_screen; i++)
        {
            uint16_t byte_index = byte_index_base + i;

            // 反转位
            if (byte_index < sizeof(OLED_BackBuffer))
            {
                OLED_BackBuffer[byte_index] ^= (1 << bit_position);
            }
        }
    }
}

void OLED_SetCursor(int16_t x, int16_t y)
{
    OLED_SendCommand(0xB0 + y);                 // 设置页地址 (0-7)
    OLED_SendCommand(0x00 + (x & 0x0F));        // 设置低4位列地址
    OLED_SendCommand(0x10 + ((x >> 4) & 0x0F)); // 设置高4位列地址
}

void OLED_DisplayChar(int16_t x, int16_t y, char ch) //! UPDATEDISPLAY REQUIRED
{
    const uint8_t font_width = 6;
    const uint8_t font_height = 8;

    if (ch < 32)
        return;

    uint8_t c = ch - 32;

    uint8_t start_page = (y >= 0) ? (y / 8) : 0;
    uint8_t end_page = ((y + font_height - 1) < OLED_HEIGHT) ? ((y + font_height - 1) / 8) : (OLED_HEIGHT / 8 - 1);

    extern volatile uint8_t oled_dirty_pages[];
    for (uint8_t page = start_page; page <= end_page && page < OLED_PAGES; page++)
    {
        oled_dirty_pages[page] = 1;
    }
    // 按列渲染
    for (uint8_t char_col = 0; char_col < font_width; char_col++)
    {
        int16_t screen_x = x + char_col;
        if (screen_x < 0 || screen_x >= OLED_WIDTH)
            continue;

        if ((c * font_width + char_col) >= sizeof(OLED_FONT_6x8))
            break;
        uint8_t font_data_col = OLED_FONT_6x8[c * font_width + char_col];

        for (uint8_t char_row_bit = 0; char_row_bit < font_height; char_row_bit++)
        {
            int16_t screen_y = y + char_row_bit;
            if (screen_y < 0 || screen_y >= OLED_HEIGHT)
                continue;

            if ((font_data_col >> char_row_bit) & 0x01)
            {
                uint8_t page = (uint8_t)screen_y / 8;
                uint8_t bit_offset_in_page = (uint8_t)screen_y % 8;
                uint16_t buffer_index = (uint16_t)screen_x + page * OLED_WIDTH;

                if (buffer_index < sizeof(OLED_BackBuffer))
                {
                    OLED_BackBuffer[buffer_index] |= (1 << bit_offset_in_page);
                }
            }
        }
    }
}

void OLED_DisplayCharInverted(int16_t x, int16_t y, char ch, uint8_t inverted) //! UPDATEDISPLAY REQUIRED
{
    const uint8_t font_width = 6;
    const uint8_t font_height = 8;

    if (ch < 32)
        return;

    uint8_t c = ch - 32;

    for (uint8_t char_col = 0; char_col < font_width; char_col++)
    {
        int16_t screen_x = x + char_col;
        if (screen_x < 0 || screen_x >= OLED_WIDTH)
            continue;

        if ((c * font_width + char_col) >= sizeof(OLED_FONT_6x8))
            break;
        uint8_t font_data_col = OLED_FONT_6x8[c * font_width + char_col];

        if (inverted)
        {
            font_data_col = ~font_data_col;
        }

        for (uint8_t char_row_bit = 0; char_row_bit < font_height; char_row_bit++)
        {
            int16_t screen_y = y + char_row_bit;
            if (screen_y < 0 || screen_y >= OLED_HEIGHT)
                continue;

            uint8_t page = (uint8_t)screen_y / 8;
            uint8_t bit_offset_in_page = (uint8_t)screen_y % 8;
            uint16_t buffer_index = (uint16_t)screen_x + page * OLED_WIDTH;

            if (buffer_index < sizeof(OLED_BackBuffer))
            {
                if ((font_data_col >> char_row_bit) & 0x01)
                {
                    OLED_BackBuffer[buffer_index] |= (1 << bit_offset_in_page);
                }
                else
                {
                    OLED_BackBuffer[buffer_index] &= ~(1 << bit_offset_in_page);
                }
            }
        }
    }
}

void OLED_DisplayString(int16_t x, int16_t y, char *str) //! UPDATEDISPLAY REQUIRED
{
    uint8_t j = 0;
    const uint8_t font_width = 6;
    const uint8_t font_height = 8;

    if (y >= OLED_HEIGHT || (y + font_height - 1) < 0)
    {
        return;
    }

    int16_t current_char_x;
    while (str[j] != '\0')
    {
        current_char_x = x + (j * font_width);
        if (current_char_x >= OLED_WIDTH)
        {
            break;
        }
        if ((current_char_x + font_width - 1) < 0)
        {
            j++;
            continue;
        }
        OLED_DisplayChar(current_char_x, y, str[j]);
        j++;
    }
}

void OLED_DisplayStringInverted(int16_t x, int16_t y, char *str, uint8_t inverted) //! UPDATEDISPLAY REQUIRED
{
    uint8_t j = 0;
    const uint8_t font_width = 6;
    const uint8_t font_height = 8;

    if (y >= OLED_HEIGHT || (y + font_height - 1) < 0)
    {
        return;
    }

    int16_t current_char_x;
    while (str[j] != '\0')
    {
        current_char_x = x + (j * font_width);
        if (current_char_x >= OLED_WIDTH)
        {
            break;
        }
        if ((current_char_x + font_width - 1) < 0)
        {
            j++;
            continue;
        }
        OLED_DisplayCharInverted(current_char_x, y, str[j], inverted);
        j++;
    }
}

void OLED_DisplayInteger(int16_t x, int16_t y, int number) //! UPDATEDISPLAY REQUIRED
{
    char str[12];
    sprintf(str, "%d", number);
    OLED_DisplayString(x, y, str);
}

void OLED_DisplayFloat(int16_t x, int16_t y, float number) //! UPDATEDISPLAY REQUIRED
{
    char str[32];
    int intPart = (int)number;
    // 保留两位小数
    int fracPart = (int)((number - intPart) * 100);

    if (fracPart < 0)
        fracPart = -fracPart;

    // 如果小数部分是一位数，则前面补零
    if (fracPart < 10)
    {
        sprintf(str, "%d.0%d", intPart, fracPart);
    }
    else
    {
        sprintf(str, "%d.%d", intPart, fracPart);
    }

    OLED_DisplayString(x, y, str);
}
