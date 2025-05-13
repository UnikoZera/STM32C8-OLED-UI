/*
 * games.c
 *
 *  Created on: May 12, 2025
 *      Author: UnikoZera
 */

// Standard C libraries
#include <stdint.h>  // For uint32_t, int16_t, uint8_t, uint16_t etc.
#include <stdbool.h> // For bool type (true, false)
#include <stdlib.h>  // For rand(), srand()
#include <string.h>  // For memcpy(), memcmp(), strlen()

// STM32 HAL and project-specific headers
#include "main.h"    // <<< CRITICAL: This should include the STM32 HAL, defining GPIOA, GPIOB, HAL_GetTick etc.
#include "games.h"   // Game definitions (Snake, 2048, Point, Direction etc.)
#include "oled.h"    // OLED screen drawing functions
#include "oled_ui.h" // OLED UI elements (if used)

// Game state variables for Snake
static Snake snake;
static Food food;
bool gameOver;                // Shared for Snake, consider renaming or making specific
static uint32_t gameTickLast; // For controlling game speed
#define GAME_TICK_MS 100      // Snake moves every 100ms, adjust for speed

// Game state variables for 2048
Tile_2048 game2048_board[GAME2048_BOARD_SIZE][GAME2048_BOARD_SIZE]; // Use GAME2048_BOARD_SIZE
uint32_t game2048_score;
bool game2048_game_over;
bool game2048_game_won;
bool game2048_move_made_this_turn; // To track if a move was successful

bool FirstRun = true; // Flag to indicate if it's the first run of the game

// Debounce and Cooldown settings for input
#define INPUT_DEBOUNCE_MS 50             // Milliseconds for switch debounce
#define INPUT_PROCESSING_COOLDOWN_MS 200 // Minimum time between processing inputs

// Helper function to draw a block on the OLED (used by Snake)
static void DrawBlock(int16_t x, int16_t y, uint8_t color)
{
    OLED_DrawFilledRectangle(x * SNAKE_BLOCK_SIZE, y * SNAKE_BLOCK_SIZE, SNAKE_BLOCK_SIZE, SNAKE_BLOCK_SIZE, color);
}

// Function to generate food at a random position (used by Snake)
static void Snake_GenerateFood()
{
    bool food_on_snake;
    do
    {
        food_on_snake = false;
        food.pos.x = rand() % GAME_AREA_WIDTH;
        food.pos.y = rand() % GAME_AREA_HEIGHT;
        for (uint16_t i = 0; i < snake.length; i++)
        {
            if (snake.body[i].x == food.pos.x && snake.body[i].y == food.pos.y)
            {
                food_on_snake = true;
                break;
            }
        }
    } while (food_on_snake);
    food.active = true;
}

void Snake_InitGame(void)
{
    // srand(HAL_GetTick()); // Seed RNG if not done elsewhere
    gameOver = false;
    snake.length = 3;
    snake.dir = DIR_RIGHT;
    snake.body[0].x = GAME_AREA_WIDTH / 2;
    snake.body[0].y = GAME_AREA_HEIGHT / 2;
    snake.body[1].x = snake.body[0].x - 1;
    snake.body[1].y = snake.body[0].y;
    snake.body[2].x = snake.body[0].x - 2;
    snake.body[2].y = snake.body[0].y;
    for (uint16_t i = snake.length; i < SNAKE_MAX_LENGTH; ++i)
    {
        snake.body[i].x = -1;
        snake.body[i].y = -1;
    }
    Snake_GenerateFood();
    gameTickLast = HAL_GetTick();
}

static void Snake_Draw(void)
{
    for (uint16_t i = 0; i < snake.length; i++)
    {
        DrawBlock(snake.body[i].x, snake.body[i].y, 1);
    }
    if (food.active)
    {
        DrawBlock(food.pos.x, food.pos.y, 1);
    }
    if (gameOver)
    {
        OLED_DisplayString(OLED_WIDTH / 2 - 3 * 6, OLED_HEIGHT / 2 - 4, "GAME");
        OLED_DisplayString(OLED_WIDTH / 2 - 3 * 6, OLED_HEIGHT / 2 + 4, "OVER");
        OLED_DisplayString(OLED_WIDTH / 2 - 5 * 6, OLED_HEIGHT / 2 + 12, "RST BTN=RS");
    }
}

static void Snake_Update(void)
{
    if (gameOver)
        return;

    Point new_head = snake.body[0];
    switch (snake.dir)
    {
    case DIR_UP:
        new_head.y--;
        break;
    case DIR_DOWN:
        new_head.y++;
        break;
    case DIR_LEFT:
        new_head.x--;
        break;
    case DIR_RIGHT:
        new_head.x++;
        break;
    }

    if (new_head.x < 0)
        new_head.x = GAME_AREA_WIDTH - 1;
    else if (new_head.x >= GAME_AREA_WIDTH)
        new_head.x = 0;
    if (new_head.y < 0)
        new_head.y = GAME_AREA_HEIGHT - 1;
    else if (new_head.y >= GAME_AREA_HEIGHT)
        new_head.y = 0;

    for (uint16_t i = 0; i < snake.length - 1; i++) // Check up to length-1
    {
        if (new_head.x == snake.body[i].x && new_head.y == snake.body[i].y)
        {
            gameOver = true;
            return;
        }
    }

    if (food.active && new_head.x == food.pos.x && new_head.y == food.pos.y)
    {
        if (snake.length < SNAKE_MAX_LENGTH)
            snake.length++;
        Snake_GenerateFood();
    }
    // No else needed for tail movement; body shift handles it.

    for (uint16_t i = snake.length - 1; i > 0; i--)
    {
        snake.body[i] = snake.body[i - 1];
    }
    snake.body[0] = new_head;
}

void Snake_HandleInput(Direction input_dir)
{
    if ((snake.dir == DIR_UP && input_dir != DIR_DOWN) ||
        (snake.dir == DIR_DOWN && input_dir != DIR_UP) ||
        (snake.dir == DIR_LEFT && input_dir != DIR_RIGHT) ||
        (snake.dir == DIR_RIGHT && input_dir != DIR_LEFT))
    {
        snake.dir = input_dir;
    }
}

void Snake_GameLoop(void)
{
    static uint16_t LastCount;
    static bool preMutiTurn = false;

    if (FirstRun)
    {
        FirstRun = false;
        LastCount = count;
    }

    if (!preMutiTurn)
    {
        if (count - LastCount > 1)
        {
            if (snake.dir == DIR_UP)
                Snake_HandleInput(DIR_RIGHT);
            else if (snake.dir == DIR_DOWN)
                Snake_HandleInput(DIR_LEFT);
            else if (snake.dir == DIR_LEFT)
                Snake_HandleInput(DIR_UP);
            else if (snake.dir == DIR_RIGHT)
                Snake_HandleInput(DIR_DOWN);

            preMutiTurn = true;
            LastCount = count;
        }
        else if (count - LastCount < -1)
        {
            if (snake.dir == DIR_UP)
                Snake_HandleInput(DIR_LEFT);
            else if (snake.dir == DIR_DOWN)
                Snake_HandleInput(DIR_RIGHT);
            else if (snake.dir == DIR_LEFT)
                Snake_HandleInput(DIR_DOWN);
            else if (snake.dir == DIR_RIGHT)
                Snake_HandleInput(DIR_UP);

            preMutiTurn = true;
            LastCount = count;
        }
    }

    if (HAL_GetTick() - gameTickLast >= GAME_TICK_MS)
    {
        Snake_Update();
        gameTickLast = HAL_GetTick();
        preMutiTurn = false;
    }
    Snake_Draw();

    if (gameOver)
    {
        if (HAL_GPIO_ReadPin(GAME_INPUT_RESTART_PORT, GAME_INPUT_RESTART_PIN) == GPIO_PIN_RESET)
        {
            HAL_Delay(INPUT_DEBOUNCE_MS);
            if (HAL_GPIO_ReadPin(GAME_INPUT_RESTART_PORT, GAME_INPUT_RESTART_PIN) == GPIO_PIN_RESET)
            {
                Snake_InitGame();
                while (HAL_GPIO_ReadPin(GAME_INPUT_RESTART_PORT, GAME_INPUT_RESTART_PIN) == GPIO_PIN_RESET)
                    ;
            }
        }
    }
    char score_str[20];
    sprintf(score_str, "Score: %d", snake.length - 3);
    OLED_DisplayString(0, OLED_HEIGHT-8, score_str);
}

#pragma region 2048 Game Definitions
// --- 2048 Game Implementation ---

static void Game2048_AddRandomTile()
{
    Point empty_tiles[GAME2048_BOARD_SIZE * GAME2048_BOARD_SIZE]; // Use GAME2048_BOARD_SIZE
    uint8_t num_empty_tiles = 0;
    for (uint8_t r = 0; r < GAME2048_BOARD_SIZE; r++) // Use GAME2048_BOARD_SIZE
    {
        for (uint8_t c = 0; c < GAME2048_BOARD_SIZE; c++) // Use GAME2048_BOARD_SIZE
        {
            if (game2048_board[r][c].value == 0)
            {
                empty_tiles[num_empty_tiles].x = c;
                empty_tiles[num_empty_tiles].y = r;
                num_empty_tiles++;
            }
        }
    }
    if (num_empty_tiles > 0)
    {
        uint16_t rand_index = rand() % num_empty_tiles;
        uint8_t r_pos = empty_tiles[rand_index].y;
        uint8_t c_pos = empty_tiles[rand_index].x;
        game2048_board[r_pos][c_pos].value = (rand() % 10 == 0) ? 4 : 2;
    }
}

void Game2048_Init(void)
{
    for (uint8_t r = 0; r < GAME2048_BOARD_SIZE; r++) // Use GAME2048_BOARD_SIZE
    {
        for (uint8_t c = 0; c < GAME2048_BOARD_SIZE; c++) // Use GAME2048_BOARD_SIZE
        {
            game2048_board[r][c].value = 0;
        }
    }
    game2048_score = 0;
    game2048_game_over = false;
    game2048_game_won = false;
    game2048_move_made_this_turn = false;
    Game2048_AddRandomTile();

    OLED_UpdateDisplayVSync(); // Update display to show the initial state
}

void Game2048_Restart(void)
{
    Game2048_Init();
}

void Game2048_DrawBoard(void)
{
    OLED_ClearBuffer();
    char str_buffer[10];
    for (uint8_t r = 0; r < GAME2048_BOARD_SIZE; r++) // Use GAME2048_BOARD_SIZE
    {
        for (uint8_t c = 0; c < GAME2048_BOARD_SIZE; c++) // Use GAME2048_BOARD_SIZE
        {
            uint16_t val = game2048_board[r][c].value;
            uint8_t tile_x = c * GAME2048_TILE_WIDTH;
            uint8_t tile_y = r * GAME2048_TILE_HEIGHT;
            uint8_t tile_inner_width = GAME2048_TILE_WIDTH - 2 * GAME2048_TILE_PADDING;
            uint8_t tile_inner_height = GAME2048_TILE_HEIGHT - 2 * GAME2048_TILE_PADDING;

            OLED_DrawFilledRectangle(tile_x + GAME2048_TILE_PADDING,
                                     tile_y + GAME2048_TILE_PADDING,
                                     tile_inner_width, tile_inner_height,
                                     (val == 0) ? 0 : 1);
            if (val > 0)
            {
                sprintf(str_buffer, "%d", val);
                uint8_t text_width = strlen(str_buffer) * 6;
                uint8_t text_height = 8;
                uint8_t text_x = tile_x + (GAME2048_TILE_WIDTH - text_width) / 2;
                uint8_t text_y = tile_y + (GAME2048_TILE_HEIGHT - text_height) / 2;
                OLED_DisplayStringInverted(text_x, text_y, str_buffer, 1);
            }
        }
    }
    sprintf(str_buffer, "Scores:%lu", game2048_score);
    OLED_DisplayString(0, OLED_HEIGHT - 8, str_buffer);

    if (game2048_game_over)
    {
        OLED_DrawFilledRectangle(0, 0, OLED_WIDTH, OLED_HEIGHT-8, 1);
        OLED_DisplayStringInverted((OLED_WIDTH - 8 * 6) / 2, GAME_BOARD_DISPLAY_HEIGHT / 2 - 8, "GAME OVER", 1);
        OLED_DisplayStringInverted((OLED_WIDTH - 10 * 6) / 2, GAME_BOARD_DISPLAY_HEIGHT / 2, "RST BTN=RS", 1);
    }
    else if (game2048_game_won)
    {
        OLED_DisplayString((OLED_WIDTH - 7 * 6) / 2, GAME_BOARD_DISPLAY_HEIGHT / 2 - 4, "YOU WIN!");
    }
}

static void Game2048_CompactAndMerge(uint8_t line_idx, Direction dir, bool is_row)
{
    uint8_t line_size = GAME2048_BOARD_SIZE;          // Use GAME2048_BOARD_SIZE
    uint16_t temp_line[GAME2048_BOARD_SIZE] = {0};    // Use GAME2048_BOARD_SIZE
    uint16_t original_line_copy[GAME2048_BOARD_SIZE]; // To check if anything changed

    for (uint8_t i = 0; i < line_size; ++i)
    {
        original_line_copy[i] = is_row ? game2048_board[line_idx][i].value : game2048_board[i][line_idx].value;
    }

    int8_t write_pos = 0;
    int8_t increment = 1;

    if (dir == DIR_DOWN || dir == DIR_RIGHT)
    {
        write_pos = line_size - 1;
        increment = -1;
    }

    int8_t compact_pos = write_pos;
    for (int8_t i = 0; i < line_size; ++i)
    {
        int8_t read_pos_actual = (dir == DIR_DOWN || dir == DIR_RIGHT) ? (line_size - 1 - i) : i;
        uint16_t current_tile_value = is_row ? game2048_board[line_idx][read_pos_actual].value
                                             : game2048_board[read_pos_actual][line_idx].value;
        if (current_tile_value != 0)
        {
            temp_line[compact_pos] = current_tile_value;
            compact_pos += increment;
        }
    }

    for (int8_t i = 0; i < line_size - 1; ++i)
    {
        int8_t current_idx = (dir == DIR_DOWN || dir == DIR_RIGHT) ? (line_size - 1 - i) : i;
        int8_t next_idx = current_idx + increment;
        if (next_idx < 0 || next_idx >= line_size)
            continue;

        if (temp_line[current_idx] != 0 && temp_line[current_idx] == temp_line[next_idx])
        {
            temp_line[current_idx] *= 2;
            game2048_score += temp_line[current_idx];
            temp_line[next_idx] = 0;
            if (temp_line[current_idx] == 2048)
                game2048_game_won = true;
        }
    }

    compact_pos = write_pos;
    for (int8_t i = 0; i < line_size; ++i)
    {
        int8_t read_pos_temp = (dir == DIR_DOWN || dir == DIR_RIGHT) ? (line_size - 1 - i) : i;
        // The condition "if (temp_line[read_pos_temp] != 0)" was problematic.
        // temp_line already contains the correct values, including zeros, after compaction and merging.
        // We should always write the value from temp_line to the board and advance compact_pos.
        if (is_row)
        {
            game2048_board[line_idx][compact_pos].value = temp_line[read_pos_temp];
        }
        else
        {
            game2048_board[compact_pos][line_idx].value = temp_line[read_pos_temp];
        }
        compact_pos += increment; // compact_pos should always be updated.
    }

    for (uint8_t i = 0; i < line_size; ++i)
    {
        uint16_t current_val = is_row ? game2048_board[line_idx][i].value : game2048_board[i][line_idx].value;
        if (original_line_copy[i] != current_val)
        {
            game2048_move_made_this_turn = true;
            break;
        }
    }
}

static void Game2048_MoveUp()
{
    for (uint8_t c = 0; c < GAME2048_BOARD_SIZE; c++) // Use GAME2048_BOARD_SIZE
    {
        Game2048_CompactAndMerge(c, DIR_UP, false);
    }
}

static void Game2048_MoveDown()
{
    for (uint8_t c = 0; c < GAME2048_BOARD_SIZE; c++) // Use GAME2048_BOARD_SIZE
    {
        Game2048_CompactAndMerge(c, DIR_DOWN, false);
    }
}

static void Game2048_MoveLeft()
{
    for (uint8_t r = 0; r < GAME2048_BOARD_SIZE; r++) // Use GAME2048_BOARD_SIZE
    {
        Game2048_CompactAndMerge(r, DIR_LEFT, true);
    }
}

static void Game2048_MoveRight()
{
    for (uint8_t r = 0; r < GAME2048_BOARD_SIZE; r++) // Use GAME2048_BOARD_SIZE
    {
        Game2048_CompactAndMerge(r, DIR_RIGHT, true);
    }
}

static bool Game2048_CanMove()
{
    for (uint8_t r = 0; r < GAME2048_BOARD_SIZE; r++) // Use GAME2048_BOARD_SIZE
    {
        for (uint8_t c = 0; c < GAME2048_BOARD_SIZE; c++) // Use GAME2048_BOARD_SIZE
        {
            if (game2048_board[r][c].value == 0)
                return true;
            uint16_t val = game2048_board[r][c].value;
            if (c < GAME2048_BOARD_SIZE - 1 && game2048_board[r][c + 1].value == val) // Use GAME2048_BOARD_SIZE
                return true;
            if (r < GAME2048_BOARD_SIZE - 1 && game2048_board[r + 1][c].value == val) // Use GAME2048_BOARD_SIZE
                return true;
        }
    }
    return false;
}

bool Game2048_HandleInput(Direction input_dir)
{
    if (game2048_game_over || game2048_game_won)
        return false;

    game2048_move_made_this_turn = false;

    switch (input_dir)
    {
    case DIR_UP:
        Game2048_MoveUp();
        break;
    case DIR_DOWN:
        Game2048_MoveDown();
        break;
    case DIR_LEFT:
        Game2048_MoveLeft();
        break;
    case DIR_RIGHT:
        Game2048_MoveRight();
        break;
    }

    if (game2048_move_made_this_turn)
    {
        Game2048_AddRandomTile();
        if (!Game2048_CanMove())
        {
            game2048_game_over = true;
        }
    }
    return game2048_move_made_this_turn;
}

void Game2048_GameLoop(void)
{
    static uint16_t LastCount;

    if (FirstRun)
    {
        LastCount = count;
        FirstRun = false;
    }

    if (count - LastCount > 1 && HAL_GPIO_ReadPin(GAME_INPUT_RESTART_PORT, GAME_INPUT_RESTART_PIN) == GPIO_PIN_SET)
    {
        Game2048_HandleInput(DIR_RIGHT);
        LastCount = count;
    }
    else if (count - LastCount < -1 && HAL_GPIO_ReadPin(GAME_INPUT_RESTART_PORT, GAME_INPUT_RESTART_PIN) == GPIO_PIN_SET)
    {
        Game2048_HandleInput(DIR_LEFT);
        LastCount = count;
    }
    else if (count - LastCount > 1 && HAL_GPIO_ReadPin(GAME_INPUT_RESTART_PORT, GAME_INPUT_RESTART_PIN) == GPIO_PIN_RESET)
    {
        Game2048_HandleInput(DIR_UP);
        LastCount = count;
    }
    else if (count - LastCount < -1 && HAL_GPIO_ReadPin(GAME_INPUT_RESTART_PORT, GAME_INPUT_RESTART_PIN) == GPIO_PIN_RESET)
    {
        Game2048_HandleInput(DIR_DOWN);
        LastCount = count;
    }

    if (game2048_game_over)
    {
        if (HAL_GPIO_ReadPin(GAME_INPUT_RESTART_PORT, GAME_INPUT_RESTART_PIN) == GPIO_PIN_RESET)
        {
            HAL_Delay(INPUT_DEBOUNCE_MS);
            if (HAL_GPIO_ReadPin(GAME_INPUT_RESTART_PORT, GAME_INPUT_RESTART_PIN) == GPIO_PIN_RESET)
            {
                Game2048_Restart();
                while (HAL_GPIO_ReadPin(GAME_INPUT_RESTART_PORT, GAME_INPUT_RESTART_PIN) == GPIO_PIN_RESET)
                    ;
            }
        }
    }


    Game2048_DrawBoard();
}

#pragma endregion 2048 Game Definitions
