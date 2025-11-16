#include "looper.h"

#include "macros.h"
#include "utils.h"
#include "square.h"
#include "sawtooth.h"
#include "triangle.h"
#include "noise.h"
#include "custom.h"

#include <stdlib.h>
#include <stdio.h>

static uint32_t loop_length_sixteenths;

static NoteAttributes *square_notes = NULL;
static NoteAttributes *sawtooth_notes = NULL;
static NoteAttributes *triangle_notes = NULL;
static NoteAttributes *noise_notes = NULL;
static NoteAttributes *custom_notes = NULL;

bool square_active = true;
bool sawtooth_active = true;
bool triangle_active = true;
bool noise_active = true;
bool custom_active = true;
uint8_t active_channel_count = 5;

static uint32_t samples_per_sixteenth;

static uint32_t current_sample;

bool staccato_pause(uint32_t sample_in_sixteenth){
    // First 7/8 of the note plays, last 1/8 is silence
    return sample_in_sixteenth >= (samples_per_sixteenth * 7) / 8;
}

bool double_note_pause(uint32_t sample_in_sixteenth){
    // First 3/8 plays, next 1/8 is silence, last 4/8 plays
    return (sample_in_sixteenth >= (samples_per_sixteenth * 3) / 8) &&
           (sample_in_sixteenth < (samples_per_sixteenth * 4) / 8);
}

void looper_init(uint32_t length_beats, uint32_t tempo_bpm_value) {
    loop_length_sixteenths = length_beats * 4;

    square_notes = (NoteAttributes *)calloc(loop_length_sixteenths, sizeof(NoteAttributes));
    sawtooth_notes = (NoteAttributes *)calloc(loop_length_sixteenths, sizeof(NoteAttributes));
    triangle_notes = (NoteAttributes *)calloc(loop_length_sixteenths, sizeof(NoteAttributes));
    noise_notes = (NoteAttributes *)calloc(loop_length_sixteenths, sizeof(NoteAttributes));
    custom_notes = (NoteAttributes *)calloc(loop_length_sixteenths, sizeof(NoteAttributes));

    // Terminate the program if memory allocation fails
    if(
        !square_notes || !sawtooth_notes || !triangle_notes || !noise_notes || !custom_notes
    ) {
        fprintf(stderr, "Error: Memory allocation failed in looper_init()\n");
        exit(EXIT_FAILURE);
    }

    current_sample = 0;
    samples_per_sixteenth = (SAMPLE_RATE * 60) / (tempo_bpm_value * 4);

    square_set_amplitude(DEFAULT_CHANNEL_AMPLITUDE);
    sawtooth_set_amplitude(DEFAULT_CHANNEL_AMPLITUDE);
    triangle_set_amplitude(DEFAULT_CHANNEL_AMPLITUDE);
    noise_set_amplitude(DEFAULT_CHANNEL_AMPLITUDE);
    custom_set_amplitude(DEFAULT_CHANNEL_AMPLITUDE);
}

void looper_free(void) {
    free(square_notes);
    free(sawtooth_notes);
    free(triangle_notes);
    free(noise_notes);
    free(custom_notes);

    square_notes = NULL;
    sawtooth_notes = NULL;
    triangle_notes = NULL;
    noise_notes = NULL;
    custom_notes = NULL;
}

void looper_set_active_channels(bool square, bool sawtooth, bool triangle, bool noise, bool custom) {
    square_active = square;
    sawtooth_active = sawtooth;
    triangle_active = triangle;
    noise_active = noise;
    custom_active = custom;

    active_channel_count = (square ? 1 : 0) + (sawtooth ? 1 : 0) + (triangle ? 1 : 0) + (noise ? 1 : 0) + (custom ? 1 : 0);
}

void looper_set_note(uint32_t sixteenth, Channel channel, NoteAttributes attributes) {
    if(sixteenth >= loop_length_sixteenths) return; // Out of bounds

    switch(channel) {
        case SQUARE:
            square_notes[sixteenth] = attributes;
            break;
        case SAWTOOTH:
            sawtooth_notes[sixteenth] = attributes;
            break;
        case TRIANGLE:
            triangle_notes[sixteenth] = attributes;
            break;
        case NOISE:
            noise_notes[sixteenth] = attributes;
            break;
        case CUSTOM:
            custom_notes[sixteenth] = attributes;
            break;
        default:
            return; // Invalid channel
    }
}

void looper_set_notes_equal(uint32_t start_sixteenth, uint32_t length_sixteenths, Channel channel, NoteAttributes attributes) {
    uint32_t end_sixteenth = start_sixteenth + length_sixteenths;
    if(end_sixteenth > loop_length_sixteenths) end_sixteenth = loop_length_sixteenths;

    for(uint32_t i = start_sixteenth; i < end_sixteenth; i++) {
        looper_set_note(i, channel, attributes);
    }
}

void looper_set_notes(uint32_t start_sixteenth, uint32_t length_sixteenths, Channel channel, NoteAttributes* notes_array) {
    uint32_t end_sixteenth = start_sixteenth + length_sixteenths;
    if(end_sixteenth > loop_length_sixteenths) end_sixteenth = loop_length_sixteenths;

    for(uint32_t i = start_sixteenth; i < end_sixteenth; i++) {
        looper_set_note(i, channel, notes_array[i - start_sixteenth]);
    }
}

void looper_change_tempo(uint32_t new_tempo_bpm) {
    uint32_t new_samples_per_sixteenth = (SAMPLE_RATE * 60) / (new_tempo_bpm * 4);

    // Adjust current_sample to maintain position in the loop
    current_sample = (uint32_t)((uint64_t)current_sample * samples_per_sixteenth / new_samples_per_sixteenth);

    samples_per_sixteenth = new_samples_per_sixteenth;
}

void looper_set_channel_amplitude(Channel channel, uint8_t amplitude) {
    switch(channel) {
        case SQUARE:
            square_set_amplitude(amplitude);
            break;
        case SAWTOOTH:
            sawtooth_set_amplitude(amplitude);
            break;
        case TRIANGLE:
            triangle_set_amplitude(amplitude);
            break;
        case NOISE:
            noise_set_amplitude(amplitude);
            break;
        case CUSTOM:
            custom_set_amplitude(amplitude);
            break;
        default:
            return; // Invalid channel
    }
}

uint8_t looper_step(void) {
    uint32_t note_index = looper_current_sixteenth() % loop_length_sixteenths;

    uint32_t sample_in_sixteenth = current_sample % samples_per_sixteenth;

    uint16_t value = 0;
    
    if(square_active && square_notes[note_index].play) {
        uint32_t frequency = linear_interpolate_32(
            square_notes[note_index].frequency_start,
            square_notes[note_index].frequency_end,
            sample_in_sixteenth,
            samples_per_sixteenth
        );

        uint32_t amplitude = linear_interpolate_32(
            square_notes[note_index].volume_start,
            square_notes[note_index].volume_end,
            sample_in_sixteenth,
            samples_per_sixteenth
        );

        if(
            square_notes[note_index].staccato && staccato_pause(sample_in_sixteenth) ||
            square_notes[note_index].is_double && double_note_pause(sample_in_sixteenth)
        ) amplitude = 0;

        square_set_frequency(frequency);

        value += apply_amplitude(square_step(), amplitude);
    } else value += square_active ? 128 : 0;
    if(sawtooth_active && sawtooth_notes[note_index].play) {
        uint32_t frequency = linear_interpolate_32(
            sawtooth_notes[note_index].frequency_start,
            sawtooth_notes[note_index].frequency_end,
            sample_in_sixteenth,
            samples_per_sixteenth
        );

        uint32_t amplitude = linear_interpolate_32(
            sawtooth_notes[note_index].volume_start,
            sawtooth_notes[note_index].volume_end,
            sample_in_sixteenth,
            samples_per_sixteenth
        );

        if(
            sawtooth_notes[note_index].staccato && staccato_pause(sample_in_sixteenth) ||
            sawtooth_notes[note_index].is_double && double_note_pause(sample_in_sixteenth)
        ) amplitude = 0;

        sawtooth_set_frequency(frequency);

        value += apply_amplitude(sawtooth_step(), amplitude);
    } else value += sawtooth_active ? 128 : 0;
    if(triangle_active && triangle_notes[note_index].play) {
        uint32_t frequency = linear_interpolate_32(
            triangle_notes[note_index].frequency_start,
            triangle_notes[note_index].frequency_end,
            sample_in_sixteenth,
            samples_per_sixteenth
        );
        
        uint32_t amplitude = linear_interpolate_32(
            triangle_notes[note_index].volume_start,
            triangle_notes[note_index].volume_end,
            sample_in_sixteenth,
            samples_per_sixteenth
        );

        if(
            triangle_notes[note_index].staccato && staccato_pause(sample_in_sixteenth) ||
            triangle_notes[note_index].is_double && double_note_pause(sample_in_sixteenth)
        ) amplitude = 0;

        triangle_set_frequency(frequency);
        value += apply_amplitude(triangle_step(), amplitude);
    } else value += triangle_active ? 128 : 0;
    if(noise_active && noise_notes[note_index].play) {
        uint32_t amplitude = linear_interpolate_32(
            noise_notes[note_index].volume_start,
            noise_notes[note_index].volume_end,
            sample_in_sixteenth,
            samples_per_sixteenth
        );

        if(
            noise_notes[note_index].staccato && staccato_pause(sample_in_sixteenth) ||
            noise_notes[note_index].is_double && double_note_pause(sample_in_sixteenth)
        ) amplitude = 0;

        value += apply_amplitude(noise_step(), amplitude);
    } else value += noise_active ? 128 : 0;
    if(custom_active && custom_notes[note_index].play) {
        uint32_t frequency = linear_interpolate_32(
            custom_notes[note_index].frequency_start,
            custom_notes[note_index].frequency_end,
            sample_in_sixteenth,
            samples_per_sixteenth
        );

        uint32_t amplitude = linear_interpolate_32(
            custom_notes[note_index].volume_start,
            custom_notes[note_index].volume_end,
            sample_in_sixteenth,
            samples_per_sixteenth
        );

        if(
            custom_notes[note_index].staccato && staccato_pause(sample_in_sixteenth) ||
            custom_notes[note_index].is_double && double_note_pause(sample_in_sixteenth)
        ) amplitude = 0;

        custom_set_frequency(frequency);

        value += apply_amplitude(custom_step(), amplitude);
    } else value += custom_active ? 128 : 0;

    current_sample++;
    if(current_sample >= samples_per_sixteenth * loop_length_sixteenths) {
        current_sample = 0;
    }

    value /= active_channel_count;
    if(value > 255) value = 255; // Clamp to 8-bit range
    return value;
}

uint32_t looper_current_sample(void) {
    return current_sample;
}

uint32_t looper_current_sixteenth(void) {
    return current_sample / samples_per_sixteenth;
}

uint32_t looper_current_beat(void) {
    return current_sample / (samples_per_sixteenth * 4);
}

void looper_to_sample(uint32_t sample) {
    current_sample = sample;
}

void looper_to_sixteenth(uint32_t sixteenth) {
    current_sample = (uint64_t)sixteenth * samples_per_sixteenth;
}

void looper_to_beat(uint32_t beat) {
    current_sample = (uint64_t)beat * samples_per_sixteenth * 4;
}

void looper_restart(void) {
    current_sample = 0;
}