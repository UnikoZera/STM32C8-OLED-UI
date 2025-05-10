/*
 * oled.h
 *
 *  Created on: May 3, 2025
 *      Author: UnikoZera
 */

#ifndef INC_OLED_H_
#define INC_OLED_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "i2c.h"

// 屏幕尺寸常量定义
#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define OLED_PAGES (OLED_HEIGHT / 8)

// 双缓冲相关函数
void OLED_InitBuffer(void);
void OLED_ClearBuffer(void);
void OLED_UpdateDisplayVSync(void); // 更新显示，使用垂直同步
void OLED_WritePixel(int16_t x, int16_t y, uint8_t color); // 写单个像素到缓冲区

// 原始底层函数
void OLED_SendCommand(uint8_t command);
void OLED_Init(void);
void OLED_SendData(uint8_t data);
void OLED_ClearDisplay(void);

// 绘图函数
void OLED_SetCursor(int16_t x, int16_t y);
void OLED_DisplayChar(int16_t x, int16_t y, char ch);
void OLED_DisplayString(int16_t x, int16_t y, char *str);
void OLED_DisplayInteger(int16_t x, int16_t y, int num);
void OLED_DisplayFloat(int16_t x, int16_t y, float number);
void OLED_DisplayFPS();
// 反色文本绘制功能
void OLED_DisplayCharInverted(int16_t x, int16_t y, char ch, uint8_t inverted);
void OLED_DisplayStringInverted(int16_t x, int16_t y, char *str, uint8_t inverted);

#endif /* INC_OLED_H_ */
