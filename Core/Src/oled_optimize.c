/*
 * oled_optimize.c
 *
 *  Created on: May 11, 2025
 *      Author: UnikoZera & GitHub Copilot
 *                                      bro偷偷改我的注释233
 */

#include "oled.h"
#include "i2c.h"

extern const uint8_t cmds[];

// 上一帧的缓存，用于差分更新
static uint8_t OLED_PrevBuffer[128 * 8];
static uint8_t diff_mode_enabled = 0;   // 差分更新模式启用标志
static uint8_t fast_update_enabled = 1; // 默认启用快速更新

// 启用差分更新模式
void OLED_EnableDiffMode(uint8_t enable)
{
    diff_mode_enabled = enable;
}

// 设置快速更新模式
void OLED_EnableFastUpdate(uint8_t enable)
{
    fast_update_enabled = enable;
}

// 智能更新显示
// 选择性更新脏页，以提高帧率
void OLED_SmartUpdate(void)
{
    if (OLED_IsBusy())
    {
        return;
    }

    uint8_t has_dirty = 0;
    uint8_t first_dirty = 255;
    uint8_t last_dirty = 0;

    if (diff_mode_enabled)
    {
        for (uint8_t page = 0; page < OLED_PAGES; page++)
        {
            uint8_t page_changed = 0;
            uint16_t start_idx = page * OLED_WIDTH;

            uint32_t *p_back32 = (uint32_t *)(OLED_BackBuffer + start_idx);
            uint32_t *p_prev32 = (uint32_t *)(OLED_PrevBuffer + start_idx);

            // 使用底层优化的 memcmp 替代手动 uint32 循环比对
            if (memcmp(OLED_BackBuffer + start_idx, OLED_PrevBuffer + start_idx, OLED_WIDTH) != 0)
            {
                page_changed = 1;
                oled_dirty_pages[page] = 1;
                memcpy(OLED_PrevBuffer + start_idx, OLED_BackBuffer + start_idx, OLED_WIDTH);
            }

            if (page_changed)
            {
                has_dirty = 1;
                if (page < first_dirty)
                    first_dirty = page;
                if (page > last_dirty)
                    last_dirty = page;

                memcpy(OLED_PrevBuffer + start_idx, OLED_BackBuffer + start_idx, OLED_WIDTH);
            }
        }
    }
    else
    {
        for (uint8_t i = 0; i < OLED_PAGES; i++)
        {
            if (oled_dirty_pages[i])
            {
                has_dirty = 1;
                if (i < first_dirty)
                    first_dirty = i;
                if (i > last_dirty)
                    last_dirty = i;
            }
        }
    }

    // 如果有脏页，只更新这些页
    if (has_dirty && fast_update_enabled)
    {
        OLED_UpdateDisplayPartial(first_dirty, last_dirty);
    }
    else if (has_dirty)
    {
        OLED_UpdateDisplayVSync();
    }
}

// 显示FPS
void OLED_OptimizedDisplayFPS(int16_t x, int16_t y)
{
    static uint32_t last_time = 0;
    static uint32_t frames = 0;
    static uint32_t fps = 0;
    static char fps_str[16] = "FPS:0";

    frames++;

    // 每秒更新一次FPS
    uint32_t current_time = HAL_GetTick();
    if (current_time - last_time >= 1000)
    {
        fps = frames;
        sprintf(fps_str, "FPS:%d", fps);
        frames = 0;
        last_time = current_time;
    }

    OLED_DisplayString(x, y, fps_str);
}