/*
 * games.c
 *
 *  Created on: May 12, 2025
 *      Author: UnikoZera
 */

#include "games.h"
#include "oled.h"   // For OLED_ClearBuffer, OLED_DrawFilledRectangle, OLED_UpdateDisplayVSync, OLED_WIDTH, OLED_HEIGHT
#include "main.h"   // For HAL_Delay, HAL_GetTick (if needed for game speed or random seed)
#include <stdlib.h> // For rand() and srand()
#include <stdint.h>

// Game state variables
static Snake snake;
static Food food;
bool gameOver;
static uint32_t gameTickLast; // For controlling game speed
#define GAME_TICK_MS 100      // Snake moves every 200ms, adjust for speed

// Helper function to draw a block on the OLED
static void DrawBlock(int16_t x, int16_t y, uint8_t color)
{
    OLED_DrawFilledRectangle(x * SNAKE_BLOCK_SIZE, y * SNAKE_BLOCK_SIZE, SNAKE_BLOCK_SIZE, SNAKE_BLOCK_SIZE, color);
}

// Function to generate food at a random position
static void Snake_GenerateFood()
{
    bool food_on_snake;
    do
    {
        food_on_snake = false;
        food.pos.x = rand() % GAME_AREA_WIDTH;
        food.pos.y = rand() % GAME_AREA_HEIGHT;
        // Check if food is generated on the snake
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
    // Seed the random number generator (optional, but good for variety)
    // srand(HAL_GetTick()); // HAL_GetTick might not be random enough for a good seed.
    // Consider a more robust seeding mechanism if available.

    gameOver = false;
    snake.length = 3; // Initial length
    snake.dir = DIR_RIGHT;

    // Initial snake position (e.g., center of the game area)
    snake.body[0].x = GAME_AREA_WIDTH / 2;
    snake.body[0].y = GAME_AREA_HEIGHT / 2;
    snake.body[1].x = snake.body[0].x - 1;
    snake.body[1].y = snake.body[0].y;
    snake.body[2].x = snake.body[0].x - 2;
    snake.body[2].y = snake.body[0].y;

    for (uint16_t i = snake.length; i < SNAKE_MAX_LENGTH; ++i)
    {
        snake.body[i].x = -1; // Mark unused segments
        snake.body[i].y = -1;
    }

    Snake_GenerateFood();
    gameTickLast = HAL_GetTick();
}

static void Snake_Draw(void)
{
    OLED_ClearBuffer();

    // Draw snake
    for (uint16_t i = 0; i < snake.length; i++)
    {
        DrawBlock(snake.body[i].x, snake.body[i].y, 1); // 1 for white
    }

    // Draw food
    if (food.active)
    {
        DrawBlock(food.pos.x, food.pos.y, 1); // 1 for white
    }

    if (gameOver)
    {
        OLED_DisplayString(OLED_WIDTH / 2 - 3 * 6, OLED_HEIGHT / 2 - 4, "GAME");
        OLED_DisplayString(OLED_WIDTH / 2 - 3 * 6, OLED_HEIGHT / 2 + 4, "OVER");
    }

}

static void Snake_Update(void)
{
    if (gameOver)
    {
        return;
    }

    Point new_head = snake.body[0];

    // Move head based on direction
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

    // Wall collision and wrap-around logic
    if (new_head.x < 0) 
    {
        new_head.x = GAME_AREA_WIDTH - 1;
    }
    else if (new_head.x >= GAME_AREA_WIDTH)
    {
        new_head.x = 0;
    }

    if (new_head.y < 0)
    {
        new_head.y = GAME_AREA_HEIGHT - 1;
    }
    else if (new_head.y >= GAME_AREA_HEIGHT)
    {
        new_head.y = 0;
    }

    // Check for self-collision (excluding the tail that will move away)
    for (uint16_t i = 0; i < snake.length - 1; i++)
    { // Check up to length-1 because the actual tail will move
        if (new_head.x == snake.body[i].x && new_head.y == snake.body[i].y)
        {
            gameOver = true;
            return;
        }
    }

    // Check if food is eaten
    if (food.active && new_head.x == food.pos.x && new_head.y == food.pos.y)
    {
        if (snake.length < SNAKE_MAX_LENGTH)
        {
            snake.length++;
        }
        Snake_GenerateFood();
    }
    else
    {
        // If food not eaten, move the tail (last segment disappears)
        // No need to explicitly remove the tail, the shift below handles it.
    }

    // Move snake body
    // Shift all segments one position down, the last one is overwritten or ignored if length increased
    for (uint16_t i = snake.length - 1; i > 0; i--)
    {
        snake.body[i] = snake.body[i - 1];
    }
    snake.body[0] = new_head; // New head position
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
    static uint16_t LastCount = 32767;
    static bool preMutiTurn = false;
    // This function would typically be called repeatedly from your main loop or a timer interrupt.
    // For now, let's assume it's called, and we manage ticks internally.

    // Example: A simple way to integrate input (replace with your actual input mechanism)
    // extern uint8_t get_joystick_direction(); // Placeholder for your input function
    // Direction current_input = get_joystick_direction();
    // if (current_input != NO_INPUT) { // Assuming NO_INPUT if no direction change
    //     Snake_HandleInput(current_input);
    // }
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
    Snake_Draw(); // Draw regardless of tick, for smoother animation if needed, or only after update.

    // If game over, you might want to wait for an input to restart
    if (gameOver)
    {
        if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12) == GPIO_PIN_RESET)
        {
            HAL_Delay(50); // Debounce delay
            if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12) == GPIO_PIN_RESET)
            {
                Snake_InitGame(); // Or handle restart explicitly elsewhere
                OLED_DisplayString(0, 0, "Score: 0"); // Display title or status
            }
            while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12) == GPIO_PIN_RESET);
            
        }
        // For now, it just stops updating and shows "GAME OVER"
    }

    char score[30];
    sprintf(score, "Score: %d", snake.length - 3); // Display score
    OLED_DisplayString(0, 0, score); // Display title or status
}

