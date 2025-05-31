/*
 * oled_ui.c
 *
 *  Created on: May 3, 2025
 *      Author: UnikoZera
 */

#include "main.h" // Include main.h for HAL types like uint32_t
#include "stdint.h"
#include "oled_ui.h"

#pragma region TWEENS // 动画缓动函数全部在这里定义

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

// 在现有EaseType_t枚举中添加这些新类型
// 指数缓动
static float EaseInExpo(float t)
{
    return (t == 0) ? 0 : powf(2, 10 * (t - 1));
}

static float EaseOutExpo(float t)
{
    return (t == 1) ? 1 : (1 - powf(2, -10 * t));
}

static float EaseInOutExpo(float t)
{
    if (t == 0)
        return 0;
    if (t == 1)
        return 1;
    if (t < 0.5f)
        return powf(2, 20 * t - 10) / 2;
    return (2 - powf(2, -20 * t + 10)) / 2;
}

// 圆形曲线缓动
static float EaseInCirc(float t)
{
    return 1 - sqrtf(1 - t * t);
}

static float EaseOutCirc(float t)
{
    return sqrtf(1 - powf(t - 1, 2));
}

static float EaseInOutCirc(float t)
{
    if (t < 0.5f)
        return (1 - sqrtf(1 - powf(2 * t, 2))) / 2;
    return (sqrtf(1 - powf(-2 * t + 2, 2)) + 1) / 2;
}

// 弹性缓动
static float EaseInElastic(float t)
{
    const float c4 = (2 * 3.14159f) / 3;

    if (t == 0)
        return 0;
    if (t == 1)
        return 1;
    return -powf(2, 10 * t - 10) * sinf((t * 10 - 10.75f) * c4);
}

static float EaseOutElastic(float t)
{
    const float c4 = (2 * 3.14159f) / 3;

    if (t == 0)
        return 0;
    if (t == 1)
        return 1;
    return powf(2, -10 * t) * sinf((t * 10 - 0.75f) * c4) + 1;
}

static float EaseInOutElastic(float t)
{
    const float c5 = (2 * 3.14159f) / 4.5f;

    if (t == 0)
        return 0;
    if (t == 1)
        return 1;
    if (t < 0.5f)
        return -(powf(2, 20 * t - 10) * sinf((20 * t - 11.125f) * c5)) / 2;
    return (powf(2, -20 * t + 10) * sinf((20 * t - 11.125f) * c5)) / 2 + 1;
}

static float EaseOutBounce(float t)
{
    const float n1 = 7.5625f;
    const float d1 = 2.75f;

    if (t < 1 / d1)
    {
        return n1 * t * t;
    }
    else if (t < 2 / d1)
    {
        t -= 1.5f / d1;
        return n1 * t * t + 0.75f;
    }
    else if (t < 2.5f / d1)
    {
        t -= 2.25f / d1;
        return n1 * t * t + 0.9375f;
    }
    else
    {
        t -= 2.625f / d1;
        return n1 * t * t + 0.984375f;
    }
}

// 反弹缓动
static float EaseInBounce(float t)
{
    return 1 - EaseOutBounce(1 - t);
}

static float EaseInOutBounce(float t)
{
    if (t < 0.5f)
        return EaseInBounce(t * 2) * 0.5f;
    return EaseOutBounce(t * 2 - 1) * 0.5f + 0.5f;
}

// 背越式缓动
static float EaseInBack(float t)
{
    const float c1 = 1.70158f;
    const float c3 = c1 + 1;

    return c3 * t * t * t - c1 * t * t;
}

static float EaseOutBack(float t)
{
    const float c1 = 1.70158f;
    const float c3 = c1 + 1;

    return 1 + c3 * powf(t - 1, 3) + c1 * powf(t - 1, 2);
}

static float EaseInOutBack(float t)
{
    const float c1 = 1.70158f;
    const float c2 = c1 * 1.525f;

    if (t < 0.5f)
        return (powf(2 * t, 2) * ((c2 + 1) * 2 * t - c2)) / 2;
    return (powf(2 * t - 2, 2) * ((c2 + 1) * (t * 2 - 2) + c2) + 2) / 2;
}

// 正弦缓动
static float EaseInSine(float t)
{
    return 1 - cosf((t * 3.14159f) / 2);
}

static float EaseOutSine(float t)
{
    return sinf((t * 3.14159f) / 2);
}

static float EaseInOutSine(float t)
{
    return -(cosf(3.14159f * t) - 1) / 2;
}

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
    case EASE_IN_EXPO:
        return EaseInExpo(progress);
    case EASE_OUT_EXPO:
        return EaseOutExpo(progress);
    case EASE_INOUT_EXPO:
        return EaseInOutExpo(progress);
    case EASE_IN_CIRC:
        return EaseInCirc(progress);
    case EASE_OUT_CIRC:
        return EaseOutCirc(progress);
    case EASE_INOUT_CIRC:
        return EaseInOutCirc(progress);
    case EASE_IN_ELASTIC:
        return EaseInElastic(progress);
    case EASE_OUT_ELASTIC:
        return EaseOutElastic(progress);
    case EASE_INOUT_ELASTIC:
        return EaseInOutElastic(progress);
    case EASE_IN_BOUNCE:
        return EaseInBounce(progress);
    case EASE_OUT_BOUNCE:
        return EaseOutBounce(progress);
    case EASE_INOUT_BOUNCE:
        return EaseInOutBounce(progress);
    case EASE_IN_BACK:
        return EaseInBack(progress);
    case EASE_OUT_BACK:
        return EaseOutBack(progress);
    case EASE_INOUT_BACK:
        return EaseInOutBack(progress);
    case EASE_IN_SINE:
        return EaseInSine(progress);
    case EASE_OUT_SINE:
        return EaseOutSine(progress);
    case EASE_INOUT_SINE:
        return EaseInOutSine(progress);
    default:
        return progress;
    }
}

float MathLerp(float start, float end, float t)
{
    // 保证 t 在 0~1 之间
    if (t < 0.0f)
        t = 0.0f;
    else if (t > 1.0f)
        t = 1.0f;
    return start + t * (end - start);
}

#pragma endregion TWEENS

#pragma region ANIMATIONTWEENS
// 这里是底层动画实现
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

// 更新动画状态，返回isActive的值
uint8_t OLED_UpdateAnimation(Animation_t *anim, uint32_t currentTime)
{
    if (!anim->isActive)
        return 0;

    uint32_t elapsedTime = currentTime - anim->startTime;
    // 动画完成
    if (elapsedTime >= anim->duration || anim->currentValue == anim->endValue) // 如果我的目标在指定的点上，也关闭isActive
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
// 结束底层动画

// 使用底层动画实现的方块动画
Animation_t blockXAnim, blockYAnim;
void InitBlockAnimation()
{
    OLED_InitAnimation(&blockXAnim, 0, 100, 2000, EASE_OUT_EXPO);
    OLED_InitAnimation(&blockYAnim, 0, 40, 2000, EASE_OUT_BOUNCE);
}

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
    OLED_UpdateDisplayVSync();
}

void AnimationLoop()
{
    InitBlockAnimation();

    while (1)
    {
        UpdateAndDrawBlock();

        // 如果动画已完成，可以重新开始或执行其他操作
        if (!blockXAnim.isActive && !blockYAnim.isActive)
        {
            // 重新开始动画或执行其他操作
            // InitBlockAnimation();
            break;
        }
    }
}
// 结束使用底层动画实现的方块动画

// 动画管理器全局管理部分
AnimationManager_t Menu_AnimationManager;
AnimationManager_t Games_AnimationManager;
AnimationManager_t Tools_AnimationManager;
AnimationManager_t Settings_AnimationManager;
AnimationManager_t About_AnimationManager;
AnimationManager_t Status_AnimationManager;
AnimationManager_t g_Title_AnimationManager; // 菜单动画管理器
AnimationManager_t g_AnimationManager;       // 全局动画管理器

void OLED_InitAnimationManager(AnimationManager_t *manager) // 这是初始化一个动画管理器，填入你的manager名字，在系统初始化时候调用它
{
    manager->count = 0;
    memset(manager->taggedAnimations, 0, sizeof(manager->taggedAnimations));
}

TaggedAnimation_t *OLED_FindTaggedAnimation(AnimationManager_t *manager, const char *tag) // 查找对应manager的标签对应的动画tag
{
    for (uint8_t i = 0; i < manager->count; i++)
    {
        if (strcmp(manager->taggedAnimations[i].tag, tag) == 0)
        {
            return &manager->taggedAnimations[i];
        }
    }
    return NULL;
}

uint8_t OLED_GetObjectPosition(AnimationManager_t *manager, const char *tag, float *x, float *y) // 获取tag当前位置
{
    TaggedAnimation_t *anim = OLED_FindTaggedAnimation(manager, tag);
    if (anim)
    {
        if (x)
            *x = anim->currentX;
        if (y)
            *y = anim->currentY;
        return 1;
    }
    return 0;
}

// 移动指定标签的对象
void OLED_MoveObject(AnimationManager_t *manager, const char *tag,
                     float startX, float startY, float targetX, float targetY,
                     uint32_t duration, EaseType_t easeType) // 这个函数是用来移动一个对象的，tag是对象的标签，startX和startY是起始坐标，targetX和targetY是目标坐标，duration是动画持续时间，easeType是缓动类型
{
    // 查找已存在的动画
    TaggedAnimation_t *anim = OLED_FindTaggedAnimation(manager, tag);

    // 如果没找到并且还有可用槽位，创建新的动画
    if (anim == NULL)
    {
        if (manager->count >= MAX_ANIMATIONS)
            return; // 动画已满，无法添加

        anim = &manager->taggedAnimations[manager->count++];
        strncpy(anim->tag, tag, sizeof(anim->tag) - 1);
        anim->tag[sizeof(anim->tag) - 1] = '\0'; // 确保字符串结束
        anim->currentX = startX;
        anim->currentY = startY;
    }

    // 初始化或更新X和Y坐标的动画
    OLED_InitAnimation(&anim->xAnimation, startX, targetX, duration, easeType);
    OLED_InitAnimation(&anim->yAnimation, startY, targetY, duration, easeType);
    anim->isActive = 1;
}

// 更新manager里面的所有动画!
void OLED_UpdateAnimationManager(AnimationManager_t *manager) //! 这里需要使用OLED_UpdateDisplayVSync()来更新显示
{
    uint32_t currentTime = HAL_GetTick();

    for (uint8_t i = 0; i < manager->count; i++)
    {
        TaggedAnimation_t *anim = &manager->taggedAnimations[i];
        if (!anim->isActive)
            continue;

        uint8_t activeX = OLED_UpdateAnimation(&anim->xAnimation, currentTime);
        uint8_t activeY = OLED_UpdateAnimation(&anim->yAnimation, currentTime);

        anim->currentX = OLED_GetAnimationValue(&anim->xAnimation);
        anim->currentY = OLED_GetAnimationValue(&anim->yAnimation);

        // 如果两个动画都结束，则标记该对象动画为非活跃
        if (!activeX && !activeY)
            anim->isActive = 0;
    }
}

uint8_t OLED_GetAnimationStates(AnimationManager_t *manager, const char *tag) // 返回值是1表示活跃，0表示非活跃
{
    TaggedAnimation_t *anim = OLED_FindTaggedAnimation(manager, tag);
    if (anim)
    {
        return anim->isActive;
    }
    return 0; // 如果没有找到，返回0表示非活跃
}

// 移动指定标签的对象到目标坐标。
// 如果对象已在向同一目标动画且动画处于活动状态，则不执行任何操作。
// 否则，它会中断当前动画（如果活动）并从当前位置开始新的动画。
void OLED_DoTweenObject(AnimationManager_t *manager, const char *tag, float targetX, float targetY, uint32_t duration, EaseType_t easeType)
{
    TaggedAnimation_t *anim = OLED_FindTaggedAnimation(manager, tag);

    if (anim == NULL)
    {
        // 如果找不到具有指定标签的动画对象，则不执行任何操作。
        // 或者，您可以在此处添加创建新对象的逻辑（如果需要）。
        // 例如: OLED_MoveObject(manager, tag, initialX, initialY, targetX, targetY, duration, easeType);
        // 但这需要为新对象定义 initialX, initialY。
        return;
    }

    // 检查动画是否已处于活动状态并且已经朝向相同的目标
    if (anim->isActive &&
        anim->xAnimation.endValue == targetX &&
        anim->yAnimation.endValue == targetY)
    {
        // 动画已在进行中且目标相同，无需操作
        return;
    }

    // 否则，（重新）启动动画到新目标
    // OLED_MoveObject 将从 anim->currentX, anim->currentY 开始动画
    OLED_MoveObject(manager, tag, anim->currentX, anim->currentY, targetX, targetY, duration, easeType);
}

// 为X或Y轴单独创建动画的函数
void OLED_MoveObjectAxis(AnimationManager_t *manager, const char *tag,
                         float startX, float startY, float targetX, float targetY,
                         uint32_t duration, EaseType_t easeType, bool onlyX, bool onlyY)
{
    // 查找已存在的动画
    TaggedAnimation_t *anim = OLED_FindTaggedAnimation(manager, tag);

    // 如果没找到并且还有可用槽位，创建新的动画
    if (anim == NULL)
    {
        if (manager->count >= MAX_ANIMATIONS)
            return; // 动画已满，无法添加

        anim = &manager->taggedAnimations[manager->count++];
        strncpy(anim->tag, tag, sizeof(anim->tag) - 1);
        anim->tag[sizeof(anim->tag) - 1] = '\0'; // 确保字符串结束
        anim->currentX = startX;
        anim->currentY = startY;
    }

    // 只初始化需要的轴的动画
    if (!onlyY) // 如果不是只修改Y轴，就修改X轴
        OLED_InitAnimation(&anim->xAnimation, startX, targetX, duration, easeType);
    if (!onlyX) // 如果不是只修改X轴，就修改Y轴
        OLED_InitAnimation(&anim->yAnimation, startY, targetY, duration, easeType);

    anim->isActive = 1;
}

// 这个函数是用来移动一个对象的，tag是对象的标签，targetX是目标坐标，duration是动画持续时间，easeType是缓动类型
//! 这里不可以在while里面调用！为一次性调用
void OLED_DoTweenObjectX(AnimationManager_t *manager, const char *tag, float targetX, uint32_t duration, EaseType_t easeType)
{
    TaggedAnimation_t *anim = OLED_FindTaggedAnimation(manager, tag);
    if (anim && anim->currentX != targetX)
    {
        OLED_MoveObjectAxis(manager, tag, anim->currentX, anim->currentY, targetX, anim->currentY, duration, easeType, true, false);
    }
}

// 这个函数是用来移动一个对象的，tag是对象的标签，targetY是目标坐标，duration是动画持续时间，easeType是缓动类型
//! 这里不可以在while里面调用！为一次性调用
void OLED_DoTweenObjectY(AnimationManager_t *manager, const char *tag, float targetY, uint32_t duration, EaseType_t easeType)
{
    TaggedAnimation_t *anim = OLED_FindTaggedAnimation(manager, tag);
    if (anim && anim->currentY != targetY)
    {
        OLED_MoveObjectAxis(manager, tag, anim->currentX, anim->currentY, anim->currentX, targetY, duration, easeType, false, true);
    }
}

#pragma endregion ANIMATIONTWEENS

#pragma region OLED_EPICFUL_UI

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
void OLED_DrawBattery(int16_t x, int16_t y, uint8_t percentage) //! UPDATEDISPLAY REQUIRED
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
void OLED_DrawProgressBar(int16_t x, int16_t y, uint8_t width, uint8_t progress) //! UPDATEDISPLAY REQUIRED
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
// Parameters x and y are changed to int16_t to handle negative coordinates.
void OLED_DrawRectangle(int16_t x, int16_t y, uint8_t width, uint8_t height) //! UPDATEDISPLAY REQUIRED
{
    // Initial check for zero dimensions
    if (width == 0 || height == 0)
    {
        return;
    }

    // Use int16_t for width and height internally for clipping calculations
    // to prevent overflow and handle adjustments correctly.
    int16_t current_width = width;
    int16_t current_height = height;

    // Clip left edge: if x is negative, adjust width and set x to 0
    if (x < 0)
    {
        current_width += x; // x is negative, so this reduces width
        x = 0;
    }

    // Clip top edge: if y is negative, adjust height and set y to 0
    if (y < 0)
    {
        current_height += y; // y is negative, so this reduces height
        y = 0;
    }

    // If width or height became non-positive after left/top clipping, nothing to draw
    if (current_width <= 0 || current_height <= 0)
    {
        return;
    }

    // Check if the adjusted rectangle is entirely off-screen to the right or bottom
    // At this point, x and y are >= 0.
    // Assuming OLED_WIDTH and OLED_HEIGHT are defined (e.g., in oled.h)
    if (x >= OLED_WIDTH || y >= OLED_HEIGHT)
    {
        return;
    }

    // Clip right edge: if x + current_width extends beyond OLED_WIDTH
    if (x + current_width > OLED_WIDTH)
    {
        current_width = OLED_WIDTH - x;
    }

    // Clip bottom edge: if y + current_height extends beyond OLED_HEIGHT
    if (y + current_height > OLED_HEIGHT)
    {
        current_height = OLED_HEIGHT - y;
    }

    // If width or height became non-positive after right/bottom clipping, nothing to draw
    if (current_width <= 0 || current_height <= 0)
    {
        return;
    }

    // At this point, x, y, current_width, and current_height define the drawable part
    // of the rectangle. Cast coordinates and dimensions to uint8_t for drawing,
    // as they are now confirmed to be within valid screen bounds.
    uint8_t final_x = (uint8_t)x;
    uint8_t final_y = (uint8_t)y;
    uint8_t final_width = (uint8_t)current_width;
    uint8_t final_height = (uint8_t)current_height;

    // Calculate the bottom-right corner coordinates
    uint8_t x2 = final_x + final_width - 1;
    uint8_t y2 = final_y + final_height - 1;

    // Draw horizontal lines
    for (uint8_t i = final_x; i <= x2; i++)
    {
        OLED_WritePixel(i, final_y, 1); // Top line
        OLED_WritePixel(i, y2, 1);      // Bottom line
    }

    // Draw vertical lines
    // Loop from final_y + 1 to y2 - 1 to avoid double-drawing corners
    // and to correctly handle height = 1 or 2.
    for (uint8_t j = final_y + 1; j < y2; j++)
    {
        OLED_WritePixel(final_x, j, 1); // Left line
        OLED_WritePixel(x2, j, 1);      // Right line
    }
}

// 绘制填充矩形
void OLED_DrawFilledRectangle(int16_t x, int16_t y, uint8_t width, uint8_t height, uint8_t color) //! UPDATEDISPLAY REQUIRED
{
    // Initial check for zero dimensions
    if (width == 0 || height == 0)
    {
        return;
    }

    // Use int16_t for width and height internally for clipping calculations
    int16_t current_width = width;
    int16_t current_height = height;

    // Clip left edge: if x is negative, adjust width and set x to 0
    if (x < 0)
    {
        current_width += x; // x is negative, so this reduces width
        x = 0;
    }

    // Clip top edge: if y is negative, adjust height and set y to 0
    if (y < 0)
    {
        current_height += y; // y is negative, so this reduces height
        y = 0;
    }

    // If width or height became non-positive after left/top clipping, nothing to draw
    if (current_width <= 0 || current_height <= 0)
    {
        return;
    }

    // Check if the adjusted rectangle is entirely off-screen to the right or bottom
    // At this point, x and y are >= 0.
    if (x >= OLED_WIDTH || y >= OLED_HEIGHT)
    {
        return;
    }

    // Clip right edge: if x + current_width extends beyond OLED_WIDTH
    if (x + current_width > OLED_WIDTH)
    {
        current_width = OLED_WIDTH - x;
    }

    // Clip bottom edge: if y + current_height extends beyond OLED_HEIGHT
    if (y + current_height > OLED_HEIGHT)
    {
        current_height = OLED_HEIGHT - y;
    }

    // If width or height became non-positive after right/bottom clipping, nothing to draw
    if (current_width <= 0 || current_height <= 0)
    {
        return;
    }

    // Cast coordinates and dimensions to uint8_t for drawing
    uint8_t final_x = (uint8_t)x;
    uint8_t final_y = (uint8_t)y;
    uint8_t final_width = (uint8_t)current_width;
    uint8_t final_height = (uint8_t)current_height;

    uint8_t x2 = final_x + final_width - 1;
    uint8_t y2 = final_y + final_height - 1;

    // 逐像素填充
    for (uint8_t j = final_y; j <= y2; j++)
    {
        for (uint8_t i = final_x; i <= x2; i++)
        {
            OLED_WritePixel(i, j, color);
        }
    }
}

// // 绘制线段 (Bresenham算法)
// void OLED_DrawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2) //! UPDATEDISPLAY REQUIRED
// {
//     int dx = abs(x2 - x1);
//     int dy = -abs(y2 - y1);
//     int sx = x1 < x2 ? 1 : -1;
//     int sy = y1 < y2 ? 1 : -1;
//     int err = dx + dy;
//     int e2;

//     while (1)
//     {
//         OLED_WritePixel(x1, y1, 1);

//         if (x1 == x2 && y1 == y2)
//             break;

//         e2 = 2 * err;
//         if (e2 >= dy)
//         {
//             if (x1 == x2)
//                 break;
//             err += dy;
//             x1 += sx;
//         }
//         if (e2 <= dx)
//         {
//             if (y1 == y2)
//                 break;
//             err += dx;
//             y1 += sy;
//         }
//     }
// }

// 绘制图标 - 40x40水平扫描格式
void OLED_DrawIcon(int16_t x, int16_t y, IconType_t iconType) //! UPDATEDISPLAY REQUIRED
{
    const unsigned char *iconData = NULL;
    uint8_t iconWidth = 36;  // 图标宽度（36像素）
    uint8_t iconHeight = 36; // 图标高度（36像素）

    // 根据图标类型选择对应的数据
    switch (iconType)
    {
    case ICON_GAME:
        iconData = IMG_GAME_DATA;
        break;
    case ICON_TOOL:
        iconData = IMG_TOOL_DATA;
        break;
    case ICON_STATUS:
        iconData = IMG_STATUS_DATA;
        break;
    case ICON_SETTINGS:
        iconData = IMG_SETTINGS_DATA;
        break;
    case ICON_ABOUT:
        iconData = IMG_ABOUT_DATA;
        break;
    case ICON_DEVELOPER:
        iconData = IMG_DEVELOPER_DATA;
        break;
    default:
        return; // 无效的图标类型，直接返回
    }

    // 检查图标数据是否有效
    if (iconData == NULL)
        return;    // 水平扫描格式：从左往右，从上往下，每8个像素一个字节
    // 36像素宽度需要5个字节每行（36/8=4.5，向上取整为5），36行共180字节
    for (uint8_t row = 0; row < iconHeight; row++)
    {
        for (uint8_t col = 0; col < iconWidth; col++)
        {
            // 计算当前像素在屏幕上的坐标
            int16_t pixel_x = x + col;
            int16_t pixel_y = y + row;

            // 边界检查
            if (pixel_x < 0 || pixel_x >= OLED_WIDTH ||
                pixel_y < 0 || pixel_y >= OLED_HEIGHT)
                continue;

            // 水平扫描格式的数据组织方式：
            // 每行有5个字节（36像素/8像素每字节，向上取整）
            // byte_index = 行索引 * 5 + 当前列在该行中的字节位置
            uint16_t byte_index = row * 5 + (col / 8);
            uint8_t bit_index = col % 8;

            // 检查字节索引是否在有效范围内
            if (byte_index < 180) // 图标数据大小为180字节（36*36像素，每行5字节）
            {
                // 提取像素值（从字节的最高位开始，从左到右）
                uint8_t pixel_value = (iconData[byte_index] >> (7 - bit_index)) & 0x01;

                // 绘制像素
                OLED_WritePixel(pixel_x, pixel_y, pixel_value);
            }
        }
    }
}

#pragma endregion OLED_EPICFUL_UI