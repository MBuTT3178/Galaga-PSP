#ifndef BEZEL_HPP
#define BEZEL_HPP

#include "sprites.hpp"
#include <cstdio>
#include <cstring>

class ArcadeBezel {
public:
    void render_left(SDL_Renderer *ren) {
        // Cabinet side panel background
        SDL_Rect rc_bg = {0, 0, BEZEL_L_W, SCR_H};
        SDL_SetRenderDrawColor(ren, 12, 14, 26, 255);
        SDL_RenderFillRect(ren, &rc_bg);

        // Decorative cabinet side stripes
        SDL_SetRenderDrawColor(ren, 24, 30, 56, 255);
        for (int y = 0; y < SCR_H; y += 16) {
            SDL_RenderDrawLine(ren, 0, y, BEZEL_L_W - 4, y);
        }

        // Gold & Cyan cabinet bezel inner divider
        SDL_SetRenderDrawColor(ren, 40, 80, 160, 255);
        SDL_RenderDrawLine(ren, BEZEL_L_W - 3, 0, BEZEL_L_W - 3, SCR_H);
        SDL_SetRenderDrawColor(ren, 248, 200, 32, 255);
        SDL_RenderDrawLine(ren, BEZEL_L_W - 2, 0, BEZEL_L_W - 2, SCR_H);
        SDL_SetRenderDrawColor(ren, 0, 220, 255, 255);
        SDL_RenderDrawLine(ren, BEZEL_L_W - 1, 0, BEZEL_L_W - 1, SCR_H);

        // Marquee Header
        draw_text(ren, 26, 12, "GALAGA", C_RED, 2);
        draw_text(ren, 24, 10, "GALAGA", C_YELLOW, 2);

        draw_text(ren, 18, 30, "MIDWAY / NAMCO", C_CYAN, 1);
        draw_text(ren, 38, 40, "- 1981 -", C_WHITE, 1);

        // Divider
        SDL_SetRenderDrawColor(ren, 60, 90, 160, 255);
        SDL_RenderDrawLine(ren, 10, 52, BEZEL_L_W - 14, 52);

        // Instruction Card 1: Dual Fighter
        SDL_Rect rc_card1 = {8, 58, BEZEL_L_W - 18, 54};
        SDL_SetRenderDrawColor(ren, 20, 28, 50, 255);
        SDL_RenderFillRect(ren, &rc_card1);
        SDL_SetRenderDrawColor(ren, 45, 65, 110, 255);
        SDL_RenderDrawRect(ren, &rc_card1);

        draw_text(ren, 14, 62, "DUAL FIGHTER", C_YELLOW, 1);
        draw_text(ren, 14, 73, "RESCUE CAPTURED", C_WHITE, 1);
        draw_text(ren, 14, 83, "SHIP FOR DOUBLE", C_WHITE, 1);
        draw_text(ren, 14, 93, "FIREPOWER!", C_CYAN, 1);

        // Instruction Card 2: Tractor Beam Warning
        SDL_Rect rc_card2 = {8, 118, BEZEL_L_W - 18, 52};
        SDL_SetRenderDrawColor(ren, 35, 18, 28, 255);
        SDL_RenderFillRect(ren, &rc_card2);
        SDL_SetRenderDrawColor(ren, 120, 40, 60, 255);
        SDL_RenderDrawRect(ren, &rc_card2);

        draw_text(ren, 14, 122, "TRACTOR BEAM", C_RED, 1);
        draw_text(ren, 14, 133, "BOSS GALAGA CAN", C_WHITE, 1);
        draw_text(ren, 14, 143, "CAPTURE SHIPS!", C_ORANGE, 1);
        draw_text(ren, 14, 153, "DESTROY ON DIVE", C_GREEN, 1);

        // Controls Box
        SDL_Rect rc_ctrl = {8, 178, BEZEL_L_W - 18, 84};
        SDL_SetRenderDrawColor(ren, 16, 22, 38, 255);
        SDL_RenderFillRect(ren, &rc_ctrl);
        SDL_SetRenderDrawColor(ren, 40, 60, 100, 255);
        SDL_RenderDrawRect(ren, &rc_ctrl);

        draw_text(ren, 14, 183, "CONTROLS", C_CYAN, 1);
        draw_text(ren, 14, 196, "D-PAD: MOVE", C_WHITE, 1);
        draw_text(ren, 14, 209, "[X]  : FIRE", C_YELLOW, 1);
        draw_text(ren, 14, 222, "START: PLAY", C_GREEN, 1);
        draw_text(ren, 14, 235, "AUTO-FIRE ON", C_CYAN, 1);
        draw_text(ren, 14, 248, "PSP HOME: EXIT", C_DIM, 1);
    }

    void render_right(SDL_Renderer *ren, int score, int high_score, int stage, int lives, int shots, int hits) {
        // Cabinet right panel background
        SDL_Rect rc_bg = {BEZEL_R_X, 0, BEZEL_R_W, SCR_H};
        SDL_SetRenderDrawColor(ren, 12, 14, 26, 255);
        SDL_RenderFillRect(ren, &rc_bg);

        // Decorative stripes
        SDL_SetRenderDrawColor(ren, 24, 30, 56, 255);
        for (int y = 0; y < SCR_H; y += 16) {
            SDL_RenderDrawLine(ren, BEZEL_R_X + 4, y, SCR_W, y);
        }

        // Gold & Cyan cabinet bezel inner divider
        SDL_SetRenderDrawColor(ren, 0, 220, 255, 255);
        SDL_RenderDrawLine(ren, BEZEL_R_X, 0, BEZEL_R_X, SCR_H);
        SDL_SetRenderDrawColor(ren, 248, 200, 32, 255);
        SDL_RenderDrawLine(ren, BEZEL_R_X + 1, 0, BEZEL_R_X + 1, SCR_H);
        SDL_SetRenderDrawColor(ren, 40, 80, 160, 255);
        SDL_RenderDrawLine(ren, BEZEL_R_X + 2, 0, BEZEL_R_X + 2, SCR_H);

        // 1UP Score
        draw_text(ren, BEZEL_R_X + 14, 10, "1UP", C_RED, 1);
        char buf[32];
        snprintf(buf, sizeof(buf), "%6d", score);
        draw_text(ren, BEZEL_R_X + 14, 22, buf, C_WHITE, 1);

        // High Score
        draw_text(ren, BEZEL_R_X + 14, 38, "HIGH SCORE", C_RED, 1);
        snprintf(buf, sizeof(buf), "%6d", high_score);
        draw_text(ren, BEZEL_R_X + 14, 50, buf, C_WHITE, 1);

        // Stage & Stats Card
        SDL_Rect rc_stats = {BEZEL_R_X + 10, 68, BEZEL_R_W - 18, 90};
        SDL_SetRenderDrawColor(ren, 20, 26, 46, 255);
        SDL_RenderFillRect(ren, &rc_stats);
        SDL_SetRenderDrawColor(ren, 45, 65, 110, 255);
        SDL_RenderDrawRect(ren, &rc_stats);

        snprintf(buf, sizeof(buf), "STAGE %d", stage);
        draw_text(ren, BEZEL_R_X + 16, 73, buf, C_CYAN, 1);

        draw_text(ren, BEZEL_R_X + 16, 86, "FREE PLAY", C_GREEN, 1);

        // Accuracy stats
        draw_text(ren, BEZEL_R_X + 16, 104, "ACCURACY", C_YELLOW, 1);
        snprintf(buf, sizeof(buf), "SHOTS %4d", shots);
        draw_text(ren, BEZEL_R_X + 16, 116, buf, C_WHITE, 1);
        snprintf(buf, sizeof(buf), "HITS  %4d", hits);
        draw_text(ren, BEZEL_R_X + 16, 127, buf, C_WHITE, 1);

        float ratio = (shots > 0) ? ((float)hits / (float)shots * 100.0f) : 0.0f;
        if (ratio > 100.0f) ratio = 100.0f;
        snprintf(buf, sizeof(buf), "RATIO %4.1f%%", ratio);
        draw_text(ren, BEZEL_R_X + 16, 138, buf, C_GREEN, 1);

        // Reserve Ships panel
        SDL_Rect rc_lives = {BEZEL_R_X + 10, 166, BEZEL_R_W - 18, 38};
        SDL_SetRenderDrawColor(ren, 16, 22, 38, 255);
        SDL_RenderFillRect(ren, &rc_lives);
        SDL_SetRenderDrawColor(ren, 40, 60, 100, 255);
        SDL_RenderDrawRect(ren, &rc_lives);

        draw_text(ren, BEZEL_R_X + 14, 170, "RESERVES", C_CYAN, 1);
        int draw_lives = (lives > 5) ? 5 : lives;
        for (int l = 0; l < draw_lives; l++) {
            int lx = BEZEL_R_X + 16 + l * 20;
            int ly = 182;
            // Draw mini player ship icon (8x8)
            SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
            SDL_Rect r_hull = {lx + 3, ly, 2, 8};
            SDL_RenderFillRect(ren, &r_hull);
            SDL_Rect r_wings = {lx, ly + 5, 8, 2};
            SDL_RenderFillRect(ren, &r_wings);
            SDL_SetRenderDrawColor(ren, 240, 32, 32, 255);
            SDL_RenderDrawPoint(ren, lx + 2, ly + 4);
            SDL_RenderDrawPoint(ren, lx + 5, ly + 4);
        }
        if (lives > 5) {
            snprintf(buf, sizeof(buf), "+%d", lives - 5);
            draw_text(ren, BEZEL_R_X + 116, 184, buf, C_YELLOW, 1);
        }

        // Stage Badges panel (Authentic Galaga Stage Flags!)
        SDL_Rect rc_flags = {BEZEL_R_X + 10, 210, BEZEL_R_W - 18, 52};
        SDL_SetRenderDrawColor(ren, 16, 22, 38, 255);
        SDL_RenderFillRect(ren, &rc_flags);
        SDL_SetRenderDrawColor(ren, 40, 60, 100, 255);
        SDL_RenderDrawRect(ren, &rc_flags);

        draw_text(ren, BEZEL_R_X + 14, 214, "STAGE BADGES", C_YELLOW, 1);
        render_stage_badges(ren, BEZEL_R_X + 16, 228, stage);
    }

private:
    void render_stage_badges(SDL_Renderer *ren, int start_x, int start_y, int stage) {
        int rem = stage;
        int cur_x = start_x;
        int cur_y = start_y;

        // 50-Stage Badges (Big Blue/Gold Star)
        int b50 = rem / 50;
        rem %= 50;
        for (int i = 0; i < b50 && i < 2; i++) {
            draw_badge_50(ren, cur_x, cur_y);
            cur_x += 16;
        }

        // 30-Stage Badges
        int b30 = rem / 30;
        rem %= 30;
        for (int i = 0; i < b30 && i < 2; i++) {
            draw_badge_30(ren, cur_x, cur_y);
            cur_x += 14;
        }

        // 20-Stage Badges
        int b20 = rem / 20;
        rem %= 20;
        for (int i = 0; i < b20 && i < 2; i++) {
            draw_badge_20(ren, cur_x, cur_y);
            cur_x += 14;
        }

        // 10-Stage Badges (Double Chevron)
        int b10 = rem / 10;
        rem %= 10;
        for (int i = 0; i < b10 && i < 3; i++) {
            draw_badge_10(ren, cur_x, cur_y);
            cur_x += 12;
        }

        // 5-Stage Badges (Red Chevron)
        int b5 = rem / 5;
        rem %= 5;
        for (int i = 0; i < b5 && i < 2; i++) {
            draw_badge_5(ren, cur_x, cur_y);
            cur_x += 12;
        }

        // 1-Stage Flags (Small Blue/Yellow Flags)
        int b1 = rem;
        for (int i = 0; i < b1 && i < 6; i++) {
            if (cur_x + 8 > BEZEL_R_X + BEZEL_R_W - 12) {
                cur_x = start_x;
                cur_y += 12;
            }
            draw_badge_1(ren, cur_x, cur_y);
            cur_x += 10;
        }
    }

    void draw_badge_1(SDL_Renderer *ren, int x, int y) {
        // Little flag pole
        SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
        SDL_RenderDrawLine(ren, x, y, x, y + 8);
        // Small blue/yellow pennant
        SDL_SetRenderDrawColor(ren, 32, 120, 240, 255);
        SDL_Rect rc = {x + 1, y, 5, 4};
        SDL_RenderFillRect(ren, &rc);
        SDL_SetRenderDrawColor(ren, 248, 224, 32, 255);
        SDL_RenderDrawPoint(ren, x + 2, y + 1);
        SDL_RenderDrawPoint(ren, x + 3, y + 2);
    }

    void draw_badge_5(SDL_Renderer *ren, int x, int y) {
        // Red Chevron
        SDL_SetRenderDrawColor(ren, 240, 32, 32, 255);
        SDL_Rect rc = {x, y + 1, 8, 3};
        SDL_RenderFillRect(ren, &rc);
        SDL_RenderDrawPoint(ren, x + 1, y);
        SDL_RenderDrawPoint(ren, x + 6, y);
        SDL_RenderDrawPoint(ren, x + 2, y + 4);
        SDL_RenderDrawPoint(ren, x + 5, y + 4);
    }

    void draw_badge_10(SDL_Renderer *ren, int x, int y) {
        // Gold Chevron
        SDL_SetRenderDrawColor(ren, 248, 224, 32, 255);
        SDL_Rect rc1 = {x, y, 9, 3};
        SDL_RenderFillRect(ren, &rc1);
        SDL_Rect rc2 = {x + 1, y + 4, 7, 3};
        SDL_RenderFillRect(ren, &rc2);
    }

    void draw_badge_20(SDL_Renderer *ren, int x, int y) {
        // Gold & Cyan Star
        SDL_SetRenderDrawColor(ren, 0, 220, 255, 255);
        SDL_Rect rc = {x + 1, y, 9, 8};
        SDL_RenderFillRect(ren, &rc);
        SDL_SetRenderDrawColor(ren, 248, 224, 32, 255);
        SDL_RenderDrawLine(ren, x + 5, y, x + 5, y + 7);
        SDL_RenderDrawLine(ren, x + 1, y + 4, x + 9, y + 4);
    }

    void draw_badge_30(SDL_Renderer *ren, int x, int y) {
        // Red & Gold Crest
        SDL_SetRenderDrawColor(ren, 240, 32, 32, 255);
        SDL_Rect rc = {x, y, 10, 8};
        SDL_RenderFillRect(ren, &rc);
        SDL_SetRenderDrawColor(ren, 248, 224, 32, 255);
        SDL_RenderDrawRect(ren, &rc);
    }

    void draw_badge_50(SDL_Renderer *ren, int x, int y) {
        // Golden Winged Insignia
        SDL_SetRenderDrawColor(ren, 248, 224, 32, 255);
        SDL_Rect rc = {x, y + 2, 12, 5};
        SDL_RenderFillRect(ren, &rc);
        SDL_SetRenderDrawColor(ren, 0, 220, 255, 255);
        SDL_RenderDrawLine(ren, x + 6, y, x + 6, y + 8);
    }
};

#endif // BEZEL_HPP
