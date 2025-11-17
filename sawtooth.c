#include "sawtooth.h"
#include "macros.h"
#include "utils.h"

static uint16_t current_sample = 0;

static uint16_t samples_per_step = 1;
static uint8_t amplitude = 255;

uint16_t sawtooth_frequency(void) {
    return samples_per_step;
}

void sawtooth_set_frequency(uint16_t frequency) {
    samples_per_step = frequency;
}

uint8_t sawtooth_amplitude(void) {
    return amplitude;
}

void sawtooth_set_amplitude(uint8_t amp) {
    amplitude = amp;
}

uint8_t sawtooth_step(void) {
    if (samples_per_step == 0) {
        return 128; // No sound if samples per cycle is zero
    }

    uint8_t output = apply_amplitude(255 * current_sample / SAMPLE_RATE, amplitude);
    current_sample = (current_sample + samples_per_step) % SAMPLE_RATE;

    return output;
}