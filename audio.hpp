#ifndef AUDIO_HPP
#define AUDIO_HPP

#include <SDL.h>
#include <cmath>
#include <cstdint>
#include <cstdlib>

/* ─── AUDIO ENGINE CONSTANTS ─────────────────────────────────────────── */
#define AUDIO_SAMPLE_RATE 22050
#define MAX_AUDIO_VOICES 6

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

struct AudioVoice {
    int active;
    int type; // 0: square wave, 1: noise, 2: modulated sine/triangle
    float freq;
    float target_freq;
    float freq_slide;
    float phase;
    int samples_left;
    int total_samples;
    float volume;
    float mod_phase;
    float mod_speed;
    float mod_depth;
};

static AudioVoice g_voices[MAX_AUDIO_VOICES];

/* Audio callback running on audio thread */
static void galaga_audio_callback(void *userdata, Uint8 *stream, int len) {
    (void)userdata;
    Sint16 *buf = (Sint16*)stream;
    int samples = len / 2;

    for (int i = 0; i < samples; i++) {
        Sint32 mixed = 0;

        for (int v = 0; v < MAX_AUDIO_VOICES; v++) {
            if (!g_voices[v].active) continue;

            AudioVoice &vc = g_voices[v];
            Sint16 sample_val = 0;

            // Envelope decay
            float env = 1.0f;
            if (vc.total_samples > 0) {
                env = (float)vc.samples_left / (float)vc.total_samples;
            }

            if (vc.type == 0) { // Square wave with pitch slide
                float cur_freq = vc.freq;
                if (vc.mod_depth > 0.0f) {
                    cur_freq += sinf(vc.mod_phase) * vc.mod_depth;
                    vc.mod_phase += vc.mod_speed;
                }
                if (cur_freq < 20.0f) cur_freq = 20.0f;

                float period = (float)AUDIO_SAMPLE_RATE / cur_freq;
                if (period < 2.0f) period = 2.0f;

                int p = (int)vc.phase % (int)period;
                sample_val = (p < (int)(period * 0.5f)) ? 4000 : -4000;
                sample_val = (Sint16)(sample_val * env * vc.volume);

                vc.phase += 1.0f;
                vc.freq += vc.freq_slide;
            } else if (vc.type == 1) { // White noise (Explosions)
                sample_val = (Sint16)(((rand() % 8000) - 4000) * env * vc.volume);
            } else if (vc.type == 2) { // Modulated FM (Tractor beam warble)
                float cur_freq = vc.freq + sinf(vc.mod_phase) * vc.mod_depth;
                vc.mod_phase += vc.mod_speed;
                if (cur_freq < 20.0f) cur_freq = 20.0f;

                float dt = cur_freq / (float)AUDIO_SAMPLE_RATE;
                vc.phase += dt;
                if (vc.phase >= 1.0f) vc.phase -= 1.0f;

                // Triangle / soft square
                float tri = (vc.phase < 0.5f) ? (4.0f * vc.phase - 1.0f) : (3.0f - 4.0f * vc.phase);
                sample_val = (Sint16)(tri * 5000.0f * env * vc.volume);
            }

            mixed += sample_val;

            vc.samples_left--;
            if (vc.samples_left <= 0) {
                vc.active = 0;
            }
        }

        // Master clamp
        if (mixed > 32767) mixed = 32767;
        if (mixed < -32768) mixed = -32768;
        buf[i] = (Sint16)mixed;
    }
}

class SoundSystem {
public:
    int tractor_voice_idx;

    void init() {
        tractor_voice_idx = -1;
        for (int i = 0; i < MAX_AUDIO_VOICES; i++) {
            g_voices[i].active = 0;
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

    int alloc_voice() {
        for (int i = 0; i < MAX_AUDIO_VOICES; i++) {
            if (!g_voices[i].active && i != tractor_voice_idx) {
                return i;
            }
        }
        // Steal non-tractor voice with least samples left
        int best = 0;
        int min_left = 999999;
        for (int i = 0; i < MAX_AUDIO_VOICES; i++) {
            if (i == tractor_voice_idx) continue;
            if (g_voices[i].samples_left < min_left) {
                min_left = g_voices[i].samples_left;
                best = i;
            }
        }
        return best;
    }

    void play(SoundId id) {
        SDL_LockAudio();
        int v = alloc_voice();
        AudioVoice &vc = g_voices[v];

        switch (id) {
            case SND_FIRE:
                vc.active = 1;
                vc.type = 0;
                vc.freq = 920.0f;
                vc.freq_slide = -3.8f;
                vc.phase = 0.0f;
                vc.samples_left = 3200; // ~0.14s
                vc.total_samples = 3200;
                vc.volume = 0.85f;
                vc.mod_depth = 0.0f;
                break;

            case SND_EXPLODE_ENEMY:
                vc.active = 1;
                vc.type = 1; // Noise
                vc.samples_left = 5000; // ~0.22s
                vc.total_samples = 5000;
                vc.volume = 0.95f;
                break;

            case SND_EXPLODE_PLAYER:
                vc.active = 1;
                vc.type = 1; // Heavy noise
                vc.samples_left = 18000; // ~0.8s
                vc.total_samples = 18000;
                vc.volume = 1.0f;
                break;

            case SND_BOSS_HIT:
                vc.active = 1;
                vc.type = 0;
                vc.freq = 320.0f;
                vc.freq_slide = -0.5f;
                vc.phase = 0.0f;
                vc.samples_left = 4000;
                vc.total_samples = 4000;
                vc.volume = 0.9f;
                vc.mod_depth = 80.0f;
                vc.mod_speed = 0.2f;
                vc.mod_phase = 0.0f;
                break;

            case SND_TRACTOR_LOOP:
                tractor_voice_idx = v;
                vc.active = 1;
                vc.type = 2; // FM mod
                vc.freq = 280.0f;
                vc.freq_slide = 0.0f;
                vc.phase = 0.0f;
                vc.samples_left = 14000; // ~0.65s per trigger loop
                vc.total_samples = 14000;
                vc.volume = 0.8f;
                vc.mod_depth = 120.0f;
                vc.mod_speed = 0.035f;
                vc.mod_phase = 0.0f;
                break;

            case SND_CAPTURED:
                vc.active = 1;
                vc.type = 0;
                vc.freq = 550.0f;
                vc.freq_slide = -0.8f;
                vc.phase = 0.0f;
                vc.samples_left = 16000; // ~0.72s
                vc.total_samples = 16000;
                vc.volume = 0.85f;
                vc.mod_depth = 40.0f;
                vc.mod_speed = 0.08f;
                vc.mod_phase = 0.0f;
                break;

            case SND_RESCUE_DUAL:
                vc.active = 1;
                vc.type = 0;
                vc.freq = 523.25f; // C5
                vc.freq_slide = 0.6f;
                vc.phase = 0.0f;
                vc.samples_left = 14000;
                vc.total_samples = 14000;
                vc.volume = 0.95f;
                vc.mod_depth = 0.0f;
                break;

            case SND_SWOOP:
                vc.active = 1;
                vc.type = 0;
                vc.freq = 680.0f;
                vc.freq_slide = -1.2f;
                vc.phase = 0.0f;
                vc.samples_left = 6500;
                vc.total_samples = 6500;
                vc.volume = 0.7f;
                vc.mod_depth = 25.0f;
                vc.mod_speed = 0.05f;
                vc.mod_phase = 0.0f;
                break;

            case SND_STAGE_INTRO:
                vc.active = 1;
                vc.type = 0;
                vc.freq = 440.0f;
                vc.freq_slide = 0.4f;
                vc.phase = 0.0f;
                vc.samples_left = 12000;
                vc.total_samples = 12000;
                vc.volume = 0.9f;
                vc.mod_depth = 0.0f;
                break;

            case SND_CHALLENGE_PERFECT:
                vc.active = 1;
                vc.type = 0;
                vc.freq = 784.0f; // G5
                vc.freq_slide = 0.5f;
                vc.phase = 0.0f;
                vc.samples_left = 16000;
                vc.total_samples = 16000;
                vc.volume = 1.0f;
                vc.mod_depth = 0.0f;
                break;

            case SND_COIN:
                vc.active = 1;
                vc.type = 0;
                vc.freq = 987.77f; // B5
                vc.freq_slide = 1.5f;
                vc.phase = 0.0f;
                vc.samples_left = 4000;
                vc.total_samples = 4000;
                vc.volume = 0.9f;
                vc.mod_depth = 0.0f;
                break;

            default:
                break;
        }

        SDL_UnlockAudio();
    }

    void stop_tractor() {
        SDL_LockAudio();
        if (tractor_voice_idx >= 0 && tractor_voice_idx < MAX_AUDIO_VOICES) {
            g_voices[tractor_voice_idx].active = 0;
            tractor_voice_idx = -1;
        }
        SDL_UnlockAudio();
    }
};

#endif // AUDIO_HPP
