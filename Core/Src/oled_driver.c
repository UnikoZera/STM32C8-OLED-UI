/*
 * oled_driver.c
 *
 *  Created on: May 10, 2025
 *      Author: UnikoZera
 */

#include "oled_driver.h"

OLED_Cube3D CUBE;

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
static uint8_t preMenuSelection = 0; // 上一个菜单选择项
const uint8_t MainMenuItemCount = 5;  // 主菜单项数量
const uint8_t SettingsItemCount = 3;  // 设置项数量
const uint8_t GamesItemCount = 3;     // 游戏项数量
const uint8_t ToolsItemCount = 2;     // 工具项数量
const uint8_t AboutItemCount = 4;     // 关于项数量
static bool cancelSelection = false;
static bool resetAnimation = false;

bool isShowFPS = false; // 是否显示FPS
unsigned char brightness = 207; // 亮度 0 - 255

#define OLED_UI_START_X -90
#define OLED_UI_END_X 2
#define OLED_UI_START_Y -6
#define OLED_UI_GAP_Y 9 + 13
#define OLED_TITLE_Start_Y -10
#define OLED_TITLE_End_Y 0

#define OLED_ICON_START_X  OLED_WIDTH + 5
#define OLED_ICON_START_Y  OLED_HEIGHT - 45
#define OLED_ICON_END_X  OLED_WIDTH/2 + 17
#define OLED_ICON_END_Y OLED_HEIGHT - 45
#define OLED_ICON_TWEEN_OUT_TIME 300
#define OLED_ICON_TWEEN_IN_TIME 500
#define OLED_ICON_TWEEN_OUT_STYLE EASE_INOUT_CUBIC
#define OLED_ICON_TWEEN_IN_STYLE EASE_INOUT_CUBIC

//开始动画部分
#define StartTweenTime 2000
#define TweenStyle EASE_INOUT_BACK

//翻页部分
#define PAGE_TWEEN_TIME 300
#define PAGE_TWEEN_STYLE EASE_INOUT_QUAD

//基本动画部分
#define BASIC_TWEEN_IN_TIME 800 //包含换级部分的选项进入
#define BASIC_TWEEN_IN_STYLE EASE_INOUT_SINE
#define BASIC_TWEEN_OUT_TIME 800 //包含换级部分的选项退出
#define BASIC_TWEEN_OUT_STYLE EASE_INOUT_SINE

//标题动画部分
#define TITLE_TWEEN_IN_TIME 400
#define TITLE_TWEEN_OUT_TIME 300
#define TITLE_TWEEN_IN_STYLE EASE_IN_CUBIC
#define TITLE_TWEEN_OUT_STYLE EASE_LINEAR

//标题背景动画
#define TITLE_BG_TWEEN_TIME 400
#define TITLE_BG_TWEEN_STYLE EASE_INOUT_SINE

//光标动画部分
#define CURSORBG_TWEEN_TIME 350
#define CURSORBG_TWEEN_STYLE EASE_INOUT_BACK

void System_UI_Loop()
{
    static bool isFirstRun = true;

    float x, y, x1, y1;
    if (!isFirstRun)
        SystemGetsSignal();

    if (isFirstRun) //第一次启动调用，初始化部分
    {
#pragma region 标题栏
        OLED_MoveObject(&g_Title_AnimationManager, "TitleGames", (OLED_WIDTH - strlen("Epicful Games") * 6), OLED_TITLE_Start_Y, (OLED_WIDTH - strlen("Epicful Games") * 6), OLED_TITLE_End_Y, StartTweenTime, TweenStyle);
        OLED_MoveObject(&g_Title_AnimationManager, "TitleTools", (OLED_WIDTH - strlen("Tools For You") * 6), OLED_TITLE_Start_Y, (OLED_WIDTH - strlen("Tools For You") * 6), OLED_TITLE_Start_Y, StartTweenTime, TweenStyle);
        OLED_MoveObject(&g_Title_AnimationManager, "TitleStatus", (OLED_WIDTH - strlen("Machine States") * 6), OLED_TITLE_Start_Y, (OLED_WIDTH - strlen("Machine States") * 6), OLED_TITLE_Start_Y, StartTweenTime, TweenStyle);
        OLED_MoveObject(&g_Title_AnimationManager, "TitleSettings", (OLED_WIDTH - strlen("STM Settings") * 6), OLED_TITLE_Start_Y, (OLED_WIDTH - strlen("STM Settings") * 6), OLED_TITLE_Start_Y, StartTweenTime, TweenStyle);
        OLED_MoveObject(&g_Title_AnimationManager, "TitleAbout", (OLED_WIDTH - strlen("Developer") * 6), OLED_TITLE_Start_Y, (OLED_WIDTH - strlen("Developer") * 6), OLED_TITLE_Start_Y, StartTweenTime, TweenStyle);

        OLED_MoveObject(&g_Title_AnimationManager, "TitleSnake", (OLED_WIDTH - strlen("Greedy Snake") * 6), OLED_TITLE_Start_Y, (OLED_WIDTH - strlen("Greedy Snake") * 6), OLED_TITLE_Start_Y, StartTweenTime, TweenStyle);
        OLED_MoveObject(&g_Title_AnimationManager, "Title2048", (OLED_WIDTH - strlen("Here is 2048!") * 6), OLED_TITLE_Start_Y, (OLED_WIDTH - strlen("Here is 2048!") * 6), OLED_TITLE_Start_Y, StartTweenTime, TweenStyle);
        OLED_MoveObject(&g_Title_AnimationManager, "TitleBack", (OLED_WIDTH - strlen("Back To Menu") * 6), OLED_TITLE_Start_Y, (OLED_WIDTH - strlen("Back To Menu") * 6), OLED_TITLE_Start_Y, StartTweenTime, TweenStyle);

        OLED_MoveObject(&g_Title_AnimationManager, "BAD APPLE!", (OLED_WIDTH - strlen("BAD APPLE!") * 6), OLED_TITLE_Start_Y, (OLED_WIDTH - strlen("BAD APPLE!") * 6), OLED_TITLE_Start_Y, StartTweenTime, TweenStyle);

        OLED_MoveObject(&g_Title_AnimationManager, "Show FPS?", (OLED_WIDTH - strlen("Show FPS?") * 6), OLED_TITLE_Start_Y, (OLED_WIDTH - strlen("Show FPS?") * 6), OLED_TITLE_Start_Y, StartTweenTime, TweenStyle);

        OLED_MoveObject(&g_Title_AnimationManager, "TitleDeveloper", (OLED_WIDTH - strlen("DEVELOPER: UnikoZera") * 6), OLED_TITLE_Start_Y, (OLED_WIDTH - strlen("DEVELOPER: UnikoZera") * 6), OLED_TITLE_Start_Y, StartTweenTime, TweenStyle);
        OLED_MoveObject(&g_Title_AnimationManager, "TitleGithub", (OLED_WIDTH - strlen("Github Adress") * 6), OLED_TITLE_Start_Y, (OLED_WIDTH - strlen("Github Adress") * 6), OLED_TITLE_Start_Y, StartTweenTime, TweenStyle);
        OLED_MoveObject(&g_Title_AnimationManager, "TitleDonate", (OLED_WIDTH - strlen("BiliBili Space") * 6), OLED_TITLE_Start_Y, (OLED_WIDTH - strlen("TitleDonate") * 6), OLED_TITLE_Start_Y, StartTweenTime, TweenStyle);
#pragma endregion 标题栏

#pragma region 游戏栏
        OLED_MoveObject(&Games_AnimationManager, "Snake", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 1, OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 1, 1, TweenStyle);
        OLED_MoveObject(&Games_AnimationManager, "2048", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 2, OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 2, 1, TweenStyle);
        OLED_MoveObject(&g_AnimationManager, "BackButton", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 3, OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 3, 1, TweenStyle);
#pragma endregion 游戏栏

#pragma region 工具栏
        OLED_MoveObject(&Tools_AnimationManager, "VideoPlayer", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 2, OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 1, 1, TweenStyle);

#pragma endregion 工具栏

#pragma region 状态栏 //这里我想做成一个界面就好,不要选项
        OLED_MoveObject(&Status_AnimationManager, "RunningTime", 0, -10, 0, -10, 1, TweenStyle);
        OLED_MoveObject(&Status_AnimationManager, "Status", 0, OLED_HEIGHT, 0, OLED_HEIGHT, 1, TweenStyle);
        OLED_MoveObject(&Status_AnimationManager, "CUBE", 150, 50, 150, 50, 1, TweenStyle);
#pragma endregion 状态栏

#pragma region 设置栏
        OLED_MoveObject(&Settings_AnimationManager, "ShowFPS", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 1, OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 1, 1, TweenStyle);
        OLED_MoveObject(&Settings_AnimationManager, "Lightness", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 2, OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 2, 1, TweenStyle);
        OLED_MoveObject(&Settings_AnimationManager, "Bar", 5, OLED_HEIGHT + 10, 0, OLED_HEIGHT + 10, 1, TweenStyle);
#pragma endregion 设置栏

#pragma region 关于栏
        OLED_MoveObject(&About_AnimationManager, "Developer", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 1, OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 1, 1, TweenStyle);
        OLED_MoveObject(&About_AnimationManager, "Github", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 2, OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 2, 1, TweenStyle);
        OLED_MoveObject(&About_AnimationManager, "Donate", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 3, OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 3, 1, TweenStyle);
#pragma endregion 关于栏

#pragma region 主菜单动画
        OLED_MoveObject(&g_AnimationManager, "TitleBGScale", 0, 8, strlen("Epicful Games") * 6 + 3, 8, StartTweenTime, EASE_INOUT_CUBIC);
        OLED_MoveObject(&g_AnimationManager, "Cursor", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * menuSelection, OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * menuSelection, StartTweenTime, TweenStyle);
        OLED_MoveObject(&g_AnimationManager, "CursorScale", 0, 10, strlen("Games") * 6 + 3, 10, StartTweenTime, TweenStyle);
        OLED_MoveObject(&Menu_AnimationManager, "GamesButton", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 1, OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 1, StartTweenTime, TweenStyle);
        HAL_Delay(100);
        OLED_MoveObject(&Menu_AnimationManager, "ToolsButton", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 2, OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 2, StartTweenTime, TweenStyle);
        HAL_Delay(100);
        OLED_MoveObject(&Menu_AnimationManager, "StatusButton", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 3, OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 3, StartTweenTime, TweenStyle);
        HAL_Delay(100);
        OLED_MoveObject(&Menu_AnimationManager, "SettingsButton", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 4, OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 4, StartTweenTime, TweenStyle);
        HAL_Delay(100);
        OLED_MoveObject(&Menu_AnimationManager, "AboutButton", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 5, OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 5, StartTweenTime, TweenStyle);

        OLED_MoveObject(&Menu_AnimationManager, "IconGames", OLED_ICON_START_X , OLED_ICON_START_Y, OLED_ICON_END_X , OLED_ICON_END_Y, StartTweenTime, TweenStyle);
        OLED_MoveObject(&Menu_AnimationManager, "IconTools", OLED_ICON_START_X , OLED_ICON_START_Y, OLED_ICON_START_X , OLED_ICON_START_Y, StartTweenTime, TweenStyle);
        OLED_MoveObject(&Menu_AnimationManager, "IconStatus", OLED_ICON_START_X , OLED_ICON_START_Y, OLED_ICON_START_X , OLED_ICON_START_Y, StartTweenTime, TweenStyle);
        OLED_MoveObject(&Menu_AnimationManager, "IconSettings", OLED_ICON_START_X , OLED_ICON_START_Y, OLED_ICON_START_X , OLED_ICON_START_Y, StartTweenTime, TweenStyle);
        OLED_MoveObject(&Menu_AnimationManager, "IconAbout", OLED_ICON_START_X , OLED_ICON_START_Y, OLED_ICON_START_X , OLED_ICON_START_Y, StartTweenTime, TweenStyle);
#pragma endregion 主菜单动画

        preMenuSelection = menuSelection;
        isFirstRun = false;
        OLED_UpdateDisplayVSync();
    }

#pragma region pager //翻页的函数

    if (menuRank == 1)
    {
        OLED_DoTweenObject(&Menu_AnimationManager, "GamesButton", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (2 - menuSelection), PAGE_TWEEN_TIME, PAGE_TWEEN_STYLE);
        OLED_DoTweenObject(&Menu_AnimationManager, "ToolsButton", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (3 - menuSelection), PAGE_TWEEN_TIME, PAGE_TWEEN_STYLE);
        OLED_DoTweenObject(&Menu_AnimationManager, "StatusButton", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (4 - menuSelection), PAGE_TWEEN_TIME, PAGE_TWEEN_STYLE);
        OLED_DoTweenObject(&Menu_AnimationManager, "SettingsButton", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (5 - menuSelection), PAGE_TWEEN_TIME, PAGE_TWEEN_STYLE);
        OLED_DoTweenObject(&Menu_AnimationManager, "AboutButton", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (6 - menuSelection), PAGE_TWEEN_TIME, PAGE_TWEEN_STYLE);
    }
    else if (menuRank == 2 && currentPage == UI_PAGE_GAMES)
    {
        OLED_DoTweenObject(&Games_AnimationManager, "Snake", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (2 - menuSelection), PAGE_TWEEN_TIME, PAGE_TWEEN_STYLE);
        OLED_DoTweenObject(&Games_AnimationManager, "2048", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (3 - menuSelection), PAGE_TWEEN_TIME, PAGE_TWEEN_STYLE);
        OLED_DoTweenObject(&g_AnimationManager, "BackButton", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (4 - menuSelection), PAGE_TWEEN_TIME, PAGE_TWEEN_STYLE);
    }
    else if (menuRank == 2 && currentPage == UI_PAGE_TOOLS)
    {
        OLED_DoTweenObject(&Tools_AnimationManager, "VideoPlayer", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (2 - menuSelection), PAGE_TWEEN_TIME, PAGE_TWEEN_STYLE);
        OLED_DoTweenObject(&g_AnimationManager, "BackButton", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (3 - menuSelection), PAGE_TWEEN_TIME, PAGE_TWEEN_STYLE);
    }
    else if (menuRank == 2 && currentPage == UI_PAGE_SETTINGS)
    {
        OLED_DoTweenObject(&Settings_AnimationManager, "ShowFPS", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (2 - menuSelection), PAGE_TWEEN_TIME, PAGE_TWEEN_STYLE);
        OLED_DoTweenObject(&Settings_AnimationManager, "Lightness", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (3 - menuSelection), PAGE_TWEEN_TIME, PAGE_TWEEN_STYLE);
        OLED_DoTweenObject(&g_AnimationManager, "BackButton", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (4 - menuSelection), PAGE_TWEEN_TIME, PAGE_TWEEN_STYLE);
    }
    else if (menuRank == 2 && currentPage == UI_PAGE_ABOUT)
    {
        OLED_DoTweenObject(&About_AnimationManager, "Developer", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (2 - menuSelection), PAGE_TWEEN_TIME, PAGE_TWEEN_STYLE);
        OLED_DoTweenObject(&About_AnimationManager, "Github", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (3 - menuSelection), PAGE_TWEEN_TIME, PAGE_TWEEN_STYLE);
        OLED_DoTweenObject(&About_AnimationManager, "Donate", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (4 - menuSelection), PAGE_TWEEN_TIME, PAGE_TWEEN_STYLE);
        OLED_DoTweenObject(&g_AnimationManager, "BackButton", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (5 - menuSelection), PAGE_TWEEN_TIME, PAGE_TWEEN_STYLE);
    }

#pragma endregion pager

#pragma region DRAWER //画出你的UI
    OLED_GetObjectPosition(&Menu_AnimationManager, "IconGames", &x, &y);
    OLED_DrawIcon(x, y, ICON_GAME);
    OLED_GetObjectPosition(&Menu_AnimationManager, "IconTools", &x, &y);
    OLED_DrawIcon(x, y, ICON_TOOL);
    OLED_GetObjectPosition(&Menu_AnimationManager, "IconStatus", &x, &y);
    OLED_DrawIcon(x, y, ICON_STATUS);
    OLED_GetObjectPosition(&Menu_AnimationManager, "IconSettings", &x, &y);
    OLED_DrawIcon(x, y, ICON_SETTINGS);
    OLED_GetObjectPosition(&Menu_AnimationManager, "IconAbout", &x, &y);
    OLED_DrawIcon(x, y, ICON_ABOUT);

    OLED_GetObjectPosition(&Menu_AnimationManager, "SettingsButton", &x, &y);
    OLED_DisplayString(x, y, "Settings");
    OLED_GetObjectPosition(&Menu_AnimationManager, "StatusButton", &x, &y);
    OLED_DisplayString(x, y, "Status");
    OLED_GetObjectPosition(&Menu_AnimationManager, "GamesButton", &x, &y);
    OLED_DisplayString(x, y, "Games");
    OLED_GetObjectPosition(&Menu_AnimationManager, "AboutButton", &x, &y);
    OLED_DisplayString(x, y, "About");
    OLED_GetObjectPosition(&Menu_AnimationManager, "ToolsButton", &x, &y);
    OLED_DisplayString(x, y, "Tools");
    OLED_GetObjectPosition(&Games_AnimationManager, "Snake", &x, &y);
    OLED_DisplayString(x, y, "Snake");
    OLED_GetObjectPosition(&Games_AnimationManager, "2048", &x, &y);
    OLED_DisplayString(x, y, "2048");
    OLED_GetObjectPosition(&g_AnimationManager, "BackButton", &x, &y);
    OLED_DisplayString(x, y, "Back");

    OLED_GetObjectPosition(&g_Title_AnimationManager, "TitleGames", &x, &y);
    OLED_DisplayString(x, y, "Epicful Games");
    OLED_GetObjectPosition(&g_Title_AnimationManager, "TitleTools", &x, &y);
    OLED_DisplayString(x, y, "Tools For You");
    OLED_GetObjectPosition(&g_Title_AnimationManager, "TitleStatus", &x, &y);
    OLED_DisplayString(x, y, "Machine States");
    OLED_GetObjectPosition(&g_Title_AnimationManager, "TitleSettings", &x, &y);
    OLED_DisplayString(x, y, "STM Settings");
    OLED_GetObjectPosition(&g_Title_AnimationManager, "TitleAbout", &x, &y);
    OLED_DisplayString(x, y, "Developer!");
    OLED_GetObjectPosition(&g_Title_AnimationManager, "TitleSnake", &x, &y);
    OLED_DisplayString(x, y, "Greedy Snake");
    OLED_GetObjectPosition(&g_Title_AnimationManager, "Title2048", &x, &y);
    OLED_DisplayString(x, y, "Here is 2048!");
    OLED_GetObjectPosition(&g_Title_AnimationManager, "TitleBack", &x, &y);
    OLED_DisplayString(x, y, "Back To Menu");
    OLED_GetObjectPosition(&g_Title_AnimationManager, "BAD APPLE!", &x, &y);
    OLED_DisplayString(x, y, "BAD APPLE!");
    OLED_GetObjectPosition(&g_Title_AnimationManager, "Show FPS?", &x, &y);
    OLED_DisplayString(x, y, "Show FPS?");

    OLED_GetObjectPosition(&g_Title_AnimationManager, "TitleDeveloper", &x, &y);    
    OLED_DisplayString(x, y, "Developer: UnikoZera");
    OLED_GetObjectPosition(&g_Title_AnimationManager, "TitleGithub", &x, &y);
    OLED_DisplayString(x, y, "GH Address");
    OLED_GetObjectPosition(&g_Title_AnimationManager, "TitleDonate", &x, &y);
    OLED_DisplayString(x, y, "BiliBili Space");

    OLED_GetObjectPosition(&Tools_AnimationManager, "VideoPlayer", &x, &y);
    OLED_DisplayString(x, y, "Video");
    OLED_GetObjectPosition(&Status_AnimationManager, "RunningTime", &x, &y);
    unsigned char Timer[] = "Running Time:00:00:00";
    sprintf(Timer, "Running Time:%02d:%02d:%02d", (HAL_GetTick() / 3600000) % 24, (HAL_GetTick() / 60000) % 60, (HAL_GetTick() / 1000) % 60);
    OLED_DisplayString(x, y, Timer);
    OLED_GetObjectPosition(&Status_AnimationManager, "Status", &x, &y);
    OLED_DisplayString(x, y, "MCU:STM32F103C8T6");
    OLED_DisplayString(x, y + 9, "Core: Cortex-M3");
    OLED_DisplayString(x, y + 18, "CrystalOS v1.0");
    OLED_DisplayString(x, y + 27, "UnikoZera Made");
    OLED_GetObjectPosition(&Status_AnimationManager, "CUBE", &x, &y);
    CUBE.transform.center_x = x;
    CUBE.transform.center_y = y;
    OLED_UpdateCubeRotation(&CUBE, 0.02f, 0.03f, 0.01f); // X,Y,Z轴旋转增量
    OLED_DrawCube3D(&CUBE);

    OLED_GetObjectPosition(&Settings_AnimationManager, "ShowFPS", &x, &y);
    if (isShowFPS)
    {
        OLED_DisplayString(x, y, "Show FPS:ON");
    }
    else
    {
        OLED_DisplayString(x, y, "Show FPS:OFF");
    }
    
    unsigned char Lightness[20] = "Lightness: 100%";
    int val = (brightness * 100) / 255; // 将亮度转换为百分比
    OLED_GetObjectPosition(&Settings_AnimationManager, "Lightness", &x, &y);
    sprintf(Lightness, "Lightness: %d%%", val);
    OLED_DisplayString(x, y, Lightness);
    OLED_GetObjectPosition(&Settings_AnimationManager, "Bar", &x, &y);
    OLED_DrawProgressBar(x, y, 82, val);

    OLED_GetObjectPosition(&About_AnimationManager, "Developer", &x, &y);
    OLED_DisplayString(x, y, "Developer");
    OLED_GetObjectPosition(&About_AnimationManager, "Github", &x, &y);
    OLED_DisplayString(x, y, "Github");
    OLED_GetObjectPosition(&About_AnimationManager, "Donate", &x, &y);
    OLED_DisplayString(x, y, "Donate");

    OLED_GetObjectPosition(&g_AnimationManager, "TitleBGScale", &x, &y);
    OLED_InvertArea(OLED_WIDTH - x, 0, x, y);

    OLED_GetObjectPosition(&g_AnimationManager, "Cursor", &x, &y);
    OLED_GetObjectPosition(&g_AnimationManager, "CursorScale", &x1, &y1);
    OLED_InvertArea(x - 2, y - 2, x1, y1); // 绘制光标
#pragma endregion DRAWER

#pragma region PlayerSelection //选择某个选项时候的详细内容
    if (menuRank == 1)
    {
        switch (menuSelection)
        {
        case 1:
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleBack", (OLED_WIDTH - strlen("Back To Menu") * 6), OLED_TITLE_Start_Y, TITLE_TWEEN_OUT_TIME, TITLE_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleStatus", OLED_WIDTH - strlen("Machine States") * 6, OLED_TITLE_Start_Y, 1, TITLE_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleSettings", OLED_WIDTH - strlen("STM Settings") * 6, OLED_TITLE_Start_Y, 1, TITLE_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleAbout", (OLED_WIDTH - strlen("Developer!") * 6), OLED_TITLE_Start_Y, TITLE_TWEEN_OUT_TIME, TITLE_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleTools", (OLED_WIDTH - strlen("Tools For You") * 6), OLED_TITLE_Start_Y, TITLE_TWEEN_OUT_TIME, TITLE_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleGames", (OLED_WIDTH - strlen("Epicful Games") * 6), OLED_TITLE_End_Y, TITLE_TWEEN_IN_TIME, TITLE_TWEEN_IN_STYLE);
            OLED_DoTweenObject(&g_AnimationManager, "TitleBGScale", strlen("Epicful Games") * 6 + 3, 8, TITLE_BG_TWEEN_TIME, TITLE_BG_TWEEN_STYLE);
            OLED_DoTweenObject(&g_AnimationManager, "CursorScale", strlen("Games") * 6 + 3, 10, CURSORBG_TWEEN_TIME, CURSORBG_TWEEN_STYLE);

            OLED_DoTweenObject(&Menu_AnimationManager, "IconGames", OLED_ICON_END_X, OLED_ICON_END_Y,OLED_ICON_TWEEN_IN_TIME, OLED_ICON_TWEEN_IN_STYLE);
            OLED_DoTweenObject(&Menu_AnimationManager, "IconTools", OLED_ICON_START_X, OLED_ICON_START_Y, OLED_ICON_TWEEN_OUT_TIME, OLED_ICON_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&Menu_AnimationManager, "IconStatus", OLED_ICON_START_X, OLED_ICON_START_Y, OLED_ICON_TWEEN_OUT_TIME, OLED_ICON_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&Menu_AnimationManager, "IconSettings", OLED_ICON_START_X, OLED_ICON_START_Y, OLED_ICON_TWEEN_OUT_TIME, OLED_ICON_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&Menu_AnimationManager, "IconAbout", OLED_ICON_START_X, OLED_ICON_START_Y, OLED_ICON_TWEEN_OUT_TIME, OLED_ICON_TWEEN_OUT_STYLE);
            break;
        case 2:
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleBack", (OLED_WIDTH - strlen("Back To Menu") * 6), OLED_TITLE_Start_Y, TITLE_TWEEN_OUT_TIME, TITLE_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleSettings", (OLED_WIDTH - strlen("STM Settings") * 6), OLED_TITLE_Start_Y, 1, TITLE_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleAbout", (OLED_WIDTH - strlen("Developer!") * 6), OLED_TITLE_Start_Y, 1, TITLE_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleStatus", (OLED_WIDTH - strlen("Machine States") * 6), OLED_TITLE_Start_Y, TITLE_TWEEN_OUT_TIME, TITLE_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleGames", (OLED_WIDTH - strlen("Epicful Games") * 6), OLED_TITLE_Start_Y, TITLE_TWEEN_OUT_TIME, TITLE_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleTools", (OLED_WIDTH - strlen("Tools For You") * 6), OLED_TITLE_End_Y, TITLE_TWEEN_IN_TIME, TITLE_TWEEN_IN_STYLE);
            OLED_DoTweenObject(&g_AnimationManager, "TitleBGScale", strlen("Tools for you") * 6 + 3, 8, TITLE_BG_TWEEN_TIME, TITLE_BG_TWEEN_STYLE);
            OLED_DoTweenObject(&g_AnimationManager, "CursorScale", strlen("Tools") * 6 + 3, 10, CURSORBG_TWEEN_TIME, CURSORBG_TWEEN_STYLE);

            OLED_DoTweenObject(&Menu_AnimationManager, "IconGames", OLED_ICON_START_X, OLED_ICON_START_Y, OLED_ICON_TWEEN_OUT_TIME, OLED_ICON_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&Menu_AnimationManager, "IconTools", OLED_ICON_END_X, OLED_ICON_END_Y, OLED_ICON_TWEEN_IN_TIME, OLED_ICON_TWEEN_IN_STYLE);
            OLED_DoTweenObject(&Menu_AnimationManager, "IconStatus", OLED_ICON_START_X, OLED_ICON_START_Y, OLED_ICON_TWEEN_OUT_TIME, OLED_ICON_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&Menu_AnimationManager, "IconSettings", OLED_ICON_START_X, OLED_ICON_START_Y, OLED_ICON_TWEEN_OUT_TIME, OLED_ICON_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&Menu_AnimationManager, "IconAbout", OLED_ICON_START_X, OLED_ICON_START_Y, OLED_ICON_TWEEN_OUT_TIME, OLED_ICON_TWEEN_OUT_STYLE);
            break;
        case 3:
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleBack", (OLED_WIDTH - strlen("Back To Menu") * 6), OLED_TITLE_Start_Y, TITLE_TWEEN_OUT_TIME, TITLE_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleGames", (OLED_WIDTH - strlen("Epicful Games") * 6), OLED_TITLE_Start_Y, 1, TITLE_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleAbout", (OLED_WIDTH - strlen("Developer!") * 6), OLED_TITLE_Start_Y, 1, TITLE_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleSettings", (OLED_WIDTH - strlen("STM Settings") * 6), OLED_TITLE_Start_Y, TITLE_TWEEN_OUT_TIME, TITLE_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleTools", (OLED_WIDTH - strlen("Tools For You") * 6), OLED_TITLE_Start_Y, TITLE_TWEEN_OUT_TIME, TITLE_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleStatus", (OLED_WIDTH - strlen("Machine States") * 6), OLED_TITLE_End_Y, TITLE_TWEEN_IN_TIME, TITLE_TWEEN_IN_STYLE);
            OLED_DoTweenObject(&g_AnimationManager, "TitleBGScale", strlen("Machine States") * 6 + 3, 8, TITLE_BG_TWEEN_TIME, TITLE_BG_TWEEN_STYLE);
            OLED_DoTweenObject(&g_AnimationManager, "CursorScale", strlen("States") * 6 + 3, 10, CURSORBG_TWEEN_TIME, CURSORBG_TWEEN_STYLE);

            OLED_DoTweenObject(&Menu_AnimationManager, "IconGames", OLED_ICON_START_X, OLED_ICON_START_Y, OLED_ICON_TWEEN_OUT_TIME, OLED_ICON_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&Menu_AnimationManager, "IconTools", OLED_ICON_START_X, OLED_ICON_START_Y, OLED_ICON_TWEEN_OUT_TIME, OLED_ICON_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&Menu_AnimationManager, "IconStatus", OLED_ICON_END_X, OLED_ICON_END_Y, OLED_ICON_TWEEN_IN_TIME, OLED_ICON_TWEEN_IN_STYLE);
            OLED_DoTweenObject(&Menu_AnimationManager, "IconSettings", OLED_ICON_START_X, OLED_ICON_START_Y, OLED_ICON_TWEEN_OUT_TIME, OLED_ICON_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&Menu_AnimationManager, "IconAbout", OLED_ICON_START_X, OLED_ICON_START_Y, OLED_ICON_TWEEN_OUT_TIME, OLED_ICON_TWEEN_OUT_STYLE);
            break;
        case 4:
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleBack", (OLED_WIDTH - strlen("Back To Menu") * 6), OLED_TITLE_Start_Y, TITLE_TWEEN_OUT_TIME, TITLE_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleGames", (OLED_WIDTH - strlen("Epicful Games") * 6), OLED_TITLE_Start_Y, 1, TITLE_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleTools", (OLED_WIDTH - strlen("Tools For You") * 6), OLED_TITLE_Start_Y, 1, TITLE_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleStatus", (OLED_WIDTH - strlen("Machine States") * 6), OLED_TITLE_Start_Y, TITLE_TWEEN_OUT_TIME, TITLE_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleAbout", (OLED_WIDTH - strlen("Developer!") * 6), OLED_TITLE_Start_Y, TITLE_TWEEN_OUT_TIME, TITLE_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleSettings", (OLED_WIDTH - strlen("STM Settings") * 6), OLED_TITLE_End_Y, TITLE_TWEEN_IN_TIME, TITLE_TWEEN_IN_STYLE);
            OLED_DoTweenObject(&g_AnimationManager, "TitleBGScale", strlen("STM Settings") * 6 + 3, 8, TITLE_BG_TWEEN_TIME, TITLE_BG_TWEEN_STYLE);
            OLED_DoTweenObject(&g_AnimationManager, "CursorScale", strlen("Settings") * 6 + 3, 10, CURSORBG_TWEEN_TIME, CURSORBG_TWEEN_STYLE);

            OLED_DoTweenObject(&Menu_AnimationManager, "IconGames", OLED_ICON_START_X, OLED_ICON_START_Y, OLED_ICON_TWEEN_OUT_TIME, OLED_ICON_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&Menu_AnimationManager, "IconTools", OLED_ICON_START_X, OLED_ICON_START_Y, OLED_ICON_TWEEN_OUT_TIME, OLED_ICON_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&Menu_AnimationManager, "IconStatus", OLED_ICON_START_X, OLED_ICON_START_Y, OLED_ICON_TWEEN_OUT_TIME, OLED_ICON_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&Menu_AnimationManager, "IconSettings", OLED_ICON_END_X, OLED_ICON_END_Y, OLED_ICON_TWEEN_IN_TIME, OLED_ICON_TWEEN_IN_STYLE);
            OLED_DoTweenObject(&Menu_AnimationManager, "IconAbout", OLED_ICON_START_X, OLED_ICON_START_Y, OLED_ICON_TWEEN_OUT_TIME, OLED_ICON_TWEEN_OUT_STYLE);
            break;
        case 5:
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleBack", (OLED_WIDTH - strlen("Back To Menu") * 6), OLED_TITLE_Start_Y, TITLE_TWEEN_OUT_TIME, TITLE_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleTools", (OLED_WIDTH - strlen("Tools For You") * 6), OLED_TITLE_Start_Y, 1, TITLE_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleStatus", (OLED_WIDTH - strlen("Machine States") * 6), OLED_TITLE_Start_Y, 1, TITLE_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleSettings", (OLED_WIDTH - strlen("STM Settings") * 6), OLED_TITLE_Start_Y, TITLE_TWEEN_OUT_TIME, TITLE_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleGames", (OLED_WIDTH - strlen("Epicful Games") * 6), OLED_TITLE_Start_Y, TITLE_TWEEN_OUT_TIME, TITLE_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleAbout", (OLED_WIDTH - strlen("Developer!") * 6), OLED_TITLE_End_Y, OLED_ICON_TWEEN_IN_TIME, OLED_ICON_TWEEN_IN_STYLE);
            OLED_DoTweenObject(&g_AnimationManager, "TitleBGScale", strlen("Developer!") * 6 + 3, 8, TITLE_BG_TWEEN_TIME, TITLE_BG_TWEEN_STYLE);
            OLED_DoTweenObject(&g_AnimationManager, "CursorScale", strlen("About") * 6 + 3, 10, CURSORBG_TWEEN_TIME, CURSORBG_TWEEN_STYLE);

            OLED_DoTweenObject(&Menu_AnimationManager, "IconGames", OLED_ICON_START_X, OLED_ICON_START_Y, OLED_ICON_TWEEN_OUT_TIME, OLED_ICON_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&Menu_AnimationManager, "IconTools", OLED_ICON_START_X, OLED_ICON_START_Y, OLED_ICON_TWEEN_OUT_TIME, OLED_ICON_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&Menu_AnimationManager, "IconStatus", OLED_ICON_START_X, OLED_ICON_START_Y, OLED_ICON_TWEEN_OUT_TIME, OLED_ICON_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&Menu_AnimationManager, "IconSettings", OLED_ICON_START_X, OLED_ICON_START_Y, OLED_ICON_TWEEN_OUT_TIME, OLED_ICON_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&Menu_AnimationManager, "IconAbout", OLED_ICON_END_X, OLED_ICON_END_Y, OLED_ICON_TWEEN_OUT_TIME, OLED_ICON_TWEEN_OUT_STYLE);
            break;
        default:
            break;
        }
    }
    else if (menuRank == 2 && currentPage == UI_PAGE_ABOUT)
    {
        switch (menuSelection)
        {
        case 1:
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleAbout", (OLED_WIDTH - strlen("Developer!") * 6), OLED_TITLE_Start_Y, TITLE_TWEEN_OUT_TIME, TITLE_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleDeveloper", (OLED_WIDTH - strlen("DEVELOPER: UnikoZera") * 6), OLED_TITLE_End_Y, TITLE_TWEEN_IN_TIME, TITLE_TWEEN_IN_STYLE);
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleGithub", (OLED_WIDTH - strlen("GH Address") * 6), OLED_TITLE_Start_Y, TITLE_TWEEN_OUT_TIME, TITLE_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleDonate", (OLED_WIDTH - strlen("BiliBili Space") * 6), OLED_TITLE_Start_Y, TITLE_TWEEN_OUT_TIME, TITLE_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleBack", (OLED_WIDTH - strlen("Back To Menu") * 6), OLED_TITLE_Start_Y, TITLE_TWEEN_OUT_TIME, TITLE_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&g_AnimationManager, "TitleBGScale", strlen("DEVELOPER: UnikoZera") * 6 + 3, 8, TITLE_BG_TWEEN_TIME, TITLE_BG_TWEEN_STYLE);
            OLED_DoTweenObject(&g_AnimationManager, "CursorScale", strlen("Developer") * 6 + 3, 10, CURSORBG_TWEEN_TIME, CURSORBG_TWEEN_STYLE);
            break;
        case 2:
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleDeveloper", (OLED_WIDTH - strlen("DEVELOPER: UnikoZera") * 6), OLED_TITLE_Start_Y, TITLE_TWEEN_OUT_TIME, TITLE_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleGithub", (OLED_WIDTH - strlen("GH Address") * 6), OLED_TITLE_End_Y, TITLE_TWEEN_IN_TIME, TITLE_TWEEN_IN_STYLE);
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleDonate", (OLED_WIDTH - strlen("BiliBili Space") * 6), OLED_TITLE_Start_Y, TITLE_TWEEN_OUT_TIME, TITLE_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleBack", (OLED_WIDTH - strlen("Back To Menu") * 6), OLED_TITLE_Start_Y, TITLE_TWEEN_OUT_TIME, TITLE_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&g_AnimationManager, "TitleBGScale", strlen("GH Address") * 6 + 3, 8, TITLE_BG_TWEEN_TIME, TITLE_BG_TWEEN_STYLE);
            OLED_DoTweenObject(&g_AnimationManager, "CursorScale", strlen("Github") * 6 + 3, 10, CURSORBG_TWEEN_TIME, CURSORBG_TWEEN_STYLE);
            break;
        case 3:
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleDeveloper", (OLED_WIDTH - strlen("DEVELOPER: UnikoZera") * 6), OLED_TITLE_Start_Y, TITLE_TWEEN_OUT_TIME, TITLE_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleGithub", (OLED_WIDTH - strlen("GH Address") * 6), OLED_TITLE_Start_Y, TITLE_TWEEN_OUT_TIME, TITLE_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleDonate", (OLED_WIDTH - strlen("BiliBili Space") * 6), OLED_TITLE_End_Y, TITLE_TWEEN_IN_TIME, TITLE_TWEEN_IN_STYLE);
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleBack", (OLED_WIDTH - strlen("Back To Menu") * 6), OLED_TITLE_Start_Y, TITLE_TWEEN_OUT_TIME, TITLE_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&g_AnimationManager, "TitleBGScale", strlen("BiliBili Space") * 6 + 3, 8, TITLE_BG_TWEEN_TIME, TITLE_BG_TWEEN_STYLE);
            OLED_DoTweenObject(&g_AnimationManager, "CursorScale", strlen("Donate") * 6 + 3, 10, CURSORBG_TWEEN_TIME, CURSORBG_TWEEN_STYLE);
            break;
        case 4:
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleDeveloper", (OLED_WIDTH - strlen("DEVELOPER: UnikoZera") * 6), OLED_TITLE_Start_Y, TITLE_TWEEN_OUT_TIME, TITLE_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleGithub", (OLED_WIDTH - strlen("GH Address") * 6), OLED_TITLE_Start_Y, TITLE_TWEEN_OUT_TIME, TITLE_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleDonate", (OLED_WIDTH - strlen("BiliBili Space") * 6), OLED_TITLE_Start_Y, TITLE_TWEEN_OUT_TIME, TITLE_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleBack", (OLED_WIDTH - strlen("Back To Menu") * 6), OLED_TITLE_End_Y, TITLE_TWEEN_IN_TIME, TITLE_TWEEN_IN_STYLE);
            OLED_DoTweenObject(&g_AnimationManager, "TitleBGScale", strlen("Back To Menu") * 6 + 3, 8, TITLE_BG_TWEEN_TIME, TITLE_BG_TWEEN_STYLE);
            OLED_DoTweenObject(&g_AnimationManager, "CursorScale", strlen("Back") * 6 + 3, 10, CURSORBG_TWEEN_TIME, CURSORBG_TWEEN_STYLE);
            break;
        default:
            break;
        }
    }
    else if (menuRank == 3 && currentPage == UI_PAGE_ABOUT && menuSelection == 1)
    {

    }
    else if (menuRank == 3 && currentPage == UI_PAGE_ABOUT && menuSelection == 2)
    {

    }
    else if (menuRank == 3 && currentPage == UI_PAGE_ABOUT && menuSelection == 3)
    {

    }
    else if (menuRank == 2 && currentPage == UI_PAGE_TOOLS)
    {
        switch (menuSelection)
        {
        case 1:
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleTools", (OLED_WIDTH - strlen("Tools For You") * 6), OLED_TITLE_Start_Y, TITLE_TWEEN_OUT_TIME, TITLE_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&g_Title_AnimationManager, "BAD APPLE!", (OLED_WIDTH - strlen("BAD APPLE!") * 6), OLED_TITLE_End_Y, TITLE_TWEEN_IN_TIME, TITLE_TWEEN_IN_STYLE);
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleBack", (OLED_WIDTH - strlen("Back To Menu") * 6), OLED_TITLE_Start_Y, TITLE_TWEEN_OUT_TIME, TITLE_TWEEN_OUT_STYLE);            OLED_DoTweenObject(&g_AnimationManager, "TitleBGScale", strlen("BAD APPLE!") * 6 + 3, 8, 500, EASE_IN_CUBIC);
            OLED_DoTweenObject(&g_AnimationManager, "TitleBGScale", strlen("BAD APPLE!") * 6 + 3, 8, TITLE_BG_TWEEN_TIME, TITLE_BG_TWEEN_STYLE);
            OLED_DoTweenObject(&g_AnimationManager, "CursorScale", strlen("Video") * 6 + 3, 10, CURSORBG_TWEEN_TIME, CURSORBG_TWEEN_STYLE);
            break;
        case 2:
            OLED_DoTweenObject(&g_Title_AnimationManager, "BAD APPLE!", (OLED_WIDTH - strlen("BAD APPLE!") * 6), OLED_TITLE_Start_Y, TITLE_TWEEN_OUT_TIME, TITLE_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleBack", (OLED_WIDTH - strlen("Back To Menu") * 6), OLED_TITLE_End_Y, TITLE_TWEEN_IN_TIME, TITLE_TWEEN_IN_STYLE);
            OLED_DoTweenObject(&g_AnimationManager, "TitleBGScale", strlen("Back To Menu") * 6 + 3, 8, TITLE_BG_TWEEN_TIME, TITLE_BG_TWEEN_STYLE);
            OLED_DoTweenObject(&g_AnimationManager, "CursorScale", strlen("Back") * 6 + 3, 10, CURSORBG_TWEEN_TIME, CURSORBG_TWEEN_STYLE);
            break;
        default:
            break;
        }
    }
    else if (menuRank == 2 && currentPage == UI_PAGE_GAMES)
    {
        switch (menuSelection)
        {
        case 1:
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleGames", (OLED_WIDTH - strlen("Epicful Games") * 6), OLED_TITLE_Start_Y, TITLE_TWEEN_OUT_TIME, TITLE_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleSnake", (OLED_WIDTH - strlen("Greedy Snake") * 6), OLED_TITLE_End_Y, TITLE_TWEEN_IN_TIME, TITLE_TWEEN_IN_STYLE);
            OLED_DoTweenObject(&g_Title_AnimationManager, "Title2048", (OLED_WIDTH - strlen("Here is 2048!") * 6), OLED_TITLE_Start_Y, TITLE_TWEEN_OUT_TIME, TITLE_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleBack", (OLED_WIDTH - strlen("Back To Menu") * 6), OLED_TITLE_Start_Y, TITLE_TWEEN_OUT_TIME, TITLE_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&g_AnimationManager, "TitleBGScale", strlen("Greedy Snake") * 6 + 3, 8, TITLE_BG_TWEEN_TIME, TITLE_BG_TWEEN_STYLE);
            OLED_DoTweenObject(&g_AnimationManager, "CursorScale", strlen("Snake") * 6 + 3, 10, CURSORBG_TWEEN_TIME, CURSORBG_TWEEN_STYLE);
            break;
        case 2:
            OLED_DoTweenObject(&g_Title_AnimationManager, "Title2048", (OLED_WIDTH - strlen("Here is 2048!") * 6), OLED_TITLE_End_Y, TITLE_TWEEN_IN_TIME, TITLE_TWEEN_IN_STYLE);
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleSnake", (OLED_WIDTH - strlen("Greedy Snake") * 6), OLED_TITLE_Start_Y, TITLE_TWEEN_OUT_TIME, TITLE_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleBack", (OLED_WIDTH - strlen("Back To Menu") * 6), OLED_TITLE_Start_Y,TITLE_TWEEN_OUT_TIME, TITLE_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&g_AnimationManager, "TitleBGScale", strlen("Here is 2048!") * 6 + 3, 8, TITLE_BG_TWEEN_TIME, TITLE_BG_TWEEN_STYLE);
            OLED_DoTweenObject(&g_AnimationManager, "CursorScale", strlen("2048") * 6 + 3, 10, CURSORBG_TWEEN_TIME, CURSORBG_TWEEN_STYLE);
            break;
        case 3:
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleBack", (OLED_WIDTH - strlen("Back To Menu") * 6), OLED_TITLE_End_Y, TITLE_TWEEN_IN_TIME, TITLE_TWEEN_IN_STYLE);
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleSnake", (OLED_WIDTH - strlen("Greedy Snake") * 6), OLED_TITLE_Start_Y, TITLE_TWEEN_OUT_TIME, TITLE_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&g_Title_AnimationManager, "Title2048", (OLED_WIDTH - strlen("Here is 2048!") * 6), OLED_TITLE_Start_Y, TITLE_TWEEN_OUT_TIME, TITLE_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&g_AnimationManager, "TitleBGScale", strlen("Back To Menu") * 6 + 3, 8, TITLE_BG_TWEEN_TIME, TITLE_BG_TWEEN_STYLE);
            OLED_DoTweenObject(&g_AnimationManager, "CursorScale", strlen("Back") * 6 + 3, 10, CURSORBG_TWEEN_TIME, CURSORBG_TWEEN_STYLE);
            break;
        default:
            break;
        }
    }
    else if (menuRank == 3 && currentPage == UI_PAGE_GAMES && menuSelection == 1)
    {
        cancelSelection = true;
        Snake_GameLoop();
    }
    else if (menuRank == 3 && currentPage == UI_PAGE_GAMES && menuSelection == 2)
    {
        cancelSelection = true;
        Game2048_GameLoop();
    }
    else if (menuRank == 3 && currentPage == UI_PAGE_TOOLS && menuSelection == 1)
    {
        cancelSelection = true;
        play_video();
    }
    else if (menuRank == 2 && currentPage == UI_PAGE_STATUS)
    {
        OLED_DoTweenObject(&g_AnimationManager, "TitleBGScale", 0, 8, TITLE_BG_TWEEN_TIME, TITLE_BG_TWEEN_STYLE);
        OLED_DoTweenObject(&g_AnimationManager, "CursorScale", 0, 8, CURSORBG_TWEEN_TIME, CURSORBG_TWEEN_STYLE);

        OLED_DoTweenObject(&Status_AnimationManager, "RunningTime", OLED_UI_END_X, 1, BASIC_TWEEN_IN_TIME, BASIC_TWEEN_IN_STYLE);
        OLED_DoTweenObject(&Status_AnimationManager, "Status", 0, OLED_UI_START_Y + OLED_UI_GAP_Y * 1 + 6, BASIC_TWEEN_IN_TIME, BASIC_TWEEN_IN_STYLE);

        OLED_DoTweenObject(&Status_AnimationManager, "CUBE", 113, 50, BASIC_TWEEN_IN_TIME, BASIC_TWEEN_IN_STYLE);
    }
    else if (menuRank == 2 && currentPage == UI_PAGE_SETTINGS)
    {
        switch (menuSelection)
        {
        case 1:
            OLED_DoTweenObject(&g_Title_AnimationManager, "TitleSettings", (OLED_WIDTH - strlen("STM Settings") * 6), OLED_TITLE_Start_Y, TITLE_TWEEN_OUT_TIME, TITLE_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&g_Title_AnimationManager, "Show FPS?", (OLED_WIDTH - strlen("Show FPS?") * 6), OLED_TITLE_End_Y, TITLE_TWEEN_IN_TIME, TITLE_TWEEN_IN_STYLE);
            OLED_DoTweenObject(&g_AnimationManager, "TitleBGScale", strlen("Show FPS?") * 6 + 3, 8, TITLE_BG_TWEEN_TIME, TITLE_BG_TWEEN_STYLE);
            OLED_DoTweenObject(&g_AnimationManager, "CursorScale", strlen("Show FPS:OFF") * 6 + 3, 10, CURSORBG_TWEEN_TIME, CURSORBG_TWEEN_STYLE);
            OLED_DoTweenObject(&Settings_AnimationManager, "Bar", 5, OLED_HEIGHT + 10, BASIC_TWEEN_OUT_TIME, BASIC_TWEEN_OUT_STYLE);
            break;
        case 2:
            OLED_DoTweenObject(&g_Title_AnimationManager, "Show FPS?", (OLED_WIDTH - strlen("Show FPS?") * 6), OLED_TITLE_Start_Y, TITLE_TWEEN_OUT_TIME, TITLE_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&g_AnimationManager, "TitleBGScale", 0, 8, TITLE_BG_TWEEN_TIME, TITLE_BG_TWEEN_STYLE);
            OLED_DoTweenObject(&g_AnimationManager, "CursorScale", strlen("Lightness: 100%") * 6 + 3, 10, CURSORBG_TWEEN_TIME, CURSORBG_TWEEN_STYLE);
            OLED_DoTweenObject(&Settings_AnimationManager, "Bar", 5, OLED_HEIGHT + 10, BASIC_TWEEN_OUT_TIME, BASIC_TWEEN_OUT_STYLE);
            break;
        case 3:
            OLED_DoTweenObject(&g_Title_AnimationManager, "Show FPS?", (OLED_WIDTH - strlen("Show FPS?") * 6), OLED_TITLE_Start_Y, TITLE_TWEEN_OUT_TIME, TITLE_TWEEN_OUT_STYLE);
            OLED_DoTweenObject(&g_AnimationManager, "TitleBGScale", 0, 8, TITLE_BG_TWEEN_TIME, TITLE_BG_TWEEN_STYLE);
            OLED_DoTweenObject(&g_AnimationManager, "CursorScale", strlen("Back") * 6 + 3, 10, CURSORBG_TWEEN_TIME, CURSORBG_TWEEN_STYLE);
            OLED_DoTweenObject(&Settings_AnimationManager, "Bar", 5, OLED_HEIGHT + 10, BASIC_TWEEN_OUT_TIME, BASIC_TWEEN_OUT_STYLE);
            break;
        default:
            break;
        }
    }
    else if (menuRank == 3 && currentPage == UI_PAGE_SETTINGS && menuSelection == 2)
    {
        OLED_DoTweenObject(&Settings_AnimationManager, "Bar", 5, OLED_HEIGHT - 9, BASIC_TWEEN_IN_TIME, BASIC_TWEEN_IN_STYLE);
    }
    
    if (menuRank != 1)
    {
        OLED_DoTweenObject(&Menu_AnimationManager, "IconGames", OLED_ICON_START_X, OLED_ICON_START_Y, OLED_ICON_TWEEN_OUT_TIME, OLED_ICON_TWEEN_OUT_STYLE);
        OLED_DoTweenObject(&Menu_AnimationManager, "IconTools", OLED_ICON_START_X, OLED_ICON_START_Y, OLED_ICON_TWEEN_OUT_TIME, OLED_ICON_TWEEN_OUT_STYLE);
        OLED_DoTweenObject(&Menu_AnimationManager, "IconStatus", OLED_ICON_START_X, OLED_ICON_START_Y, OLED_ICON_TWEEN_OUT_TIME, OLED_ICON_TWEEN_OUT_STYLE);
        OLED_DoTweenObject(&Menu_AnimationManager, "IconSettings", OLED_ICON_START_X, OLED_ICON_START_Y, OLED_ICON_TWEEN_OUT_TIME, OLED_ICON_TWEEN_OUT_STYLE);
        OLED_DoTweenObject(&Menu_AnimationManager, "IconAbout", OLED_ICON_START_X, OLED_ICON_START_Y, OLED_ICON_TWEEN_OUT_TIME, OLED_ICON_TWEEN_OUT_STYLE);
    }
    
    
#pragma endregion PlayerSelection

#pragma region ResetAnimation //完成换级时候让动画复位

    if (resetAnimation && !(OLED_GetAnimationStates(&Games_AnimationManager, "Snake") || OLED_GetAnimationStates(&Status_AnimationManager, "RunningTime") || OLED_GetAnimationStates(&Settings_AnimationManager, "ShowFPS") || OLED_GetAnimationStates(&About_AnimationManager, "Developer") || OLED_GetAnimationStates(&Tools_AnimationManager, "VideoPlayer")))
    {
        OLED_DoTweenObject(&Games_AnimationManager, "Snake", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 1, 1, EASE_IN_CIRC);
        OLED_DoTweenObject(&Games_AnimationManager, "2048", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 2, 1, EASE_IN_CIRC);


        OLED_DoTweenObject(&About_AnimationManager, "Developer", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 1, 1, EASE_IN_CIRC);
        OLED_DoTweenObject(&About_AnimationManager, "Github", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 2, 1, EASE_IN_CIRC);
        OLED_DoTweenObject(&About_AnimationManager, "Donate", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 3, 1, EASE_IN_CIRC);

        OLED_DoTweenObject(&Tools_AnimationManager, "VideoPlayer", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 1, 1, EASE_IN_CIRC);

        OLED_DoTweenObject(&Settings_AnimationManager, "ShowFPS", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 1, 1, EASE_IN_CIRC);
        OLED_DoTweenObject(&Settings_AnimationManager, "Lightness", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 2, 1, EASE_IN_CIRC);
        resetAnimation = false;
    }

    if (menuRank == 3 && currentPage == UI_PAGE_SETTINGS)
    {

        OLED_SendCommand(0x81); 
        OLED_SendCommand(brightness); 
    }
    

#pragma endregion ResetAnimation

    preMenuSelection = menuSelection;
}
#define RollScale 5
void SystemGetsSignal() // 这里是旋钮数据的获取
{
#pragma region 管理翻页
    static uint16_t preCount = 32767;
    uint16_t pageCount;
    if (currentPage == UI_PAGE_MENU)
    {
        pageCount = MainMenuItemCount;
    }
    else if (currentPage == UI_PAGE_SETTINGS)
    {
        pageCount = SettingsItemCount;
    }
    else if (currentPage == UI_PAGE_ABOUT)
    {
        pageCount = AboutItemCount;
    }
    else if (currentPage == UI_PAGE_TOOLS)
    {
        pageCount = ToolsItemCount;
    }
    else if (currentPage == UI_PAGE_GAMES)
    {
        pageCount = GamesItemCount;
    }

    if (!cancelSelection)
    {
        if (count - preCount > 1)
        {
            if (menuSelection < pageCount)
            {
                menuSelection++;
            }
            else
            {
                menuSelection = 1;
            }

            preCount = count;
        }
        else if (count - preCount < -1)
        {

            if (menuSelection > 1)
            {
                menuSelection--;
            }
            else
            {
                menuSelection = pageCount;
            }

            preCount = count;
        }
    }
    else if (cancelSelection && currentPage == UI_PAGE_SETTINGS && menuRank == 3)
    {
        if (brightness + RollScale >= 255 && count - preCount > 1)
        {
            brightness = 255;
            preCount = count;
        }
        else if (brightness - RollScale <= 0 && count - preCount < -1)
        {
            brightness = 0;
            preCount = count;
        }
        else if (count - preCount > 1)
        {
            brightness += RollScale;
            preCount = count;
        }
        else if (count - preCount < -1)
        {
            brightness -= RollScale;
            preCount = count;
        }
    }
    else
        preCount = count;


#pragma endregion 管理翻页

    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12) == GPIO_PIN_RESET)
    {
        HAL_Delay(50); // 防抖动延时
        if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12) == GPIO_PIN_RESET && !gameOver && !game2048_game_over)
        {
// #pragma region 游戏部分
            if (menuRank == 1 && menuSelection == 1)
            {
                menuRank = 2;
                menuSelection = 1;
                currentPage = UI_PAGE_GAMES;
                OLED_DoTweenObject(&Menu_AnimationManager, "GamesButton", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (2 - menuSelection), BASIC_TWEEN_OUT_TIME, BASIC_TWEEN_OUT_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&Menu_AnimationManager, "ToolsButton", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (3 - menuSelection), BASIC_TWEEN_OUT_TIME, BASIC_TWEEN_OUT_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&Menu_AnimationManager, "StatusButton", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (4 - menuSelection), BASIC_TWEEN_OUT_TIME, BASIC_TWEEN_OUT_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&Menu_AnimationManager, "SettingsButton", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (5 - menuSelection), BASIC_TWEEN_OUT_TIME, BASIC_TWEEN_OUT_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&Menu_AnimationManager, "AboutButton", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (6 - menuSelection), BASIC_TWEEN_OUT_TIME, BASIC_TWEEN_OUT_STYLE);

                OLED_DoTweenObject(&Games_AnimationManager, "Snake", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 1, BASIC_TWEEN_IN_TIME, BASIC_TWEEN_IN_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&Games_AnimationManager, "2048", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 2, BASIC_TWEEN_IN_TIME, BASIC_TWEEN_IN_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObjectY(&g_AnimationManager, "BackButton", OLED_UI_START_Y + OLED_UI_GAP_Y * 3, 1, BASIC_TWEEN_IN_STYLE);
                OLED_DoTweenObjectX(&g_AnimationManager, "BackButton", OLED_UI_END_X, BASIC_TWEEN_IN_TIME, BASIC_TWEEN_IN_STYLE);
            }
            else if (menuRank == 2 && menuSelection == 3 && currentPage == UI_PAGE_GAMES)
            {
                menuRank = 1;
                menuSelection = 1;
                currentPage = UI_PAGE_MENU;
                OLED_DoTweenObject(&Games_AnimationManager, "Snake", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * -1, BASIC_TWEEN_OUT_TIME, BASIC_TWEEN_OUT_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&Games_AnimationManager, "2048", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 0, BASIC_TWEEN_OUT_TIME, BASIC_TWEEN_OUT_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&g_AnimationManager, "BackButton", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 1, BASIC_TWEEN_OUT_TIME, BASIC_TWEEN_OUT_STYLE);

                OLED_DoTweenObject(&Menu_AnimationManager, "GamesButton", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 1, BASIC_TWEEN_IN_TIME, BASIC_TWEEN_IN_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&Menu_AnimationManager, "ToolsButton", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 2, BASIC_TWEEN_IN_TIME, BASIC_TWEEN_IN_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&Menu_AnimationManager, "StatusButton", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 3, BASIC_TWEEN_IN_TIME, BASIC_TWEEN_IN_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&Menu_AnimationManager, "SettingsButton", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 4, BASIC_TWEEN_IN_TIME, BASIC_TWEEN_IN_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&Menu_AnimationManager, "AboutButton", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 5, BASIC_TWEEN_IN_TIME, BASIC_TWEEN_IN_STYLE);
                resetAnimation = true;
            }
            else if (menuRank == 2 && menuSelection == 1 && currentPage == UI_PAGE_GAMES)
            {
                menuRank = 3;
                currentPage = UI_PAGE_GAMES;
                Snake_InitGame();
                OLED_DoTweenObject(&g_AnimationManager, "TitleBGScale", 0, 8, TITLE_BG_TWEEN_TIME, TITLE_BG_TWEEN_STYLE);
                OLED_DoTweenObject(&g_AnimationManager, "CursorScale", 0, 8, CURSORBG_TWEEN_TIME, CURSORBG_TWEEN_STYLE);
                OLED_DoTweenObject(&Games_AnimationManager, "Snake", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 1, BASIC_TWEEN_OUT_TIME, BASIC_TWEEN_OUT_STYLE);
                OLED_DoTweenObject(&g_Title_AnimationManager, "TitleSnake", (OLED_WIDTH - strlen("Greedy Snake") * 6), OLED_TITLE_Start_Y, TITLE_TWEEN_OUT_TIME, TITLE_TWEEN_OUT_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&Games_AnimationManager, "2048", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 2, BASIC_TWEEN_OUT_TIME, BASIC_TWEEN_OUT_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&g_AnimationManager, "BackButton", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 3, BASIC_TWEEN_OUT_TIME, BASIC_TWEEN_OUT_STYLE);
            }
            else if (menuRank == 3 && menuSelection == 1 && cancelSelection && UI_PAGE_GAMES && !gameOver && currentPage == UI_PAGE_GAMES)
            {
                menuRank = 2;
                currentPage = UI_PAGE_GAMES;
                OLED_DoTweenObject(&g_Title_AnimationManager, "TitleSnake", (OLED_WIDTH - strlen("Greedy Snake") * 6), OLED_TITLE_End_Y, TITLE_TWEEN_IN_TIME, TITLE_TWEEN_IN_STYLE);
                OLED_DoTweenObject(&g_AnimationManager, "TitleBGScale", strlen("Greedy Snake") * 6 + 3, 8, TITLE_BG_TWEEN_TIME, TITLE_BG_TWEEN_STYLE);
                OLED_DoTweenObject(&g_AnimationManager, "CursorScale", strlen("Snake") * 6 + 3, 10, CURSORBG_TWEEN_TIME, CURSORBG_TWEEN_STYLE);
                OLED_DoTweenObject(&Games_AnimationManager, "Snake", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 1, BASIC_TWEEN_IN_TIME, BASIC_TWEEN_IN_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&Games_AnimationManager, "2048", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 2, BASIC_TWEEN_IN_TIME, BASIC_TWEEN_IN_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&g_AnimationManager, "BackButton", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 3, BASIC_TWEEN_IN_TIME, BASIC_TWEEN_IN_STYLE);
                cancelSelection = false;
            }
            else if (menuRank == 2 && menuSelection == 2 && currentPage == UI_PAGE_GAMES)
            {
                menuRank = 3;
                currentPage = UI_PAGE_GAMES;
                Game2048_Init();
                OLED_DoTweenObject(&g_Title_AnimationManager, "Title2048", (OLED_WIDTH - strlen("Here is 2048!") * 6), OLED_TITLE_Start_Y, 1, TITLE_TWEEN_OUT_STYLE);
                OLED_DoTweenObject(&g_AnimationManager, "TitleBGScale", 0, 8, 1, TITLE_BG_TWEEN_STYLE);
                OLED_DoTweenObject(&g_AnimationManager, "CursorScale", 0, 8, 1, CURSORBG_TWEEN_STYLE);
                OLED_DoTweenObject(&Games_AnimationManager, "Snake", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 0, 1, BASIC_TWEEN_OUT_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&Games_AnimationManager, "2048", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 1, 1, BASIC_TWEEN_OUT_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&g_AnimationManager, "BackButton", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 2, 1, BASIC_TWEEN_OUT_STYLE);
            }
            else if (menuRank == 3 && menuSelection == 2 && cancelSelection && UI_PAGE_GAMES && !game2048_game_over && currentPage == UI_PAGE_GAMES)
            {
                menuRank = 2;
                currentPage = UI_PAGE_GAMES;
                OLED_DoTweenObject(&g_Title_AnimationManager, "Title2048", (OLED_WIDTH - strlen("Here is 2048!") * 6), OLED_TITLE_End_Y, TITLE_TWEEN_IN_TIME, TITLE_TWEEN_IN_STYLE);
                OLED_DoTweenObject(&g_AnimationManager, "TitleBGScale", strlen("Here is 2048!") * 6 + 3, 8, TITLE_BG_TWEEN_TIME, TITLE_BG_TWEEN_STYLE);
                OLED_DoTweenObject(&g_AnimationManager, "CursorScale", strlen("2048") * 6 + 3, 10, CURSORBG_TWEEN_TIME, CURSORBG_TWEEN_STYLE);
                OLED_DoTweenObject(&Games_AnimationManager, "Snake", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 0, BASIC_TWEEN_IN_TIME, BASIC_TWEEN_IN_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&Games_AnimationManager, "2048", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 1, BASIC_TWEEN_IN_TIME, BASIC_TWEEN_IN_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&g_AnimationManager, "BackButton", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 2, BASIC_TWEEN_IN_TIME, BASIC_TWEEN_IN_STYLE);
                cancelSelection = false;
            }
// #pragma endregion 游戏部分
// #pragma region 关于部分
            else if (menuRank == 1 && menuSelection == 5)
            {
                menuRank = 2;
                menuSelection = 1;
                currentPage = UI_PAGE_ABOUT;
                OLED_DoTweenObject(&Menu_AnimationManager, "GamesButton", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * -3, BASIC_TWEEN_OUT_TIME, BASIC_TWEEN_OUT_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&Menu_AnimationManager, "ToolsButton", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * -2, BASIC_TWEEN_OUT_TIME, BASIC_TWEEN_OUT_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&Menu_AnimationManager, "StatusButton", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * -1, BASIC_TWEEN_OUT_TIME, BASIC_TWEEN_OUT_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&Menu_AnimationManager, "SettingsButton", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 0, BASIC_TWEEN_OUT_TIME, BASIC_TWEEN_OUT_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&Menu_AnimationManager, "AboutButton", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 1, BASIC_TWEEN_OUT_TIME, BASIC_TWEEN_OUT_STYLE);

                OLED_DoTweenObject(&About_AnimationManager, "Developer", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 1, BASIC_TWEEN_IN_TIME, BASIC_TWEEN_IN_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&About_AnimationManager, "Github", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 2, BASIC_TWEEN_IN_TIME, BASIC_TWEEN_IN_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&About_AnimationManager, "Donate", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 3, BASIC_TWEEN_IN_TIME, BASIC_TWEEN_IN_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObjectY(&g_AnimationManager, "BackButton", OLED_UI_START_Y + OLED_UI_GAP_Y * 4, 1, BASIC_TWEEN_IN_STYLE);
                OLED_DoTweenObjectX(&g_AnimationManager, "BackButton", OLED_UI_END_X, BASIC_TWEEN_IN_TIME, BASIC_TWEEN_IN_STYLE);
            }
            else if (menuRank == 2 && menuSelection == 4 && currentPage == UI_PAGE_ABOUT)
            {
                menuRank = 1;
                menuSelection = 5;
                currentPage = UI_PAGE_MENU;
                OLED_DoTweenObject(&About_AnimationManager, "Developer", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * -2, BASIC_TWEEN_OUT_TIME, BASIC_TWEEN_OUT_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&About_AnimationManager, "Github", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * -1, BASIC_TWEEN_OUT_TIME, BASIC_TWEEN_OUT_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&About_AnimationManager, "Donate", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 0, BASIC_TWEEN_OUT_TIME, BASIC_TWEEN_OUT_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&g_AnimationManager, "BackButton", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 1, BASIC_TWEEN_OUT_TIME, BASIC_TWEEN_OUT_STYLE);

                OLED_DoTweenObject(&Menu_AnimationManager, "GamesButton", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (2 - menuSelection), BASIC_TWEEN_IN_TIME, BASIC_TWEEN_IN_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&Menu_AnimationManager, "ToolsButton", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (3 - menuSelection), BASIC_TWEEN_IN_TIME, BASIC_TWEEN_IN_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&Menu_AnimationManager, "StatusButton", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (4 - menuSelection), BASIC_TWEEN_IN_TIME, BASIC_TWEEN_IN_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&Menu_AnimationManager, "SettingsButton", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (5 - menuSelection), BASIC_TWEEN_IN_TIME, BASIC_TWEEN_IN_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&Menu_AnimationManager, "AboutButton", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (6 - menuSelection), BASIC_TWEEN_IN_TIME, BASIC_TWEEN_IN_STYLE);
                resetAnimation = true;
            }
            else if (menuRank == 2 && menuSelection == 1 && currentPage == UI_PAGE_ABOUT)
            {
                /* code */
            }
            else if (menuRank == 2 && menuSelection == 2 && currentPage == UI_PAGE_ABOUT)
            {
                /* code */
            }
            else if (menuRank == 2 && menuSelection == 3 && currentPage == UI_PAGE_ABOUT)
            {
                /* code */
            }
// #pragma endregion 关于部分
// #pragma region 工具部分
            else if (menuRank == 1 && menuSelection == 2)
            {
                menuRank = 2;
                menuSelection = 1;
                currentPage = UI_PAGE_TOOLS;
                OLED_DoTweenObject(&Menu_AnimationManager, "GamesButton", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 0, BASIC_TWEEN_OUT_TIME, BASIC_TWEEN_OUT_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&Menu_AnimationManager, "ToolsButton", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 1, BASIC_TWEEN_OUT_TIME, BASIC_TWEEN_OUT_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&Menu_AnimationManager, "StatusButton", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 2, BASIC_TWEEN_OUT_TIME, BASIC_TWEEN_OUT_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&Menu_AnimationManager, "SettingsButton", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 3, BASIC_TWEEN_OUT_TIME, BASIC_TWEEN_OUT_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&Menu_AnimationManager, "AboutButton", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 4, BASIC_TWEEN_OUT_TIME, BASIC_TWEEN_OUT_STYLE);

                OLED_DoTweenObject(&Tools_AnimationManager, "VideoPlayer", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 1, BASIC_TWEEN_IN_TIME, BASIC_TWEEN_IN_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObjectY(&g_AnimationManager, "BackButton", OLED_UI_START_Y + OLED_UI_GAP_Y * 2, 1, BASIC_TWEEN_IN_STYLE);
                OLED_DoTweenObjectX(&g_AnimationManager, "BackButton", OLED_UI_END_X, BASIC_TWEEN_IN_TIME, BASIC_TWEEN_IN_STYLE);
            }
            else if (menuRank == 2 && menuSelection == 1 && currentPage == UI_PAGE_TOOLS)
            {
                menuRank = 3;
                currentPage = UI_PAGE_TOOLS;
                video_first_play = true;
                OLED_DoTweenObject(&g_Title_AnimationManager, "BAD APPLE!", (OLED_WIDTH - strlen("BAD APPLE!") * 6), OLED_TITLE_Start_Y, 1, TITLE_TWEEN_OUT_STYLE);
                OLED_DoTweenObject(&g_AnimationManager, "TitleBGScale", 0, 8, 1, TITLE_BG_TWEEN_STYLE);
                OLED_DoTweenObject(&g_AnimationManager, "CursorScale", 0, 8, 1, CURSORBG_TWEEN_STYLE);
                OLED_DoTweenObject(&Tools_AnimationManager, "VideoPlayer", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 1, 1, BASIC_TWEEN_OUT_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObjectY(&g_AnimationManager, "BackButton", OLED_UI_START_Y + OLED_UI_GAP_Y * 2, 1, BASIC_TWEEN_OUT_STYLE);
                OLED_DoTweenObjectX(&g_AnimationManager, "BackButton", OLED_UI_START_X, 1, BASIC_TWEEN_OUT_STYLE);
            }
            else if (menuRank == 3 && menuSelection == 1 && cancelSelection && currentPage == UI_PAGE_TOOLS)
            {
                menuRank = 2;
                currentPage = UI_PAGE_TOOLS;
                OLED_DoTweenObject(&g_Title_AnimationManager, "BAD APPLE!", (OLED_WIDTH - strlen("BAD APPLE!") * 6), OLED_TITLE_End_Y, TITLE_TWEEN_IN_TIME, TITLE_TWEEN_IN_STYLE);
                OLED_DoTweenObject(&g_AnimationManager, "TitleBGScale", strlen("BAD APPLE!") * 6 + 3, 8, TITLE_BG_TWEEN_TIME, TITLE_BG_TWEEN_STYLE);
                OLED_DoTweenObject(&g_AnimationManager, "CursorScale", strlen("Video") * 6 + 3, 10, CURSORBG_TWEEN_TIME, CURSORBG_TWEEN_STYLE);
                OLED_DoTweenObject(&Tools_AnimationManager, "VideoPlayer", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 1, BASIC_TWEEN_IN_TIME, BASIC_TWEEN_IN_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObjectY(&g_AnimationManager, "BackButton", OLED_UI_START_Y + OLED_UI_GAP_Y * 2, 1, BASIC_TWEEN_IN_STYLE);
                OLED_DoTweenObjectX(&g_AnimationManager, "BackButton", OLED_UI_END_X, BASIC_TWEEN_IN_TIME, BASIC_TWEEN_IN_STYLE);
                cancelSelection = false;
            }
            else if (menuRank == 2 && menuSelection == 2 && currentPage == UI_PAGE_TOOLS)
            {
                menuRank = 1;
                menuSelection = 2;
                currentPage = UI_PAGE_MENU;
                OLED_DoTweenObject(&Tools_AnimationManager, "VideoPlayer", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 0, BASIC_TWEEN_OUT_TIME, BASIC_TWEEN_OUT_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&g_AnimationManager, "BackButton", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 1, BASIC_TWEEN_OUT_TIME, BASIC_TWEEN_OUT_STYLE);

                OLED_DoTweenObject(&Menu_AnimationManager, "GamesButton", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (2 - menuSelection), BASIC_TWEEN_IN_TIME, BASIC_TWEEN_IN_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&Menu_AnimationManager, "ToolsButton", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (3 - menuSelection), BASIC_TWEEN_IN_TIME, BASIC_TWEEN_IN_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&Menu_AnimationManager, "StatusButton", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (4 - menuSelection), BASIC_TWEEN_IN_TIME, BASIC_TWEEN_IN_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&Menu_AnimationManager, "SettingsButton", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (5 - menuSelection), BASIC_TWEEN_IN_TIME, BASIC_TWEEN_IN_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&Menu_AnimationManager, "AboutButton", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (6 - menuSelection), BASIC_TWEEN_IN_TIME, BASIC_TWEEN_IN_STYLE);
                resetAnimation = true;
            }
// #pragma endregion 工具部分
// #pragma region 状态部分
            else if (menuRank == 1 && menuSelection == 3)
            {
                menuRank = 2;
                menuSelection = 3;
                currentPage = UI_PAGE_STATUS;
                OLED_DoTweenObject(&g_Title_AnimationManager, "TitleStatus", (OLED_WIDTH - strlen("Machine States") * 6), OLED_TITLE_Start_Y, TITLE_TWEEN_OUT_TIME, TITLE_TWEEN_OUT_STYLE);
                OLED_DoTweenObject(&Menu_AnimationManager, "GamesButton", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * -1, BASIC_TWEEN_OUT_TIME, BASIC_TWEEN_OUT_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&Menu_AnimationManager, "ToolsButton", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 0, BASIC_TWEEN_OUT_TIME, BASIC_TWEEN_OUT_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&Menu_AnimationManager, "StatusButton", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 1, BASIC_TWEEN_OUT_TIME, BASIC_TWEEN_OUT_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&Menu_AnimationManager, "SettingsButton", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 2, BASIC_TWEEN_OUT_TIME, BASIC_TWEEN_OUT_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&Menu_AnimationManager, "AboutButton", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 3, BASIC_TWEEN_OUT_TIME, BASIC_TWEEN_OUT_STYLE);
            }
            else if (menuRank == 2 && currentPage == UI_PAGE_STATUS)
            {
                menuRank = 1;
                menuSelection = 3;
                currentPage = UI_PAGE_MENU;

                OLED_DoTweenObject(&g_Title_AnimationManager, "TitleStatus", (OLED_WIDTH - strlen("Status") * 6), OLED_TITLE_End_Y, TITLE_TWEEN_IN_TIME, TITLE_TWEEN_IN_STYLE);

                OLED_DoTweenObject(&Status_AnimationManager, "RunningTime", 0, -10, BASIC_TWEEN_OUT_TIME, BASIC_TWEEN_OUT_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&Status_AnimationManager, "Status", 0, OLED_HEIGHT, BASIC_TWEEN_OUT_TIME, BASIC_TWEEN_OUT_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&Status_AnimationManager, "CUBE", 150, 50, BASIC_TWEEN_OUT_TIME, BASIC_TWEEN_OUT_STYLE);

                OLED_DoTweenObject(&Menu_AnimationManager, "GamesButton", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * -1, BASIC_TWEEN_IN_TIME, BASIC_TWEEN_IN_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&Menu_AnimationManager, "ToolsButton", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 0, BASIC_TWEEN_IN_TIME, BASIC_TWEEN_IN_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&Menu_AnimationManager, "StatusButton", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 1, BASIC_TWEEN_IN_TIME, BASIC_TWEEN_IN_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&Menu_AnimationManager, "SettingsButton", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 2, BASIC_TWEEN_IN_TIME, BASIC_TWEEN_IN_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&Menu_AnimationManager, "AboutButton", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 3, BASIC_TWEEN_IN_TIME, BASIC_TWEEN_IN_STYLE);
                resetAnimation = true;
            }
// #pragma endregion 状态部分
// #pragma region 设置部分
            else if (menuRank == 1 && menuSelection == 4) 
            {
                menuRank = 2;
                menuSelection = 1;
                currentPage = UI_PAGE_SETTINGS;
                OLED_DoTweenObject(&Menu_AnimationManager, "GamesButton", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * -2, BASIC_TWEEN_OUT_TIME, BASIC_TWEEN_OUT_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&Menu_AnimationManager, "ToolsButton", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * -1, BASIC_TWEEN_OUT_TIME, BASIC_TWEEN_OUT_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&Menu_AnimationManager, "StatusButton", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 0, BASIC_TWEEN_OUT_TIME, BASIC_TWEEN_OUT_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&Menu_AnimationManager, "SettingsButton", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 1, BASIC_TWEEN_OUT_TIME, BASIC_TWEEN_OUT_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&Menu_AnimationManager, "AboutButton", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 2, BASIC_TWEEN_OUT_TIME, BASIC_TWEEN_OUT_STYLE);

                OLED_DoTweenObject(&g_Title_AnimationManager, "TitleSettings", (OLED_WIDTH - strlen("STM Settings") * 6), OLED_TITLE_Start_Y, TITLE_TWEEN_OUT_TIME, TITLE_TWEEN_OUT_STYLE);
                OLED_DoTweenObject(&Settings_AnimationManager, "ShowFPS", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 1, BASIC_TWEEN_IN_TIME, BASIC_TWEEN_IN_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&Settings_AnimationManager, "Lightness", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 2, BASIC_TWEEN_IN_TIME, BASIC_TWEEN_IN_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObjectY(&g_AnimationManager, "BackButton", OLED_UI_START_Y + OLED_UI_GAP_Y * 3, 1, BASIC_TWEEN_IN_STYLE);
                OLED_DoTweenObjectX(&g_AnimationManager, "BackButton", OLED_UI_END_X, BASIC_TWEEN_IN_TIME, BASIC_TWEEN_IN_STYLE);
            }
            else if (menuRank == 2 && menuSelection == 1 && currentPage == UI_PAGE_SETTINGS)
            {
                if (isShowFPS)
                {
                    isShowFPS = false;
                }
                else
                {
                    isShowFPS = true;
                }
            }
            else if (menuRank == 2 && menuSelection == 2 && currentPage == UI_PAGE_SETTINGS)
            {
                menuRank = 3;
                currentPage = UI_PAGE_SETTINGS;
                cancelSelection = true;
            }
            else if (menuRank == 3 && menuSelection == 2 && currentPage == UI_PAGE_SETTINGS)
            {
                menuRank = 2;
                currentPage = UI_PAGE_SETTINGS;
                cancelSelection = false;
            }
            else if (menuRank == 2 && menuSelection == 3 && currentPage == UI_PAGE_SETTINGS)
            {
                menuRank = 1;
                menuSelection = 4;
                currentPage = UI_PAGE_MENU;

                OLED_DoTweenObject(&g_Title_AnimationManager, "TitleSettings", (OLED_WIDTH - strlen("STM Settings") * 6), OLED_TITLE_End_Y, TITLE_TWEEN_IN_TIME, TITLE_TWEEN_IN_STYLE);
                OLED_DoTweenObject(&Settings_AnimationManager, "ShowFPS", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * -1, BASIC_TWEEN_OUT_TIME, BASIC_TWEEN_OUT_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&Settings_AnimationManager, "Lightness", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 0, BASIC_TWEEN_OUT_TIME, BASIC_TWEEN_OUT_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&g_AnimationManager, "BackButton", OLED_UI_START_X, OLED_UI_START_Y + OLED_UI_GAP_Y * 1, BASIC_TWEEN_OUT_TIME, BASIC_TWEEN_OUT_STYLE);

                OLED_DoTweenObject(&Menu_AnimationManager, "GamesButton", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (2 - menuSelection), TITLE_TWEEN_IN_TIME, TITLE_TWEEN_IN_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&Menu_AnimationManager, "ToolsButton", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (3 - menuSelection), TITLE_TWEEN_IN_TIME, TITLE_TWEEN_IN_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&Menu_AnimationManager, "StatusButton", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (4 - menuSelection), TITLE_TWEEN_IN_TIME, TITLE_TWEEN_IN_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&Menu_AnimationManager, "SettingsButton", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (5 - menuSelection), TITLE_TWEEN_IN_TIME, TITLE_TWEEN_IN_STYLE);
                HAL_Delay(100);
                OLED_DoTweenObject(&Menu_AnimationManager, "AboutButton", OLED_UI_END_X, OLED_UI_START_Y + OLED_UI_GAP_Y * (6 - menuSelection), TITLE_TWEEN_IN_TIME, TITLE_TWEEN_IN_STYLE);
                resetAnimation = true;
            }
// #pragma endregion 设置部分

            while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12) == GPIO_PIN_RESET)
            {
                // 等待按钮释放
                // 这里肯定会导致fps的下降233
            }
        }
    }
}
