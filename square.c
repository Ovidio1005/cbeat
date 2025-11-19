#include "square.h"
#include "macros.h"
#include "utils.h"

#include <stdint.h>

static uint16_t current_sample = 0;

static uint8_t duty_cycle = 127; // 50% duty cycle
static uint16_t cutoff_sample = SAMPLE_RATE / 2; // Before cutoff = high, after = low. Recalculated in set_duty_cycle

static uint16_t samples_per_step = 1;
static uint8_t amplitude = 255;

uint8_t square_duty_cycle(void) {
    return duty_cycle;
}

void square_set_duty_cycle(uint8_t duty) {
    duty_cycle = duty;
    cutoff_sample = (samples_per_step * duty_cycle) / 255;
}

uint16_t square_frequency(void) {
    return samples_per_step;
}

void square_set_frequency(uint16_t frequency) {
    samples_per_step = frequency;
}

uint8_t square_amplitude(void) {
    return amplitude;
}

void square_set_amplitude(uint8_t amp) {
    amplitude = amp;
}

uint8_t square_step(void) {
    if (samples_per_step == 0) {
        return 0; // No sound if samples per cycle is zero
    }

    uint8_t output;
    if(current_sample < cutoff_sample) {
        output = apply_amplitude(255, amplitude);
    } else {
        output = apply_amplitude(0, amplitude);
    }

    current_sample = (current_sample + samples_per_step) % SAMPLE_RATE;
    
    return output;
}