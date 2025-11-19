#pragma once

/**
 * @file composer.h
 * 
 * @author Ovidio1005
 * @date 2025-11-18
 */

 // TODO: Documentation

#include "looper.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

#define DECAY_VOLUME_FACTOR 4
#define HIT_VOLUME_FACTOR 4
#define DECAY_SLOW_SAMPLES 32000
#define DECAY_MEDIUM_SAMPLES 16000
#define DECAY_FAST_SAMPLES 8000
#define HIT_SAMPLES 4000

typedef enum envelope {
    CONSTANT,
    DECAY_SLOW,
    DECAY_MEDIUM,
    DECAY_FAST,
    HIT
} Envelope;

// Only used for glissando and semitone shift
int composer_get_note_index(uint16_t frequency);
uint16_t composer_get_frequency(int note_index);

void composer_set_note(
    Channel channel,
    uint16_t start_beat, uint16_t start_sixteenth, uint16_t length_sixteenths,
    uint8_t volume, Envelope envelope, bool staccato, bool doubles,
    uint16_t frequency
);

void composer_set_notes(
    Channel channel,
    uint16_t start_beat, uint16_t start_sixteenth, uint16_t length_sixteenths,
    uint8_t volume, Envelope envelope, bool staccato, bool doubles,
    int count, ... // Variable number of frequency (uint16_t)
);

void composer_set_slide(
    Channel channel,
    uint16_t start_beat, uint16_t start_sixteenth, uint16_t length_sixteenths,
    uint8_t volume, Envelope envelope, bool staccato, bool doubles,
    uint16_t frequency_start, uint16_t frequency_end
);

void composer_set_slides(
    Channel channel,
    uint16_t start_beat, uint16_t start_sixteenth, uint16_t length_sixteenths,
    uint8_t volume, Envelope envelope, bool staccato, bool doubles,
    int count, ... // Variable number of frequency pairs (uint16_t frequency_start, uint16_t frequency_end)
);

void composer_set_rest(
    Channel channel,
    uint16_t start_beat, uint16_t start_sixteenth, uint16_t length_sixteenths
);

void composer_set_rests(
    Channel channel,
    uint16_t start_beat, uint16_t start_sixteenth, uint16_t length_sixteenths,
    uint16_t interval_sixteenths, int count
);

void composer_set_glissando(
    Channel channel,
    uint16_t start_beat, uint16_t start_sixteenth, uint16_t length_sixteenths,
    uint8_t volume, Envelope envelope, bool staccato, bool doubles,
    int start_note_index, int note_index_step
);

void composer_apply_dynamics(
    Channel channel,
    uint16_t start_beat, uint16_t start_sixteenth, uint16_t length_sixteenths,
    uint8_t start_volume_factor, uint8_t end_volume_factor
);

void composer_copy_section(
    Channel src_channel, uint16_t src_start_beat, uint16_t src_start_sixteenth,
    Channel dest_channel, uint16_t dest_start_beat, uint16_t dest_start_sixteenth,
    uint16_t length_sixteenths
);

void composer_shift_semitones(
    Channel channel,
    uint16_t start_beat, uint16_t start_sixteenth, uint16_t length_sixteenths,
    int semitone_shift
);

void composer_shift_octaves(
    Channel channel,
    uint16_t start_beat, uint16_t start_sixteenth, uint16_t length_sixteenths,
    int octave_shift
);