/*
 * oled_ui.c
 *
 *  Created on: May 3, 2025
 *      Author: UnikoZera
 */

#include "oled_ui.h"

#pragma region TWEENS

static float EaseLinear(float t)
{
    return t;
}

static float EaseInQuad(float t)
{
    return t * t;
}

static float EaseOutQuad(float t)
{
    return t * (2 - t);
}

static float EaseInOutQuad(float t)
{
    return t < 0.5f ? 2 * t * t : -1 + (4 - 2 * t) * t;
}

static float EaseInCubic(float t)
{
    return t * t * t;
}

static float EaseOutCubic(float t)
{
    float t1 = t - 1;
    return t1 * t1 * t1 + 1;
}

static float EaseInOutCubic(float t)
{
    return t < 0.5f ? 4 * t * t * t : (t - 1) * (2 * t - 2) * (2 * t - 2) + 1;
}

// 获取缓动值
static float GetEaseValue(float progress, EaseType_t easeType)
{
    switch (easeType)
    {
    case EASE_LINEAR:
        return EaseLinear(progress);
    case EASE_IN_QUAD:
        return EaseInQuad(progress);
    case EASE_OUT_QUAD:
        return EaseOutQuad(progress);
    case EASE_INOUT_QUAD:
        return EaseInOutQuad(progress);
    case EASE_IN_CUBIC:
        return EaseInCubic(progress);
    case EASE_OUT_CUBIC:
        return EaseOutCubic(progress);
    case EASE_INOUT_CUBIC:
        return EaseInOutCubic(progress);
    default:
        return progress;
    }
}

// 初始化动画
void OLED_InitAnimation(Animation_t *anim, float startValue, float endValue,
                        uint32_t duration, EaseType_t easeType)
{
    anim->startTime = HAL_GetTick();
    anim->duration = duration;
    anim->startValue = startValue;
    anim->endValue = endValue;
    anim->currentValue = startValue;
    anim->isActive = 1;
    anim->easeType = easeType;
}

// 更新动画状态
uint8_t OLED_UpdateAnimation(Animation_t *anim, uint32_t currentTime)
{
    if (!anim->isActive)
        return 0;

    uint32_t elapsedTime = currentTime - anim->startTime;

    // 动画完成
    if (elapsedTime >= anim->duration)
    {
        anim->currentValue = anim->endValue;
        anim->isActive = 0;
        return 0;
    }

    // 计算当前进度 (0.0 - 1.0)
    float progress = (float)elapsedTime / anim->duration;

    // 应用缓动函数
    float easedProgress = GetEaseValue(progress, anim->easeType);

    // 计算当前值
    anim->currentValue = anim->startValue + (anim->endValue - anim->startValue) * easedProgress;

    return 1;
}

// 获取当前动画值
float OLED_GetAnimationValue(Animation_t *anim)
{
    return anim->currentValue;
}

Animation_t blockXAnim, blockYAnim;
// 初始化方块动画
void InitBlockAnimation()
{
    // 设置方块从(0,0)移动到(100,40)，持续2秒，使用缓出三次方缓动
    OLED_InitAnimation(&blockXAnim, 0, 100, 2000, EASE_IN_QUAD);
    OLED_InitAnimation(&blockYAnim, 0, 40, 2000, EASE_OUT_CUBIC);
}

// 更新并绘制方块
void UpdateAndDrawBlock()
{
    uint32_t currentTime = HAL_GetTick();

    // 更新动画
    OLED_UpdateAnimation(&blockXAnim, currentTime);
    OLED_UpdateAnimation(&blockYAnim, currentTime);

    // 获取当前位置
    uint8_t x = (uint8_t)OLED_GetAnimationValue(&blockXAnim);
    uint8_t y = (uint8_t)OLED_GetAnimationValue(&blockYAnim);

    // 清空缓冲区
    OLED_ClearBuffer();

    // 绘制方块
    OLED_DrawRectangle(x, y, 20, 20); // 绘制方块

    // 更新显示
    OLED_UpdateDisplay();
}

// 在主循环中调用
void AnimationLoop()
{
    InitBlockAnimation();

    while (1)
    {
        UpdateAndDrawBlock();
        HAL_Delay(10); // 约60FPS的刷新率

        // 如果动画已完成，可以重新开始或执行其他操作
        if (!blockXAnim.isActive && !blockYAnim.isActive)
        {
            // 重新开始动画或执行其他操作
            // InitBlockAnimation();
            break;
        }
    }
}

#pragma endregion TWEENS

#pragma region OLED_EPICFUL_UI

// UI 常量定义
#define UI_MAX_PAGES 5      // 最大页面数
#define UI_TITLE_HEIGHT 1   // 标题栏高度(页)
#define UI_STATUS_HEIGHT 1  // 状态栏高度(页)
#define UI_CONTENT_HEIGHT 6 // 内容区域高度(页)

// UI 状态与变量
typedef enum
{
    UI_PAGE_MAIN = 0, // 主页面
    UI_PAGE_STATUS,   // 状态页面
    UI_PAGE_SETTINGS, // 设置页面
    UI_PAGE_GRAPH,    // 图表页面
    UI_PAGE_ABOUT     // 关于页面
} UI_Page_t;

UI_Page_t currentPage = UI_PAGE_MAIN; // 当前显示的页面
uint8_t menuSelection = 0;            // 菜单选择项
uint8_t menuItemCount = 4;            // 菜单项数量
uint8_t settingsSelection = 0;        // 设置页面选择项
uint8_t dataIndex = 0;                // 数据索引

// 绘制UI标题栏
void OLED_DrawTitleBar(char *title) // 确保标题长度不超过22个字符 //! UPDATEDISPLAY REQUIRED
{
    unsigned char titleLength = strlen(title) * 6;

    // 绘制标题栏背景 (白色填充)
    OLED_DrawFilledRectangle(0, 0, OLED_WIDTH, 8, 1);

    // 显示标题 (黑底白字，反色效果)
    OLED_DisplayStringInverted((OLED_WIDTH - titleLength) / 2, 0, title, 1);
}

// 绘制电池图标
void OLED_DrawBattery(uint8_t x, uint8_t y, uint8_t percentage) //! UPDATEDISPLAY REQUIRED
{
    // 绘制电池外框
    // 外框宽度为12像素，高度为8像素
    OLED_DrawRectangle(x, y, 12, 8);

    // 绘制电池正极
    OLED_WritePixel(x + 12, y + 2, 1);
    OLED_WritePixel(x + 12, y + 3, 1);
    OLED_WritePixel(x + 12, y + 4, 1);
    OLED_WritePixel(x + 12, y + 5, 1);

    // 计算并绘制电池电量
    uint8_t filledWidth = (percentage * 10) / 100;
    if (filledWidth > 0)
    {
        OLED_DrawFilledRectangle(x + 1, y + 1, filledWidth, 6, 1);
    }
}

// 绘制进度条
void OLED_DrawProgressBar(uint8_t x, uint8_t y, uint8_t width, uint8_t progress) //! UPDATEDISPLAY REQUIRED
{
    // 绘制进度条外框
    OLED_DrawRectangle(x, y, width, 6);

    // 计算并绘制进度条填充部分
    if (progress > 0)
    {
        uint8_t fillWidth = ((width - 2) * progress) / 100;
        if (fillWidth > 0)
        {
            OLED_DrawFilledRectangle(x + 1, y + 1, fillWidth, 4, 1);
        }
    }
}

// 绘制空心矩形
void OLED_DrawRectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height) //! UPDATEDISPLAY REQUIRED
{
    // 基础边界检查和裁剪
    if (x >= OLED_WIDTH || y >= OLED_HEIGHT || width == 0 || height == 0)
        return;
    if (x + width > OLED_WIDTH)
        width = OLED_WIDTH - x;
    if (y + height > OLED_HEIGHT)
        height = OLED_HEIGHT - y;

    uint8_t x2 = x + width - 1;
    uint8_t y2 = y + height - 1;

    // 绘制水平线
    for (uint8_t i = x; i <= x2; i++)
    {
        OLED_WritePixel(i, y, 1);  // 顶线
        OLED_WritePixel(i, y2, 1); // 底线
    }

    // 绘制垂直线
    for (uint8_t j = y + 1; j < y2; j++)
    {
        OLED_WritePixel(x, j, 1);  // 左线
        OLED_WritePixel(x2, j, 1); // 右线
    }
}

// 绘制填充矩形
void OLED_DrawFilledRectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color) //! UPDATEDISPLAY REQUIRED
{
    // 边界检查和裁剪
    if (x >= OLED_WIDTH || y >= OLED_HEIGHT || width == 0 || height == 0)
        return;
    if (x + width > OLED_WIDTH)
        width = OLED_WIDTH - x;
    if (y + height > OLED_HEIGHT)
        height = OLED_HEIGHT - y;

    uint8_t x2 = x + width - 1;
    uint8_t y2 = y + height - 1;

    // 逐像素填充
    for (uint8_t j = y; j <= y2; j++)
    {
        for (uint8_t i = x; i <= x2; i++)
        {
            OLED_WritePixel(i, j, color);
        }
    }
}

// 绘制线段 (Bresenham算法)
void OLED_DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) //! UPDATEDISPLAY REQUIRED
{
    int dx = abs(x2 - x1);
    int dy = -abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = dx + dy;
    int e2;

    while (1)
    {
        OLED_WritePixel(x1, y1, 1);

        if (x1 == x2 && y1 == y2)
            break;

        e2 = 2 * err;
        if (e2 >= dy)
        {
            if (x1 == x2)
                break;
            err += dy;
            x1 += sx;
        }
        if (e2 <= dx)
        {
            if (y1 == y2)
                break;
            err += dx;
            y1 += sy;
        }
    }
}
#pragma endregion OLED_EPICFUL_UI