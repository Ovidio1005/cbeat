#pragma once

/**
 * @file looper.h
 * @brief Header file for the looper module, which manages a multi-channel audio looper.
 * 
 * This module allows for the creation and manipulation of a musical looper with multiple waveform channels.
 * Each channel can have its own set of notes defined by various attributes such as frequency and volume.
 * The looper supports real-time tempo changes and provides functions to step through the audio samples.
 * Note that the chosen BPM is only approximate, as the looper is limited to having an integer number of samples per sixteenth note.
 * 
 * @author Ovidio1005
 * @date 2025-11-15
 */

// TODO: Update documentation to reflect the following changes:
// - In the looper, require the active channels to be set on initialization, and allocate note arrays only for those channels
// - Get rid of the channel-amplitude logic - we already have volume per note
// - Condense `play`, `is_double`, and `staccato` into a single `uint8_t` bitmask to save 2 bytes per note

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Attributes defining a (portion of a) musical note.
 */
typedef struct note_attributes {
    /** Bitmask of flags for note properties.
     * 
     * Bit 0: Play - If set to false (0), the note represents a pause and every other attribute is ignored.
     * Bit 1: Staccato - Set to false (0) to chain multiple notes into a single, longer note; set to true (1) to add a short pause between notes.
     * Bit 2: Double Note - If set to true (1), a small pause is added to the middle of the note, effectively turning it into two shorter notes.
     */
    uint8_t flags;

    /** The starting frequency of the note in Hz. */
    uint16_t frequency_start;
    /** The ending frequency of the note in Hz. */
    uint16_t frequency_end;
    /** The starting volume of the note (0-255). */
    uint8_t volume_start;
    /** The ending volume of the note (0-255). */
    uint8_t volume_end;
} NoteAttributes;

/**
 * @brief Enumeration of available waveform channels.
 */
typedef enum channel {
    SQUARE,
    SAWTOOTH,
    TRIANGLE,
    NOISE,
    CUSTOM
} Channel;

/**
 * @brief Initializes the looper with the specified length and tempo.
 * 
 * @details This function allocate the necessary memory and sets the current step to the beginning of the loop.
 * It must be called before any other function in this module.
 * 
 * The contents of the loop for each active channel are set to all pauses by default.
 * Memory for the inactive channels is not allocated, so they should not be used.
 * The number of active channels is used to scale the final waveform output.
 * 
 * This function does not free any previously allocated memory;
 * looper_free() must be called before calling this function again.
 * 
 * @sa `looper_free()`
 * @sa `looper_set_channel_amplitude()`
 * 
 * @param length_beats Length of the loop in beats.
 * @param tempo_bpm Tempo in beats per minute.
 * @param square_enabled Whether the square channel is active.
 * @param sawtooth_enabled Whether the sawtooth channel is active.
 * @param triangle_enabled Whether the triangle channel is active.
 * @param noise_enabled Whether the noise channel is active.
 * @param custom_enabled Whether the custom channel is active.
 */
void looper_init(
    uint16_t length_beats, uint16_t tempo_bpm,
    bool square_enabled, bool sawtooth_enabled, bool triangle_enabled, bool noise_enabled, bool custom_enabled
);

/**
 * @brief Frees all allocated resources used by the looper.
 * 
 * @details This function should be called when the looper is no longer needed, to avoid memory leaks.
 * It frees all memory allocated by looper_init(), and should be called before calling looper_init() again.
 * 
 * Once this function is called, no other functions in this module should be used until looper_init() is called again.
 * 
 * @sa `looper_init()`
 */
void looper_free(void);

/**
 * @brief Sets the note attributes for a specific sixteenth note on a given channel.
 * 
 * @param sixteenth The sixteenth note index within the loop to set the note for.
 * @param channel The waveform channel to set the note on.
 * @param attributes The attributes of the note to set.
 */
void looper_set_note(uint16_t sixteenth, Channel channel, NoteAttributes attributes);

/**
 * @brief Sets the note attributes for a range of sixteenth notes on a given channel.
 * 
 * @param start_sixteenth The starting sixteenth note index within the loop to set the notes for.
 * @param length_sixteenths The number of sixteenth notes to set.
 * @param channel The waveform channel to set the notes on.
 * @param attributes The attributes of the notes to set.
 */
void looper_set_notes_equal(uint16_t start_sixteenth, uint16_t length_sixteenths, Channel channel, NoteAttributes attributes);

/**
 * @brief Sets the note attributes for a range of sixteenth notes on a given channel using an array of note attributes.
 * 
 * @param start_sixteenth The starting sixteenth note index within the loop to set the notes for.
 * @param length_sixteenths The number of sixteenth notes to set.
 * @param channel The waveform channel to set the notes on.
 * @param notes_array An array of NoteAttributes structures containing the attributes for each sixteenth note.
 */
void looper_set_notes(uint16_t start_sixteenth, uint16_t length_sixteenths, Channel channel, NoteAttributes* notes_array);

/**
 * @brief Reads the note attributes for a range of sixteenth notes on a given channel into an output array.
 * 
 * @param start_sixteenth The starting sixteenth note index within the loop to read the notes from.
 * @param length_sixteenths The number of sixteenth notes to read.
 * @param channel The waveform channel to read the notes from.
 * @param out_notes_array An array of NoteAttributes structures to store the read attributes. Must be at least length_sixteenths in size.
 * @return The number of notes read and stored in out_notes_array.
 */
uint16_t looper_read_notes(uint16_t start_sixteenth, uint16_t length_sixteenths, Channel channel, NoteAttributes* out_notes_array);

/**
 * @brief Changes the tempo of the looper.
 * 
 * @param new_tempo_bpm The new tempo in beats per minute.
 */
void looper_change_tempo(uint16_t new_tempo_bpm);

/**
 * @brief Retrieves the number of samples per sixteenth note at the current tempo.
 * 
 * @return The number of samples per sixteenth note.
 */
uint16_t looper_samples_per_sixteenth(void);

// TODO: remove after changing the rest of the documentation (kept for reference for now)
// /**
//  * @brief Sets the amplitude for a specific waveform channel.
//  * 
//  * @details The final waveform output is a sum of all channels, each
//  * scaled by its amplitude, and divided by the active channel count.
//  * 
//  * @sa `looper_set_active_channels()`
//  * 
//  * @param channel The waveform channel to set the amplitude for.
//  * @param amplitude The amplitude value (0-255).
//  */
// void looper_set_channel_amplitude(Channel channel, uint8_t amplitude);

/**
 * @brief Computes the value of the combined waveform output at the current sample, and advances to the next sample.
 * 
 * @details The final waveform output is a sum of all channels, each scaled
 * by its amplitude, and divided by the active channel count. If the next
 * sample exceeds the length of the loop, it wraps around to the beginning.
 * 
 * @return The combined waveform output value at the current sample as an 8-bit signed integer.
 */
uint8_t looper_step(void);

/**
 * @brief Retrieves the current position within the loop.
 * 
 * @return The current sample index within the loop.
 */
uint32_t looper_current_sample(void);

/**
 * @brief Retrieves the current sixteenth note position within the loop.
 * 
 * @return The current sixteenth note index within the loop.
 */
uint16_t looper_current_sixteenth(void);

/**
 * @brief Retrieves the current beat position within the loop.
 * 
 * @return The current beat index within the loop.
 */
uint16_t looper_current_beat(void);

/**
 * @brief Sets the current position within the loop to the specified sample index.
 * 
 * @param sample The sample index to set the current position to.
 */
void looper_to_sample(uint32_t sample);

/**
 * @brief Sets the current position within the loop to the beginning of the specified sixteenth note.
 * 
 * @param sixteenth The sixteenth note index to set the current position to.
 */
void looper_to_sixteenth(uint16_t sixteenth);

/**
 * @brief Sets the current position within the loop to the beginning of the specified beat.
 * 
 * @param beat The beat index to set the current position to.
 */
void looper_to_beat(uint16_t beat);

/**
 * @brief Restarts the loop, setting the current position to the beginning.
 */
void looper_restart(void);