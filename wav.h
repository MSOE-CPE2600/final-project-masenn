/**
 * @file wav.h
 * @brief header for the wave file
 * 
 * Course: CPE2600
 * Section: 011
 * Assignment: Final
 * Name: Matthew Senn
 */

#ifndef WAV_H
#define WAV_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#define BIT_DEPTH 16
#define VOLUME .2

typedef struct WaveParameters{
    int sample_freq, sample_channels, sample_length,bit_depth;
    float freq_base, amp_depth, freq_depth, rot_freq;
    uint16_t MAX_VAL;
    char* name;
} WaveParameters;


void write_wave_header(FILE *file, WaveParameters* wave);

void read_config();
void generate_sample(FILE* fp, WaveParameters* params);
void generate_package(WaveParameters* params);
float midi_to_freq(float num);
void create_dir(const char* path);

#endif