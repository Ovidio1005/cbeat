#include "triangle.h"
#include "macros.h"
#include "utils.h"

static const uint16_t HALF_CYCLE = SAMPLE_RATE / 2;

static uint16_t current_sample = 0;

static uint16_t samples_per_step = 1;
static uint8_t amplitude = 255;

uint16_t triangle_frequency(void) {
    return samples_per_step;
}

void triangle_set_frequency(uint16_t frequency) {
    samples_per_step = frequency;
}

uint8_t triangle_amplitude(void) {
    return amplitude;
}

void triangle_set_amplitude(uint8_t amp) {
    amplitude = amp;
}

uint8_t triangle_step(void) {
    if (samples_per_step == 0) {
        return 128; // No sound if samples per cycle is zero
    }

    uint8_t output;
    if(current_sample < HALF_CYCLE) {
        output = apply_amplitude(amplitude * current_sample / HALF_CYCLE, amplitude);
    } else {
        output = apply_amplitude(amplitude * (SAMPLE_RATE - current_sample) / HALF_CYCLE, amplitude);
    }

    current_sample = (current_sample + samples_per_step) % SAMPLE_RATE;

    return output;
}