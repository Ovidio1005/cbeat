/**
 * @file composer.h
 * 
 * @author Ovidio1005
 * @date 2025-11-18
 */

#include "looper.h"
#include <stdarg.h>

#define DECAY_MIN_VOLUME 128
#define HIT_MIN_VOLUME 64
#define DECAY_SLOW_SAMPLES 16000
#define DECAY_MEDIUM_SAMPLES 4000
#define DECAY_FAST_SAMPLES 1000
#define HIT_SAMPLES 500

typedef enum envelope {
    CONSTANT,
    DECAY_SLOW,
    DECAY_MEDIUM,
    DECAY_FAST,
    HIT
} Envelope;

// Only used for glissando
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
    int start_note_index, int note_index_step, int count
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