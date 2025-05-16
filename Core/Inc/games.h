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
#include "oled.h"

// #define GAME_INPUT_UP_PORT GPIOA
// #define GAME_INPUT_UP_PIN GPIO_PIN_0 // Example: PA0
// #define GAME_INPUT_DOWN_PORT GPIOA
// #define GAME_INPUT_DOWN_PIN GPIO_PIN_1 // Example: PA1
// #define GAME_INPUT_LEFT_PORT GPIOA
// #define GAME_INPUT_LEFT_PIN GPIO_PIN_2 // Example: PA2
// #define GAME_INPUT_RIGHT_PORT GPIOA
// #define GAME_INPUT_RIGHT_PIN GPIO_PIN_3 // Example: PA3
#define GAME_BUTTON_PORT GPIOB
#define GAME_BUTTON_PIN GPIO_PIN_11
#define GAME_INPUT_RESTART_PORT GPIOB
#define GAME_INPUT_RESTART_PIN GPIO_PIN_12

#define SNAKE_MAX_LENGTH 100
#define SNAKE_BLOCK_SIZE 4 // 每个方块的大小（像素）
#define GAME_AREA_WIDTH (OLED_WIDTH / SNAKE_BLOCK_SIZE)
#define GAME_AREA_HEIGHT ((OLED_HEIGHT / SNAKE_BLOCK_SIZE) - 2)

typedef struct
{
    int16_t x;
    int16_t y;
} Point;

typedef enum
{
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
} Direction;

typedef struct
{
    Point body[SNAKE_MAX_LENGTH];
    uint16_t length;
    Direction dir;
} Snake;

typedef struct
{
    Point pos;
    bool active;
} Food;

extern bool gameOver;

void Snake_InitGame(void);
void Snake_GameLoop(void);
void Snake_HandleInput(Direction input_dir); // 假设我们有一个方式来获取输入方向

// Defines for 2048 Game
#define GAME2048_BOARD_SIZE 4 // Changed back to 4x4

#define GAME_BOARD_DISPLAY_HEIGHT (OLED_HEIGHT - 8) // Reserve 8px for score line, so 56px for board
// Adjusted TILE_WIDTH and TILE_HEIGHT for 4x4 board
#define GAME2048_TILE_WIDTH (OLED_WIDTH / GAME2048_BOARD_SIZE) 
#define GAME2048_TILE_HEIGHT (GAME_BOARD_DISPLAY_HEIGHT / GAME2048_BOARD_SIZE)
#define GAME2048_TILE_PADDING 1                               // Padding around tiles

typedef struct
{
    uint16_t value; // Tile value (0 for empty, 2, 4, 8, ...)
    // Add more properties if needed, e.g., for animation
} Tile_2048;

extern Tile_2048 game2048_board[GAME2048_BOARD_SIZE][GAME2048_BOARD_SIZE]; // Changed back to GAME2048_BOARD_SIZE
extern uint32_t game2048_score;
extern bool game2048_game_over;
extern bool game2048_game_won; // To check for 2048 tile

void Game2048_Init(void);
void Game2048_DrawBoard(void);
bool Game2048_HandleInput(Direction input_dir); // Returns true if a move was made
void Game2048_GameLoop(void);                   // Main loop for 2048
void Game2048_Restart(void);

#endif /* INC_GAMES_H_ */
