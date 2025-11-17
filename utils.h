/**
 * @file utils.h
 * @brief Utility functions for interpolation.
 * 
 * @author Ovidio1005
 * @date 2025-11-16
 */

#include <stdint.h>

/**
 * @brief Linearly interpolates between two 16-bit unsigned integers.
 * @details The interpolated value will never exceed the bounds of `start` and `end`, and will be clamped if `position` is outside the range [0, length].
 * @param start The starting value.
 * @param end The ending value.
 * @param position The current position in the interpolation, between 0 and `length`.
 * @param length The total length of the interpolation.
 * @return The interpolated 16-bit unsigned integer.
 */
uint16_t linear_interpolate_16(uint16_t start, uint16_t end, uint16_t position, uint16_t length);
/**
 * @brief Linearly interpolates between two 8-bit unsigned integers.
 * @details The interpolated value will never exceed the bounds of `start` and `end`, and will be clamped if `position` is outside the range [0, length].
 * @param start The starting value.
 * @param end The ending value.
 * @param position The current position in the interpolation, between 0 and `length`.
 * @param length The total length of the interpolation.
 * @return The interpolated 8-bit unsigned integer.
 */
uint8_t linear_interpolate_8(uint8_t start, uint8_t end, uint16_t position, uint16_t length);

/**
 * @brief Applies amplitude scaling to an 8-bit audio sample.
 * @details This function scales the input from the range [0, 255] to the range [128 - `amplitude`/2, 128 + `amplitude`/2], keeping the sample centered around 128.
 * @param sample_value The original 8-bit audio sample value.
 * @param amplitude The amplitude scaling factor (0-255).
 * @return The amplitude-scaled 8-bit audio sample value.
 */
uint8_t apply_amplitude(uint8_t sample_value, uint8_t amplitude);