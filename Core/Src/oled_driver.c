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
const uint8_t GamesItemCount = 3;    // 游戏项数量
const uint8_t ToolsItemCount = 5;    // 工具项数量
const uint8_t AboutItemCount = 5;    // 关于项数量

#define OLED_UI_START_X -90
#define OLED_UI_END_X 2
#define OLED_UI_START_Y -6
#define OLED_UI_GAP_Y 9 + 13
#define StartTweenTime 3000
#define TweenStyle EASE_INOUT_ELASTIC

void System_UI_Loop()
{
    static bool isFirstRun = true;  
    float x, y, x1, y1;
    if (isFirstRun)
    {
        OLED_MoveObject(&Games_AnimationManager, "Snake", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 1, OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 1, 1, TweenStyle);
        OLED_MoveObject(&Games_AnimationManager, "2048", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 2, OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 2, 1, TweenStyle);
        OLED_MoveObject(&g_AnimationManager, "BackButton", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 3, OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 3, 1, TweenStyle);

        #pragma region 主菜单动画
        OLED_MoveObject(&g_AnimationManager, "Cursor", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * menuSelection, OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * menuSelection, StartTweenTime, TweenStyle);
        OLED_MoveObject(&g_AnimationManager, "CursorScale",  0, 10, strlen("Games")*6+ 3, 10 , StartTweenTime, TweenStyle);
        OLED_MoveObject(&Menu_AnimationManager, "GamesButton", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 1, OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 1, StartTweenTime, TweenStyle);
        HAL_Delay(100);
        OLED_MoveObject(&Menu_AnimationManager, "ToolsButton", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 2, OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 2, StartTweenTime, TweenStyle);
        HAL_Delay(100);
        OLED_MoveObject(&Menu_AnimationManager, "StatusButton", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 3, OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 3, StartTweenTime, TweenStyle);
        HAL_Delay(100);
        OLED_MoveObject(&Menu_AnimationManager, "SettingsButton", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 4, OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 4, StartTweenTime, TweenStyle);
        HAL_Delay(100);
        OLED_MoveObject(&Menu_AnimationManager, "AboutButton", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 5, OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 5, StartTweenTime, TweenStyle);
        #pragma endregion 主菜单动画

        isFirstRun = false;
    }

    #pragma region pager

    if (menuRank == 1)
    {
        OLED_DoTweenObject(&Menu_AnimationManager, "GamesButton",    OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (2 - menuSelection), 200, EASE_IN_CIRC, true);
        OLED_DoTweenObject(&Menu_AnimationManager, "ToolsButton",    OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (3 - menuSelection), 200, EASE_IN_CIRC, true);
        OLED_DoTweenObject(&Menu_AnimationManager, "StatusButton",   OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (4 - menuSelection), 200, EASE_IN_CIRC, true);
        OLED_DoTweenObject(&Menu_AnimationManager, "SettingsButton", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (5 - menuSelection), 200, EASE_IN_CIRC, true);
        OLED_DoTweenObject(&Menu_AnimationManager, "AboutButton",    OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (6 - menuSelection), 200, EASE_IN_CIRC, true);
    }
    else if (menuRank == 2 && currentPage == UI_PAGE_GAMES)
    {
        OLED_DoTweenObject(&Games_AnimationManager, "Snake", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (2 - menuSelection), 200, EASE_IN_CIRC, true);
        OLED_DoTweenObject(&Games_AnimationManager, "2048", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (3 - menuSelection), 200, EASE_IN_CIRC, true);
        OLED_DoTweenObject(&g_AnimationManager, "BackButton", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (4 - menuSelection), 200, EASE_IN_CIRC, true);
    }
    else if (menuRank == 2 && currentPage == UI_PAGE_SETTINGS)
    {
        /* code */
    }
    else if (menuRank == 2 && currentPage == UI_PAGE_STATUS)
    {
        /* code */
    }
    else if (menuRank == 2 && currentPage == UI_PAGE_ABOUT)
    {
        /* code */
    }
    else if (menuRank == 2 && currentPage == UI_PAGE_TOOLS)
    {
        /* code */
    }
    


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
    OLED_GetObjectPosition(&g_AnimationManager, "Cursor", &x, &y);
    OLED_GetObjectPosition(&g_AnimationManager, "CursorScale", &x1, &y1);
    OLED_InvertArea(x - 2, y - 2, x1, y1); // 绘制光标
    OLED_GetObjectPosition(&Games_AnimationManager, "Snake", &x, &y);
    OLED_DisplayString(x, y, "Snake");
    OLED_GetObjectPosition(&Games_AnimationManager, "2048", &x, &y);
    OLED_DisplayString(x, y, "2048");
    OLED_GetObjectPosition(&g_AnimationManager, "BackButton", &x, &y);
    OLED_DisplayString(x, y, "Back");

    #pragma endregion DRAWER




    #pragma region PlayerSelection
    if (menuRank == 1)
    {
        switch (menuSelection)
        {
        case 1:
            // OLED_DisplayString((OLED_WIDTH - strlen("Epicful Games") * 6) / 2, 0, "Epicful Games");
            OLED_DrawTitleBar("Epicful Games");
            OLED_DoTweenObject(&g_AnimationManager, "CursorScale", strlen("Games")*6+ 3, 10, 100, EASE_INOUT_CIRC, true);
            break;
        case 2:
            // OLED_DisplayString((OLED_WIDTH - strlen("Tools For You") * 6) / 2, 0, "Tools For You");
            OLED_DrawTitleBar("Tools For You");
            OLED_DoTweenObject(&g_AnimationManager, "CursorScale", strlen("Tools")*6+ 3, 10, 100, EASE_INOUT_CIRC, true);
            break;
        case 3:
            // OLED_DisplayString((OLED_WIDTH - strlen("Machine States") * 6) / 2, 0, "Machine States");
            OLED_DrawTitleBar("Machine States");
            OLED_DoTweenObject(&g_AnimationManager, "CursorScale", strlen("States")*6+ 3, 10, 100, EASE_INOUT_CIRC, true);
            break;
        case 4:
            // OLED_DisplayString((OLED_WIDTH - strlen("Settings For STM") * 6) / 2, 0, "Settings For STM");
            OLED_DrawTitleBar("Settings For STM");
            OLED_DoTweenObject(&g_AnimationManager, "CursorScale", strlen("Settings")*6+ 3, 10, 100, EASE_INOUT_CIRC, true);
            break;
        case 5:
            // OLED_DisplayString((OLED_WIDTH - strlen("About Developer") * 6) / 2, 0, "About Developer");
            OLED_DrawTitleBar("About Developer!");
            OLED_DoTweenObject(&g_AnimationManager, "CursorScale", strlen("About")*6+ 3, 10, 100, EASE_INOUT_CIRC, true);
            break;
        default:
            OLED_DrawTitleBar("UnikoZera's UI");
            break;
        }
    }
    else if (menuRank == 2 && currentPage == UI_PAGE_SETTINGS)
    {

    }
    else if (menuRank == 2 && currentPage == UI_PAGE_STATUS)
    {
        /* code */
    }
    else if (menuRank == 2 && currentPage == UI_PAGE_ABOUT)
    {
        /* code */
    }
    else if (menuRank == 2 && currentPage == UI_PAGE_TOOLS)
    {
        /* code */
    }
    else if (menuRank == 2 && currentPage == UI_PAGE_GAMES)
    {
        switch (menuSelection)
        {
        case 1:
            // OLED_DisplayString((OLED_WIDTH - strlen("Snake") * 6) / 2, 0, "Snake");
            OLED_DrawTitleBar("Snake");
            OLED_DoTweenObject(&g_AnimationManager, "CursorScale", strlen("Snake")*6+ 3, 10, 100, EASE_INOUT_CIRC, true);
            break;
        case 2:
            // OLED_DisplayString((OLED_WIDTH - strlen("2048") * 6) / 2, 0, "2048");
            OLED_DrawTitleBar("2048");
            OLED_DoTweenObject(&g_AnimationManager, "CursorScale", strlen("2048")*6+ 3, 10, 100, EASE_INOUT_CIRC, true);
            break;
        case 3:
            // OLED_DisplayString((OLED_WIDTH - strlen("Back") * 6) / 2, 0, "Back");
            OLED_DrawTitleBar("Back To Menu");
            OLED_DoTweenObject(&g_AnimationManager, "CursorScale", strlen("Back")*6+ 3, 10, 100, EASE_INOUT_CIRC, true);
            break;
        default:
            break;
        }

    }
    #pragma endregion PlayerSelection
    
    
    SystemGetsSignal();
    OLED_DisplayInteger(32, 32, menuSelection); // 显示编码器计数值
}


void SystemGetsSignal() //这里是旋钮数据的获取
{
    static uint16_t preCount = 32767;

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

    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12) == GPIO_PIN_RESET)
    {
        HAL_Delay(100); // 防抖动延时
        if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12) == GPIO_PIN_RESET)
        {
            if (menuRank == 1 && menuSelection == 1)
            {
                menuRank = 2;
                currentPage = UI_PAGE_SETTINGS;
                OLED_DoTweenObject(&Menu_AnimationManager, "GamesButton", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (2 - menuSelection), 1000, EASE_IN_CIRC, false);
                OLED_DoTweenObject(&Menu_AnimationManager, "ToolsButton", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (3 - menuSelection), 1000, EASE_IN_CIRC, false);
                OLED_DoTweenObject(&Menu_AnimationManager, "StatusButton", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (4 - menuSelection), 1000, EASE_IN_CIRC, false);
                OLED_DoTweenObject(&Menu_AnimationManager, "SettingsButton", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (5 - menuSelection), 1000, EASE_IN_CIRC, false);
                OLED_DoTweenObject(&Menu_AnimationManager, "AboutButton", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (6 - menuSelection), 1000, EASE_IN_CIRC, false);

                // OLED_DoTweenObject(&g_AnimationManager, "Cursor", -50, 10, 100, EASE_IN_CIRC, false);
                OLED_DoTweenObjectX(&g_AnimationManager, "Cursor", -100, 1000, EASE_IN_CIRC);
                OLED_DoTweenObject(&Games_AnimationManager, "Snake", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 1, 1000, EASE_IN_CIRC, false);
                HAL_Delay(100);
                OLED_DoTweenObject(&Games_AnimationManager, "2048", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 2, 1000, EASE_IN_CIRC, false);
                HAL_Delay(100);
                OLED_DoTweenObject(&g_AnimationManager, "BackButton", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 3, 1000, EASE_IN_CIRC, false);
            }
            while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12) == GPIO_PIN_RESET)
            {

            }
        }
    }

}
