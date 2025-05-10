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
#include "oled_optimize.h"

// 屏幕尺寸常量定义
#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define OLED_PAGES (OLED_HEIGHT / 8)

// 更新方式
#define OLED_UPDATE_NORMAL   0 // 普通更新
#define OLED_UPDATE_DMA      1 // DMA更新
#define OLED_UPDATE_PARTIAL  2 // 局部更新
#define OLED_ADDR 0x3C // I2C for SSD1315

// 状态标志
#define OLED_BUSY  1
#define OLED_READY 0

// 外部变量声明
extern volatile uint32_t oled_last_update_time;  // 上次更新显示的时间
extern volatile uint8_t oled_update_flag;        // 更新标志位
extern volatile uint8_t oled_dma_busy;           // DMA传输忙标志
extern volatile uint8_t oled_dirty_pages[];      // 标记哪些页需要更新
extern uint8_t OLED_BackBuffer[];               // 显示后台缓冲区
extern uint8_t OLED_FrontBuffer[];              // 显示前台缓冲区
extern const uint8_t cmds[];                    // OLED命令数组

// 双缓冲相关函数
void OLED_InitBuffer(void);
void OLED_ClearBuffer(void);
void OLED_UpdateDisplayVSync(void); // 更新显示，使用垂直同步
void OLED_UpdateDisplayDMA(void);   // 使用DMA更新显示
void OLED_UpdateDisplayPartial(uint8_t startPage, uint8_t endPage); // 局部更新显示
void OLED_WritePixel(int16_t x, int16_t y, uint8_t color); // 写单个像素到缓冲区

// 状态查询
uint8_t OLED_IsBusy(void); // 检查OLED是否忙

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

// 反色文本绘制功能
void OLED_DisplayCharInverted(int16_t x, int16_t y, char ch, uint8_t inverted);
void OLED_DisplayStringInverted(int16_t x, int16_t y, char *str, uint8_t inverted);

#endif /* INC_OLED_H_ */
