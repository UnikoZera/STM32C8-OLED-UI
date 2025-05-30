/*
 * icon.h
 *
 *  Created on: May 30, 2025
 *      Author: UnikoZera
 */

#ifndef INC_ICON_H_
#define INC_ICON_H_


//水平扫描，从上往下，从左往右 36X36分辨率
extern unsigned char IMG_GAME_DATA[180]; 
extern unsigned char IMG_TOOL_DATA[180];
extern unsigned char IMG_STATUS_DATA[180];
extern unsigned char IMG_SETTINGS_DATA[180];
extern unsigned char IMG_ABOUT_DATA[180];


typedef enum
{
    ICON_GAME,
    ICON_TOOL,
    ICON_STATUS,
    ICON_SETTINGS,
    ICON_ABOUT,
} IconType_t;

#endif /* INC_ICON_H_ */
