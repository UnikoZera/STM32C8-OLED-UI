/*
 * oled_driver.c
 *
 *  Created on: May 10, 2025
 *      Author: UnikoZera
 */

#include "oled_driver.h"
#include "oled_ui.h"
#include "main.h"
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

uint8_t menuRank = 1;                 // 菜单等级
uint8_t menuSelection = 1;            // 菜单选择项
UI_Page_t currentPage = UI_PAGE_MENU; // 当前显示的页面
uint8_t settingsSelection = 0;        // 设置页面选择项
uint8_t dataIndex = 0;                // 数据索引
const uint8_t MainMenuItemCount = 5;  // 主菜单项数量
const uint8_t SettingsItemCount = 5; // 设置项数量
const uint8_t StatusItemCount = 5;   // 状态项数量
const uint8_t GamesItemCount = 5;    // 游戏项数量
const uint8_t ToolsItemCount = 5;    // 工具项数量
const uint8_t AboutItemCount = 5;    // 关于项数量

#define OLED_UI_START_X -90
#define OLED_UI_END_X 2
#define OLED_UI_START_Y -6
#define OLED_UI_GAP_Y 9 + 13
#define StartTweenTime 3000
#define TweenStyle EASE_INOUT_CUBIC

void System_UI_Loop()
{
    static bool isFirstRun = true;  
    float x, y, x1, y1;
    if (isFirstRun)
    {
        OLED_MoveObject(&Cursor_AnimationManager, "Cursor", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * menuSelection, OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * menuSelection, StartTweenTime, TweenStyle);
        OLED_MoveObject(&Cursor_AnimationManager, "CursorScale",  0, 10, strlen("Settings")*6+ 3, 10 , StartTweenTime, TweenStyle);
        OLED_MoveObject(&Menu_AnimationManager, "SettingsButton", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 1, OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 1, StartTweenTime, TweenStyle);
        HAL_Delay(100);
        OLED_MoveObject(&Menu_AnimationManager, "StatusButton", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 2, OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 2, StartTweenTime, TweenStyle);
        HAL_Delay(100);
        OLED_MoveObject(&Menu_AnimationManager, "GamesButton", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 3, OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 3, StartTweenTime, TweenStyle);
        HAL_Delay(100);
        OLED_MoveObject(&Menu_AnimationManager, "AboutButton", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 4, OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 4, StartTweenTime, TweenStyle);
        HAL_Delay(100);
        OLED_MoveObject(&Menu_AnimationManager, "ToolsButton", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 5, OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 5, StartTweenTime, TweenStyle);
        isFirstRun = false;
    }

    #pragma region pager

    OLED_DoTweenObject(&Menu_AnimationManager, "SettingsButton", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (2 - menuSelection), 500, EASE_OUT_CIRC, true);
    OLED_DoTweenObject(&Menu_AnimationManager, "StatusButton",   OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (3 - menuSelection), 500, EASE_OUT_CIRC, true);
    OLED_DoTweenObject(&Menu_AnimationManager, "GamesButton",    OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (4 - menuSelection), 500, EASE_OUT_CIRC, true);
    OLED_DoTweenObject(&Menu_AnimationManager, "AboutButton",    OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (5 - menuSelection), 500, EASE_OUT_CIRC, true);
    OLED_DoTweenObject(&Menu_AnimationManager, "ToolsButton",    OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (6 - menuSelection), 500, EASE_OUT_CIRC, true);


    #pragma endregion pager

    #pragma region DRAWER
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
    OLED_GetObjectPosition(&Cursor_AnimationManager, "Cursor", &x, &y);
    OLED_GetObjectPosition(&Cursor_AnimationManager, "CursorScale", &x1, &y1);
    
    OLED_InvertArea(x - 2, y - 2, x1, y1); // 绘制光标
    #pragma endregion DRAWER
    if (!isFirstRun)
        SystemGetsSignal();

    if (menuRank == 1)
    {
        switch (menuSelection)
        {
        case 1:
            // OLED_DisplayString((OLED_WIDTH - strlen("Settings For STM") * 6) / 2, 0, "Settings For STM");
            OLED_DrawTitleBar("Settings For STM");
            OLED_DoTweenObject(&Cursor_AnimationManager, "CursorScale", strlen("Settings")*6+ 3, 10, 200, EASE_IN_CIRC, true);
            break;
        case 2:
            // OLED_DisplayString((OLED_WIDTH - strlen("Machine States") * 6) / 2, 0, "Machine States");
            OLED_DrawTitleBar("Machine States");
            // OLED_InvertArea(x - 2, y - 2, strlen("States"), 10); // 绘制光标
            OLED_DoTweenObject(&Cursor_AnimationManager, "CursorScale", strlen("States")*6+ 3, 10, 200, EASE_IN_CIRC, true);
            break;
        case 3:
            // OLED_DisplayString((OLED_WIDTH - strlen("Epicful Games") * 6) / 2, 0, "Epicful Games");
            OLED_DrawTitleBar("Epicful Games");
            // OLED_InvertArea(x - 2, y - 2, strlen("Games"), 10); // 绘制光标
            OLED_DoTweenObject(&Cursor_AnimationManager, "CursorScale", strlen("Games")*6+ 3, 10, 200, EASE_IN_CIRC, true);
            break;
        case 4:
            // OLED_DisplayString((OLED_WIDTH - strlen("About Developer") * 6) / 2, 0, "About Developer");
            OLED_DrawTitleBar("About Developer");
            // OLED_InvertArea(x - 2, y - 2, strlen("Developer"), 10); // 绘制光标
            OLED_DoTweenObject(&Cursor_AnimationManager, "CursorScale", strlen("About")*6+ 3, 10, 200, EASE_IN_CIRC, true);
            break;
        case 5:
            // OLED_DisplayString((OLED_WIDTH - strlen("Tools For You") * 6) / 2, 0, "Tools For You");
            OLED_DrawTitleBar("Tools For You");
            // OLED_InvertArea(x - 2, y - 2, strlen("Tools"), 10); // 绘制光标
            OLED_DoTweenObject(&Cursor_AnimationManager, "CursorScale", strlen("Tools")*6+ 3, 10, 200, EASE_IN_CIRC, true);
            break;
        default:
            OLED_DrawTitleBar("UnikoZera's UI");
            break;
        }
    }
    
}


void SystemGetsSignal() //这里是旋钮数据的获取
{
    static uint16_t preCount = 0;

    if(count-preCount > 1)
    {
        if(menuSelection < MainMenuItemCount)
        {
            menuSelection++;
        }
        else
        {
            menuSelection = 1;
        }

        preCount = count;
    }
    else if(count-preCount < -1)
    {
        if(menuSelection > 1)
        {
            menuSelection--;
        }
        else
        {
            menuSelection = MainMenuItemCount;
        }

        preCount = count;
    }




}
