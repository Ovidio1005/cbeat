#include "looper.h"

#include "macros.h"
#include "utils.h"
#include "square.h"
#include "sawtooth.h"
#include "triangle.h"
#include "noise.h"
#include "custom.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

static uint16_t loop_length_sixteenths;
uint32_t loop_length_samples; // Used in looper_step to avoid recalculating it every time

static NoteAttributes *square_notes = NULL;
static NoteAttributes *sawtooth_notes = NULL;
static NoteAttributes *triangle_notes = NULL;
static NoteAttributes *noise_notes = NULL;
static NoteAttributes *custom_notes = NULL;

uint8_t active_channel_count = 0;

static uint16_t samples_per_sixteenth;

static uint32_t current_sample;

void compute_attributes(NoteAttributes attributes, uint16_t sample_in_sixteenth, uint16_t* out_frequency, uint8_t* out_amplitude) {
    if(
        // Play flag is false
        ((attributes.flags & 0x01) == 0) ||
        // Staccato: first 7/8 of the note plays, last 1/8 is silence
        (attributes.flags & 0x02 && sample_in_sixteenth >= (samples_per_sixteenth / 8) * 7) ||
        // Double note: first 3/8 plays, next 1/8 is silence, last 4/8 plays
        (attributes.flags & 0x04 && sample_in_sixteenth >= (samples_per_sixteenth / 8) * 3 && sample_in_sixteenth < (samples_per_sixteenth / 8) * 4)
    ) {
        *out_frequency = 0;
        *out_amplitude = 0;
    } else {
        *out_frequency = linear_interpolate_16(
            attributes.frequency_start,
            attributes.frequency_end,
            sample_in_sixteenth,
            samples_per_sixteenth
        );

        *out_amplitude = linear_interpolate_8(
            attributes.volume_start,
            attributes.volume_end,
            sample_in_sixteenth,
            samples_per_sixteenth
        );
    }
}

void looper_init(
    uint16_t length_beats, uint16_t tempo_bpm_value,
    bool square_enabled, bool sawtooth_enabled, bool triangle_enabled, bool noise_enabled, bool custom_enabled
) {
    loop_length_sixteenths = length_beats * 4;

    if(square_enabled) {
        square_notes = (NoteAttributes *)calloc(loop_length_sixteenths, sizeof(NoteAttributes));
        if(!square_notes) {
            fprintf(stderr, "Error: Memory allocation failed for square channel in looper_init()\n");
            exit(EXIT_FAILURE);
        }
        active_channel_count++;
    }
    if(sawtooth_enabled) {
        sawtooth_notes = (NoteAttributes *)calloc(loop_length_sixteenths, sizeof(NoteAttributes));
        if(!sawtooth_notes) {
            fprintf(stderr, "Error: Memory allocation failed for sawtooth channel in looper_init()\n");
            exit(EXIT_FAILURE);
        }
        active_channel_count++;
    }
    if(triangle_enabled) {
        triangle_notes = (NoteAttributes *)calloc(loop_length_sixteenths, sizeof(NoteAttributes));
        if(!triangle_notes) {
            fprintf(stderr, "Error: Memory allocation failed for triangle channel in looper_init()\n");
            exit(EXIT_FAILURE);
        }
        active_channel_count++;
    }
    if(noise_enabled) {
        noise_notes = (NoteAttributes *)calloc(loop_length_sixteenths, sizeof(NoteAttributes));
        if(!noise_notes) {
            fprintf(stderr, "Error: Memory allocation failed for noise channel in looper_init()\n");
            exit(EXIT_FAILURE);
        }
        active_channel_count++;
    }
    if(custom_enabled) {
        custom_notes = (NoteAttributes *)calloc(loop_length_sixteenths, sizeof(NoteAttributes));
        if(!custom_notes) {
            fprintf(stderr, "Error: Memory allocation failed for custom channel in looper_init()\n");
            exit(EXIT_FAILURE);
        }
        active_channel_count++;
    }

    current_sample = 0;
    samples_per_sixteenth = (SAMPLE_RATE * 60) / (tempo_bpm_value * 4);
    loop_length_samples = samples_per_sixteenth * loop_length_sixteenths;
}

void looper_free(void) {
    if(square_notes) {
        free(square_notes);
        square_notes = NULL;
    }
    if(sawtooth_notes) {
        free(sawtooth_notes);
        sawtooth_notes = NULL;
    }
    if(triangle_notes) {
        free(triangle_notes);
        triangle_notes = NULL;
    }
    if(noise_notes) {
        free(noise_notes);
        noise_notes = NULL;
    }
    if(custom_notes) {
        free(custom_notes);
        custom_notes = NULL;
    }

    active_channel_count = 0;
}

void looper_set_note(uint16_t sixteenth, Channel channel, NoteAttributes attributes) {
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

void looper_set_notes_equal(uint16_t start_sixteenth, uint16_t length_sixteenths, Channel channel, NoteAttributes attributes) {
    uint16_t end_sixteenth = start_sixteenth + length_sixteenths;
    if(end_sixteenth > loop_length_sixteenths) end_sixteenth = loop_length_sixteenths;

    for(uint16_t i = start_sixteenth; i < end_sixteenth; i++) {
        looper_set_note(i, channel, attributes);
    }
}

void looper_set_notes(uint16_t start_sixteenth, uint16_t length_sixteenths, Channel channel, NoteAttributes* notes_array) {
    uint16_t end_sixteenth = start_sixteenth + length_sixteenths;
    if(end_sixteenth > loop_length_sixteenths) end_sixteenth = loop_length_sixteenths;

    for(uint16_t i = start_sixteenth; i < end_sixteenth; i++) {
        looper_set_note(i, channel, notes_array[i - start_sixteenth]);
    }
}

uint16_t looper_read_notes(uint16_t start_sixteenth, uint16_t length_sixteenths, Channel channel, NoteAttributes* out_notes_array){
    NoteAttributes* source_array = NULL;
    switch(channel) {
        case SQUARE:
            source_array = square_notes;
            break;
        case SAWTOOTH:
            source_array = sawtooth_notes;
            break;
        case TRIANGLE:
            source_array = triangle_notes;
            break;
        case NOISE:
            source_array = noise_notes;
            break;
        case CUSTOM:
            source_array = custom_notes;
            break;
        default:
            return 0; // Invalid channel
    }

    if(!source_array || start_sixteenth >= loop_length_sixteenths) return 0; // Out of bounds or channel not enabled

    uint16_t available = loop_length_sixteenths - start_sixteenth;
    uint16_t i;
    for(i = 0; i < length_sixteenths && i < available; i++) {
        out_notes_array[i] = source_array[start_sixteenth + i];
    }

    return i; // Number of notes read
}

void looper_change_tempo(uint16_t new_tempo_bpm) {
    uint16_t new_samples_per_sixteenth = (SAMPLE_RATE * 60) / (new_tempo_bpm * 4);

    // Adjust current_sample to maintain position in the loop
    current_sample = (uint32_t)((uint64_t)current_sample * samples_per_sixteenth / new_samples_per_sixteenth);

    samples_per_sixteenth = new_samples_per_sixteenth;
    loop_length_samples = samples_per_sixteenth * loop_length_sixteenths;
}

uint16_t looper_samples_per_sixteenth(void) {
    return samples_per_sixteenth;
}

uint8_t looper_step(void) {
    uint16_t note_index = looper_current_sixteenth() % loop_length_sixteenths;

    uint16_t sample_in_sixteenth = current_sample % samples_per_sixteenth;

    uint16_t value = 0;
    
    if(square_notes) {
        uint16_t frequency;
        uint8_t amplitude;
        compute_attributes(square_notes[note_index], sample_in_sixteenth, &frequency, &amplitude);

        square_set_frequency(frequency);
        square_set_amplitude(amplitude);

        value += square_step();
    }
    if(sawtooth_notes) {
        uint16_t frequency;
        uint8_t amplitude;
        compute_attributes(sawtooth_notes[note_index], sample_in_sixteenth, &frequency, &amplitude);

        sawtooth_set_frequency(frequency);
        sawtooth_set_amplitude(amplitude);

        value += sawtooth_step();
    }
    if(triangle_notes) {
        uint16_t frequency;
        uint8_t amplitude;
        compute_attributes(triangle_notes[note_index], sample_in_sixteenth, &frequency, &amplitude);

        triangle_set_frequency(frequency);
        triangle_set_amplitude(amplitude);

        value += triangle_step();
    }
    if(noise_notes) {
        uint16_t frequency; // Frequency not used for noise, but needed for compute_attributes
        uint8_t amplitude;
        compute_attributes(noise_notes[note_index], sample_in_sixteenth, &frequency, &amplitude);

        noise_set_amplitude(amplitude);

        value += noise_step();
    }
    if(custom_notes) {
        uint16_t frequency;
        uint8_t amplitude;
        compute_attributes(custom_notes[note_index], sample_in_sixteenth, &frequency, &amplitude);

        custom_set_frequency(frequency);
        custom_set_amplitude(amplitude);

        value += custom_step();
    }

    current_sample = (current_sample + 1) % loop_length_samples;

    value /= active_channel_count;
    if(value > 255) value = 255; // Clamp to 8-bit range
    return value;
}

uint32_t looper_current_sample(void) {
    return current_sample;
}

uint16_t looper_current_sixteenth(void) {
    return current_sample / samples_per_sixteenth;
}

uint16_t looper_current_beat(void) {
    return current_sample / (samples_per_sixteenth * 4);
}

void looper_to_sample(uint32_t sample) {
    current_sample = sample;
}

void looper_to_sixteenth(uint16_t sixteenth) {
    current_sample = (uint32_t)sixteenth * samples_per_sixteenth;
}

void looper_to_beat(uint16_t beat) {
    current_sample = (uint32_t)beat * samples_per_sixteenth * 4;
}

void looper_restart(void) {
    current_sample = 0;
}