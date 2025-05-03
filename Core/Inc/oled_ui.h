/*
 * oled_ui.h
 *
 *  Created on: May 3, 2025
 *      Author: UnikoZera
 */

#ifndef INC_OLED_UI_H_
#define INC_OLED_UI_H_

#include "oled.h"
#define MAX_ANIMATIONS 10
typedef enum {
    EASE_LINEAR,      // 线性缓动
    EASE_IN_QUAD,     // 二次方缓入
    EASE_OUT_QUAD,    // 二次方缓出
    EASE_INOUT_QUAD,  // 二次方缓入缓出
    EASE_IN_CUBIC,    // 三次方缓入
    EASE_OUT_CUBIC,   // 三次方缓出
    EASE_INOUT_CUBIC, // 三次方缓入缓出
    EASE_IN_EXPO,     // 指数缓入
    EASE_OUT_EXPO,    // 指数缓出
    EASE_INOUT_EXPO,  // 指数缓入缓出
    EASE_IN_CIRC,     // 圆形曲线缓入
    EASE_OUT_CIRC,    // 圆形曲线缓出
    EASE_INOUT_CIRC,  // 圆形曲线缓入缓出
    EASE_IN_ELASTIC,  // 弹性缓入
    EASE_OUT_ELASTIC, // 弹性缓出
    EASE_INOUT_ELASTIC,// 弹性缓入缓出
    EASE_IN_BOUNCE,   // 反弹缓入
    EASE_OUT_BOUNCE,  // 反弹缓出
    EASE_INOUT_BOUNCE,// 反弹缓入缓出
    EASE_IN_BACK,     // 背越式缓入
    EASE_OUT_BACK,    // 背越式缓出
    EASE_INOUT_BACK,  // 背越式缓入缓出
    EASE_IN_SINE,     // 正弦缓入
    EASE_OUT_SINE,    // 正弦缓出
    EASE_INOUT_SINE   // 正弦缓入缓出
} EaseType_t;

typedef struct
{
    uint32_t startTime;  // 开始时间(毫秒)
    uint32_t duration;   // 持续时间(毫秒)
    float startValue;    // 起始值
    float endValue;      // 结束值
    float currentValue;  // 当前值
    uint8_t isActive;    // 动画是否活跃
    EaseType_t easeType; // 缓动类型
} Animation_t;

typedef struct
{
    Animation_t animations[MAX_ANIMATIONS];
    uint8_t count;
} AnimationManager_t;

void OLED_InitAnimation(Animation_t *anim, float startValue, float endValue,
                         uint32_t duration, EaseType_t easeType);
uint8_t OLED_UpdateAnimation(Animation_t *anim, uint32_t currentTime);
float OLED_GetAnimationValue(Animation_t *anim);
void InitBlockAnimation(void);
void UpdateAndDrawBlock(void);
void AnimationLoop(void);

// UI绘图函数
void OLED_DrawTitleBar(char *title);
void OLED_DrawBattery(uint8_t x, uint8_t y, uint8_t percentage);
void OLED_DrawProgressBar(uint8_t x, uint8_t y, uint8_t width, uint8_t percent);
void OLED_DrawRectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height);
void OLED_DrawFilledRectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color);
void OLED_DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
void OLED_InvertArea(uint8_t x, uint8_t y, uint8_t width, uint8_t height);


#endif /* INC_OLED_UI_H_ */
