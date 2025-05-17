/*
 * videoPlayer.c
 *
 *  Created on: May 17, 2025
 *      Author: UnikoZera
 */

#include "video_player.h"
#include "bad_apple.h"
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

    //退出视频播放状态在这里！
        /*code here*/

    OLED_SmartUpdate();
}

void play_video()
{
    unsigned char frame_buffer[920]; // Buffer for one decompressed frame (114*64/8 = 912, rounded up)
    
    const unsigned char *frame_start = bad_apple;
    int frame_count = 0;
    unsigned int current_offset = 0;

    while (current_offset < sizeof(bad_apple))
    {
        frame_count++;
        unsigned short length = *(unsigned short *)(frame_start + current_offset);
        if (length == 0)
        { // End of video data marker or error
            break;
        }

        // Ensure we don't read past the end of video for the length itself
        if (current_offset + 2 > sizeof(bad_apple))
        {
            break;
        }

        // Ensure the compressed data length doesn't exceed remaining data
        if (current_offset + 2 + length > sizeof(bad_apple))
        {
            break;
        }

        int decompressed_size = lz77_decompress(frame_start + current_offset + 2, length, frame_buffer, sizeof(frame_buffer));

        if (decompressed_size > 0)
        {
            display_frame_oled(frame_buffer);
        }
        else
        {
            // Handle decompression error, maybe log or break
            break;
        }

        current_offset += length + 2;

        // Optional: Add a delay here if needed to control playback speed
        HAL_Delay(5); //这里调整播放速度
    }
    // Optionally, display total frames or a message when done
}
