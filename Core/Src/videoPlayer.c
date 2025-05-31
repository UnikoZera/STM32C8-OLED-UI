/*
 * videoPlayer.c
 *
 *  Created on: May 17, 2025
 *      Author: UnikoZera
 */

#include "video_player.h"
#include "flash.h"
#include "oled.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stddef.h>

#define Video_Basic_Addr 0x00000000 // Base address for video data
bool video_first_play;

int lz77_decompress(const uint8_t *input, size_t input_size, uint8_t *output, size_t output_size)
{
    const uint8_t *input_end = input + input_size;
    uint8_t *output_end = output + output_size;
    uint8_t *output_ptr = output;

    while (input < input_end)
    {
        uint8_t flag = *input++;
        if (flag < 0x80)
        {
            // Literal run
            size_t len = flag & 0x7F;
            if (input + len > input_end || output_ptr + len > output_end)
            {
                return -1; // Overflow check
            }
            for (size_t i = 0; i < len; i++)
            {
                *output_ptr++ = *input++;
            }
        }
        else
        {
            // Back reference
            if (input + 2 > input_end)
            {
                return -2; // Input overflow
            }
            uint16_t offset = ((flag & 0x7F) << 8) | *input++;
            offset += 1; // Offset starts from 1
            uint8_t length = *input++;
            if (length == 0 || offset > (size_t)(output_ptr - output))
            {
                return -3; // Invalid length or offset
            }
            uint8_t *src = output_ptr - offset;
            if (output_ptr + length > output_end)
            {
                return -4; // Output overflow
            }
            for (uint8_t i = 0; i < length; i++)
            {
                *output_ptr++ = *src++;
            }
        }
    }
    return (int)(output_ptr - output);
}

// Global variables for video playback control
static unsigned int current_frame_index = 0;
static unsigned int total_frames = 0; // Changed from uint8_t to unsigned int

// Declare the goodapple data if not in goodapple.h (it should be)
// extern const unsigned char goodapple[]; // Ensure this is available

void display_frame_oled(unsigned char *frame)
{
    // OLED_ClearBuffer();
    for (int i = 0; i < 114 * 64; i++)
    { // Assuming OLED is 128x64, video is 114x64
        bool pixel = (frame[i / 8] & (1 << (i % 8))) != 0;
        // Adjust x-coordinate to center the 114-width video on a 128-width screen
        int x = (i % 114) + (OLED_WIDTH - 114) / 2;
        int y = i / 114;
        if (x < OLED_WIDTH && y < OLED_HEIGHT)
        {                                         // Boundary check
            OLED_WritePixel(x, y, pixel ? 1 : 0); // Draw pixel (1 for white, 0 for black)
        }
    }
    // OLED_SmartUpdate();
}

void video_player_init()
{
    W25Q64_Read(Video_Basic_Addr, (uint8_t *)&total_frames, sizeof(total_frames)); // Read the frame count from the flash memory, cast to uint8_t* and use sizeof
    current_frame_index = 0;
}

void play_video()
{
    static uint16_t LastCount_video = 0;


    if (video_first_play)
    {
        video_first_play = false;
        LastCount_video = count; // Initialize LastCount_video
        video_player_init();
    }


    unsigned char frame_buffer[920]; // Buffer for one decompressed frame (114*64/8 = 912, rounded up)

    if (total_frames == 0)
    {
        video_player_init();
    }

    if (current_frame_index >= total_frames)
    {
        current_frame_index = 0;
    }

    get_target_frame_index(current_frame_index, frame_buffer, sizeof(frame_buffer));

    display_frame_oled(frame_buffer);

    current_frame_index++;

    if(count - LastCount_video >= 2)
    {
        video_fast_forward(10);
        LastCount_video = count;
    }
    else if (count - LastCount_video <= -2)
    {
        video_rewind(10);
        LastCount_video = count;
    }
}

void video_fast_forward(int frames_to_skip)
{
    if (total_frames == 0)
        video_player_init(); // Ensure total_frames is initialized
    current_frame_index += frames_to_skip;
    if (current_frame_index >= total_frames)
    {
        current_frame_index = total_frames - 1; // Go to last frame
    }
    if (current_frame_index < 0) // Should not happen with fast forward, but good practice
    {
        current_frame_index = 0;
    }
}

void video_rewind(int frames_to_skip)
{
    if (total_frames == 0)
        video_player_init(); // Ensure total_frames is initialized

    if ((int)current_frame_index - frames_to_skip < 0) // Check before subtraction to prevent underflow with unsigned int
    {
        current_frame_index = 0; // Go to first frame
    }
    else
    {
        current_frame_index -= frames_to_skip;
    }
}

int get_current_frame_index()
{
    return current_frame_index;
}

unsigned int get_total_frames()
{
    if (total_frames == 0)
        video_player_init();
    return total_frames;
}

void set_current_frame_index(int frame_index)
{
    if (total_frames == 0)
        video_player_init();
    if (frame_index >= 0 && (unsigned int)frame_index < total_frames)
    {
        current_frame_index = (unsigned int)frame_index;
    }
    else if ((unsigned int)frame_index >= total_frames)
    {
        current_frame_index = total_frames - 1;
    }
    else // frame_index < 0
    {
        current_frame_index = 0;
    }
}

int get_target_frame_index(unsigned int index, uint8_t *dst, size_t output_size)
{
    uint32_t target_ptr_val;
    uint32_t next_ptr_val;
    unsigned char temp[920];
    uint32_t compressed_length;
    uint32_t picture_start_offset;
    uint32_t total_frames_from_flash;

    W25Q64_Read(Video_Basic_Addr, (uint8_t *)&total_frames_from_flash, sizeof(total_frames_from_flash));
    picture_start_offset = sizeof(uint32_t) + (total_frames_from_flash + 1) * sizeof(uint32_t);
    W25Q64_Read(Video_Basic_Addr + sizeof(uint32_t) + (index * sizeof(uint32_t)), (uint8_t *)&target_ptr_val, sizeof(target_ptr_val));
    W25Q64_Read(Video_Basic_Addr + sizeof(uint32_t) + ((index + 1) * sizeof(uint32_t)), (uint8_t *)&next_ptr_val, sizeof(next_ptr_val));

    compressed_length = next_ptr_val - target_ptr_val;

    if (compressed_length == 0)
    {
        return 0;
    }
    if (next_ptr_val < target_ptr_val)
    {
        return -2;
    }
    if (compressed_length > sizeof(temp))
    {
        return -1;
    }

    W25Q64_Read(Video_Basic_Addr + picture_start_offset + target_ptr_val, temp, compressed_length);

    int decompressed_bytes = lz77_decompress(temp, compressed_length, dst, output_size);
    return decompressed_bytes;
}
