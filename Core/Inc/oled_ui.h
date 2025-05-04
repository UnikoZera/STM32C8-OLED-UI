/*
 * oled_ui.h
 *
 *  Created on: May 3, 2025
 *      Author: UnikoZera
 */

#ifndef INC_OLED_UI_H_
#define INC_OLED_UI_H_

#include "oled.h"
typedef enum
{
    EASE_LINEAR,        // 线性缓动
    EASE_IN_QUAD,       // 二次方缓入
    EASE_OUT_QUAD,      // 二次方缓出
    EASE_INOUT_QUAD,    // 二次方缓入缓出
    EASE_IN_CUBIC,      // 三次方缓入
    EASE_OUT_CUBIC,     // 三次方缓出
    EASE_INOUT_CUBIC,   // 三次方缓入缓出
    EASE_IN_EXPO,       // 指数缓入
    EASE_OUT_EXPO,      // 指数缓出
    EASE_INOUT_EXPO,    // 指数缓入缓出
    EASE_IN_CIRC,       // 圆形曲线缓入
    EASE_OUT_CIRC,      // 圆形曲线缓出
    EASE_INOUT_CIRC,    // 圆形曲线缓入缓出
    EASE_IN_ELASTIC,    // 弹性缓入
    EASE_OUT_ELASTIC,   // 弹性缓出
    EASE_INOUT_ELASTIC, // 弹性缓入缓出
    EASE_IN_BOUNCE,     // 反弹缓入
    EASE_OUT_BOUNCE,    // 反弹缓出
    EASE_INOUT_BOUNCE,  // 反弹缓入缓出
    EASE_IN_BACK,       // 背越式缓入
    EASE_OUT_BACK,      // 背越式缓出
    EASE_INOUT_BACK,    // 背越式缓入缓出
    EASE_IN_SINE,       // 正弦缓入
    EASE_OUT_SINE,      // 正弦缓出
    EASE_INOUT_SINE     // 正弦缓入缓出
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
} Animation_t; // 动画对象结构体，包含起始时间、持续时间、起始值、结束值、当前值、活跃状态和缓动类型，是最底层的部分

typedef struct
{
    char tag[16];           // 动画对象的标识符
    Animation_t xAnimation; // X坐标动画
    Animation_t yAnimation; // Y坐标动画
    uint8_t isActive;       // 是否活跃
    float currentX;         // 当前X坐标
    float currentY;         // 当前Y坐标
} TaggedAnimation_t; // 带标签的动画对象结构体，包含X和Y坐标动画以及当前坐标和活跃状态

#define MAX_ANIMATIONS 20 // 最大动画数量(包含X、Y轴同时移动)
typedef struct
{
    TaggedAnimation_t taggedAnimations[MAX_ANIMATIONS];
    uint8_t count;
} AnimationManager_t; // 动画管理器结构体，每个动画对象都有一个标签和两个动画（X和Y坐标）他包含了多个动画对象的数组和一个计数器，表示当前动画对象的数量

extern AnimationManager_t g_AnimationManager; // 动画管理器实例

void OLED_InitAnimation(Animation_t *anim, float startValue, float endValue,
                        uint32_t duration, EaseType_t easeType);
uint8_t OLED_UpdateAnimation(Animation_t *anim, uint32_t currentTime);
float OLED_GetAnimationValue(Animation_t *anim);
void OLED_InitAnimationManager(AnimationManager_t *manager);
TaggedAnimation_t *OLED_FindTaggedAnimation(AnimationManager_t *manager, const char *tag);
void OLED_MoveObject(AnimationManager_t *manager, const char *tag,
                     float startX, float startY, float targetX, float targetY,
                     uint32_t duration, EaseType_t easeType);
void OLED_UpdateAnimationManager(AnimationManager_t *manager);
uint8_t OLED_GetObjectPosition(AnimationManager_t *manager, const char *tag, float *x, float *y);

//这部分是为了不使用Manager的动画，方便后续的高度自定义封装的案例
void InitBlockAnimation(void); // 不使用Manager的动画
void UpdateAndDrawBlock(void); //不使用Manager的动画
void AnimationLoop(void); // 不使用Manager的动画

// UI绘图函数
void OLED_DrawTitleBar(char *title);
void OLED_DrawBattery(uint8_t x, uint8_t y, uint8_t percentage);
void OLED_DrawProgressBar(uint8_t x, uint8_t y, uint8_t width, uint8_t percent);
void OLED_DrawRectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height);
void OLED_DrawFilledRectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color);
void OLED_DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
void OLED_InvertArea(uint8_t x, uint8_t y, uint8_t width, uint8_t height);

#endif /* INC_OLED_UI_H_ */
