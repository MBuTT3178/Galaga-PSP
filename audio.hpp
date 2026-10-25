#ifndef AUDIO_HPP
#define AUDIO_HPP

#include <SDL.h>
#include <cstdlib>
#include <cstdint>

#define AUDIO_SAMPLE_RATE 22050
#define MAX_AUDIO_VOICES 4

struct SynthVoice {
    int active;
    float freq;
    int samples_left;
    int phase;
    int type; // 0 = tone, 1 = noise
};

static SynthVoice g_voices[MAX_AUDIO_VOICES];

static void galaga_audio_callback(void *userdata, Uint8 *stream, int len) {
    (void)userdata;
    Sint16 *buf = (Sint16*)stream;
    int samples = len / 2;

    for (int i = 0; i < samples; i++) {
        Sint32 mixed = 0;
        for (int v = 0; v < MAX_AUDIO_VOICES; v++) {
            if (!g_voices[v].active) continue;
            if (g_voices[v].type == 0) {
                int period = (int)(22050.0f / g_voices[v].freq);
                if (period <= 0) period = 1;
                Sint16 val = ((g_voices[v].phase % period) < (period / 2)) ? 4000 : -4000;
                mixed += val;
                g_voices[v].phase++;
            } else {
                mixed += (rand() % 8000) - 4000;
            }
            g_voices[v].samples_left--;
            if (g_voices[v].samples_left <= 0) {
                g_voices[v].active = 0;
            }
        }
        if (mixed > 32767) mixed = 32767;
        if (mixed < -32768) mixed = -32768;
        buf[i] = (Sint16)mixed;
    }
}

enum SoundId {
    SND_NONE = 0,
    SND_FIRE,
    SND_EXPLODE_ENEMY,
    SND_EXPLODE_PLAYER,
    SND_BOSS_HIT,
    SND_TRACTOR_LOOP,
    SND_CAPTURED,
    SND_RESCUE_DUAL,
    SND_SWOOP,
    SND_STAGE_INTRO,
    SND_CHALLENGE_PERFECT,
    SND_COIN
};

class SoundSystem {
public:
    void init() {
        for (int i = 0; i < MAX_AUDIO_VOICES; i++) {
            g_voices[i].active = 0;
            g_voices[i].phase = 0;
            g_voices[i].samples_left = 0;
        }

        SDL_AudioSpec want, have;
        SDL_zero(want);
        want.freq = AUDIO_SAMPLE_RATE;
        want.format = AUDIO_S16SYS;
        want.channels = 1;
        want.samples = 512;
        want.callback = galaga_audio_callback;

        if (SDL_OpenAudio(&want, &have) == 0) {
            SDL_PauseAudio(0);
        }
    }

    void shutdown() {
        SDL_CloseAudio();
    }

    void play(SoundId id) {
        float freq = 440.0f;
        int ms = 80;
        int type = 0;

        switch (id) {
            case SND_FIRE:
                freq = 880.0f; ms = 45; type = 0;
                break;
            case SND_EXPLODE_ENEMY:
                freq = 160.0f; ms = 90; type = 1;
                break;
            case SND_EXPLODE_PLAYER:
                freq = 110.0f; ms = 300; type = 1;
                break;
            case SND_BOSS_HIT:
                freq = 280.0f; ms = 60; type = 0;
                break;
            case SND_TRACTOR_LOOP:
                freq = 340.0f; ms = 120; type = 0;
                break;
            case SND_CAPTURED:
                freq = 220.0f; ms = 250; type = 0;
                break;
            case SND_RESCUE_DUAL:
                freq = 660.0f; ms = 160; type = 0;
                break;
            case SND_SWOOP:
                freq = 480.0f; ms = 70; type = 0;
                break;
            case SND_STAGE_INTRO:
                freq = 587.0f; ms = 120; type = 0;
                break;
            case SND_CHALLENGE_PERFECT:
                freq = 784.0f; ms = 220; type = 0;
                break;
            case SND_COIN:
                freq = 988.0f; ms = 60; type = 0;
                break;
            default:
                return;
        }

        // Lock-free voice allocation (find inactive or overwrite first)
        for (int v = 0; v < MAX_AUDIO_VOICES; v++) {
            if (!g_voices[v].active) {
                g_voices[v].freq = freq;
                g_voices[v].samples_left = (int)(22050.0f * ms / 1000.0f);
                g_voices[v].phase = 0;
                g_voices[v].type = type;
                g_voices[v].active = 1;
                return;
            }
        }
        g_voices[0].freq = freq;
        g_voices[0].samples_left = (int)(22050.0f * ms / 1000.0f);
        g_voices[0].phase = 0;
        g_voices[0].type = type;
        g_voices[0].active = 1;
    }

    void stop_tractor() {
        for (int v = 0; v < MAX_AUDIO_VOICES; v++) {
            if (g_voices[v].type == 0 && (int)g_voices[v].freq == 340) {
                g_voices[v].active = 0;
            }
        }
    }
};

#endif // AUDIO_HPP
