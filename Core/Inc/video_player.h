/*
 * video_player.h
 *
 *  Created on: May 17, 2025
 *      Author: UnikoZera
 */

#ifndef INC_VIDEO_PLAYER_H_
#define INC_VIDEO_PLAYER_H_

#include "oled.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stddef.h>

// Function to decompress LZ77 data
int lz77_decompress(const uint8_t* input, size_t input_size, uint8_t* output, size_t output_size);

// Function to display a single frame
void display_frame_oled(unsigned char *frame);

// Function to play the video
void play_video();

#endif /* INC_VIDEO_PLAYER_H_ */
