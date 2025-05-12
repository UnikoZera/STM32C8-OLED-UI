/*
 * games.h
 *
 *  Created on: May 12, 2025
 *      Author: UnikoZera
 */

#ifndef INC_GAMES_H_
#define INC_GAMES_H_

#include <stdint.h>                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     
#include <stdbool.h>

#define SNAKE_MAX_LENGTH 100
#define SNAKE_BLOCK_SIZE 4 // 每个方块的大小（像素）
#define GAME_AREA_WIDTH (OLED_WIDTH / SNAKE_BLOCK_SIZE)
#define GAME_AREA_HEIGHT (OLED_HEIGHT / SNAKE_BLOCK_SIZE)

typedef struct {
    int16_t x;
    int16_t y;
} Point;

typedef enum {
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
} Direction;

typedef struct {
    Point body[SNAKE_MAX_LENGTH];
    uint16_t length;
    Direction dir;
} Snake;

typedef struct {
    Point pos;
    bool active;
} Food;

void Snake_InitGame(void);
void Snake_GameLoop(void);
void Snake_HandleInput(Direction input_dir); // 假设我们有一个方式来获取输入方向

#endif /* INC_GAMES_H_ */
