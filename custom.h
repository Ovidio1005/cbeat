/**
 * @file custom.h
 * @brief Header file for custom wave generator functions.
 * 
 * @details This module provides functions to generate a wave signal based on user-defined data
 * as an unsigned 8-bit integer output. Use the `custom_set_data` function to set the waveform
 * data. The custom data is expected to be `SAMPLE_RATE` samples long, representing one second
 * of audio; if less data is provided, the remaining samples will be filled with silence (128).
 * The returned values will be scaled from a range of 0-255 to a range of -`amplitude`/2 to
 * +`amplitude`/2, centered around 128, and will loop `frequency` times per second, based on
 * the sample rate defined in macros.h. Use the `custom_step` function to retrieve the next
 * sample of the custom waveform.
 * 
 * @sa `SAMPLE_RATE` defined in macros.h
 * 
 * @author Ovidio1005
 * @date 2025-11-16
 */

#include <stdint.h>

/**
 * @brief Set the custom waveform data.
 * @details The data array should contain up to `SAMPLE_RATE` samples. If fewer samples are
 * provided, the remaining samples will be filled with silence (128).
 * @param data Pointer to an array of unsigned 8-bit integers representing the waveform data.
 * @param length The number of samples provided in the data array.
 */
void custom_set_data(const uint8_t* data, uint32_t length);

/**
 * @brief Get the current frequency of the custom waveform.
 * @return The frequency in Hz.
 */
uint32_t custom_frequency(void);
/**
 * @brief Set the frequency of the custom waveform.
 * @details The waveform will loop every `SAMPLE_RATE / frequency` samples.
 * @sa `SAMPLE_RATE` defined in macros.h
 * @param frequency The desired frequency in Hz.
 */
void custom_set_frequency(uint32_t frequency);

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