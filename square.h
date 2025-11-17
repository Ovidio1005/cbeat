/**
 * @file square.h
 * @brief Header file for square wave generator functions.
 * 
 * @details This module provides functions to generate a square wave signal as an
 * unsigned 8-bit integer output. The returned values will be -`amplitude`/2 or
 * +`amplitude`/2, and will loop `frequency` times per second, based on the sample
 * rate defined in macros.h. Use the `square_step` function to retrieve the next
 * sample of the square wave.
 * 
 * @sa `SAMPLE_RATE` defined in macros.h
 * 
 * @author Ovidio1005
 * @date 2025-11-16
 */

#include <stdint.h>

/**
 * @brief Get the current duty cycle of the square wave.
 * @return The duty cycle as an unsigned 8-bit integer, with 0 being 0% and 255 being 100%.
 */
uint8_t square_duty_cycle(void);
/**
 * @brief Set the duty cycle of the square wave.
 * @param duty The desired duty cycle as an unsigned 8-bit integer, with 0 being 0% and 255 being 100%.
 */
void square_set_duty_cycle(uint8_t duty);

/**
 * @brief Get the current frequency of the square wave.
 * @return The frequency in Hz.
 */
uint16_t square_frequency(void);
/**
 * @brief Set the frequency of the square wave.
 * @details The waveform will loop every `SAMPLE_RATE / frequency` samples.
 * @sa `SAMPLE_RATE` defined in macros.h
 * @param frequency The desired frequency in Hz.
 */
void square_set_frequency(uint16_t frequency);

/**
 * @brief Get the current amplitude of the triangle wave.
 * @return The amplitude as an unsigned 8-bit integer.
 */
uint8_t square_amplitude(void);
/**
 * @brief Set the amplitude of the square wave.
 * @details The output values will be -`amplitude`/2 or +`amplitude`/2.
 * @param amplitude The desired amplitude as an unsigned 8-bit integer.
 */
void square_set_amplitude(uint8_t amplitude);

/**
 * @brief Get the value for the current sample of the square wave, and advance to the next sample.
 * @return The sample value as an unsigned 8-bit integer.
 */
uint8_t square_step(void);