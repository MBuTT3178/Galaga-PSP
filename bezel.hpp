#ifndef BEZEL_HPP
#define BEZEL_HPP

#include "sprites.hpp"
#include <cstdio>
#include <cstring>

class ArcadeBezel {
public:
    void render_left() {
        // Cabinet side panel background
        draw_fill_rect(0, 0, BEZEL_L_W, SCR_H, RGB(12, 14, 26));

        // Decorative cabinet side stripes
        for (int y = 0; y < SCR_H; y += 16) {
            draw_line(0, y, BEZEL_L_W - 4, y, RGB(24, 30, 56));
        }

        // Gold & Cyan cabinet bezel inner divider
        draw_line(BEZEL_L_W - 3, 0, BEZEL_L_W - 3, SCR_H, RGB(40, 80, 160));
        draw_line(BEZEL_L_W - 2, 0, BEZEL_L_W - 2, SCR_H, RGB(248, 200, 32));
        draw_line(BEZEL_L_W - 1, 0, BEZEL_L_W - 1, SCR_H, RGB(0, 220, 255));

        // Marquee Header
        draw_text(26, 12, "GALAGA", C_RED, 2);
        draw_text(24, 10, "GALAGA", C_YELLOW, 2);

        draw_text(18, 30, "MIDWAY / NAMCO", C_CYAN, 1);
        draw_text(38, 40, "- 1981 -", C_WHITE, 1);

        // Divider
        draw_line(10, 52, BEZEL_L_W - 14, 52, RGB(60, 90, 160));

        // Instruction Card 1: Dual Fighter
        draw_fill_rect(8, 58, BEZEL_L_W - 18, 54, RGB(20, 28, 50));
        draw_rect(8, 58, BEZEL_L_W - 18, 54, RGB(45, 65, 110));

        draw_text(14, 62, "DUAL FIGHTER", C_YELLOW, 1);
        draw_text(14, 73, "RESCUE CAPTURED", C_WHITE, 1);
        draw_text(14, 83, "SHIP FOR DOUBLE", C_WHITE, 1);
        draw_text(14, 93, "FIREPOWER!", C_CYAN, 1);

        // Instruction Card 2: Tractor Beam Warning
        draw_fill_rect(8, 118, BEZEL_L_W - 18, 52, RGB(35, 18, 28));
        draw_rect(8, 118, BEZEL_L_W - 18, 52, RGB(120, 40, 60));

        draw_text(14, 122, "TRACTOR BEAM", C_RED, 1);
        draw_text(14, 133, "BOSS GALAGA CAN", C_WHITE, 1);
        draw_text(14, 143, "CAPTURE SHIPS!", C_ORANGE, 1);
        draw_text(14, 153, "DESTROY ON DIVE", C_GREEN, 1);

        // Controls Box
        draw_fill_rect(8, 178, BEZEL_L_W - 18, 84, RGB(16, 22, 38));
        draw_rect(8, 178, BEZEL_L_W - 18, 84, RGB(40, 60, 100));

        draw_text(14, 183, "CONTROLS", C_CYAN, 1);
        draw_text(14, 196, "D-PAD: MOVE", C_WHITE, 1);
        draw_text(14, 209, "[X]  : FIRE", C_YELLOW, 1);
        draw_text(14, 222, "START: PLAY", C_GREEN, 1);
        draw_text(14, 235, "AUTO-FIRE ON", C_CYAN, 1);
        draw_text(14, 248, "PSP HOME: EXIT", C_DIM, 1);
    }

    void render_right(int score, int high_score, int stage, int lives, int shots, int hits) {
        // Cabinet right panel background
        draw_fill_rect(BEZEL_R_X, 0, BEZEL_R_W, SCR_H, RGB(12, 14, 26));

        // Decorative stripes
        for (int y = 0; y < SCR_H; y += 16) {
            draw_line(BEZEL_R_X + 4, y, SCR_W, y, RGB(24, 30, 56));
        }

        // Gold & Cyan cabinet bezel inner divider
        draw_line(BEZEL_R_X, 0, BEZEL_R_X, SCR_H, RGB(0, 220, 255));
        draw_line(BEZEL_R_X + 1, 0, BEZEL_R_X + 1, SCR_H, RGB(248, 200, 32));
        draw_line(BEZEL_R_X + 2, 0, BEZEL_R_X + 2, SCR_H, RGB(40, 80, 160));

        // 1UP Score
        draw_text(BEZEL_R_X + 14, 10, "1UP", C_RED, 1);
        char buf[32];
        snprintf(buf, sizeof(buf), "%6d", score);
        draw_text(BEZEL_R_X + 14, 22, buf, C_WHITE, 1);

        // High Score
        draw_text(BEZEL_R_X + 14, 38, "HIGH SCORE", C_RED, 1);
        snprintf(buf, sizeof(buf), "%6d", high_score);
        draw_text(BEZEL_R_X + 14, 50, buf, C_WHITE, 1);

        // Stage & Stats Card
        draw_fill_rect(BEZEL_R_X + 10, 68, BEZEL_R_W - 18, 90, RGB(20, 26, 46));
        draw_rect(BEZEL_R_X + 10, 68, BEZEL_R_W - 18, 90, RGB(45, 65, 110));

        snprintf(buf, sizeof(buf), "STAGE %d", stage);
        draw_text(BEZEL_R_X + 16, 73, buf, C_CYAN, 1);
        draw_text(BEZEL_R_X + 16, 86, "FREE PLAY", C_GREEN, 1);

        // Accuracy stats
        draw_text(BEZEL_R_X + 16, 104, "ACCURACY", C_YELLOW, 1);
        snprintf(buf, sizeof(buf), "SHOTS %4d", shots);
        draw_text(BEZEL_R_X + 16, 116, buf, C_WHITE, 1);
        snprintf(buf, sizeof(buf), "HITS  %4d", hits);
        draw_text(BEZEL_R_X + 16, 127, buf, C_WHITE, 1);

        float ratio = (shots > 0) ? ((float)hits / (float)shots * 100.0f) : 0.0f;
        if (ratio > 100.0f) ratio = 100.0f;
        snprintf(buf, sizeof(buf), "RATIO %4.1f%%", ratio);
        draw_text(BEZEL_R_X + 16, 138, buf, C_GREEN, 1);

        // Reserve Ships panel
        draw_fill_rect(BEZEL_R_X + 10, 166, BEZEL_R_W - 18, 38, RGB(16, 22, 38));
        draw_rect(BEZEL_R_X + 10, 166, BEZEL_R_W - 18, 38, RGB(40, 60, 100));

        draw_text(BEZEL_R_X + 14, 170, "RESERVES", C_CYAN, 1);
        int draw_lives = (lives > 5) ? 5 : lives;
        for (int l = 0; l < draw_lives; l++) {
            int lx = BEZEL_R_X + 16 + l * 20;
            int ly = 182;
            // Mini ship icon
            draw_fill_rect(lx + 3, ly, 2, 8, C_WHITE);
            draw_fill_rect(lx, ly + 5, 8, 2, C_WHITE);
            draw_pixel(lx + 2, ly + 4, C_RED);
            draw_pixel(lx + 5, ly + 4, C_RED);
        }
        if (lives > 5) {
            snprintf(buf, sizeof(buf), "+%d", lives - 5);
            draw_text(BEZEL_R_X + 116, 184, buf, C_YELLOW, 1);
        }

        // Stage Badges panel
        draw_fill_rect(BEZEL_R_X + 10, 210, BEZEL_R_W - 18, 52, RGB(16, 22, 38));
        draw_rect(BEZEL_R_X + 10, 210, BEZEL_R_W - 18, 52, RGB(40, 60, 100));

        draw_text(BEZEL_R_X + 14, 214, "STAGE BADGES", C_YELLOW, 1);
        render_stage_badges(BEZEL_R_X + 16, 228, stage);
    }

private:
    void render_stage_badges(int start_x, int start_y, int stage) {
        int rem = stage;
        int cur_x = start_x;
        int cur_y = start_y;

        // 50-Stage Badges
        int b50 = rem / 50;
        rem %= 50;
        for (int i = 0; i < b50 && i < 2; i++) {
            draw_badge_50(cur_x, cur_y);
            cur_x += 16;
        }

        // 30-Stage Badges
        int b30 = rem / 30;
        rem %= 30;
        for (int i = 0; i < b30 && i < 2; i++) {
            draw_badge_30(cur_x, cur_y);
            cur_x += 14;
        }

        // 20-Stage Badges
        int b20 = rem / 20;
        rem %= 20;
        for (int i = 0; i < b20 && i < 2; i++) {
            draw_badge_20(cur_x, cur_y);
            cur_x += 14;
        }

        // 10-Stage Badges
        int b10 = rem / 10;
        rem %= 10;
        for (int i = 0; i < b10 && i < 3; i++) {
            draw_badge_10(cur_x, cur_y);
            cur_x += 12;
        }

        // 5-Stage Badges
        int b5 = rem / 5;
        rem %= 5;
        for (int i = 0; i < b5 && i < 2; i++) {
            draw_badge_5(cur_x, cur_y);
            cur_x += 12;
        }

        // 1-Stage Flags
        int b1 = rem;
        for (int i = 0; i < b1 && i < 6; i++) {
            if (cur_x + 8 > BEZEL_R_X + BEZEL_R_W - 12) {
                cur_x = start_x;
                cur_y += 12;
            }
            draw_badge_1(cur_x, cur_y);
            cur_x += 10;
        }
    }

    void draw_badge_1(int x, int y) {
        draw_line(x, y, x, y + 8, C_WHITE);
        draw_fill_rect(x + 1, y, 5, 4, RGB(32, 120, 240));
        draw_pixel(x + 2, y + 1, C_YELLOW);
        draw_pixel(x + 3, y + 2, C_YELLOW);
    }

    void draw_badge_5(int x, int y) {
        draw_fill_rect(x, y + 1, 8, 3, C_RED);
        draw_pixel(x + 1, y, C_RED);
        draw_pixel(x + 6, y, C_RED);
        draw_pixel(x + 2, y + 4, C_RED);
        draw_pixel(x + 5, y + 4, C_RED);
    }

    void draw_badge_10(int x, int y) {
        draw_fill_rect(x, y, 9, 3, C_YELLOW);
        draw_fill_rect(x + 1, y + 4, 7, 3, C_YELLOW);
    }

    void draw_badge_20(int x, int y) {
        draw_fill_rect(x + 1, y, 9, 8, C_CYAN);
        draw_line(x + 5, y, x + 5, y + 7, C_YELLOW);
        draw_line(x + 1, y + 4, x + 9, y + 4, C_YELLOW);
    }

    void draw_badge_30(int x, int y) {
        draw_fill_rect(x, y, 10, 8, C_RED);
        draw_rect(x, y, 10, 8, C_YELLOW);
    }

    void draw_badge_50(int x, int y) {
        draw_fill_rect(x, y + 2, 12, 5, C_YELLOW);
        draw_line(x + 6, y, x + 6, y + 8, C_CYAN);
    }
};

#endif // BEZEL_HPP
