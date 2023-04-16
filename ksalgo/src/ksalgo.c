#include "ksalgo.h"
#include <stdlib.h>
#include <math.h>

float rand_float(float min, float max)
{
        float tmp, random, range;

        if (min > max) {
                tmp = min;
                min = max;
                max = tmp;
        }

        random = ((float) rand()) / (float) RAND_MAX;
        range = max - min;
        return (random * range) + min;
}


void ksalgo_init_note(KsalgoNote *note)
{
        note->active = false;
        note->buffer_pos = 0;
        note->buffer = NULL;
}


void ksalgo_start_note(KsalgoNote *note, float frequency, float decay, int buffer_multiplier, int sample_rate)
{
        int buff_index;

        if (note->active == true)
                return;

        note->buffer_size = (int)(sample_rate / frequency) * buffer_multiplier;
        note->buffer = realloc(note->buffer, note->buffer_size * sizeof(float));

        for (buff_index = 0; buff_index < note->buffer_size; buff_index++) {
                note->buffer[buff_index] = rand_float(-0.5f, 0.5f);
        }

        note->active = true;
        note->buffer_pos = 0;
        note->decay = decay;
}


void ksalgo_stop_note(KsalgoNote *note)
{
        note->active = false;
}


void ksalgo_free_note(KsalgoNote* note)
{
        if (note->buffer != NULL) {
                free(note->buffer);
                note->buffer = NULL;
        }
        ksalgo_stop_note(note);
}


float ksalgo_process_note(KsalgoNote *note)
{
        float output;
        float average;
        int next_pos;

        if (!note->active) {
                return 0.0f;
        }

        output = note->buffer[note->buffer_pos];
        next_pos = (note->buffer_pos + 1) % note->buffer_size;
        average = (note->buffer[note->buffer_pos] + note->buffer[next_pos]) * 0.5f;

        note->buffer[note->buffer_pos] =  average * note->decay;
        note->buffer_pos = next_pos;

        return output;
}


void ksalgo_mix_audio(KsalgoNote *notes, int notes_size, float *buffer, int buffer_size)
{
        int buff_index;
        int note_index;
        float max_sample = 0.0f;

        for (buff_index = 0; buff_index < buffer_size; buff_index++) {
                buffer[buff_index] = 0.0f;
                for (note_index = 0; note_index < notes_size; note_index++) {
                        buffer[buff_index] += ksalgo_process_note(&notes[note_index]);
                }
                if (fabsf(buffer[buff_index]) > max_sample) {
                        max_sample = fabsf(buffer[buff_index]);
                }
        }

        if (max_sample > 1.0f) {
                for (buff_index = 0; buff_index < buffer_size; buff_index++) {
                        buffer[buff_index] /= max_sample;
                }
        }
}

