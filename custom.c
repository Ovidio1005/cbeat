#include "custom.h"
#include "macros.h"
#include "utils.h"
#include <stdlib.h>
#include <stdio.h>

static uint32_t current_sample = 0;

static uint8_t* audio_data = NULL;
static uint32_t audio_data_length = 0;

static uint32_t samples_per_step = 1;
static uint8_t amplitude = 255;

void custom_set_data(const uint8_t* data, uint32_t length) {
    audio_data_length = length;
    audio_data = (uint8_t*)malloc(SAMPLE_RATE * sizeof(uint8_t));

    // Terminate the program if memory allocation fails
    if(!audio_data) {
        fprintf(stderr, "Error: Memory allocation failed in custom_set_data()\n");
        exit(1);
    }

    for (int i = 0; i < length; i++) {
        audio_data[i] = data[i];
    }
}

uint32_t custom_frequency(void) {
    return samples_per_step;
}

void custom_set_frequency(uint32_t frequency) {
    samples_per_step = frequency;
}

uint8_t custom_amplitude(void) {
    return amplitude;
}

void custom_set_amplitude(uint8_t amp) {
    amplitude = amp;
}

uint8_t custom_step(void) {
    if (samples_per_step == 0) {
        return 128; // No sound if samples per cycle is zero
    }

    uint8_t output = apply_amplitude(audio_data[current_sample], amplitude);
    current_sample = (current_sample + samples_per_step) % SAMPLE_RATE;

    return output;
}