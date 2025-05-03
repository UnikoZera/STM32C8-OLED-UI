/*
 * oled.c
 *
 *  Created on: May 3, 2025
 *      Author: UnikoZera
 */

#include "oled.h"
#define OLED_ADDR 0x3C // I2C for SSD1315

static volatile uint32_t oled_last_update_time = 0; // 上次更新显示的时间
static volatile uint8_t oled_update_flag = 0;       // 更新标志位

// 128 (宽) x 8 (页) = 1024 字节
uint8_t OLED_BackBuffer[128 * 8];
uint8_t OLED_FrontBuffer[128 * 8];

// ASCII 6x8 字体数组 (32-127字符)
const uint8_t OLED_FONT_6x8[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 空格 (32)
    0x00, 0x00, 0x5F, 0x00, 0x00, 0x00, // !
    0x00, 0x07, 0x00, 0x07, 0x00, 0x00, // "
    0x14, 0x7F, 0x14, 0x7F, 0x14, 0x00, // #
    0x24, 0x2A, 0x7F, 0x2A, 0x12, 0x00, // $
    0x23, 0x13, 0x08, 0x64, 0x62, 0x00, // %
    0x36, 0x49, 0x56, 0x20, 0x50, 0x00, // &
    0x00, 0x08, 0x07, 0x03, 0x00, 0x00, // '
    0x00, 0x1C, 0x22, 0x41, 0x00, 0x00, // (
    0x00, 0x41, 0x22, 0x1C, 0x00, 0x00, // )
    0x14, 0x08, 0x3E, 0x08, 0x14, 0x00, // *
    0x08, 0x08, 0x3E, 0x08, 0x08, 0x00, // +
    0x00, 0x50, 0x30, 0x00, 0x00, 0x00, // ,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x00, // -
    0x00, 0x60, 0x60, 0x00, 0x00, 0x00, // .
    0x20, 0x10, 0x08, 0x04, 0x02, 0x00, // /
    0x3E, 0x51, 0x49, 0x45, 0x3E, 0x00, // 0 (48)
    0x00, 0x42, 0x7F, 0x40, 0x00, 0x00, // 1
    0x42, 0x61, 0x51, 0x49, 0x46, 0x00, // 2
    0x21, 0x41, 0x45, 0x4B, 0x31, 0x00, // 3
    0x18, 0x14, 0x12, 0x7F, 0x10, 0x00, // 4
    0x27, 0x45, 0x45, 0x45, 0x39, 0x00, // 5
    0x3C, 0x4A, 0x49, 0x49, 0x30, 0x00, // 6
    0x01, 0x71, 0x09, 0x05, 0x03, 0x00, // 7
    0x36, 0x49, 0x49, 0x49, 0x36, 0x00, // 8
    0x06, 0x49, 0x49, 0x29, 0x1E, 0x00, // 9
    0x00, 0x36, 0x36, 0x00, 0x00, 0x00, // :
    0x00, 0x56, 0x36, 0x00, 0x00, 0x00, // ;
    0x08, 0x14, 0x22, 0x41, 0x00, 0x00, // <
    0x14, 0x14, 0x14, 0x14, 0x14, 0x00, // =
    0x00, 0x41, 0x22, 0x14, 0x08, 0x00, // >
    0x02, 0x01, 0x51, 0x09, 0x06, 0x00, // ?
    0x32, 0x49, 0x79, 0x41, 0x3E, 0x00, // @
    0x7E, 0x11, 0x11, 0x11, 0x7E, 0x00, // A (65)
    0x7F, 0x49, 0x49, 0x49, 0x36, 0x00, // B
    0x3E, 0x41, 0x41, 0x41, 0x22, 0x00, // C
    0x7F, 0x41, 0x41, 0x22, 0x1C, 0x00, // D
    0x7F, 0x49, 0x49, 0x49, 0x41, 0x00, // E
    0x7F, 0x09, 0x09, 0x09, 0x01, 0x00, // F
    0x3E, 0x41, 0x49, 0x49, 0x7A, 0x00, // G
    0x7F, 0x08, 0x08, 0x08, 0x7F, 0x00, // H
    0x00, 0x41, 0x7F, 0x41, 0x00, 0x00, // I
    0x20, 0x40, 0x41, 0x3F, 0x01, 0x00, // J
    0x7F, 0x08, 0x14, 0x22, 0x41, 0x00, // K
    0x7F, 0x40, 0x40, 0x40, 0x40, 0x00, // L
    0x7F, 0x02, 0x0C, 0x02, 0x7F, 0x00, // M
    0x7F, 0x04, 0x08, 0x10, 0x7F, 0x00, // N
    0x3E, 0x41, 0x41, 0x41, 0x3E, 0x00, // O
    0x7F, 0x09, 0x09, 0x09, 0x06, 0x00, // P
    0x3E, 0x41, 0x51, 0x21, 0x5E, 0x00, // Q
    0x7F, 0x09, 0x19, 0x29, 0x46, 0x00, // R
    0x46, 0x49, 0x49, 0x49, 0x31, 0x00, // S (83)
    0x01, 0x01, 0x7F, 0x01, 0x01, 0x00, // T
    0x3F, 0x40, 0x40, 0x40, 0x3F, 0x00, // U
    0x1F, 0x20, 0x40, 0x20, 0x1F, 0x00, // V
    0x3F, 0x40, 0x38, 0x40, 0x3F, 0x00, // W
    0x63, 0x14, 0x08, 0x14, 0x63, 0x00, // X
    0x07, 0x08, 0x70, 0x08, 0x07, 0x00, // Y
    0x61, 0x51, 0x49, 0x45, 0x43, 0x00, // Z (90)
    0x00, 0x7F, 0x41, 0x41, 0x00, 0x00, // [
    0x02, 0x04, 0x08, 0x10, 0x20, 0x00, /* \ */
    0x00, 0x41, 0x41, 0x7F, 0x00, 0x00, // ]
    0x04, 0x02, 0x01, 0x02, 0x04, 0x00, // ^
    0x40, 0x40, 0x40, 0x40, 0x40, 0x00, // _
    0x00, 0x01, 0x02, 0x04, 0x00, 0x00, // `
    0x20, 0x54, 0x54, 0x54, 0x78, 0x00, // a (97)
    0x7F, 0x48, 0x44, 0x44, 0x38, 0x00, // b
    0x38, 0x44, 0x44, 0x44, 0x20, 0x00, // c
    0x38, 0x44, 0x44, 0x48, 0x7F, 0x00, // d
    0x38, 0x54, 0x54, 0x54, 0x18, 0x00, // e
    0x08, 0x7E, 0x09, 0x01, 0x02, 0x00, // f
    0x06, 0x49, 0x49, 0x49, 0x3F, 0x00, // g
    0x7F, 0x08, 0x04, 0x04, 0x78, 0x00, // h
    0x00, 0x44, 0x7D, 0x40, 0x00, 0x00, // i
    0x20, 0x40, 0x44, 0x3D, 0x00, 0x00, // j
    0x7F, 0x10, 0x28, 0x44, 0x00, 0x00, // k
    0x00, 0x41, 0x7F, 0x40, 0x00, 0x00, // l
    0x7C, 0x04, 0x18, 0x04, 0x78, 0x00, // m
    0x7C, 0x08, 0x04, 0x04, 0x78, 0x00, // n
    0x38, 0x44, 0x44, 0x44, 0x38, 0x00, // o
    0x7C, 0x14, 0x14, 0x14, 0x08, 0x00, // p
    0x08, 0x14, 0x14, 0x18, 0x7C, 0x00, // q
    0x7C, 0x08, 0x04, 0x04, 0x08, 0x00, // r
    0x48, 0x54, 0x54, 0x54, 0x20, 0x00, // s
    0x04, 0x3F, 0x44, 0x40, 0x20, 0x00, // t
    0x3C, 0x40, 0x40, 0x20, 0x7C, 0x00, // u
    0x1C, 0x20, 0x40, 0x20, 0x1C, 0x00, // v
    0x3C, 0x40, 0x30, 0x40, 0x3C, 0x00, // w
    0x44, 0x28, 0x10, 0x28, 0x44, 0x00, // x
    0x0C, 0x50, 0x50, 0x50, 0x3C, 0x00, // y
    0x44, 0x64, 0x54, 0x4C, 0x44, 0x00  // z (122)
};

// 初始化缓冲区
void OLED_InitBuffer(void)
{
    // 清空缓冲区
    memset(OLED_BackBuffer, 0, sizeof(OLED_BackBuffer));
    memset(OLED_FrontBuffer, 0, sizeof(OLED_FrontBuffer));
}

// 清空缓冲区
void OLED_ClearBuffer(void)
{
    // 重置缓冲区为全0 (全黑)
    memset(OLED_BackBuffer, 0, sizeof(OLED_BackBuffer));
}

uint8_t OLED_IsBusy(void)
{
    // 如果标记为忙，检查是否已经过了足够时间
    if (oled_update_flag)
    {
        // SSD1315/SSD1306 典型帧率约为60Hz，每帧约16.7ms
        uint32_t current_time = HAL_GetTick();
        if (current_time - oled_last_update_time >= 30)
        {
            oled_update_flag = 0; // 已经过了足够时间，不再忙
        }
    }
    return oled_update_flag;
}

void OLED_UpdateDisplayVSync(void)
{
    while (OLED_IsBusy()) 
    {

    }
    oled_update_flag = 1;
    oled_last_update_time = HAL_GetTick();

    // 交换前后缓冲区
    memcpy(OLED_FrontBuffer, OLED_BackBuffer, 128 * 8); // 复制当前缓冲区到前缓冲区

    // 将前缓冲区发送到显示器
    uint8_t data[129]; // 数据缓冲区 (包括控制字节)
    data[0] = 0x40;    // 数据控制字节

    // 逐页发送数据，每页一次性发送整行
    for (uint8_t page = 0; page < 8; page++)
    {
        // 设置页地址
        OLED_SendCommand(0xB0 + page);
        // 设置列起始地址
        OLED_SendCommand(0x00); // 低位地址
        OLED_SendCommand(0x10); // 高位地址

        // 复制当前页到发送缓冲区
        memcpy(data + 1, &OLED_FrontBuffer[page * OLED_WIDTH], OLED_WIDTH);

        // 发送一整行数据
        HAL_I2C_Master_Transmit(&hi2c1, OLED_ADDR << 1, data, OLED_WIDTH + 1, HAL_MAX_DELAY);
    }
}

void OLED_WritePixel(uint8_t x, uint8_t y, uint8_t color)
{
    // 边界检查
    if (x >= 128 || y >= 64)
        return;

    // 计算像素所在的字节位置
    uint16_t byte_index = x + (y / 8) * 128;
    uint8_t bit_position = y % 8;

    if (color) // 如果需要点亮像素
        OLED_BackBuffer[byte_index] |= (1 << bit_position);
    else // 如果需要熄灭像素
        OLED_BackBuffer[byte_index] &= ~(1 << bit_position);
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
void OLED_InvertArea(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
    // 边界检查和裁剪
    if (x >= OLED_WIDTH || y >= OLED_HEIGHT || width == 0 || height == 0)
        return;
    if (x + width > OLED_WIDTH)
        width = OLED_WIDTH - x;
    if (y + height > OLED_HEIGHT)
        height = OLED_HEIGHT - y;

    uint8_t end_x = x + width - 1;
    uint8_t end_y = y + height - 1;

    // 遍历区域内的每个像素并反转
    for (uint8_t j = y; j <= end_y; j++)
    {
        for (uint8_t i = x; i <= end_x; i++)
        {
            // 计算当前像素在缓冲区中的位置
            uint16_t byte_index = i + (j / 8) * OLED_WIDTH;
            uint8_t bit_position = j % 8;

            // 反转该像素（将0变为1，1变为0）
            OLED_BackBuffer[byte_index] ^= (1 << bit_position);
        }
    }
}

// 设置光标位置
void OLED_SetCursor(uint8_t x, uint8_t y)
{
    OLED_SendCommand(0xB0 + y);                 // 设置页地址 (0-7)
    OLED_SendCommand(0x00 + (x & 0x0F));        // 设置低4位列地址
    OLED_SendCommand(0x10 + ((x >> 4) & 0x0F)); // 设置高4位列地址
}

void OLED_DisplayChar(uint8_t x, uint8_t y, char ch) //! UPDATEDISPLAY REQUIRED
{
    // 边界检查
    if (x >= OLED_WIDTH || y >= OLED_HEIGHT || ch < 32)
        return;

    uint8_t c = ch - 32;    // 获取字体数组中的索引
    uint8_t font_width = 6; // 6x8字体宽度

    // 防止越界
    if (x + font_width > OLED_WIDTH)
    {
        font_width = OLED_WIDTH - x;
    }

    // 获取字符所在页
    uint8_t page = y / 8;
    uint8_t bit_offset = y % 8;

    // 将字体数据写入缓冲区
    for (uint8_t i = 0; i < font_width; i++)
    {
        if ((c * 6 + i) >= sizeof(OLED_FONT_6x8))
            break; // 安全检查

        uint8_t font_data = OLED_FONT_6x8[c * 6 + i];

        // 如果字符跨页，需要分别处理上下两页
        uint16_t buffer_index = x + i + (page * OLED_WIDTH);

        if (bit_offset == 0)
        {
            // 字符刚好在页的起始位置，直接写入
            OLED_BackBuffer[buffer_index] = font_data;
        }
        else
        {
            // 字符跨页，分别处理上下两页
            // 当前页
            OLED_BackBuffer[buffer_index] |= font_data << bit_offset;

            // 下一页（如果有）
            if (page < OLED_PAGES - 1)
            {
                OLED_BackBuffer[buffer_index + OLED_WIDTH] |= font_data >> (8 - bit_offset);
            }
        }
    }
}

void OLED_DisplayCharInverted(uint8_t x, uint8_t y, char ch, uint8_t inverted) //! UPDATEDISPLAY REQUIRED
{
    // 边界检查
    if (x >= OLED_WIDTH || y >= OLED_HEIGHT || ch < 32)
        return;

    uint8_t c = ch - 32;    // 获取字体数组中的索引
    uint8_t font_width = 6; // 6x8字体宽度

    // 防止越界
    if (x + font_width > OLED_WIDTH)
    {
        font_width = OLED_WIDTH - x;
    }

    // 获取字符所在页
    uint8_t page = y / 8;
    uint8_t bit_offset = y % 8;

    // 将字体数据写入缓冲区
    for (uint8_t i = 0; i < font_width; i++)
    {
        if ((c * 6 + i) >= sizeof(OLED_FONT_6x8))
            break; // 安全检查

        uint8_t font_data = OLED_FONT_6x8[c * 6 + i];
        // 如果需要反色显示，对字体数据取反
        if (inverted)
        {
            font_data = ~font_data;
        }

        // 如果字符跨页，需要分别处理上下两页
        uint16_t buffer_index = x + i + (page * OLED_WIDTH);

        if (bit_offset == 0)
        {
            // 字符刚好在页的起始位置，直接写入
            OLED_BackBuffer[buffer_index] = font_data;
        }
        else
        {
            // 字符跨页，分别处理上下两页
            // 当前页 - 先清除当前位置，再写入
            OLED_BackBuffer[buffer_index] &= ~(0xFF << bit_offset);
            OLED_BackBuffer[buffer_index] |= font_data << bit_offset;

            // 下一页（如果有） - 先清除当前位置，再写入
            if (page < OLED_PAGES - 1)
            {
                OLED_BackBuffer[buffer_index + OLED_WIDTH] &= ~(0xFF >> (8 - bit_offset));
                OLED_BackBuffer[buffer_index + OLED_WIDTH] |= font_data >> (8 - bit_offset);
            }
        }
    }
}

void OLED_DisplayString(uint8_t x, uint8_t y, char *str) //! UPDATEDISPLAY REQUIRED
{
    uint8_t j = 0;
    uint8_t font_width = 6; // 6x8字体宽度

    while (str[j] != '\0')
    {
        uint8_t current_x = x + (j * font_width);
        // 如果字符起始位置超出屏幕，则停止
        if (current_x >= OLED_WIDTH)
            break;
        OLED_DisplayChar(current_x, y, str[j]);
        j++;
    }
}

void OLED_DisplayStringInverted(uint8_t x, uint8_t y, char *str, uint8_t inverted) //! UPDATEDISPLAY REQUIRED
{
    uint8_t j = 0;
    uint8_t font_width = 6; // 6x8字体宽度

    while (str[j] != '\0')
    {
        uint8_t current_x = x + (j * font_width);
        // 如果字符起始位置超出屏幕，则停止
        if (current_x >= OLED_WIDTH)
            break;
        OLED_DisplayCharInverted(current_x, y, str[j], inverted);
        j++;
    }
}

// 显示整数 (使用双缓冲)
void OLED_DisplayInteger(uint8_t x, uint8_t y, int number) //! UPDATEDISPLAY REQUIRED
{
    char str[12];
    sprintf(str, "%d", number);
    OLED_DisplayString(x, y, str);
}

// 显示浮点数 (使用双缓冲)
void OLED_DisplayFloat(uint8_t x, uint8_t y, float number) //! UPDATEDISPLAY REQUIRED
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
