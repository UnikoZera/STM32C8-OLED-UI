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

void video_player_init(void); // Initialize the video player
// Function to play the video
void play_video();

void video_fast_forward(int frames_to_skip); // Fast forward the video
void video_rewind(int frames_to_skip); // Rewind the video

void set_current_frame_index(int frame_index);

int get_target_frame_index(unsigned int index, uint8_t *dst, size_t output_size); // Get the target frame index - Updated signature

extern bool video_first_play; // Flag to check if it's the first play
#endif /* INC_VIDEO_PLAYER_H_ */
