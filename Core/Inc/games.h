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
#define GAME2048_BOARD_SIZE 4
#define GAME2048_TILE_SIZE (OLED_WIDTH / GAME2048_BOARD_SIZE) // Assuming OLED_WIDTH is divisible by 4
#define GAME2048_TILE_PADDING 2                               // Padding around tiles

typedef struct
{
    uint16_t value; // Tile value (0 for empty, 2, 4, 8, ...)
    // Add more properties if needed, e.g., for animation
} Tile_2048;

extern Tile_2048 game2048_board[GAME2048_BOARD_SIZE][GAME2048_BOARD_SIZE];
extern uint32_t game2048_score;
extern bool game2048_game_over;
extern bool game2048_game_won; // To check for 2048 tile

void Game2048_Init(void);
void Game2048_DrawBoard(void);
bool Game2048_HandleInput(Direction input_dir); // Returns true if a move was made
void Game2048_GameLoop(void);                   // Main loop for 2048
void Game2048_Restart(void);

#endif /* INC_GAMES_H_ */
