/*
 * oled_driver.h
 *
 *  Created on: May 10, 2025
 *      Author: UnikoZera
 */

#ifndef INC_OLED_DRIVER_H_
#define INC_OLED_DRIVER_H_

#include "games.h"
#include "oled_ui.h"
#include "oled.h"
#include "drawer.h"
#include "main.h"
#include "video_player.h"

extern OLED_Cube3D CUBE;

void System_UI_Loop();
void SystemGetsSignal(); //这里是旋钮数据的获取


#endif /* INC_OLED_DRIVER_H_ */
