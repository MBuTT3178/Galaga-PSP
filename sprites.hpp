#ifndef SPRITES_HPP
#define SPRITES_HPP

#include <SDL.h>
#include <cstdint>
#include <cmath>

/* ─── CONSTANTS & DIMENSIONS ─────────────────────────────────────────── */
#define SCR_W 480
#define SCR_H 272

/* Vertical 3:4 Arcade Aspect Ratio centered on PSP */
#define PLAY_X 138
#define PLAY_Y 0
#define PLAY_W 204
#define PLAY_H 272

#define BEZEL_L_W 138
#define BEZEL_R_X (PLAY_X + PLAY_W) // 342
#define BEZEL_R_W 138

/* Colors */
struct RgbColor {
    uint8_t r, g, b, a;
};

static const RgbColor C_BLACK   = {0, 0, 0, 255};
static const RgbColor C_WHITE   = {255, 255, 255, 255};
static const RgbColor C_RED     = {240, 32, 32, 255};
static const RgbColor C_BLUE    = {32, 96, 240, 255};
static const RgbColor C_CYAN    = {32, 224, 240, 255};
static const RgbColor C_YELLOW  = {248, 224, 32, 255};
static const RgbColor C_GREEN   = {32, 208, 64, 255};
static const RgbColor C_MAGENTA = {224, 32, 192, 255};
static const RgbColor C_ORANGE  = {248, 140, 32, 255};
static const RgbColor C_DK_BLUE = {16, 24, 64, 255};
static const RgbColor C_DIM     = {120, 130, 150, 255};
static const RgbColor C_TRANS   = {0, 0, 0, 0};

/* ─── 8x8 BITMAP FONT ─────────────────────────────────────────────────── */
static const unsigned char font8x8_galaga[96][8] = {
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, // 32 ' '
    {0x18,0x3C,0x3C,0x18,0x18,0x00,0x18,0x00}, // 33 '!'
    {0x66,0x66,0x24,0x00,0x00,0x00,0x00,0x00}, // 34 '"'
    {0x6C,0x6C,0xFE,0x6C,0xFE,0x6C,0x6C,0x00}, // 35 '#'
    {0x18,0x3E,0x60,0x3C,0x06,0x7C,0x18,0x00}, // 36 '$'
    {0x00,0xC6,0xCC,0x18,0x30,0x66,0xC6,0x00}, // 37 '%'
    {0x38,0x6C,0x38,0x76,0xDC,0xCC,0x76,0x00}, // 38 '&'
    {0x18,0x18,0x30,0x00,0x00,0x00,0x00,0x00}, // 39 '''
    {0x0C,0x18,0x30,0x30,0x30,0x18,0x0C,0x00}, // 40 '('
    {0x30,0x18,0x0C,0x0C,0x0C,0x18,0x30,0x00}, // 41 ')'
    {0x00,0x66,0x3C,0xFF,0x3C,0x66,0x00,0x00}, // 42 '*'
    {0x00,0x18,0x18,0x7E,0x18,0x18,0x00,0x00}, // 43 '+'
    {0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x30}, // 44 ','
    {0x00,0x00,0x00,0x7E,0x00,0x00,0x00,0x00}, // 45 '-'
    {0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00}, // 46 '.'
    {0x06,0x0C,0x18,0x30,0x60,0xC0,0x80,0x00}, // 47 '/'
    {0x3C,0x66,0x6E,0x7E,0x76,0x66,0x3C,0x00}, // 48 '0'
    {0x18,0x38,0x18,0x18,0x18,0x18,0x7E,0x00}, // 49 '1'
    {0x3C,0x66,0x06,0x0C,0x18,0x30,0x7E,0x00}, // 50 '2'
    {0x3C,0x66,0x06,0x1C,0x06,0x66,0x3C,0x00}, // 51 '3'
    {0x0C,0x1C,0x34,0x64,0x7E,0x04,0x04,0x00}, // 52 '4'
    {0x7E,0x60,0x7C,0x06,0x06,0x66,0x3C,0x00}, // 53 '5'
    {0x1C,0x30,0x60,0x7C,0x66,0x66,0x3C,0x00}, // 54 '6'
    {0x7E,0x06,0x0C,0x18,0x18,0x18,0x18,0x00}, // 55 '7'
    {0x3C,0x66,0x66,0x3C,0x66,0x66,0x3C,0x00}, // 56 '8'
    {0x3C,0x66,0x66,0x3E,0x06,0x0C,0x38,0x00}, // 57 '9'
    {0x00,0x00,0x18,0x00,0x00,0x18,0x00,0x00}, // 58 ':'
    {0x00,0x00,0x18,0x00,0x00,0x18,0x18,0x30}, // 59 ';'
    {0x0C,0x18,0x30,0x60,0x30,0x18,0x0C,0x00}, // 60 '<'
    {0x00,0x00,0x7E,0x00,0x7E,0x00,0x00,0x00}, // 61 '='
    {0x30,0x18,0x0C,0x06,0x0C,0x18,0x30,0x00}, // 62 '>'
    {0x3C,0x66,0x06,0x0C,0x18,0x00,0x18,0x00}, // 63 '?'
    {0x3C,0x66,0x6E,0x6E,0x60,0x62,0x3C,0x00}, // 64 '@'
    {0x18,0x3C,0x66,0x66,0x7E,0x66,0x66,0x00}, // 65 'A'
    {0x7C,0x66,0x66,0x7C,0x66,0x66,0x7C,0x00}, // 66 'B'
    {0x3C,0x66,0x60,0x60,0x60,0x66,0x3C,0x00}, // 67 'C'
    {0x78,0x6C,0x66,0x66,0x66,0x6C,0x78,0x00}, // 68 'D'
    {0x7E,0x60,0x60,0x78,0x60,0x60,0x7E,0x00}, // 69 'E'
    {0x7E,0x60,0x60,0x78,0x60,0x60,0x60,0x00}, // 70 'F'
    {0x3C,0x66,0x60,0x6E,0x66,0x66,0x3E,0x00}, // 71 'G'
    {0x66,0x66,0x66,0x7E,0x66,0x66,0x66,0x00}, // 72 'H'
    {0x3C,0x18,0x18,0x18,0x18,0x18,0x3C,0x00}, // 73 'I'
    {0x1E,0x0C,0x0C,0x0C,0x0C,0x6C,0x38,0x00}, // 74 'J'
    {0x66,0x6C,0x78,0x70,0x78,0x6C,0x66,0x00}, // 75 'K'
    {0x60,0x60,0x60,0x60,0x60,0x60,0x7E,0x00}, // 76 'L'
    {0x63,0x77,0x7F,0x6B,0x63,0x63,0x63,0x00}, // 77 'M'
    {0x66,0x76,0x7E,0x7E,0x6E,0x66,0x66,0x00}, // 78 'N'
    {0x3C,0x66,0x66,0x66,0x66,0x66,0x3C,0x00}, // 79 'O'
    {0x7C,0x66,0x66,0x7C,0x60,0x60,0x60,0x00}, // 80 'P'
    {0x3C,0x66,0x66,0x66,0x6A,0x6C,0x36,0x00}, // 81 'Q'
    {0x7C,0x66,0x66,0x7C,0x78,0x6C,0x66,0x00}, // 82 'R'
    {0x3C,0x66,0x60,0x3C,0x06,0x66,0x3C,0x00}, // 83 'S'
    {0x7E,0x18,0x18,0x18,0x18,0x18,0x18,0x00}, // 84 'T'
    {0x66,0x66,0x66,0x66,0x66,0x66,0x3C,0x00}, // 85 'U'
    {0x66,0x66,0x66,0x66,0x66,0x3C,0x18,0x00}, // 86 'V'
    {0x63,0x63,0x63,0x6B,0x7F,0x77,0x63,0x00}, // 87 'W'
    {0x66,0x66,0x3C,0x18,0x3C,0x66,0x66,0x00}, // 88 'X'
    {0x66,0x66,0x66,0x3C,0x18,0x18,0x18,0x00}, // 89 'Y'
    {0x7E,0x06,0x0C,0x18,0x30,0x60,0x7E,0x00}, // 90 'Z'
    {0x3C,0x30,0x30,0x30,0x30,0x30,0x3C,0x00}, // 91 '['
    {0xC0,0x60,0x30,0x18,0x0C,0x06,0x02,0x00}, // 92 '\'
    {0x3C,0x0C,0x0C,0x0C,0x0C,0x0C,0x3C,0x00}, // 93 ']'
    {0x10,0x38,0x7C,0xEE,0x00,0x00,0x00,0x00}, // 94 '^'
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF}, // 95 '_'
};

inline void draw_text(SDL_Renderer *ren, int x, int y, const char *str, const RgbColor &c, int scale = 1) {
    SDL_SetRenderDrawColor(ren, c.r, c.g, c.b, c.a);
    for (int i = 0; str[i] != '\0'; i++) {
        char ch = str[i];
        if (ch >= 'a' && ch <= 'z') ch -= 32;
        if (ch < 32 || ch > 95) { x += 8 * scale; continue; }
        const unsigned char *glyph = font8x8_galaga[ch - 32];
        for (int r = 0; r < 8; r++) {
            unsigned char bits = glyph[r];
            for (int col = 0; col < 8; col++) {
                if (bits & (0x80 >> col)) {
                    int px = x + col * scale;
                    int py = y + r * scale;
                    if (px >= 0 && px < SCR_W && py >= 0 && py < SCR_H) {
                        if (scale == 1) {
                            SDL_RenderDrawPoint(ren, px, py);
                        } else {
                            SDL_Rect rc = {px, py, scale, scale};
                            SDL_RenderFillRect(ren, &rc);
                        }
                    }
                }
            }
        }
        x += 8 * scale;
    }
}

/* ─── 16x16 SPRITE MATRICES ──────────────────────────────────────────── */
/*
 * Color indices:
 * 0 = Transparent
 * 1 = White
 * 2 = Red
 * 3 = Blue
 * 4 = Cyan
 * 5 = Yellow
 * 6 = Green
 * 7 = Orange
 */

// Player Fighter (16x16)
static const uint8_t SPR_FIGHTER[16][16] = {
    {0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,2,2,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,2,2,1,0,0,0,0,0,0},
    {0,0,0,0,0,1,1,4,4,1,1,0,0,0,0,0},
    {0,0,0,0,0,1,1,4,4,1,1,0,0,0,0,0},
    {0,0,0,0,1,1,2,2,2,2,1,1,0,0,0,0},
    {0,0,0,1,1,1,2,2,2,2,1,1,1,0,0,0},
    {0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0},
    {0,1,1,3,1,1,1,1,1,1,1,1,3,1,1,0},
    {1,1,3,3,1,1,1,1,1,1,1,1,3,3,1,1},
    {1,3,3,3,1,1,1,0,0,1,1,1,3,3,3,1},
    {1,3,3,0,1,1,0,0,0,0,1,1,0,3,3,1},
    {1,1,0,0,1,2,0,0,0,0,2,1,0,0,1,1},
    {1,0,0,0,2,2,0,0,0,0,2,2,0,0,0,1},
    {0,0,0,0,2,0,0,0,0,0,0,2,0,0,0,0}
};

// Captured Fighter (Flashing Red variant)
static const uint8_t SPR_CAPTURED_FIGHTER[16][16] = {
    {0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,2,5,5,2,0,0,0,0,0,0},
    {0,0,0,0,0,0,2,5,5,2,0,0,0,0,0,0},
    {0,0,0,0,0,2,2,7,7,2,2,0,0,0,0,0},
    {0,0,0,0,0,2,2,7,7,2,2,0,0,0,0,0},
    {0,0,0,0,2,2,5,5,5,5,2,2,0,0,0,0},
    {0,0,0,2,2,2,5,5,5,5,2,2,2,0,0,0},
    {0,0,2,2,2,2,2,2,2,2,2,2,2,2,0,0},
    {0,2,2,1,2,2,2,2,2,2,2,2,1,2,2,0},
    {2,2,1,1,2,2,2,2,2,2,2,2,1,1,2,2},
    {2,1,1,1,2,2,2,0,0,2,2,2,1,1,1,2},
    {2,1,1,0,2,2,0,0,0,0,2,2,0,1,1,2},
    {2,2,0,0,2,5,0,0,0,0,5,2,0,0,2,2},
    {2,0,0,0,5,5,0,0,0,0,5,5,0,0,0,2},
    {0,0,0,0,5,0,0,0,0,0,0,5,0,0,0,0}
};

// Boss Galaga - Healthy Green (16x16)
static const uint8_t SPR_BOSS_GREEN[16][16] = {
    {0,0,0,0,6,6,0,0,0,0,6,6,0,0,0,0},
    {0,0,0,6,6,6,6,0,0,6,6,6,6,0,0,0},
    {0,0,6,6,5,5,6,6,6,6,5,5,6,6,0,0},
    {0,6,6,5,5,5,5,6,6,5,5,5,5,6,6,0},
    {0,6,6,6,5,5,6,6,6,6,5,5,6,6,6,0},
    {6,6,3,6,6,6,3,3,3,3,6,6,6,3,6,6},
    {6,3,3,3,3,3,3,3,3,3,3,3,3,3,3,6},
    {6,3,3,1,1,3,3,3,3,3,3,1,1,3,3,6},
    {6,6,3,1,1,3,6,6,6,6,3,1,1,3,6,6},
    {0,6,6,3,3,6,6,5,5,6,6,3,3,6,6,0},
    {0,0,6,6,6,6,5,5,5,5,6,6,6,6,0,0},
    {0,0,0,6,6,6,6,6,6,6,6,6,6,0,0,0},
    {0,0,6,6,6,0,6,6,6,6,0,6,6,6,0,0},
    {0,6,6,6,0,0,0,6,6,0,0,0,6,6,6,0},
    {6,6,6,0,0,0,0,0,0,0,0,0,0,6,6,6},
    {6,6,0,0,0,0,0,0,0,0,0,0,0,0,6,6}
};

// Boss Galaga - Damaged Blue (16x16, takes 1 more hit)
static const uint8_t SPR_BOSS_BLUE[16][16] = {
    {0,0,0,0,3,3,0,0,0,0,3,3,0,0,0,0},
    {0,0,0,3,3,3,3,0,0,3,3,3,3,0,0,0},
    {0,0,3,3,2,2,3,3,3,3,2,2,3,3,0,0},
    {0,3,3,2,2,2,2,3,3,2,2,2,2,3,3,0},
    {0,3,3,3,2,2,3,3,3,3,2,2,3,3,3,0},
    {3,3,4,3,3,3,4,4,4,4,3,3,3,4,3,3},
    {3,4,4,4,4,4,4,4,4,4,4,4,4,4,4,3},
    {3,4,4,1,1,4,4,4,4,4,4,1,1,4,4,3},
    {3,3,4,1,1,4,3,3,3,3,4,1,1,4,3,3},
    {0,3,3,4,4,3,3,2,2,3,3,4,4,3,3,0},
    {0,0,3,3,3,3,2,2,2,2,3,3,3,3,0,0},
    {0,0,0,3,3,3,3,3,3,3,3,3,3,0,0,0},
    {0,0,3,3,3,0,3,3,3,3,0,3,3,3,0,0},
    {0,3,3,3,0,0,0,3,3,0,0,0,3,3,3,0},
    {3,3,3,0,0,0,0,0,0,0,0,0,0,3,3,3},
    {3,3,0,0,0,0,0,0,0,0,0,0,0,0,3,3}
};

// Goei (Red Butterfly) - Frame 0 (Wings Spread)
static const uint8_t SPR_GOEI_0[16][16] = {
    {0,0,0,0,0,2,2,0,0,2,2,0,0,0,0,0},
    {0,0,0,0,2,2,2,2,2,2,2,2,0,0,0,0},
    {0,0,0,2,2,5,5,2,2,5,5,2,2,0,0,0},
    {0,0,2,2,2,5,5,2,2,5,5,2,2,2,0,0},
    {0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0},
    {2,2,2,7,7,2,2,1,1,2,2,7,7,2,2,2},
    {2,2,7,7,7,7,2,1,1,2,7,7,7,7,2,2},
    {2,7,7,7,7,7,7,2,2,7,7,7,7,7,7,2},
    {2,7,7,7,2,2,7,2,2,7,2,2,7,7,7,2},
    {0,2,2,2,0,0,2,2,2,2,0,0,2,2,2,0},
    {0,0,0,0,0,2,2,5,5,2,2,0,0,0,0,0},
    {0,0,0,0,2,2,5,5,5,5,2,2,0,0,0,0},
    {0,0,0,2,2,2,5,5,5,5,2,2,2,0,0,0},
    {0,0,2,2,2,0,2,2,2,2,0,2,2,2,0,0},
    {0,2,2,0,0,0,0,2,2,0,0,0,0,2,2,0},
    {2,2,0,0,0,0,0,0,0,0,0,0,0,0,2,2}
};

// Goei (Red Butterfly) - Frame 1 (Wings Flapped)
static const uint8_t SPR_GOEI_1[16][16] = {
    {0,0,0,0,0,2,2,0,0,2,2,0,0,0,0,0},
    {0,0,0,0,2,2,2,2,2,2,2,2,0,0,0,0},
    {0,0,0,2,2,5,5,2,2,5,5,2,2,0,0,0},
    {0,0,0,2,2,5,5,2,2,5,5,2,2,0,0,0},
    {0,0,2,2,2,2,2,2,2,2,2,2,2,2,0,0},
    {0,2,2,7,7,2,2,1,1,2,2,7,7,2,2,0},
    {0,2,7,7,7,7,2,1,1,2,7,7,7,7,2,0},
    {2,2,7,7,7,7,7,2,2,7,7,7,7,7,2,2},
    {2,2,7,7,2,2,7,2,2,7,2,2,7,7,2,2},
    {2,2,2,2,0,0,2,2,2,2,0,0,2,2,2,2},
    {0,2,2,0,0,2,2,5,5,2,2,0,0,2,2,0},
    {0,0,0,0,2,2,5,5,5,5,2,2,0,0,0,0},
    {0,0,0,2,2,2,5,5,5,5,2,2,2,0,0,0},
    {0,0,2,2,0,0,2,2,2,2,0,0,2,2,0,0},
    {0,2,2,0,0,0,0,2,2,0,0,0,0,2,2,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};

// Zako (Blue Bee) - Frame 0 (Wings Down)
static const uint8_t SPR_ZAKO_0[16][16] = {
    {0,0,0,0,0,0,2,0,0,2,0,0,0,0,0,0},
    {0,0,0,0,0,2,2,0,0,2,2,0,0,0,0,0},
    {0,0,0,0,5,5,5,5,5,5,5,5,0,0,0,0},
    {0,0,0,5,5,1,1,5,5,1,1,5,5,0,0,0},
    {0,0,5,5,5,1,1,5,5,1,1,5,5,5,0,0},
    {0,3,3,5,5,5,5,5,5,5,5,5,5,3,3,0},
    {3,3,3,3,5,5,2,2,2,2,5,5,3,3,3,3},
    {3,3,3,3,3,2,2,2,2,2,2,3,3,3,3,3},
    {3,3,3,3,3,2,2,2,2,2,2,3,3,3,3,3},
    {0,3,3,3,5,5,2,2,2,2,5,5,3,3,3,0},
    {0,0,3,5,5,5,5,5,5,5,5,5,5,3,0,0},
    {0,0,0,5,5,5,5,2,2,5,5,5,5,0,0,0},
    {0,0,0,0,5,5,2,2,2,2,5,5,0,0,0,0},
    {0,0,0,0,0,2,2,2,2,2,2,0,0,0,0,0},
    {0,0,0,0,0,0,2,2,2,2,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0}
};

// Zako (Blue Bee) - Frame 1 (Wings Up)
static const uint8_t SPR_ZAKO_1[16][16] = {
    {0,0,3,3,0,0,2,0,0,2,0,0,3,3,0,0},
    {0,3,3,3,3,2,2,0,0,2,2,3,3,3,3,0},
    {3,3,3,3,5,5,5,5,5,5,5,5,3,3,3,3},
    {3,3,3,5,5,1,1,5,5,1,1,5,5,3,3,3},
    {0,3,5,5,5,1,1,5,5,1,1,5,5,5,3,0},
    {0,0,0,5,5,5,5,5,5,5,5,5,5,0,0,0},
    {0,0,0,0,5,5,2,2,2,2,5,5,0,0,0,0},
    {0,0,0,0,0,2,2,2,2,2,2,0,0,0,0,0},
    {0,0,0,0,0,2,2,2,2,2,2,0,0,0,0,0},
    {0,0,0,0,5,5,2,2,2,2,5,5,0,0,0,0},
    {0,0,0,5,5,5,5,5,5,5,5,5,5,0,0,0},
    {0,0,0,5,5,5,5,2,2,5,5,5,5,0,0,0},
    {0,0,0,0,5,5,2,2,2,2,5,5,0,0,0,0},
    {0,0,0,0,0,2,2,2,2,2,2,0,0,0,0,0},
    {0,0,0,0,0,0,2,2,2,2,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0}
};

// Transform Alien / Bosconian Spy (Challenging Stage bonus alien)
static const uint8_t SPR_TRANSFORM_ALIEN[16][16] = {
    {0,0,0,0,0,7,7,7,7,7,7,0,0,0,0,0},
    {0,0,0,7,7,5,5,5,5,5,5,7,7,0,0,0},
    {0,0,7,5,5,1,1,5,5,1,1,5,5,7,0,0},
    {0,7,5,5,1,1,1,5,5,1,1,1,5,5,7,0},
    {7,5,5,1,1,4,4,1,1,4,4,1,1,5,5,7},
    {7,5,1,1,4,4,4,4,4,4,4,4,1,1,5,7},
    {7,5,1,4,4,6,6,4,4,6,6,4,4,1,5,7},
    {7,7,4,4,6,6,6,6,6,6,6,6,4,4,7,7},
    {7,7,4,4,6,6,6,6,6,6,6,6,4,4,7,7},
    {7,5,1,4,4,6,6,4,4,6,6,4,4,1,5,7},
    {7,5,1,1,4,4,4,4,4,4,4,4,1,1,5,7},
    {7,5,5,1,1,4,4,1,1,4,4,1,1,5,5,7},
    {0,7,5,5,1,1,1,5,5,1,1,1,5,5,7,0},
    {0,0,7,5,5,1,1,5,5,1,1,5,5,7,0,0},
    {0,0,0,7,7,5,5,5,5,5,5,7,7,0,0,0},
    {0,0,0,0,0,7,7,7,7,7,7,0,0,0,0,0}
};

// Explosions: 4 frames of expanding fiery energy
static const uint8_t SPR_EXP_0[16][16] = {
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,5,5,1,0,0,0,0,0,0},
    {0,0,0,0,0,1,5,2,2,5,1,0,0,0,0,0},
    {0,0,0,0,1,5,2,7,7,2,5,1,0,0,0,0},
    {0,0,0,1,5,2,7,1,1,7,2,5,1,0,0,0},
    {0,0,1,5,2,7,1,1,1,1,7,2,5,1,0,0},
    {0,1,5,2,7,1,1,1,1,1,1,7,2,5,1,0},
    {0,1,5,2,7,1,1,1,1,1,1,7,2,5,1,0},
    {0,0,1,5,2,7,1,1,1,1,7,2,5,1,0,0},
    {0,0,0,1,5,2,7,1,1,7,2,5,1,0,0,0},
    {0,0,0,0,1,5,2,7,7,2,5,1,0,0,0,0},
    {0,0,0,0,0,1,5,2,2,5,1,0,0,0,0,0},
    {0,0,0,0,0,0,1,5,5,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};

static const uint8_t SPR_EXP_1[16][16] = {
    {0,0,0,0,0,2,0,0,0,0,2,0,0,0,0,0},
    {0,2,0,0,2,7,5,0,0,5,7,2,0,0,2,0},
    {0,0,7,2,7,5,1,0,0,1,5,7,2,7,0,0},
    {0,0,2,5,1,0,0,0,0,0,0,1,5,2,0,0},
    {0,2,7,1,0,0,7,5,5,7,0,0,1,7,2,0},
    {2,7,5,0,0,7,5,1,1,5,7,0,0,5,7,2},
    {0,5,1,0,7,5,1,1,1,1,5,7,0,1,5,0},
    {0,0,0,0,5,1,1,1,1,1,1,5,0,0,0,0},
    {0,0,0,0,5,1,1,1,1,1,1,5,0,0,0,0},
    {0,5,1,0,7,5,1,1,1,1,5,7,0,1,5,0},
    {2,7,5,0,0,7,5,1,1,5,7,0,0,5,7,2},
    {0,2,7,1,0,0,7,5,5,7,0,0,1,7,2,0},
    {0,0,2,5,1,0,0,0,0,0,0,1,5,2,0,0},
    {0,0,7,2,7,5,1,0,0,1,5,7,2,7,0,0},
    {0,2,0,0,2,7,5,0,0,5,7,2,0,0,2,0},
    {0,0,0,0,0,2,0,0,0,0,2,0,0,0,0,0}
};

static const uint8_t SPR_EXP_2[16][16] = {
    {2,0,0,2,0,0,0,0,0,0,0,0,2,0,0,2},
    {0,7,0,0,5,0,0,2,2,0,0,5,0,0,7,0},
    {0,0,5,0,0,1,0,5,5,0,1,0,0,5,0,0},
    {2,0,0,1,0,0,0,0,0,0,0,0,1,0,0,2},
    {0,5,0,0,0,2,0,0,0,0,2,0,0,0,5,0},
    {0,0,1,0,2,7,5,0,0,5,7,2,0,1,0,0},
    {0,0,0,0,0,5,1,0,0,1,5,0,0,0,0,0},
    {0,2,5,0,0,0,0,0,0,0,0,0,0,5,2,0},
    {0,2,5,0,0,0,0,0,0,0,0,0,0,5,2,0},
    {0,0,0,0,0,5,1,0,0,1,5,0,0,0,0,0},
    {0,0,1,0,2,7,5,0,0,5,7,2,0,1,0,0},
    {0,5,0,0,0,2,0,0,0,0,2,0,0,0,5,0},
    {2,0,0,1,0,0,0,0,0,0,0,0,1,0,0,2},
    {0,0,5,0,0,1,0,5,5,0,1,0,0,5,0,0},
    {0,7,0,0,5,0,0,2,2,0,0,5,0,0,7,0},
    {2,0,0,2,0,0,0,0,0,0,0,0,2,0,0,2}
};

static const uint8_t SPR_EXP_3[16][16] = {
    {0,2,0,0,0,0,0,0,0,0,0,0,0,0,2,0},
    {2,0,0,0,0,7,0,0,0,0,7,0,0,0,0,2},
    {0,0,0,5,0,0,0,0,0,0,0,0,5,0,0,0},
    {0,0,5,0,0,0,0,0,0,0,0,0,0,5,0,0},
    {0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0},
    {0,7,0,0,0,2,0,0,0,0,2,0,0,0,7,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,2,0,0,0,0,0,0,2,0,0,0,0},
    {0,0,0,0,2,0,0,0,0,0,0,2,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,7,0,0,0,2,0,0,0,0,2,0,0,0,7,0},
    {0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0},
    {0,0,5,0,0,0,0,0,0,0,0,0,0,5,0,0},
    {0,0,0,5,0,0,0,0,0,0,0,0,5,0,0,0},
    {2,0,0,0,0,7,0,0,0,0,7,0,0,0,0,2},
    {0,2,0,0,0,0,0,0,0,0,0,0,0,0,2,0}
};

/* ─── DIRECT HARDWARE-ACCELERATED SPRITE RASTERIZERS ────────────────── */
static const RgbColor SPR_PALETTE[8] = {
    C_TRANS,   // 0
    C_WHITE,   // 1
    C_RED,     // 2
    C_BLUE,    // 3
    C_CYAN,    // 4
    C_YELLOW,  // 5
    C_GREEN,   // 6
    C_ORANGE   // 7
};

inline void draw_sprite_16x16(SDL_Renderer *ren, int screen_x, int screen_y, const uint8_t matrix[16][16], float angle_deg = 0.0f) {
    if (angle_deg == 0.0f) {
        for (int r = 0; r < 16; r++) {
            int py = screen_y + r;
            if (py < 0 || py >= SCR_H) continue;
            int c = 0;
            while (c < 16) {
                uint8_t cidx = matrix[r][c];
                if (cidx == 0) {
                    c++;
                    continue;
                }
                int run_len = 1;
                while (c + run_len < 16 && matrix[r][c + run_len] == cidx) {
                    run_len++;
                }
                int px = screen_x + c;
                if (px + run_len > 0 && px < SCR_W) {
                    int draw_x = px < 0 ? 0 : px;
                    int draw_len = (px + run_len > SCR_W) ? (SCR_W - draw_x) : (px + run_len - draw_x);
                    if (draw_len > 0) {
                        const RgbColor &col = SPR_PALETTE[cidx % 8];
                        SDL_SetRenderDrawColor(ren, col.r, col.g, col.b, 255);
                        if (draw_len == 1) {
                            SDL_RenderDrawPoint(ren, draw_x, py);
                        } else {
                            SDL_Rect rc = {draw_x, py, draw_len, 1};
                            SDL_RenderFillRect(ren, &rc);
                        }
                    }
                }
                c += run_len;
            }
        }
    } else {
        // Rotated 16x16 sprite centered at (screen_x + 7.5, screen_y + 7.5)
        float rad = angle_deg * (3.14159265f / 180.0f);
        float cos_a = cosf(rad);
        float sin_a = sinf(rad);
        float cx = (float)screen_x + 7.5f;
        float cy = (float)screen_y + 7.5f;

        for (int r = 0; r < 16; r++) {
            float dy = (float)r - 7.5f;
            for (int c = 0; c < 16; c++) {
                uint8_t cidx = matrix[r][c];
                if (cidx == 0) continue;
                float dx = (float)c - 7.5f;
                int rx = (int)(cx + dx * cos_a - dy * sin_a + 0.5f);
                int ry = (int)(cy + dx * sin_a + dy * cos_a + 0.5f);
                if (rx >= 0 && rx < SCR_W && ry >= 0 && ry < SCR_H) {
                    const RgbColor &col = SPR_PALETTE[cidx % 8];
                    SDL_SetRenderDrawColor(ren, col.r, col.g, col.b, 255);
                    SDL_RenderDrawPoint(ren, rx, ry);
                }
            }
        }
    }
}

inline void draw_dual_fighter(SDL_Renderer *ren, int screen_x, int screen_y) {
    draw_sprite_16x16(ren, screen_x, screen_y, SPR_FIGHTER, 0.0f);
    draw_sprite_16x16(ren, screen_x + 16, screen_y, SPR_FIGHTER, 0.0f);
}

inline void draw_explosion(SDL_Renderer *ren, int screen_x, int screen_y, int frame, bool is_player = false) {
    if (frame < 0 || frame > 3) return;
    const uint8_t (*matrices[4])[16] = {SPR_EXP_0, SPR_EXP_1, SPR_EXP_2, SPR_EXP_3};
    if (is_player) {
        // Double-scale player explosion
        const uint8_t (*mat)[16] = matrices[frame];
        for (int r = 0; r < 16; r++) {
            for (int c = 0; c < 16; c++) {
                uint8_t cidx = mat[r][c];
                if (cidx == 0) continue;
                const RgbColor &col = SPR_PALETTE[cidx % 8];
                SDL_SetRenderDrawColor(ren, col.r, col.g, col.b, 255);
                SDL_Rect rc = {screen_x - 4 + c * 2, screen_y - 4 + r * 2, 2, 2};
                if (rc.x >= 0 && rc.x + rc.w <= SCR_W && rc.y >= 0 && rc.y + rc.h <= SCR_H) {
                    SDL_RenderFillRect(ren, &rc);
                }
            }
        }
    } else {
        draw_sprite_16x16(ren, screen_x, screen_y, matrices[frame], 0.0f);
    }
}

/* ─── STARFIELD ENGINE ───────────────────────────────────────────────── */
#define MAX_STARS 72

struct Star {
    float x, y;
    float speed;
    int color_cycle;
    uint8_t base_color;
};

class Starfield {
public:
    Star stars[MAX_STARS];
    bool warp_speed;

    void init() {
        warp_speed = false;
        for (int i = 0; i < MAX_STARS; i++) {
            stars[i].x = (float)(rand() % PLAY_W);
            stars[i].y = (float)(rand() % PLAY_H);
            int layer = rand() % 3;
            if (layer == 0) stars[i].speed = 0.5f;
            else if (layer == 1) stars[i].speed = 1.0f;
            else stars[i].speed = 1.8f;
            stars[i].color_cycle = rand() % 60;
            stars[i].base_color = rand() % 4;
        }
    }

    void update(float dt) {
        float speed_mult = warp_speed ? 6.0f : 1.0f;
        for (int i = 0; i < MAX_STARS; i++) {
            stars[i].y += stars[i].speed * speed_mult * 60.0f * dt;
            if (stars[i].y >= PLAY_H) {
                stars[i].y -= PLAY_H;
                stars[i].x = (float)(rand() % PLAY_W);
            }
            stars[i].color_cycle++;
        }
    }

    void render(SDL_Renderer *ren) {
        for (int i = 0; i < MAX_STARS; i++) {
            int sx = PLAY_X + (int)stars[i].x;
            int sy = PLAY_Y + (int)stars[i].y;

            // Twinkle / color cycle
            int cphase = (stars[i].color_cycle / 15 + stars[i].base_color) % 4;
            RgbColor c = C_WHITE;
            switch (cphase) {
                case 0: c = C_WHITE; break;
                case 1: c = C_CYAN; break;
                case 2: c = C_YELLOW; break;
                case 3: c = C_BLUE; break;
                default: c = C_WHITE; break;
            }

            SDL_SetRenderDrawColor(ren, c.r, c.g, c.b, 255);
            SDL_RenderDrawPoint(ren, sx, sy);
            if (stars[i].speed > 1.2f) {
                // Brighter star has 2 pixels
                SDL_RenderDrawPoint(ren, sx, sy + 1);
            }
        }
    }
};

/* ─── TRACTOR BEAM PROCEDURAL RENDERER ───────────────────────────────── */
inline void render_tractor_beam(SDL_Renderer *ren, float top_x, float top_y, float length, int frame) {
    if (length <= 4.0f) return;

    // Expanding trapezoidal ripple cone
    float bottom_y = top_y + length;
    float max_half_w = 42.0f * (length / 150.0f);
    if (max_half_w > 42.0f) max_half_w = 42.0f;

    // Animated ripple rings
    int num_rings = (int)(length / 12.0f);
    if (num_rings < 2) num_rings = 2;

    for (int i = 0; i < num_rings; i++) {
        float frac = (float)i / (float)num_rings;
        float y = top_y + frac * length;
        float half_w = 4.0f + frac * (max_half_w - 4.0f);

        // Oscillating ripple offset
        float wave = sinf((float)frame * 0.3f + frac * 8.0f) * 3.0f;
        half_w += wave;

        // Alternating cyan/blue/white energy stripes
        int col_idx = (i + frame / 3) % 3;
        if (col_idx == 0) SDL_SetRenderDrawColor(ren, 64, 224, 255, 220); // Cyan
        else if (col_idx == 1) SDL_SetRenderDrawColor(ren, 40, 100, 255, 200); // Blue
        else SDL_SetRenderDrawColor(ren, 255, 255, 255, 240); // White core

        // Draw horizontal energy arc
        int rx1 = PLAY_X + (int)(top_x - half_w);
        int rx2 = PLAY_X + (int)(top_x + half_w);
        int ry = PLAY_Y + (int)y;
        SDL_RenderDrawLine(ren, rx1, ry, rx2, ry);

        // Draw side boundary beams
        SDL_SetRenderDrawColor(ren, 120, 240, 255, 255);
        SDL_RenderDrawPoint(ren, rx1 - 1, ry);
        SDL_RenderDrawPoint(ren, rx2 + 1, ry);
    }

    // Outer boundary connecting rays
    SDL_SetRenderDrawColor(ren, 0, 200, 255, 255);
    SDL_RenderDrawLine(ren, PLAY_X + (int)top_x - 3, PLAY_Y + (int)top_y,
                            PLAY_X + (int)(top_x - max_half_w), PLAY_Y + (int)bottom_y);
    SDL_RenderDrawLine(ren, PLAY_X + (int)top_x + 3, PLAY_Y + (int)top_y,
                            PLAY_X + (int)(top_x + max_half_w), PLAY_Y + (int)bottom_y);
}

#endif // SPRITES_HPP
