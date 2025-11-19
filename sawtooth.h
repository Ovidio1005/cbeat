#pragma once

/**
 * @file sawtooth.h
 * @brief Header file for sawtooth wave generator functions.
 * 
 * @details This module provides functions to generate a sawtooth wave signal as an
 * unsigned 8-bit integer output. The returned values will be between -`amplitude`/2
 * and +`amplitude`/2, centered around 128, and will loop `frequency` times per second,
 * based on the sample rate defined in macros.h. Use the `sawtooth_step` function to
 * retrieve the next sample of the sawtooth wave.
 * 
 * @sa `SAMPLE_RATE` defined in macros.h
 * 
 * @author Ovidio1005
 * @date 2025-11-16
 */

#include <stdint.h>

/**
 * @brief Get the current frequency of the sawtooth wave.
 * @return The frequency in Hz.
 */
uint16_t sawtooth_frequency(void);
/**
 * @brief Set the frequency of the sawtooth wave.
 * @details The waveform will loop every `SAMPLE_RATE / frequency` samples.
 * @sa `SAMPLE_RATE` defined in macros.h
 * @param frequency The desired frequency in Hz.
 */
void sawtooth_set_frequency(uint16_t frequency);

/**
 * @brief Get the current amplitude of the sawtooth wave.
 * @return The amplitude as an unsigned 8-bit integer.
 */
uint8_t sawtooth_amplitude(void);
/**
 * @brief Set the amplitude of the sawtooth wave.
 * @details The output values will range from -`amplitude`/2 to +`amplitude`/2, centered around 128.
 * @param amplitude The desired amplitude as an unsigned 8-bit integer.
 */
void sawtooth_set_amplitude(uint8_t amplitude);

/**
 * @brief Get the value for the current sample of the sawtooth wave, and advance to the next sample.
 * @return The sample value as an unsigned 8-bit integer.
 */
uint8_t sawtooth_step(void);