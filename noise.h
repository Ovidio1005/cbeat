/**
 * @file noise.h
 * @brief Header file for noise waveform generator functions.
 * 
 * @details This module provides functions to generate a noise waveform signal as an
 * unsigned 8-bit integer output. The returned values will be between -`amplitude`/2
 * and +`amplitude`/2, centered around 128. Unlike other waveforms, noise does not have
 * a defined frequency. Use the `noise_step` function to retrieve the next sample of the
 * noise waveform.
 * 
 * @author Ovidio1005
 * @date 2025-11-16
 */

#include <stdint.h>

/**
 * @brief Get the current amplitude of the noise waveform.
 * @return The amplitude as an unsigned 8-bit integer.
 */
uint8_t noise_amplitude(void);
/**
 * @brief Set the amplitude of the noise waveform.
 * @details The output values will range from -`amplitude`/2 to +`amplitude`/2, centered around 128.
 * @param amplitude The desired amplitude as an unsigned 8-bit integer.
 */
void noise_set_amplitude(uint8_t amplitude);

/**
 * @brief Get the value for the current sample of the noise waveform, and advance to the next sample.
 * @return The sample value as an unsigned 8-bit integer.
 */
uint8_t noise_step(void);