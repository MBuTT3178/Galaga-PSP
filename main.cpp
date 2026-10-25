/*
 * Galaga (Arcade 1981) — Sony PSP Port
 * Native C++11 / PSPSDK Hardware Direct Port
 * 1-Player Vertical Portrait Arcade Action
 * Optimized for Sony PSP Hardware & PPSSPP
 */

#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspctrl.h>
#include <psppower.h>
#include <pspaudiolib.h>
#include <pspiofilemgr.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <vector>

#include "sprites.hpp"
#include "audio.hpp"
#include "enemy_ai.hpp"
#include "bezel.hpp"

PSP_MODULE_INFO("GalagaPSP", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);
PSP_HEAP_SIZE_KB(4096);

/* ─── PSP HARDWARE DISPLAY & DOUBLE BUFFERING ───────────────────────────── */
uint32_t *fb_draw = (uint32_t *)VRAM_BASE1;
static void *fb_disp = (void *)VRAM_BASE0;

static void init_display(void) {
    sceDisplaySetMode(0, SCR_W, SCR_H);
    fb_draw = (uint32_t *)VRAM_BASE1;
    fb_disp = (void *)VRAM_BASE0;
    sceDisplaySetFrameBuf(fb_disp, BUF_W, PSP_DISPLAY_PIXEL_FORMAT_8888, PSP_DISPLAY_SETBUF_IMMEDIATE);
}

static void flip_buffers(void) {
    sceDisplaySetFrameBuf((void *)fb_draw, BUF_W, PSP_DISPLAY_PIXEL_FORMAT_8888, PSP_DISPLAY_SETBUF_NEXTFRAME);
    sceDisplayWaitVblankStart();
    fb_disp = (void *)fb_draw;
    fb_draw = (fb_draw == (uint32_t *)VRAM_BASE0) ? (uint32_t *)VRAM_BASE1 : (uint32_t *)VRAM_BASE0;
}

/* ─── PSP RUNTIME EXIT CALLBACK HANDLERS ────────────────────────────────── */
static int g_running = 1;

static int exit_cb(int arg1, int arg2, void *common) {
    (void)arg1; (void)arg2; (void)common;
    g_running = 0;
    sceKernelExitGame();
    return 0;
}

static int cb_thread(SceSize args, void *argp) {
    (void)args; (void)argp;
    int cbid = sceKernelCreateCallback("exit_cb", exit_cb, NULL);
    sceKernelRegisterExitCallback(cbid);
    sceKernelSleepThreadCB();
    return 0;
}

static void setup_callbacks(void) {
    int thid = sceKernelCreateThread("cb_thread", cb_thread, 0x11, 0xFA0, 0, 0);
    if (thid >= 0) sceKernelStartThread(thid, 0, 0);
}

/* ─── GAME CONSTANTS & STRUCTURES ───────────────────────────────────────── */
#define MAX_MISSILES 4
#define MAX_ENEMY_BULLETS 8
#define MAX_EXPLOSIONS 8

struct Missile {
    bool active;
    float x, y;
    float vy;
};

struct EnemyBullet {
    bool active;
    float x, y;
    float vx, vy;
};

struct Explosion {
    bool active;
    float x, y;
    float timer;
    int frame;
    bool is_player;
};

enum GameState {
    GSTATE_TITLE,
    GSTATE_STAGE_START,
    GSTATE_SWARM_IN,
    GSTATE_PLAYING,
    GSTATE_CHALLENGING_STAGE,
    GSTATE_CHALLENGE_RESULTS,
    GSTATE_PLAYER_DYING,
    GSTATE_SHIP_CAPTURING,
    GSTATE_GAME_OVER
};

/* ─── INPUT SYSTEM ──────────────────────────────────────────────────────── */
struct InputState {
    bool left, right;
    bool fire;
    bool fire_pressed;
    bool start_pressed;
};

static SceCtrlData s_pad_curr;
static SceCtrlData s_pad_prev;

static void read_inputs(InputState &in) {
    s_pad_prev = s_pad_curr;
    sceCtrlPeekBufferPositive(&s_pad_curr, 1);

    uint32_t pressed = s_pad_curr.Buttons & ~s_pad_prev.Buttons;

    // Movement: D-Pad or Analog Nub
    bool d_left = (s_pad_curr.Buttons & PSP_CTRL_LEFT) != 0;
    bool d_right = (s_pad_curr.Buttons & PSP_CTRL_RIGHT) != 0;
    bool a_left = (s_pad_curr.Lx < 80);
    bool a_right = (s_pad_curr.Lx > 175);

    in.left = d_left || a_left;
    in.right = d_right || a_right;

    // Buttons
    in.fire = (s_pad_curr.Buttons & PSP_CTRL_CROSS) != 0;
    in.fire_pressed = (pressed & PSP_CTRL_CROSS) != 0;
    in.start_pressed = (pressed & (PSP_CTRL_START | PSP_CTRL_CROSS)) != 0;
}

/* ─── HIGH SCORE PERSISTENCE ────────────────────────────────────────────── */
static int load_high_score() {
    FILE *f = fopen("ms0:/PSP/SAVEDATA/GALAGA0000/hiscore.dat", "rb");
    if (!f) f = fopen("hiscore.dat", "rb");
    if (!f) return 20000;
    int hs = 20000;
    fread(&hs, sizeof(int), 1, f);
    fclose(f);
    return (hs >= 20000) ? hs : 20000;
}

static void save_high_score(int hs) {
    sceIoMkdir("ms0:/PSP", 0777);
    sceIoMkdir("ms0:/PSP/SAVEDATA", 0777);
    sceIoMkdir("ms0:/PSP/SAVEDATA/GALAGA0000", 0777);
    FILE *f = fopen("ms0:/PSP/SAVEDATA/GALAGA0000/hiscore.dat", "wb");
    if (!f) f = fopen("hiscore.dat", "wb");
    if (f) {
        fwrite(&hs, sizeof(int), 1, f);
        fclose(f);
    }
}

/* ─── MAIN APP CLASS ────────────────────────────────────────────────────── */
class GalagaApp {
public:
    Starfield starfield;
    SoundSystem sound;
    ArcadeBezel bezel;
    FormationGrid grid;

    GameState state;
    float state_timer;

    // Gameplay Statistics
    int score;
    int high_score;
    int stage;
    int lives;
    int shots_fired;
    int hits_landed;

    // Player Ship
    float player_x, player_y;
    bool is_dual;
    float ship_w;
    float capture_angle;
    float capture_target_y;
    int capturing_boss_id;
    int active_captured_ship_id;
    float invulnerable_timer;
    float fire_cooldown;

    // Challenging Stage Tracking
    int challenge_wave;
    float challenge_wave_timer;
    int challenge_hits;
    int challenge_total_spawned;

    // Swarm Entry Progression
    int swarm_step;
    float swarm_step_timer;

    // Attack Timers
    float dive_attack_timer;

    // Projectiles & FX
    Missile missiles[MAX_MISSILES];
    EnemyBullet bullets[MAX_ENEMY_BULLETS];
    Explosion explosions[MAX_EXPLOSIONS];

    void init() {
        starfield.init();
        sound.init();
        g_paths.init();
        grid.init();

        state = GSTATE_TITLE;
        state_timer = 0.0f;
        score = 0;
        high_score = load_high_score();
        stage = 1;
        lives = 3;
        shots_fired = 0;
        hits_landed = 0;
        active_captured_ship_id = -1;

        reset_player();
        clear_projectiles();
    }

    void reset_player() {
        player_x = PLAY_W * 0.5f - 8.0f;
        player_y = 248.0f;
        is_dual = false;
        ship_w = 16.0f;
        capture_angle = 0.0f;
        capturing_boss_id = -1;
        invulnerable_timer = 2.0f;
        fire_cooldown = 0.0f;
    }

    void clear_projectiles() {
        for (int i = 0; i < MAX_MISSILES; i++) missiles[i].active = false;
        for (int i = 0; i < MAX_ENEMY_BULLETS; i++) bullets[i].active = false;
        for (int i = 0; i < MAX_EXPLOSIONS; i++) explosions[i].active = false;
    }

    void start_game() {
        score = 0;
        stage = 1;
        lives = 3;
        shots_fired = 0;
        hits_landed = 0;
        active_captured_ship_id = -1;
        reset_player();
        start_stage(1);
    }

    void start_stage(int stg) {
        stage = stg;
        clear_projectiles();
        sound.stop_tractor();
        grid.init();

        state = GSTATE_STAGE_START;
        state_timer = 0.0f;
        starfield.warp_speed = true;
        sound.play(SND_STAGE_INTRO);

        bool is_challenge = (stage >= 3 && (stage - 3) % 4 == 0);
        if (is_challenge) {
            challenge_wave = 0;
            challenge_wave_timer = 0.0f;
            challenge_hits = 0;
            challenge_total_spawned = 0;
        } else {
            swarm_step = 0;
            swarm_step_timer = 0.0f;
        }
    }

    void spawn_explosion(float x, float y, bool is_player = false) {
        for (int i = 0; i < MAX_EXPLOSIONS; i++) {
            if (!explosions[i].active) {
                explosions[i].active = true;
                explosions[i].x = x;
                explosions[i].y = y;
                explosions[i].timer = 0.0f;
                explosions[i].frame = 0;
                explosions[i].is_player = is_player;
                break;
            }
        }
    }

    void fire_missile() {
        if (is_dual) {
            int free1 = -1, free2 = -1;
            for (int i = 0; i < MAX_MISSILES; i++) {
                if (!missiles[i].active) {
                    if (free1 < 0) free1 = i;
                    else if (free2 < 0) { free2 = i; break; }
                }
            }
            if (free1 >= 0 && free2 >= 0) {
                missiles[free1].active = true;
                missiles[free1].x = player_x + 4.0f;
                missiles[free1].y = player_y;
                missiles[free1].vy = -340.0f;

                missiles[free2].active = true;
                missiles[free2].x = player_x + 20.0f;
                missiles[free2].y = player_y;
                missiles[free2].vy = -340.0f;

                shots_fired += 2;
                sound.play(SND_FIRE);
            }
        } else {
            int active_count = 0;
            int free_idx = -1;
            for (int i = 0; i < MAX_MISSILES; i++) {
                if (missiles[i].active) active_count++;
                else if (free_idx < 0) free_idx = i;
            }
            if (active_count < 2 && free_idx >= 0) {
                missiles[free_idx].active = true;
                missiles[free_idx].x = player_x + 7.0f;
                missiles[free_idx].y = player_y;
                missiles[free_idx].vy = -340.0f;

                shots_fired++;
                sound.play(SND_FIRE);
            }
        }
    }

    void spawn_enemy_bullet(float x, float y, float target_x) {
        for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
            if (!bullets[i].active) {
                bullets[i].active = true;
                bullets[i].x = x;
                bullets[i].y = y;
                float dx = target_x - x;
                float dist = sqrtf(dx * dx + 120.0f * 120.0f);
                bullets[i].vx = (dx / dist) * 75.0f;
                bullets[i].vy = 125.0f;
                break;
            }
        }
    }

    void spawn_swarm_wave(int wave_idx) {
        if (wave_idx == 0) {
            for (int i = 0; i < 4; i++) {
                int eid = i;
                Enemy &e = grid.enemies[eid];
                e.active = true;
                e.type = TYPE_BOSS;
                e.hp = 2;
                e.state = ESTATE_ENTERING;
                e.form_row = 0;
                e.form_col = 3 + i;
                e.current_path = g_paths.enter_top_left;
                e.path_time = -i * 0.16f;
            }
            for (int i = 0; i < 4; i++) {
                int eid = 4 + i;
                Enemy &e = grid.enemies[eid];
                e.active = true;
                e.type = TYPE_GOEI;
                e.hp = 1;
                e.state = ESTATE_ENTERING;
                e.form_row = 1;
                e.form_col = 3 + i;
                e.current_path = g_paths.enter_top_left;
                e.path_time = -(4 + i) * 0.16f;
            }
        }
        else if (wave_idx == 1) {
            for (int i = 0; i < 8; i++) {
                int eid = 8 + i;
                Enemy &e = grid.enemies[eid];
                e.active = true;
                e.type = TYPE_GOEI;
                e.hp = 1;
                e.state = ESTATE_ENTERING;
                e.form_row = (i < 4) ? 1 : 2;
                e.form_col = (i < 4) ? ((i < 2) ? 1 + i : 5 + i) : (1 + (i - 4));
                e.current_path = g_paths.enter_top_right;
                e.path_time = -i * 0.16f;
            }
        }
        else if (wave_idx == 2) {
            for (int i = 0; i < 8; i++) {
                int eid = 16 + i;
                Enemy &e = grid.enemies[eid];
                e.active = true;
                e.type = TYPE_ZAKO;
                e.hp = 1;
                e.state = ESTATE_ENTERING;
                e.form_row = 3;
                e.form_col = 1 + i;
                e.current_path = g_paths.enter_bot_left;
                e.path_time = -i * 0.16f;
            }
        }
        else if (wave_idx == 3) {
            for (int i = 0; i < 8; i++) {
                int eid = 24 + i;
                Enemy &e = grid.enemies[eid];
                e.active = true;
                e.type = TYPE_ZAKO;
                e.hp = 1;
                e.state = ESTATE_ENTERING;
                e.form_row = 4;
                e.form_col = 1 + i;
                e.current_path = g_paths.enter_bot_right;
                e.path_time = -i * 0.16f;
            }
        }
        else if (wave_idx == 4) {
            int extras[6][2] = {{3,0}, {3,9}, {4,0}, {4,9}, {2,7}, {2,8}};
            for (int i = 0; i < 6; i++) {
                int eid = 32 + i;
                Enemy &e = grid.enemies[eid];
                e.active = true;
                e.type = (extras[i][0] == 2) ? TYPE_GOEI : TYPE_ZAKO;
                e.hp = 1;
                e.state = ESTATE_ENTERING;
                e.form_row = extras[i][0];
                e.form_col = extras[i][1];
                e.current_path = (i % 2 == 0) ? g_paths.enter_top_left : g_paths.enter_top_right;
                e.path_time = -i * 0.16f;
            }
        }
    }

    void spawn_challenge_wave(int wave_idx) {
        FlightPath p;
        EnemyType t = TYPE_ZAKO;
        if (wave_idx == 0) { p = g_paths.challenge_wave1; t = TYPE_ZAKO; }
        else if (wave_idx == 1) { p = g_paths.challenge_wave2; t = TYPE_GOEI; }
        else if (wave_idx == 2) { p = g_paths.challenge_wave3; t = TYPE_TRANSFORM; }
        else if (wave_idx == 3) { p = g_paths.challenge_wave4; t = TYPE_GOEI; }
        else { p = g_paths.challenge_wave5; t = TYPE_TRANSFORM; }

        for (int i = 0; i < 8; i++) {
            int eid = wave_idx * 8 + i;
            if (eid >= MAX_ENEMIES) break;
            Enemy &e = grid.enemies[eid];
            e.active = true;
            e.type = t;
            e.hp = 1;
            e.state = ESTATE_CHALLENGE;
            e.current_path = p;
            e.path_time = -i * 0.15f;
            challenge_total_spawned++;
        }
    }

    /* ─── UPDATE LOOP ───────────────────────────────────────────────────── */
    void update(float dt, const InputState &in) {
        starfield.update(dt);

        switch (state) {
            case GSTATE_TITLE:
                if (in.start_pressed) {
                    start_game();
                }
                break;

            case GSTATE_STAGE_START:
                state_timer += dt;
                if (state_timer >= 2.2f) {
                    starfield.warp_speed = false;
                    bool is_challenge = (stage >= 3 && (stage - 3) % 4 == 0);
                    if (is_challenge) {
                        state = GSTATE_CHALLENGING_STAGE;
                        challenge_wave = 0;
                        challenge_wave_timer = 0.0f;
                        spawn_challenge_wave(0);
                    } else {
                        state = GSTATE_SWARM_IN;
                        swarm_step = 0;
                        swarm_step_timer = 0.0f;
                        spawn_swarm_wave(0);
                    }
                }
                break;

            case GSTATE_SWARM_IN:
                grid.update(dt, player_x + (is_dual ? 16.0f : 8.0f));
                if (!grid.tractor_beam_active) sound.stop_tractor();
                if (invulnerable_timer > 0.0f) invulnerable_timer -= dt;

                swarm_step_timer += dt;
                if (swarm_step < 4 && swarm_step_timer >= 1.6f) {
                    swarm_step++;
                    swarm_step_timer = 0.0f;
                    spawn_swarm_wave(swarm_step);
                } else if (swarm_step >= 4 && swarm_step_timer >= 2.4f) {
                    state = GSTATE_PLAYING;
                    dive_attack_timer = 1.0f;
                }
                update_player(dt, in);
                update_projectiles(dt);
                check_missile_hits();
                break;

            case GSTATE_PLAYING:
                grid.update(dt, player_x + (is_dual ? 16.0f : 8.0f));
                if (!grid.tractor_beam_active) sound.stop_tractor();
                if (invulnerable_timer > 0.0f) invulnerable_timer -= dt;

                update_player(dt, in);
                update_projectiles(dt);
                check_missile_hits();
                check_player_collisions();

                dive_attack_timer -= dt;
                if (dive_attack_timer <= 0.0f) {
                    dive_attack_timer = 2.0f + (rand() % 20) * 0.1f;
                    trigger_random_dive();
                }

                if (grid.alive_count == 0) {
                    start_stage(stage + 1);
                }
                break;

            case GSTATE_CHALLENGING_STAGE:
                grid.update(dt, player_x + (is_dual ? 16.0f : 8.0f));
                if (invulnerable_timer > 0.0f) invulnerable_timer -= dt;

                update_player(dt, in);
                update_projectiles(dt);
                check_missile_hits();

                challenge_wave_timer += dt;
                if (challenge_wave_timer >= 2.4f && challenge_wave < 4) {
                    challenge_wave++;
                    challenge_wave_timer = 0.0f;
                    spawn_challenge_wave(challenge_wave);
                }

                if (challenge_wave >= 4 && challenge_wave_timer >= 3.8f && grid.alive_count == 0) {
                    state = GSTATE_CHALLENGE_RESULTS;
                    state_timer = 0.0f;
                    if (challenge_hits >= 40) {
                        sound.play(SND_CHALLENGE_PERFECT);
                        score += 10000;
                    } else {
                        score += challenge_hits * 100;
                    }
                    if (score > high_score) {
                        high_score = score;
                        save_high_score(high_score);
                    }
                }
                break;

            case GSTATE_CHALLENGE_RESULTS:
                state_timer += dt;
                if (state_timer >= 3.2f) {
                    start_stage(stage + 1);
                }
                break;

            case GSTATE_SHIP_CAPTURING:
                grid.update(dt);
                capture_angle += 720.0f * dt;
                player_y -= 45.0f * dt;

                if (capturing_boss_id >= 0) {
                    Enemy &boss = grid.enemies[capturing_boss_id];
                    player_x += (boss.x - player_x) * (6.0f * dt);

                    if (player_y <= boss.y + 16.0f) {
                        sound.stop_tractor();
                        sound.play(SND_CAPTURED);
                        boss.has_captured_ship = true;
                        spawn_captured_ship_enemy(capturing_boss_id);

                        lives--;
                        if (lives > 0) {
                            reset_player();
                            state = GSTATE_PLAYING;
                        } else {
                            state = GSTATE_GAME_OVER;
                            state_timer = 0.0f;
                        }
                    }
                }
                break;

            case GSTATE_PLAYER_DYING:
                grid.update(dt);
                state_timer += dt;
                if (state_timer >= 1.6f) {
                    if (lives > 0) {
                        reset_player();
                        state = GSTATE_PLAYING;
                    } else {
                        state = GSTATE_GAME_OVER;
                        state_timer = 0.0f;
                        if (score > high_score) {
                            high_score = score;
                            save_high_score(high_score);
                        }
                    }
                }
                break;

            case GSTATE_GAME_OVER:
                state_timer += dt;
                if (state_timer >= 4.0f) {
                    state = GSTATE_TITLE;
                }
                break;
        }

        for (int i = 0; i < MAX_EXPLOSIONS; i++) {
            if (explosions[i].active) {
                explosions[i].timer += dt;
                explosions[i].frame = (int)(explosions[i].timer / 0.06f);
                if (explosions[i].frame >= 4) {
                    explosions[i].active = false;
                }
            }
        }
    }

    void update_player(float dt, const InputState &in) {
        float speed = 140.0f;
        if (in.left) player_x -= speed * dt;
        if (in.right) player_x += speed * dt;

        float max_x = PLAY_W - (is_dual ? 34.0f : 18.0f);
        if (player_x < 2.0f) player_x = 2.0f;
        if (player_x > max_x) player_x = max_x;

        if (fire_cooldown > 0.0f) fire_cooldown -= dt;
        if (in.fire_pressed || (in.fire && fire_cooldown <= 0.0f)) {
            fire_missile();
            fire_cooldown = 0.16f;
        }
    }

    void update_projectiles(float dt) {
        for (int i = 0; i < MAX_MISSILES; i++) {
            if (missiles[i].active) {
                missiles[i].y += missiles[i].vy * dt;
                if (missiles[i].y < -10.0f) {
                    missiles[i].active = false;
                }
            }
        }

        for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
            if (bullets[i].active) {
                bullets[i].x += bullets[i].vx * dt;
                bullets[i].y += bullets[i].vy * dt;
                if (bullets[i].y > PLAY_H + 10.0f || bullets[i].x < -10.0f || bullets[i].x > PLAY_W + 10.0f) {
                    bullets[i].active = false;
                }
            }
        }
    }

    void trigger_random_dive() {
        std::vector<int> candidates;
        for (int i = 0; i < MAX_ENEMIES; i++) {
            if (grid.enemies[i].active && grid.enemies[i].state == ESTATE_IN_FORMATION) {
                candidates.push_back(i);
            }
        }
        if (candidates.empty()) return;

        int pick = candidates[rand() % candidates.size()];
        Enemy &e = grid.enemies[pick];

        bool want_tractor = false;
        if (e.type == TYPE_BOSS && !is_dual && active_captured_ship_id < 0 && state == GSTATE_PLAYING) {
            if (rand() % 100 < 35) {
                want_tractor = true;
            }
        }

        e.start_dive(player_x + 8.0f, want_tractor);
        sound.play(SND_SWOOP);

        if (want_tractor) {
            sound.play(SND_TRACTOR_LOOP);
        } else {
            spawn_enemy_bullet(e.x + 8.0f, e.y + 12.0f, player_x + 8.0f);
        }
    }

    void spawn_captured_ship_enemy(int boss_id) {
        for (int i = 0; i < MAX_ENEMIES; i++) {
            if (!grid.enemies[i].active) {
                Enemy &cap = grid.enemies[i];
                cap.active = true;
                cap.type = TYPE_CAPTURED_SHIP;
                cap.state = ESTATE_IN_FORMATION;
                cap.form_row = 0;
                cap.form_col = grid.enemies[boss_id].form_col;
                cap.hp = 1;
                active_captured_ship_id = i;
                grid.enemies[boss_id].captured_ship_enemy_id = i;
                break;
            }
        }
    }

    void check_missile_hits() {
        for (int m = 0; m < MAX_MISSILES; m++) {
            if (!missiles[m].active) continue;

            float mx = missiles[m].x;
            float my = missiles[m].y;

            for (int e = 0; e < MAX_ENEMIES; e++) {
                if (!grid.enemies[e].active || grid.enemies[e].path_time < 0.0f) continue;

                Enemy &en = grid.enemies[e];
                float ex = en.x;
                float ey = en.y;

                if (mx >= ex && mx <= ex + 16.0f && my >= ey && my <= ey + 16.0f) {
                    missiles[m].active = false;
                    hits_landed++;
                    en.hp--;

                    if (en.hp <= 0) {
                        en.active = false;
                        spawn_explosion(ex, ey);
                        sound.play(SND_EXPLODE_ENEMY);

                        int pts = 100;
                        if (en.type == TYPE_ZAKO) pts = (en.state == ESTATE_IN_FORMATION) ? 50 : 100;
                        else if (en.type == TYPE_GOEI) pts = (en.state == ESTATE_IN_FORMATION) ? 80 : 160;
                        else if (en.type == TYPE_BOSS) pts = (en.state == ESTATE_IN_FORMATION) ? 150 : 400;
                        else if (en.type == TYPE_CAPTURED_SHIP) pts = 1000;
                        else if (en.type == TYPE_TRANSFORM) pts = 1000;

                        score += pts;
                        if (score > high_score) {
                            high_score = score;
                            save_high_score(high_score);
                        }

                        if (state == GSTATE_CHALLENGING_STAGE) {
                            challenge_hits++;
                        }

                        if (en.type == TYPE_BOSS && en.has_captured_ship) {
                            sound.stop_tractor();
                            int cap_id = en.captured_ship_enemy_id;
                            if (cap_id >= 0 && cap_id < MAX_ENEMIES && grid.enemies[cap_id].active) {
                                Enemy &cap = grid.enemies[cap_id];
                                if (en.state == ESTATE_DIVING || en.state == ESTATE_RETURNING) {
                                    cap.state = ESTATE_RESCUED;
                                    sound.play(SND_RESCUE_DUAL);
                                } else {
                                    cap.type = TYPE_GOEI;
                                    cap.start_dive(player_x + 8.0f, false);
                                }
                            }
                        }

                        if (en.id == active_captured_ship_id) {
                            active_captured_ship_id = -1;
                        }
                    } else {
                        sound.play(SND_BOSS_HIT);
                    }
                    break;
                }
            }
        }
    }

    void check_player_collisions() {
        if (state != GSTATE_PLAYING && state != GSTATE_SWARM_IN) return;

        float pw = is_dual ? 32.0f : 16.0f;
        float ph = 16.0f;

        for (int i = 0; i < MAX_ENEMIES; i++) {
            if (!grid.enemies[i].active || grid.enemies[i].path_time < 0.0f) continue;
            Enemy &e = grid.enemies[i];

            // 1. Tractor Beam capture check
            if (e.state == ESTATE_TRACTOR_BEAM && e.tractor_length > 20.0f && !is_dual && active_captured_ship_id < 0) {
                float beam_top_y = e.y + 16.0f;
                float beam_bot_y = beam_top_y + e.tractor_length;
                float beam_center_x = e.x + 8.0f;
                float beam_half_w = 4.0f + (e.tractor_length / 150.0f) * 38.0f;

                if (player_y >= beam_top_y && player_y <= beam_bot_y &&
                    player_x + 8.0f >= beam_center_x - beam_half_w &&
                    player_x + 8.0f <= beam_center_x + beam_half_w) {
                    state = GSTATE_SHIP_CAPTURING;
                    capturing_boss_id = i;
                    capture_angle = 0.0f;
                    return;
                }
            }

            // 2. Rescued ship docking
            if (e.state == ESTATE_RESCUED) {
                if (e.y >= player_y - 8.0f) {
                    e.active = false;
                    active_captured_ship_id = -1;
                    is_dual = true;
                    ship_w = 32.0f;
                    player_x = e.x;
                    sound.play(SND_RESCUE_DUAL);
                }
            }

            // 3. Enemy body collision
            if (invulnerable_timer <= 0.0f && e.state != ESTATE_RESCUED) {
                if (player_x < e.x + 14.0f && player_x + pw > e.x + 2.0f &&
                    player_y < e.y + 14.0f && player_y + ph > e.y + 2.0f) {
                    player_hit();
                    e.active = false;
                    spawn_explosion(e.x, e.y);
                    return;
                }
            }
        }

        // 4. Enemy bullets
        if (invulnerable_timer <= 0.0f) {
            for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
                if (!bullets[i].active) continue;
                if (bullets[i].x >= player_x + 2.0f && bullets[i].x <= player_x + pw - 2.0f &&
                    bullets[i].y >= player_y && bullets[i].y <= player_y + ph) {
                    bullets[i].active = false;
                    player_hit();
                    return;
                }
            }
        }
    }

    void player_hit() {
        if (is_dual) {
            is_dual = false;
            ship_w = 16.0f;
            invulnerable_timer = 1.8f;
            spawn_explosion(player_x + 8.0f, player_y, true);
            sound.play(SND_EXPLODE_PLAYER);
        } else {
            spawn_explosion(player_x, player_y, true);
            sound.play(SND_EXPLODE_PLAYER);
            lives--;
            state = GSTATE_PLAYER_DYING;
            state_timer = 0.0f;
        }
    }

    /* ─── RENDER LOOP ───────────────────────────────────────────────────── */
    void render() {
        // 1. Starfield
        starfield.render();

        // 2. Active Tractor Beams
        for (int i = 0; i < MAX_ENEMIES; i++) {
            if (grid.enemies[i].active && grid.enemies[i].state == ESTATE_TRACTOR_BEAM) {
                render_tractor_beam(grid.enemies[i].x + 8.0f, grid.enemies[i].y + 16.0f,
                                    grid.enemies[i].tractor_length, (int)(grid.breath_timer * 15.0f));
            }
        }

        // 3. Enemies
        for (int i = 0; i < MAX_ENEMIES; i++) {
            if (!grid.enemies[i].active || grid.enemies[i].path_time < 0.0f) continue;
            Enemy &e = grid.enemies[i];
            int sx = PLAY_X + (int)e.x;
            int sy = PLAY_Y + (int)e.y;

            if (e.type == TYPE_ZAKO) {
                draw_sprite_16x16(sx, sy, (e.anim_frame == 0) ? SPR_ZAKO_0 : SPR_ZAKO_1, e.angle);
            } else if (e.type == TYPE_GOEI) {
                draw_sprite_16x16(sx, sy, (e.anim_frame == 0) ? SPR_GOEI_0 : SPR_GOEI_1, e.angle);
            } else if (e.type == TYPE_BOSS) {
                draw_sprite_16x16(sx, sy, (e.hp == 2) ? SPR_BOSS_GREEN : SPR_BOSS_BLUE, e.angle);
            } else if (e.type == TYPE_CAPTURED_SHIP) {
                draw_sprite_16x16(sx, sy, SPR_CAPTURED_FIGHTER, e.angle);
            } else if (e.type == TYPE_TRANSFORM) {
                draw_sprite_16x16(sx, sy, SPR_TRANSFORM_ALIEN, e.angle);
            }
        }

        // 4. Player Missiles
        for (int i = 0; i < MAX_MISSILES; i++) {
            if (missiles[i].active) {
                draw_fill_rect(PLAY_X + (int)missiles[i].x, PLAY_Y + (int)missiles[i].y, 2, 6, RGB(255, 240, 64));
                draw_pixel(PLAY_X + (int)missiles[i].x, PLAY_Y + (int)missiles[i].y + 6, RGB(240, 32, 32));
            }
        }

        // 5. Enemy Bullets
        for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
            if (bullets[i].active) {
                draw_fill_rect(PLAY_X + (int)bullets[i].x - 1, PLAY_Y + (int)bullets[i].y - 2, 3, 5, RGB(240, 40, 40));
                draw_pixel(PLAY_X + (int)bullets[i].x, PLAY_Y + (int)bullets[i].y, RGB(255, 220, 60));
            }
        }

        // 6. Player Ship
        if (state != GSTATE_PLAYER_DYING && state != GSTATE_GAME_OVER) {
            bool draw_ship = (invulnerable_timer <= 0.0f) || (((int)(invulnerable_timer * 14.0f)) % 2 == 0);
            if (draw_ship) {
                int sx = PLAY_X + (int)player_x;
                int sy = PLAY_Y + (int)player_y;

                if (state == GSTATE_SHIP_CAPTURING) {
                    draw_sprite_16x16(sx, sy, SPR_FIGHTER, capture_angle);
                } else if (is_dual) {
                    draw_dual_fighter(sx, sy);
                } else {
                    draw_sprite_16x16(sx, sy, SPR_FIGHTER, 0.0f);
                }
            }
        }

        // 7. Explosions
        for (int i = 0; i < MAX_EXPLOSIONS; i++) {
            if (explosions[i].active && explosions[i].frame < 4) {
                int sx = PLAY_X + (int)explosions[i].x;
                int sy = PLAY_Y + (int)explosions[i].y;
                draw_explosion(sx, sy, explosions[i].frame, explosions[i].is_player);
            }
        }

        // 8. Banners
        if (state == GSTATE_TITLE) {
            draw_text(PLAY_X + 28, 70, "GALAGA", C_RED, 2);
            draw_text(PLAY_X + 26, 68, "GALAGA", C_YELLOW, 2);

            draw_text(PLAY_X + 44, 110, "1 PLAYER ONLY", C_CYAN, 1);
            if (((int)(sceKernelGetSystemTimeLow() / 450000)) % 2 == 0) {
                draw_text(PLAY_X + 32, 140, "PUSH START BUTTON", C_GREEN, 1);
            }
            draw_text(PLAY_X + 44, 165, "OR PRESS [X]", C_YELLOW, 1);
            draw_text(PLAY_X + 40, 190, "[X]   : FIRE", C_WHITE, 1);
            draw_text(PLAY_X + 20, 220, "(C) 1981 NAMCO LTD.", C_BLUE, 1);
        }
        else if (state == GSTATE_STAGE_START) {
            char buf[32];
            bool is_challenge = (stage >= 3 && (stage - 3) % 4 == 0);
            if (is_challenge) {
                draw_text(PLAY_X + 24, 120, "CHALLENGING STAGE", C_CYAN, 1);
            } else {
                snprintf(buf, sizeof(buf), "STAGE %d", stage);
                draw_text(PLAY_X + 68, 120, buf, C_CYAN, 1);
            }
            draw_text(PLAY_X + 74, 140, "START!", C_RED, 1);
        }
        else if (state == GSTATE_CHALLENGE_RESULTS) {
            draw_text(PLAY_X + 26, 90, "CHALLENGING STAGE", C_CYAN, 1);
            char buf[32];
            snprintf(buf, sizeof(buf), "NUMBER OF HITS %2d", challenge_hits);
            draw_text(PLAY_X + 28, 120, buf, C_YELLOW, 1);

            if (challenge_hits >= 40) {
                draw_text(PLAY_X + 48, 145, "PERFECT!", C_GREEN, 1);
                draw_text(PLAY_X + 24, 160, "SPECIAL 10000 PTS", C_CYAN, 1);
            } else {
                snprintf(buf, sizeof(buf), "BONUS %5d PTS", challenge_hits * 100);
                draw_text(PLAY_X + 38, 145, buf, C_WHITE, 1);
            }
        }
        else if (state == GSTATE_GAME_OVER) {
            draw_text(PLAY_X + 56, 120, "GAME OVER", C_RED, 1);
        }

        // 9. Side Cabinet Bezels (Left & Right)
        bezel.render_left();
        bezel.render_right(score, high_score, stage, lives, shots_fired, hits_landed);
    }
};

/* ─── PROGRAM ENTRY POINT ───────────────────────────────────────────────── */
int main(int argc, char *argv[]) {
    (void)argc; (void)argv;
    setup_callbacks();

    scePowerSetClockFrequency(333, 333, 166);
    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

    init_display();

    GalagaApp app;
    app.init();

    InputState in;
    memset(&in, 0, sizeof(in));
    memset(&s_pad_curr, 0, sizeof(s_pad_curr));
    memset(&s_pad_prev, 0, sizeof(s_pad_prev));

    uint32_t last_time_us = sceKernelGetSystemTimeLow();

    while (g_running) {
        uint32_t now_us = sceKernelGetSystemTimeLow();
        float dt = (float)(now_us - last_time_us) * 0.000001f;
        if (dt < 0.0f || dt > 0.05f) dt = 0.016667f;
        last_time_us = now_us;

        read_inputs(in);
        app.update(dt, in);

        clear_screen();
        app.render();

        flip_buffers();
    }

    app.sound.shutdown();
    sceKernelExitGame();
    return 0;
}
