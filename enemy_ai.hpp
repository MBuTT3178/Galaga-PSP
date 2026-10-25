#ifndef ENEMY_AI_HPP
#define ENEMY_AI_HPP

#include "sprites.hpp"
#include <vector>
#include <cmath>

/* ─── ENEMY TYPES & STATES ───────────────────────────────────────────── */
enum EnemyType {
    TYPE_ZAKO = 0,       // Blue Bee (100 pts diving, 50 pts in formation)
    TYPE_GOEI,           // Red Butterfly (160 pts diving, 80 pts in formation)
    TYPE_BOSS,           // Boss Galaga (take 2 hits; 400 pts diving + escorts, 150 pts in formation)
    TYPE_CAPTURED_SHIP,  // Captured player fighter
    TYPE_TRANSFORM       // Challenging stage bonus alien (1000-3000 pts)
};

enum EnemyState {
    ESTATE_INACTIVE = 0,
    ESTATE_ENTERING,      // Flying on entrance spline
    ESTATE_IN_FORMATION,  // Parked in grid formation (breathing)
    ESTATE_DIVING,        // Dive bombing run
    ESTATE_TRACTOR_BEAM,  // Boss Galaga emitting tractor beam
    ESTATE_RETURNING,     // Flying back up into grid slot
    ESTATE_RESCUED,       // Captured ship falling down to dock with player
    ESTATE_CHALLENGE      // Flying on challenging stage choreography
};

/* ─── 2D VECTOR & BEZIER MATH ────────────────────────────────────────── */
struct Vec2 {
    float x, y;
    Vec2() : x(0), y(0) {}
    Vec2(float _x, float _y) : x(_x), y(_y) {}
};

inline Vec2 get_bezier_point(float t, const Vec2 &p0, const Vec2 &p1, const Vec2 &p2, const Vec2 &p3) {
    float u = 1.0f - t;
    float tt = t * t;
    float uu = u * u;
    float uuu = uu * u;
    float ttt = tt * t;

    Vec2 p;
    p.x = uuu * p0.x + 3.0f * uu * t * p1.x + 3.0f * u * tt * p2.x + ttt * p3.x;
    p.y = uuu * p0.y + 3.0f * uu * t * p1.y + 3.0f * u * tt * p2.y + ttt * p3.y;
    return p;
}

/* ─── BEZIER SEGMENT PATH DEFINITION ─────────────────────────────────── */
struct BezierSegment {
    Vec2 p0, p1, p2, p3;
    float duration; // In seconds
};

struct FlightPath {
    std::vector<BezierSegment> segs;
    float total_duration;

    void add(const Vec2 &p0, const Vec2 &p1, const Vec2 &p2, const Vec2 &p3, float dur) {
        BezierSegment s;
        s.p0 = p0; s.p1 = p1; s.p2 = p2; s.p3 = p3;
        s.duration = dur;
        segs.push_back(s);
        recalc();
    }

    void recalc() {
        total_duration = 0.0f;
        for (size_t i = 0; i < segs.size(); i++) {
            total_duration += segs[i].duration;
        }
    }

    Vec2 evaluate(float time, float *out_angle = nullptr) const {
        if (segs.empty()) return Vec2(0, 0);
        if (time >= total_duration) {
            const BezierSegment &last = segs.back();
            if (out_angle) *out_angle = 0.0f;
            return last.p3;
        }

        float acc = 0.0f;
        for (size_t i = 0; i < segs.size(); i++) {
            if (time <= acc + segs[i].duration) {
                float local_t = (time - acc) / segs[i].duration;
                if (local_t < 0.0f) local_t = 0.0f;
                if (local_t > 1.0f) local_t = 1.0f;

                Vec2 pt = get_bezier_point(local_t, segs[i].p0, segs[i].p1, segs[i].p2, segs[i].p3);
                if (out_angle) {
                    // Approximate tangent angle
                    float delta = 0.02f;
                    float t_next = (local_t + delta <= 1.0f) ? local_t + delta : 1.0f;
                    Vec2 pt2 = get_bezier_point(t_next, segs[i].p0, segs[i].p1, segs[i].p2, segs[i].p3);
                    *out_angle = atan2f(pt2.y - pt.y, pt2.x - pt.x) * (180.0f / 3.14159265f) - 90.0f;
                }
                return pt;
            }
            acc += segs[i].duration;
        }
        return segs.back().p3;
    }
};

/* ─── GLOBAL PRECOMPUTED FLIGHT PATH PRESETS ─────────────────────────── */
struct FlightPathPresets {
    FlightPath enter_top_left;
    FlightPath enter_top_right;
    FlightPath enter_bot_left;
    FlightPath enter_bot_right;
    FlightPath challenge_wave1;
    FlightPath challenge_wave2;
    FlightPath challenge_wave3;
    FlightPath challenge_wave4;
    FlightPath challenge_wave5;

    void init() {
        // Path 0: Enter Top-Left
        enter_top_left.segs.clear();
        enter_top_left.add(Vec2(-16, 40), Vec2(70, 40), Vec2(160, 110), Vec2(150, 170), 1.2f);
        enter_top_left.add(Vec2(150, 170), Vec2(140, 220), Vec2(70, 210), Vec2(60, 150), 1.1f);
        enter_top_left.add(Vec2(60, 150), Vec2(50, 100), Vec2(90, 70), Vec2(102, 60), 0.9f);

        // Path 1: Enter Top-Right
        enter_top_right.segs.clear();
        enter_top_right.add(Vec2(PLAY_W + 16, 40), Vec2(PLAY_W - 70, 40), Vec2(44, 110), Vec2(54, 170), 1.2f);
        enter_top_right.add(Vec2(54, 170), Vec2(64, 220), Vec2(134, 210), Vec2(144, 150), 1.1f);
        enter_top_right.add(Vec2(144, 150), Vec2(154, 100), Vec2(114, 70), Vec2(102, 60), 0.9f);

        // Path 2: Enter Bottom-Left
        enter_bot_left.segs.clear();
        enter_bot_left.add(Vec2(-16, 230), Vec2(80, 220), Vec2(160, 180), Vec2(150, 110), 1.3f);
        enter_bot_left.add(Vec2(150, 110), Vec2(140, 50), Vec2(70, 40), Vec2(60, 90), 1.1f);
        enter_bot_left.add(Vec2(60, 90), Vec2(50, 130), Vec2(90, 110), Vec2(102, 80), 0.8f);

        // Path 3: Enter Bottom-Right
        enter_bot_right.segs.clear();
        enter_bot_right.add(Vec2(PLAY_W + 16, 230), Vec2(PLAY_W - 80, 220), Vec2(44, 180), Vec2(54, 110), 1.3f);
        enter_bot_right.add(Vec2(54, 110), Vec2(64, 50), Vec2(134, 40), Vec2(144, 90), 1.1f);
        enter_bot_right.add(Vec2(144, 90), Vec2(154, 130), Vec2(114, 110), Vec2(102, 80), 0.8f);

        // Challenging Stage Wave 1: S-Curve across
        challenge_wave1.segs.clear();
        challenge_wave1.add(Vec2(-16, 50), Vec2(60, 50), Vec2(100, 120), Vec2(140, 130), 1.2f);
        challenge_wave1.add(Vec2(140, 130), Vec2(190, 140), Vec2(170, 230), Vec2(100, 220), 1.2f);
        challenge_wave1.add(Vec2(100, 220), Vec2(30, 210), Vec2(30, 120), Vec2(-20, 110), 1.1f);

        // Challenging Stage Wave 2: Mirrored S-Curve
        challenge_wave2.segs.clear();
        challenge_wave2.add(Vec2(PLAY_W + 16, 50), Vec2(PLAY_W - 60, 50), Vec2(104, 120), Vec2(64, 130), 1.2f);
        challenge_wave2.add(Vec2(64, 130), Vec2(14, 140), Vec2(34, 230), Vec2(104, 220), 1.2f);
        challenge_wave2.add(Vec2(104, 220), Vec2(174, 210), Vec2(174, 120), Vec2(PLAY_W + 20, 110), 1.1f);

        // Challenging Stage Wave 3: Figure-Eight Loop
        challenge_wave3.segs.clear();
        challenge_wave3.add(Vec2(-16, 200), Vec2(70, 210), Vec2(150, 160), Vec2(140, 100), 1.2f);
        challenge_wave3.add(Vec2(140, 100), Vec2(130, 40), Vec2(70, 50), Vec2(60, 110), 1.1f);
        challenge_wave3.add(Vec2(60, 110), Vec2(50, 170), Vec2(130, 230), Vec2(PLAY_W + 20, 240), 1.2f);

        // Challenging Stage Wave 4: Upper Loop Descent
        challenge_wave4.segs.clear();
        challenge_wave4.add(Vec2(PLAY_W + 16, 80), Vec2(120, 80), Vec2(50, 140), Vec2(60, 200), 1.2f);
        challenge_wave4.add(Vec2(60, 200), Vec2(70, 250), Vec2(140, 240), Vec2(150, 180), 1.1f);
        challenge_wave4.add(Vec2(150, 180), Vec2(160, 120), Vec2(100, 40), Vec2(-20, 30), 1.2f);

        // Challenging Stage Wave 5: Centered Spiral
        challenge_wave5.segs.clear();
        challenge_wave5.add(Vec2(102, -16), Vec2(160, 50), Vec2(170, 150), Vec2(102, 180), 1.2f);
        challenge_wave5.add(Vec2(102, 180), Vec2(30, 150), Vec2(40, 50), Vec2(102, 60), 1.1f);
        challenge_wave5.add(Vec2(102, 60), Vec2(180, 70), Vec2(180, 240), Vec2(102, 280), 1.2f);
    }
};

static FlightPathPresets g_paths;

/* ─── ENEMY INDIVIDUAL INSTANCE ──────────────────────────────────────── */
#define MAX_ENEMIES 56

struct Enemy {
    int id;
    bool active;
    EnemyType type;
    EnemyState state;

    float x, y;
    float angle;
    int hp;            // Boss Galaga has 2 HP, others 1
    int anim_frame;
    float anim_timer;

    // Formation positioning
    int form_row;      // 0..4
    int form_col;      // 0..9
    Vec2 form_target;

    // Path tracking
    FlightPath current_path;
    float path_time;

    // Dive / Attack behaviors
    float dive_timer;
    bool is_tractor_boss;
    float tractor_length;
    float tractor_timer;
    bool has_captured_ship;
    int captured_ship_enemy_id;

    // Escort tracking (for Boss Galagas with Goei escorts)
    int escort1_id;
    int escort2_id;

    void reset() {
        active = false;
        type = TYPE_ZAKO;
        state = ESTATE_INACTIVE;
        x = 0; y = 0;
        angle = 0;
        hp = 1;
        anim_frame = 0;
        anim_timer = 0;
        form_row = -1;
        form_col = -1;
        form_target = Vec2(0, 0);
        path_time = 0;
        dive_timer = 0;
        is_tractor_boss = false;
        tractor_length = 0;
        tractor_timer = 0;
        has_captured_ship = false;
        captured_ship_enemy_id = -1;
        escort1_id = -1;
        escort2_id = -1;
    }

    void start_dive(float target_player_x, bool want_tractor = false) {
        state = want_tractor ? ESTATE_TRACTOR_BEAM : ESTATE_DIVING;
        is_tractor_boss = want_tractor;
        path_time = 0.0f;
        tractor_length = 0.0f;
        tractor_timer = 0.0f;

        current_path.segs.clear();
        Vec2 start_pt(x, y);

        if (want_tractor) {
            // Boss descends to mid-bottom, stops to fire tractor beam
            Vec2 mid1(x + (target_player_x - x) * 0.4f, y + 50.0f);
            Vec2 mid2(target_player_x + 10.0f, 130.0f);
            Vec2 stop_pt(target_player_x, 170.0f);
            current_path.add(start_pt, mid1, mid2, stop_pt, 1.4f);
        } else {
            // Standard dive-bomb curve towards player coordinates
            float side_offset = (x < target_player_x) ? 40.0f : -40.0f;
            Vec2 cp1(x + side_offset, y + 40.0f);
            Vec2 cp2(target_player_x + side_offset, 180.0f);
            Vec2 cp3(target_player_x, 260.0f);
            current_path.add(start_pt, cp1, cp2, cp3, 1.6f);

            // Loop back from bottom to top
            Vec2 loop_bot(target_player_x - side_offset, 275.0f);
            Vec2 loop_up(target_player_x - side_offset * 1.5f, 140.0f);
            Vec2 top_exit(x, -20.0f);
            current_path.add(cp3, loop_bot, loop_up, top_exit, 1.4f);
        }
    }

    void resume_after_tractor() {
        state = ESTATE_DIVING;
        path_time = 0.0f;
        current_path.segs.clear();
        Vec2 start_pt(x, y);
        Vec2 cp1(x, y + 40.0f);
        Vec2 cp2(x + 50.0f, 260.0f);
        Vec2 cp3(102.0f, 275.0f);
        current_path.add(start_pt, cp1, cp2, cp3, 1.1f);

        Vec2 loop_up(40.0f, 160.0f);
        Vec2 top_exit(102.0f, -20.0f);
        current_path.add(cp3, loop_up, Vec2(60, 40), top_exit, 1.2f);
    }
};

/* ─── FORMATION GRID CONTROLLER ──────────────────────────────────────── */
class FormationGrid {
public:
    Enemy enemies[MAX_ENEMIES];
    float breath_timer;
    float grid_base_x;
    float grid_base_y;
    float col_spread;
    int alive_count;
    bool tractor_beam_active;

    void init() {
        breath_timer = 0.0f;
        grid_base_x = PLAY_W * 0.5f;
        grid_base_y = 38.0f;
        col_spread = 15.0f;
        alive_count = 0;
        tractor_beam_active = false;
        for (int i = 0; i < MAX_ENEMIES; i++) {
            enemies[i].reset();
            enemies[i].id = i;
        }
    }

    Vec2 get_slot_pos(int row, int col) const {
        float x = grid_base_x + ((float)col - 4.5f) * col_spread;
        float y = grid_base_y + (float)row * 13.5f;
        return Vec2(x, y);
    }

    void update(float dt, float target_player_x = 102.0f) {
        breath_timer += dt;
        // Oscillating grid breathing
        grid_base_x = (PLAY_W * 0.5f) + sinf(breath_timer * 1.6f) * 11.0f;
        col_spread = 15.0f + sinf(breath_timer * 2.2f) * 1.2f;

        alive_count = 0;
        tractor_beam_active = false;

        for (int i = 0; i < MAX_ENEMIES; i++) {
            if (!enemies[i].active) continue;
            alive_count++;
            Enemy &e = enemies[i];

            // Animation tick
            e.anim_timer += dt;
            if (e.anim_timer >= 0.25f) {
                e.anim_timer = 0.0f;
                e.anim_frame = 1 - e.anim_frame;
            }

            // State updates
            if (e.state == ESTATE_IN_FORMATION) {
                // If this is a captured ship attached to a diving boss, lock beside the diving boss
                bool following_boss = false;
                if (e.type == TYPE_CAPTURED_SHIP) {
                    for (int b = 0; b < MAX_ENEMIES; b++) {
                        if (enemies[b].active && enemies[b].type == TYPE_BOSS && enemies[b].captured_ship_enemy_id == e.id) {
                            if (enemies[b].state == ESTATE_DIVING || enemies[b].state == ESTATE_RETURNING) {
                                e.x = enemies[b].x + 16.0f;
                                e.y = enemies[b].y;
                                e.angle = enemies[b].angle;
                                following_boss = true;
                            }
                            break;
                        }
                    }
                }

                if (!following_boss) {
                    Vec2 target = get_slot_pos(e.form_row, e.form_col);
                    e.x = target.x;
                    e.y = target.y;
                    e.angle = 0.0f; // Facing down in formation
                }
            }
            else if (e.state == ESTATE_ENTERING || e.state == ESTATE_CHALLENGE) {
                e.path_time += dt;
                float angle_deg = 0.0f;
                Vec2 pt = e.current_path.evaluate(e.path_time, &angle_deg);
                e.x = pt.x;
                e.y = pt.y;
                e.angle = angle_deg;

                if (e.path_time >= e.current_path.total_duration) {
                    if (e.state == ESTATE_ENTERING) {
                        e.state = ESTATE_IN_FORMATION;
                    } else {
                        // Challenging wave flew offscreen
                        e.active = false;
                    }
                }
            }
            else if (e.state == ESTATE_DIVING) {
                e.path_time += dt;
                float angle_deg = 0.0f;
                Vec2 pt = e.current_path.evaluate(e.path_time, &angle_deg);
                e.x = pt.x;
                e.y = pt.y;
                e.angle = angle_deg;

                if (e.path_time >= e.current_path.total_duration || e.y < -15.0f) {
                    // Re-enter from top of screen into formation
                    e.state = ESTATE_RETURNING;
                    e.y = -10.0f;
                    Vec2 target = get_slot_pos(e.form_row, e.form_col);
                    e.x = target.x;
                }
            }
            else if (e.state == ESTATE_RETURNING) {
                // Glide down from top into slot
                Vec2 target = get_slot_pos(e.form_row, e.form_col);
                float dy = 120.0f * dt;
                e.y += dy;
                e.x += (target.x - e.x) * (6.0f * dt);
                e.angle = 0.0f;

                if (e.y >= target.y) {
                    e.y = target.y;
                    e.x = target.x;
                    e.state = ESTATE_IN_FORMATION;
                }
            }
            else if (e.state == ESTATE_TRACTOR_BEAM) {
                if (e.path_time < e.current_path.total_duration) {
                    e.path_time += dt;
                    float angle_deg = 0.0f;
                    Vec2 pt = e.current_path.evaluate(e.path_time, &angle_deg);
                    e.x = pt.x;
                    e.y = pt.y;
                    e.angle = angle_deg;
                } else {
                    // Holding position and projecting tractor beam
                    e.angle = 0.0f;
                    tractor_beam_active = true;
                    e.tractor_timer += dt;
                    if (e.tractor_length < 96.0f) {
                        e.tractor_length += 80.0f * dt;
                    }

                    // Beam stays active for 3.8 seconds
                    if (e.tractor_timer >= 3.8f) {
                        e.resume_after_tractor();
                    }
                }
            }
            else if (e.state == ESTATE_RESCUED) {
                // Rescued ship descending to dock with player, smoothly aligning horizontally
                e.y += 90.0f * dt;
                e.x += (target_player_x - e.x) * (4.0f * dt);
                e.angle += 360.0f * dt;
            }
        }
    }
};

#endif // ENEMY_AI_HPP
