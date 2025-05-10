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
static uint8_t diff_mode_enabled = 0; // 差分更新模式启用标志
static uint8_t fast_update_enabled = 1; // 默认启用快速更新

// I2C DMA传输完成回调函数
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c->Instance == hi2c1.Instance)
    {
        // 标记DMA传输已完成
        oled_dma_busy = 0; // 设置为就绪
    }
}

// 启用差分更新模式
void OLED_EnableDiffMode(uint8_t enable)
{
    diff_mode_enabled = enable;
    if (enable)
    {
        memcpy(OLED_PrevBuffer, OLED_BackBuffer, OLED_WIDTH * OLED_PAGES);
    }
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
    // 如果OLED/DMA忙，直接返回
    if (OLED_IsBusy())
    {
        return;
    }

    // 检查是否有脏页需要更新
    uint8_t has_dirty = 0;
    uint8_t first_dirty = 255;
    uint8_t last_dirty = 0;

    // 如果启用了差分更新，检查哪些页已经变化
    if (diff_mode_enabled)
    {
        for (uint8_t page = 0; page < OLED_PAGES; page++)
        {
            // 检查此页中是否有任何字节发生变化
            uint8_t page_changed = 0;
            uint16_t start_idx = page * OLED_WIDTH;

            for (uint16_t i = 0; i < OLED_WIDTH; i++)
            {
                if (OLED_BackBuffer[start_idx + i] != OLED_PrevBuffer[start_idx + i])
                {
                    page_changed = 1;
                    oled_dirty_pages[page] = 1;
                    break;
                }
            }

            if (page_changed)
            {
                has_dirty = 1;
                if (page < first_dirty)
                    first_dirty = page;
                if (page > last_dirty)
                    last_dirty = page;

                // 更新上一帧缓存
                memcpy(
                    OLED_PrevBuffer + start_idx,
                    OLED_BackBuffer + start_idx,
                    OLED_WIDTH);
            }
        }
    }
    else
    {
        // 如果未启用差分更新，使用脏页标记
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