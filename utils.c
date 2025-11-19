#include "utils.h"
#include <stdint.h>

// All of these functions cast the values to a larger type to prevent overflow during calculations.

uint16_t linear_interpolate_16_long(uint16_t start, uint16_t end, uint32_t position, uint32_t length) {
    if(length == 0) return start; // Avoid division by zero
    if(position <= 0) return start;
    if(position >= length) return end;

    return start + (((int64_t)end - (int64_t)start) * (int64_t)position) / length;
}

uint16_t linear_interpolate_16(uint16_t start, uint16_t end, uint16_t position, uint16_t length) {
    if(length == 0) return start; // Avoid division by zero
    if(position <= 0) return start;
    if(position >= length) return end;

    return start + (((int32_t)end - (int32_t)start) * (int32_t)position) / length;
}

uint8_t linear_interpolate_8(uint8_t start, uint8_t end, uint16_t position, uint16_t length) {
    if(length == 0) return start; // Avoid division by zero
    if(position <= 0) return start;
    if(position >= length) return end;

    return start + (((int16_t)end - (int16_t)start) * (int16_t)position) / length;
}

uint16_t quadratic_interpolate_16(uint16_t start, uint16_t end, uint16_t position, uint16_t length) {
    if(length == 0) return start; // Avoid division by zero
    if(position <= 0) return start;
    if(position >= length) return end;

    uint64_t pos_squared = (uint64_t)position * (uint64_t)position;
    uint64_t len_squared = (uint64_t)length * (uint64_t)length;

    return start + (((int64_t)end - (int64_t)start) * pos_squared) / len_squared;
}

uint8_t quadratic_interpolate_8(uint8_t start, uint8_t end, uint16_t position, uint16_t length) {
    if(length == 0) return start; // Avoid division by zero
    if(position <= 0) return start;
    if(position >= length) return end;

    uint32_t pos_squared = (uint32_t)position * (uint32_t)position;
    uint32_t len_squared = (uint32_t)length * (uint32_t)length;

    return start + (((int32_t)end - (int32_t)start) * pos_squared) / len_squared;
}

uint8_t apply_amplitude(uint8_t sample_value, uint8_t amplitude) {
    int16_t zero_centered = (int16_t)sample_value - 128;
    zero_centered = (zero_centered * amplitude) / 255;
    return (uint8_t)(zero_centered + 128);
}