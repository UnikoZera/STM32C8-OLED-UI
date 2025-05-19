/*
 * videoPlayer.c
 *
 *  Created on: May 17, 2025
 *      Author: UnikoZera
 */

#include "video_player.h"
#include "goodapple.h"
#include "oled.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stddef.h>

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
                return -1; // Input overflow
            }
            uint16_t offset = ((flag & 0x7F) << 8) | *input++;
            offset += 1; // Offset starts from 1
            uint8_t length = *input++;
            if (length == 0 || offset > (size_t)(output_ptr - output))
            {
                return -1; // Invalid length or offset
            }
            uint8_t *src = output_ptr - offset;
            if (output_ptr + length > output_end)
            {
                return -1; // Output overflow
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
static int current_frame_index = 0;
static unsigned int total_frames = 0;

// Declare the goodapple data if not in goodapple.h (it should be)
// extern const unsigned char goodapple[]; // Ensure this is available

void display_frame_oled(unsigned char *frame)
{
    OLED_ClearBuffer();
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

    // 退出视频播放状态在这里！
    /*code here*/

    OLED_SmartUpdate();
}

void video_player_init()
{
    // Calculate total frames from the pointers array
    // Each pointer is 4 bytes (unsigned int). The last pointer marks the end of the last frame.
    total_frames = (sizeof(pointers) / sizeof(unsigned int)) - 1;
    current_frame_index = 0;
}

void play_video()
{
    unsigned char frame_buffer[920]; // Buffer for one decompressed frame (114*64/8 = 912, rounded up)

    if (total_frames == 0)
    { // Initialize if not already done
        video_player_init();
    }

    if (current_frame_index >= total_frames)
    {
        current_frame_index = 0; // Loop back to the beginning or stop
    }

    // Get the start offset of the current frame from the pointers array
    unsigned int frame_offset = *((unsigned int *)pointers + current_frame_index);
    const unsigned char *frame_data_start = goodapple + frame_offset;

    // Get the start offset of the next frame to calculate the current frame's compressed length
    unsigned int next_frame_offset = *((unsigned int *)pointers + current_frame_index + 1);
    unsigned short compressed_length = next_frame_offset - frame_offset;

    if (compressed_length == 0)
    {
        // End of video or error
        return;
    }

    int decompressed_size = lz77_decompress(frame_data_start, compressed_length, frame_buffer, sizeof(frame_buffer));

    if (decompressed_size > 0)
    {
        display_frame_oled(frame_buffer);
        current_frame_index++; // Move to the next frame for the next call
    }
    else
    {
        // Handle decompression error
        // Consider resetting current_frame_index or logging an error
    }

    // Optional: Add a delay here if needed to control playback speed
    // HAL_Delay(5); // Delay is managed by the caller or main loop typically
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
    current_frame_index -= frames_to_skip;
    if (current_frame_index < 0)
    {
        current_frame_index = 0; // Go to first frame
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
    if (frame_index >= 0 && frame_index < total_frames)
    {
        current_frame_index = frame_index;
    }
    else if (frame_index >= total_frames)
    {
        current_frame_index = total_frames - 1;
    }
    else
    {
        current_frame_index = 0;
    }
}
