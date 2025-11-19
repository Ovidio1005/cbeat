#pragma once

/**
 * @file custom.h
 * @brief Header file for custom waveform generator functions.
 * 
 * @details This module provides functions to generate a wave signal based on user-defined data
 * as an unsigned 8-bit integer output. Use the `custom_set_data` function to allocate memory and
 * set the waveform data, and the `custom_free` function to free the allocated memory.
 * 
 * The returned values will be scaled from a range of 0-255 to a range of -`amplitude`/2 to
 * +`amplitude`/2, centered around 128. The samples will be stepped throug at a rate based on
 * the sample rate defined in macros.h and the frequency provided with `custom_set_frequency()`;
 * the frequency refers to how many times per second the waveform will go through `SAMPLE_RATE`
 * samples, not how often the provided data will be looped through. Use the `custom_step` function
 * to retrieve the next sample of the custom waveform.
 * 
 * @sa `SAMPLE_RATE` defined in macros.h
 * 
 * @author Ovidio1005
 * @date 2025-11-16
 */

#include <stdint.h>

/**
 * @brief Set the custom waveform data.
 * @details This function will dynamically allocate memory for the waveform data. `custom_step` should
 * never be called before this function. Previously allocated memory will not be freed automatically;
 * use the `custom_free` function to free it when no longer needed.
 * @param data Pointer to an array of unsigned 8-bit integers representing the waveform data.
 * @param length The number of samples provided in the data array.
 */
void custom_set_data(const uint8_t* data, uint16_t length);

/**
 * @brief Free the memory allocated for the custom waveform data.
 * @details This function should be called to free the memory allocated by `custom_set_data`
 * when the custom waveform is no longer needed, or when the data is to be replaced.
 */
void custom_free(void);

/**
 * @brief Get the current frequency of the custom waveform.
 * @return The frequency in Hz.
 */
uint16_t custom_frequency(void);
/**
 * @brief Set the frequency of the custom waveform.
 * @details The waveform will loop every `SAMPLE_RATE / frequency` samples.
 * @sa `SAMPLE_RATE` defined in macros.h
 * @param frequency The desired frequency in Hz.
 */
void custom_set_frequency(uint16_t frequency);

/**
 * @brief Get the current amplitude of the custom waveform.
 * @return The amplitude as an unsigned 8-bit integer.
 */
uint8_t custom_amplitude(void);
/**
 * @brief Set the amplitude of the custom waveform.
 * @details The output values will range from -`amplitude`/2 to +`amplitude`/2, centered around 128.
 * @param amplitude The desired amplitude as an unsigned 8-bit integer.
 */
void custom_set_amplitude(uint8_t amplitude);

/**
 * @brief Get the value for the current sample of the custom waveform, and advance to the next sample.
 * @return The sample value as an unsigned 8-bit integer.
 */
uint8_t custom_step(void);