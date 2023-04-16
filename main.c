#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <stdbool.h>
#include <stdio.h>
#include "ksalgo.h"

#define AUDIO_BUFFER_SIZE 1024
#define NUM_NOTES 12
#define SAMPLE_RATE 44100

SDL_AudioDeviceID audio_device;
float audio_buffer[AUDIO_BUFFER_SIZE];
KsalgoNote notes[NUM_NOTES];

void handle_event(SDL_Event event, bool* running, bool* key_down, float* frequency, int* note_index);
void audio_callback(void* userdata, Uint8* stream, int len);
void draw_waveform(SDL_Renderer* renderer, float* audio_buffer, int buffer_size, int screen_width, int screen_height);

int main()
{
        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_AudioSpec want;
        SDL_AudioSpec have;
        SDL_Event event;
        bool running;
        bool key_down;
        float frequency;
        int note_index;

        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
                printf("Error initializing SDL: %s\n", SDL_GetError());
                return 1;
        }

        window = SDL_CreateWindow("Karplus-Strong Synth",
                                  SDL_WINDOWPOS_UNDEFINED,
                                  SDL_WINDOWPOS_UNDEFINED,
                                  640, 480,
                                  SDL_WINDOW_SHOWN);
        if (!window) {
                printf("Error creating SDL window: %s\n", SDL_GetError());
                SDL_Quit();
                return 1;
        }

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (!renderer) {
                printf("Error creating SDL renderer: %s\n", SDL_GetError());
                SDL_DestroyWindow(window);
                SDL_Quit();
                return 1;
        }

        for (note_index = 0; note_index < NUM_NOTES; note_index++) {
                ksalgo_init_note(&notes[note_index]);
        }

        SDL_zero(want);
        want.freq = SAMPLE_RATE;
        want.format = AUDIO_S16SYS;
        want.channels = 1;
        want.samples = AUDIO_BUFFER_SIZE;
        want.callback = audio_callback;

        audio_device = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
        if (!audio_device) {
                printf("Error opening audio device: %s\n", SDL_GetError());
                SDL_DestroyWindow(window);
                SDL_Quit();
                return 1;
        }

        SDL_PauseAudioDevice(audio_device, 0);

        running = true;
        while (running) {
                while (SDL_PollEvent(&event)) {
                        handle_event(event, &running, &key_down, &frequency, &note_index);

                        if (note_index >= 0 && key_down)
                                ksalgo_start_note(&notes[note_index], frequency, 0.995f, 2, SAMPLE_RATE);

                        else if (note_index >= 0)
                                ksalgo_free_note(&notes[note_index]);
                }
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
                SDL_RenderClear(renderer);
                draw_waveform(renderer, audio_buffer, AUDIO_BUFFER_SIZE, 640, 480);
                SDL_RenderPresent(renderer);

                SDL_Delay(10);
        }

        SDL_CloseAudioDevice(audio_device);
        SDL_DestroyWindow(window);
        SDL_Quit();

        return 0;
}


void handle_event(SDL_Event event, bool* running, bool* key_down, float* frequency, int* note_index)
{
        if (event.type == SDL_QUIT) {
                *running = false;
                return;
        }
        else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
                *key_down = (event.type == SDL_KEYDOWN);
                *frequency = 0.0f;
                *note_index = -1;

                switch (event.key.keysym.sym) {
                        case SDLK_q: *frequency = 246.94f; *note_index = 0;  break; /* B3 */
                        case SDLK_w: *frequency = 261.63f; *note_index = 1;  break; /* C4 */
                        case SDLK_e: *frequency = 293.66f; *note_index = 2;  break; /* D4 */
                        case SDLK_a: *frequency = 329.63f; *note_index = 3;  break; /* E4 */
                        case SDLK_s: *frequency = 349.23f; *note_index = 4;  break; /* F4 */
                        case SDLK_d: *frequency = 392.00f; *note_index = 5;  break; /* G4 */
                        case SDLK_f: *frequency = 440.00f; *note_index = 6;  break; /* A4 */
                        case SDLK_g: *frequency = 493.88f; *note_index = 7;  break; /* B4 */
                        case SDLK_h: *frequency = 523.25f; *note_index = 8;  break; /* C5 */
                        case SDLK_j: *frequency = 587.33f; *note_index = 9;  break; /* D5 */
                        case SDLK_k: *frequency = 659.25f; *note_index = 10; break; /* E5 */
                        case SDLK_l: *frequency = 698.46f; *note_index = 11; break; /* F5 */
                }
        }
}


void audio_callback(void* userdata, Uint8* stream, int len)
{
        int samples;
        int buffer_index;
        Sint16* buffer;

        (void) userdata;

        samples = len / sizeof(Sint16);
        ksalgo_mix_audio(notes, NUM_NOTES, audio_buffer, samples);

        buffer = (Sint16*) stream;
        for (buffer_index = 0; buffer_index < samples; buffer_index++) {
                buffer[buffer_index] = (Sint16) (audio_buffer[buffer_index] * 32767.0f);
        }
}


void draw_waveform(SDL_Renderer* renderer, float* audio_buffer, int buffer_size, int screen_width, int screen_height)
{
        int half_height = screen_height / 2;
        int buffer_index;
        int x1, y1, x2, y2;

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

        for (buffer_index = 1; buffer_index < buffer_size; buffer_index++) {
                x1 = (buffer_index - 1) * screen_width / buffer_size;
                y1 = half_height - (int) (audio_buffer[buffer_index - 1] * half_height);
                x2 = buffer_index * screen_width / buffer_size;
                y2 = half_height - (int) (audio_buffer[buffer_index] * half_height);

                SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
        }
}
