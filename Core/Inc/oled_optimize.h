/*
 * oled_optimize.h
 *
 *  Created on: May 11, 2025
 *      Author: UnikoZera & GitHub Copilot
 */

#ifndef INC_OLED_OPTIMIZE_H_
#define INC_OLED_OPTIMIZE_H_

#include <stdint.h>

// 智能更新函数声明
void OLED_SmartUpdate(void);

// FPS显示函数
void OLED_OptimizedDisplayFPS(int16_t x, int16_t y);

// 差分更新设置
void OLED_EnableDiffMode(uint8_t enable);
void OLED_EnableFastUpdate(uint8_t enable);

#endif /* INC_OLED_OPTIMIZE_H_ */
