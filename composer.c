#include "composer.h"
#include "utils.h"
#include "macros.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

static const uint16_t NOTES[] = {
    C_0, CSHARP_0, D_0, DSHARP_0, E_0, F_0, FSHARP_0, G_0, GSHARP_0, A_0, ASHARP_0, B_0,
    C_1, CSHARP_1, D_1, DSHARP_1, E_1, F_1, FSHARP_1, G_1, GSHARP_1, A_1, ASHARP_1, B_1,
    C_2, CSHARP_2, D_2, DSHARP_2, E_2, F_2, FSHARP_2, G_2, GSHARP_2, A_2, ASHARP_2, B_2,
    C_3, CSHARP_3, D_3, DSHARP_3, E_3, F_3, FSHARP_3, G_3, GSHARP_3, A_3, ASHARP_3, B_3,
    C_4, CSHARP_4, D_4, DSHARP_4, E_4, F_4, FSHARP_4, G_4, GSHARP_4, A_4, ASHARP_4, B_4,
    C_5, CSHARP_5, D_5, DSHARP_5, E_5, F_5, FSHARP_5, G_5, GSHARP_5, A_5, ASHARP_5, B_5,
    C_6, CSHARP_6, D_6, DSHARP_6, E_6, F_6, FSHARP_6, G_6, GSHARP_6, A_6, ASHARP_6, B_6,
    C_7, CSHARP_7, D_7, DSHARP_7, E_7, F_7, FSHARP_7, G_7, GSHARP_7, A_7, ASHARP_7, B_7,
    C_8, CSHARP_8, D_8, DSHARP_8, E_8, F_8, FSHARP_8, G_8, GSHARP_8, A_8, ASHARP_8, B_8
};

uint8_t apply_envelope(Envelope envelope, uint8_t volume, uint32_t sample) {
    if(sample > 0xFFFF) sample = 0xFFFF;

    uint8_t min_volume;
    uint32_t decay_length;

    switch(envelope) {
        case CONSTANT:
            return volume;
        case DECAY_SLOW:
            min_volume = volume / DECAY_VOLUME_FACTOR;
            decay_length = DECAY_SLOW_SAMPLES;
            break;
        case DECAY_MEDIUM:
            min_volume = volume / DECAY_VOLUME_FACTOR;
            decay_length = DECAY_MEDIUM_SAMPLES;
            break;
        case DECAY_FAST:
            min_volume = volume / DECAY_VOLUME_FACTOR;
            decay_length = DECAY_FAST_SAMPLES;
            break;
        case HIT:
            min_volume = volume / HIT_VOLUME_FACTOR;
            decay_length = HIT_SAMPLES;
            break;
        default:
            return volume;
    }

    // Note: the function already clamps position to [0, length]
    if(envelope == HIT) {
        return quadratic_interpolate_8(volume, min_volume, sample, decay_length);
    } else {
        return linear_interpolate_8(volume, min_volume, sample, decay_length);
    }
}

int composer_get_note_index(uint16_t frequency) {
    for (int i = 0; i < sizeof(NOTES) / sizeof(uint16_t); i++) {
        if (NOTES[i] >= frequency) {
            return i;
        }
    }
    return -1; // Not found
}

uint16_t composer_get_frequency(int note_index) {
    if (note_index < 0 || note_index >= sizeof(NOTES) / sizeof(uint16_t)) {
        return 0; // Invalid index
    }
    return NOTES[note_index];
}

void composer_set_note(
    Channel channel,
    uint16_t start_beat, uint16_t start_sixteenth, uint16_t length_sixteenths,
    uint8_t volume, Envelope envelope, bool staccato, bool doubles,
    uint16_t frequency
){
    for(int i = 0; i < length_sixteenths; i++) {
        uint16_t sixteenth = (start_beat * 4) + start_sixteenth + i;
        uint32_t sample_in_note = (uint32_t)looper_samples_per_sixteenth() * i;
        
        NoteAttributes attrs = {
            .flags = 1 + (staccato && i == length_sixteenths - 1 ? 2 : 0) + (doubles ? 4 : 0),
            .frequency_start = frequency,
            .frequency_end = frequency,
            .volume_start = apply_envelope(envelope, volume, sample_in_note),
            .volume_end = apply_envelope(envelope, volume, sample_in_note + looper_samples_per_sixteenth())
        };

        looper_set_note(sixteenth, channel, attrs);
    }
}

void composer_set_notes(
    Channel channel,
    uint16_t start_beat, uint16_t start_sixteenth, uint16_t length_sixteenths,
    uint8_t volume, Envelope envelope, bool staccato, bool doubles,
    int count, ... // Variable number of frequency (uint16_t)
){
    va_list args;
    va_start(args, count);

    for(int i = 0; i < count; i++) {
        uint16_t frequency = va_arg(args, int);

        uint16_t total_sixteenth = (start_beat * 4) + start_sixteenth + (i * length_sixteenths);
        uint16_t beat = total_sixteenth / 4;
        uint16_t sixteenth = total_sixteenth % 4;

        composer_set_note(
            channel,
            beat, sixteenth, length_sixteenths,
            volume, envelope, staccato, doubles,
            frequency
        );
    }

    va_end(args);
}

void composer_set_slide(
    Channel channel,
    uint16_t start_beat, uint16_t start_sixteenth, uint16_t length_sixteenths,
    uint8_t volume, Envelope envelope, bool staccato, bool doubles,
    uint16_t frequency_start, uint16_t frequency_end
){
    uint32_t length_samples = (uint32_t)looper_samples_per_sixteenth() * (uint32_t)length_sixteenths;

    for(int i = 0; i < length_sixteenths; i++) {
        uint16_t sixteenth = (start_beat * 4) + start_sixteenth + i;
        uint32_t sample_in_note = (uint32_t)looper_samples_per_sixteenth() * i;
        
        NoteAttributes attrs = {
            .flags = 1 + (staccato && i == length_sixteenths - 1 ? 2 : 0) + (doubles ? 4 : 0),
            .frequency_start = linear_interpolate_16(frequency_start, frequency_end, sample_in_note, length_samples),
            .frequency_end = linear_interpolate_16(frequency_start, frequency_end, sample_in_note + looper_samples_per_sixteenth(), length_samples),
            .volume_start = apply_envelope(envelope, volume, sample_in_note),
            .volume_end = apply_envelope(envelope, volume, sample_in_note + looper_samples_per_sixteenth())
        };

        looper_set_note(sixteenth, channel, attrs);
    }
}

void composer_set_slides(
    Channel channel,
    uint16_t start_beat, uint16_t start_sixteenth, uint16_t length_sixteenths,
    uint8_t volume, Envelope envelope, bool staccato, bool doubles,
    int count, ... // Variable number of frequency pairs (uint16_t frequency_start, uint16_t frequency_end)
){
    va_list args;
    va_start(args, count * 2);

    for(int i = 0; i < count; i++) {
        uint16_t frequency_start = va_arg(args, int);
        uint16_t frequency_end = va_arg(args, int);

        uint16_t total_sixteenth = (start_beat * 4) + start_sixteenth + (i * length_sixteenths);
        uint16_t beat = total_sixteenth / 4;
        uint16_t sixteenth = total_sixteenth % 4;

        composer_set_slide(
            channel,
            beat, sixteenth, length_sixteenths,
            volume, envelope, staccato, doubles,
            frequency_start, frequency_end
        );
    }

    va_end(args);
}

void composer_set_rest(
    Channel channel,
    uint16_t start_beat, uint16_t start_sixteenth, uint16_t length_sixteenths
){
    for(int i = 0; i < length_sixteenths; i++) {
        uint16_t sixteenth = (start_beat * 4) + start_sixteenth + i;
        
        NoteAttributes attrs = {
            .flags = 0
        };

        looper_set_note(sixteenth, channel, attrs);
    }
}

void composer_set_rests(
    Channel channel,
    uint16_t start_beat, uint16_t start_sixteenth, uint16_t length_sixteenths,
    uint16_t interval_sixteenths, int count
){
    uint16_t sixteenth = (start_beat) * 4 + start_sixteenth;

    for(int i = 0; i < count; i++) {
        composer_set_rest(channel, sixteenth / 4, sixteenth % 4, length_sixteenths);
        sixteenth += interval_sixteenths;
    }
}

void composer_set_glissando(
    Channel channel,
    uint16_t start_beat, uint16_t start_sixteenth, uint16_t length_sixteenths,
    uint8_t volume, Envelope envelope, bool staccato, bool doubles,
    int start_note_index, int note_index_step
){
    for(int i = 0; i < length_sixteenths; i++) {
        uint16_t sixteenth = (start_beat * 4) + start_sixteenth + i;
        uint16_t start_freqency, end_freqency;
        if(doubles) {
            start_freqency = composer_get_frequency(start_note_index + (note_index_step * i * 2));
            end_freqency = composer_get_frequency(start_note_index + (note_index_step * ((i * 2) + 1)));
        } else {
            start_freqency = composer_get_frequency(start_note_index + (note_index_step * i));
            end_freqency = start_freqency;
        }
        
        NoteAttributes attrs = {
            .flags = 1 + (staccato && i == length_sixteenths - 1 ? 2 : 0) + (doubles ? 4 : 0),
            .frequency_start = start_freqency,
            .frequency_end = end_freqency,
            .volume_start = apply_envelope(envelope, volume, 0),
            .volume_end = apply_envelope(envelope, volume, looper_samples_per_sixteenth())
        };

        looper_set_note(sixteenth, channel, attrs);
    }
}

void composer_apply_dynamics(
    Channel channel,
    uint16_t start_beat, uint16_t start_sixteenth, uint16_t length_sixteenths,
    uint8_t start_volume_factor, uint8_t end_volume_factor
){
    NoteAttributes attrs_array[length_sixteenths];
    int count = looper_read_notes((start_beat * 4) + start_sixteenth, length_sixteenths, channel, attrs_array);

    for(int i = 0; i < count; i++) {
        attrs_array[i].volume_start = ((uint16_t)attrs_array[i].volume_start * linear_interpolate_16_short(start_volume_factor, end_volume_factor, i, length_sixteenths)) / 255;
        attrs_array[i].volume_end = ((uint16_t)attrs_array[i].volume_end * linear_interpolate_16_short(start_volume_factor, end_volume_factor, i + 1, length_sixteenths)) / 255;
    }

    looper_set_notes(
        (start_beat * 4) + start_sixteenth,
        count,
        channel,
        attrs_array
    );
}

void composer_copy_section(
    Channel src_channel, uint16_t src_start_beat, uint16_t src_start_sixteenth,
    Channel dest_channel, uint16_t dest_start_beat, uint16_t dest_start_sixteenth,
    uint16_t length_sixteenths
){
    NoteAttributes attrs_array[length_sixteenths];
    int count = looper_read_notes((src_start_beat * 4) + src_start_sixteenth, length_sixteenths, src_channel, attrs_array);
    looper_set_notes(
        (dest_start_beat * 4) + dest_start_sixteenth,
        count,
        dest_channel,
        attrs_array
    );
}

// Note: will round notes that are not exactly on a semitone to the next semitone
void composer_shift_semitones(
    Channel channel,
    uint16_t start_beat, uint16_t start_sixteenth, uint16_t length_sixteenths,
    int semitone_shift
){
    NoteAttributes attrs_array[length_sixteenths];
    int count = looper_read_notes((start_beat * 4) + start_sixteenth, length_sixteenths, channel, attrs_array);

    for(int i = 0; i < length_sixteenths; i++) {
        attrs_array[i].frequency_start = composer_get_frequency(composer_get_note_index(attrs_array[i].frequency_start) + semitone_shift);
        attrs_array[i].frequency_end = composer_get_frequency(composer_get_note_index(attrs_array[i].frequency_end) + semitone_shift);
    }

    looper_set_notes(
        (start_beat * 4) + start_sixteenth,
        count,
        channel,
        attrs_array
    );
}

void composer_shift_octaves(
    Channel channel,
    uint16_t start_beat, uint16_t start_sixteenth, uint16_t length_sixteenths,
    int octave_shift
){
    if(octave_shift == 0) return;

    NoteAttributes attrs_array[length_sixteenths];
    int count = looper_read_notes((start_beat * 4) + start_sixteenth, length_sixteenths, channel, attrs_array);

    for(int i = 0; i < count; i++) {
        if(octave_shift >= 0) {
            attrs_array[i].frequency_start *= (1 << octave_shift);
            attrs_array[i].frequency_end *= (1 << octave_shift);
        } else {
            attrs_array[i].frequency_start /= (1 << -octave_shift);
            attrs_array[i].frequency_end /= (1 << -octave_shift);
        }
    }

    looper_set_notes(
        (start_beat * 4) + start_sixteenth,
        count,
        channel,
        attrs_array
    );
}