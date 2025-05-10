/*
 * oled_driver.c
 *
 *  Created on: May 10, 2025
 *      Author: UnikoZera
 */

#include "oled_driver.h"
#include "oled_ui.h"

// UI 常量定义
#define UI_MAX_PAGES 5      // 最大页面数
#define UI_TITLE_HEIGHT 1   // 标题栏高度(页)
#define UI_STATUS_HEIGHT 1  // 状态栏高度(页)
#define UI_CONTENT_HEIGHT 6 // 内容区域高度(页)

// UI 状态与变量
typedef enum
{
    UI_PAGE_MENU,     // 菜单页面
    UI_PAGE_SETTINGS, // 设置页面
    UI_PAGE_STATUS,   // 状态页面
    UI_PAGE_ABOUT,    // 关于页面
    UI_PAGE_TOOLS,    // 工具页面
    UI_PAGE_GAMES,    // 游戏页面

} UI_Page_t;

UI_Page_t currentPage = UI_PAGE_MENU; // 当前显示的页面
uint8_t menuSelection = 0;            // 菜单选择项
uint8_t menuItemCount = 5;            // 菜单项数量
uint8_t settingsSelection = 0;        // 设置页面选择项
uint8_t dataIndex = 0;                // 数据索引

#define OLED_UI_START_X -90
#define OLED_UI_END_X 2
#define OLED_UI_START_Y 4
#define OLED_UI_GAP_Y 9 + 13
#define StartTweenTime 3000
#define TweenStyle EASE_INOUT_CUBIC

void System_UI_Loop()
{
    static bool isFirstRun = true;
    float x, y;
    if (isFirstRun)
    {
        OLED_MoveObject(&Menu_AnimationManager, "SettingsButton", OLED_UI_START_X, OLED_UI_START_Y + 9, OLED_UI_END_X, OLED_UI_START_Y + 9, StartTweenTime, TweenStyle);
        HAL_Delay(100);
        OLED_MoveObject(&Menu_AnimationManager, "StatusButton", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 1, OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 1, StartTweenTime, TweenStyle);
        HAL_Delay(100);
        OLED_MoveObject(&Menu_AnimationManager, "GamesButton", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 2, OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 2, StartTweenTime, TweenStyle);
        HAL_Delay(100);
        OLED_MoveObject(&Menu_AnimationManager, "AboutButton", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 3, OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 3, StartTweenTime, TweenStyle);
        HAL_Delay(100);
        OLED_MoveObject(&Menu_AnimationManager, "ToolsButton", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 4, OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 4, StartTweenTime, TweenStyle);
        isFirstRun = false;
    }

    OLED_GetObjectPosition(&Menu_AnimationManager, "SettingsButton", &x, &y);
    OLED_DisplayString(x, y, "Settings");
    OLED_GetObjectPosition(&Menu_AnimationManager, "StatusButton", &x, &y);
    OLED_DisplayString(x , y, "Status");
    OLED_GetObjectPosition(&Menu_AnimationManager, "GamesButton", &x, &y);
    OLED_DisplayString(x , y, "Games");
    OLED_GetObjectPosition(&Menu_AnimationManager, "AboutButton", &x, &y);
    OLED_DisplayString(x, y, "About");
    OLED_GetObjectPosition(&Menu_AnimationManager, "ToolsButton", &x, &y);
    OLED_DisplayString(x, y, "Tools");

    OLED_DrawTitleBar("UnikoZera's OLED UI");
}
