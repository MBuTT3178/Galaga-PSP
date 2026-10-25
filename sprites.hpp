#ifndef SPRITES_HPP
#define SPRITES_HPP

#include <cstdint>
#include <cmath>
#include <cstdlib>
#include <cstring>

/* ─── NATIVE PSP VRAM CONSTANTS & DIMENSIONS ────────────────────────────── */
#define SCR_W 480
#define SCR_H 272
#define BUF_W 512
#define FRAME_BYTES (BUF_W * SCR_H * 4)

#define VRAM_BASE0  0x44000000
#define VRAM_BASE1  0x44088000

/* Vertical 3:4 Arcade Aspect Ratio centered on PSP */
#define PLAY_X 138
#define PLAY_Y 0
#define PLAY_W 204
#define PLAY_H 272

#define BEZEL_L_W 138
#define BEZEL_R_X (PLAY_X + PLAY_W) // 342
#define BEZEL_R_W 138

/* Colors: RGBA 8888 for PSP VRAM */
#define RGBA(r, g, b, a) ((uint32_t)(r) | ((uint32_t)(g) << 8) | ((uint32_t)(b) << 16) | ((uint32_t)(a) << 24))
#define RGB(r, g, b)     RGBA(r, g, b, 255)

static const uint32_t C_BLACK   = RGB(0, 0, 0);
static const uint32_t C_WHITE   = RGB(255, 255, 255);
static const uint32_t C_RED     = RGB(240, 32, 32);
static const uint32_t C_BLUE    = RGB(32, 96, 240);
static const uint32_t C_CYAN    = RGB(32, 224, 240);
static const uint32_t C_YELLOW  = RGB(248, 224, 32);
static const uint32_t C_GREEN   = RGB(32, 208, 64);
static const uint32_t C_MAGENTA = RGB(224, 32, 192);
static const uint32_t C_ORANGE  = RGB(248, 140, 32);
static const uint32_t C_DK_BLUE = RGB(16, 24, 64);
static const uint32_t C_DIM     = RGB(120, 130, 150);
static const uint32_t C_TRANS   = 0;

extern uint32_t *fb_draw;

/* ─── DIRECT HARDWARE RASTER PRIMITIVES ─────────────────────────────────── */
inline void draw_pixel(int x, int y, uint32_t color) {
    if ((unsigned int)x < SCR_W && (unsigned int)y < SCR_H) {
        fb_draw[y * BUF_W + x] = color;
    }
}

inline void draw_fill_rect(int x, int y, int w, int h, uint32_t color) {
    if (x < 0) { w += x; x = 0; }
    if (y < 0) { h += y; y = 0; }
    if (x + w > SCR_W) w = SCR_W - x;
    if (y + h > SCR_H) h = SCR_H - y;
    if (w <= 0 || h <= 0) return;

    for (int r = 0; r < h; r++) {
        uint32_t *row = &fb_draw[(y + r) * BUF_W + x];
        for (int c = 0; c < w; c++) {
            row[c] = color;
        }
    }
}

inline void draw_rect(int x, int y, int w, int h, uint32_t color) {
    draw_fill_rect(x, y, w, 1, color);
    draw_fill_rect(x, y + h - 1, w, 1, color);
    draw_fill_rect(x, y, 1, h, color);
    draw_fill_rect(x + w - 1, y, 1, h, color);
}

inline void draw_line(int x0, int y0, int x1, int y1, uint32_t color) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;
    while (1) {
        draw_pixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

inline void clear_screen() {
    memset(fb_draw, 0, FRAME_BYTES);
}

/* ─── 8x8 BITMAP FONT ───────────────────────────────────────────────────── */
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

inline void draw_text(int x, int y, const char *str, uint32_t color, int scale = 1) {
    for (int i = 0; str[i] != '\0'; i++) {
        char ch = str[i];
        if (ch >= 'a' && ch <= 'z') ch -= 32;
        if (ch < 32 || ch > 95) { x += 8 * scale; continue; }
        const unsigned char *glyph = font8x8_galaga[ch - 32];
        for (int r = 0; r < 8; r++) {
            unsigned char bits = glyph[r];
            for (int col = 0; col < 8; col++) {
                if (bits & (0x80 >> col)) {
                    if (scale == 1) {
                        draw_pixel(x + col, y + r, color);
                    } else {
                        draw_fill_rect(x + col * scale, y + r * scale, scale, scale, color);
                    }
                }
            }
        }
        x += 8 * scale;
    }
}

/* ─── 16x16 SPRITE MATRICES ─────────────────────────────────────────────── */
/* Color indices: 0:Trans, 1:White, 2:Red, 3:Blue, 4:Cyan, 5:Yellow, 6:Green, 7:Orange */

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

// Boss Galaga - Damaged Blue (16x16)
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

// Goei (Red Butterfly) - Frame 0
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

// Goei (Red Butterfly) - Frame 1
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

// Zako (Blue Bee) - Frame 0
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

// Zako (Blue Bee) - Frame 1
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

// Transform Alien / Bosconian Spy
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

// Explosions
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

static const uint32_t SPR_PALETTE[8] = {
    C_TRANS,   // 0
    C_WHITE,   // 1
    C_RED,     // 2
    C_BLUE,    // 3
    C_CYAN,    // 4
    C_YELLOW,  // 5
    C_GREEN,   // 6
    C_ORANGE   // 7
};

inline void draw_sprite_16x16(int screen_x, int screen_y, const uint8_t matrix[16][16], float angle_deg = 0.0f) {
    if (angle_deg == 0.0f) {
        for (int r = 0; r < 16; r++) {
            int py = screen_y + r;
            if ((unsigned int)py >= SCR_H) continue;
            for (int c = 0; c < 16; c++) {
                uint8_t cidx = matrix[r][c];
                if (cidx == 0) continue;
                int px = screen_x + c;
                if ((unsigned int)px < SCR_W) {
                    fb_draw[py * BUF_W + px] = SPR_PALETTE[cidx % 8];
                }
            }
        }
    } else {
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
                if ((unsigned int)rx < SCR_W && (unsigned int)ry < SCR_H) {
                    fb_draw[ry * BUF_W + rx] = SPR_PALETTE[cidx % 8];
                }
            }
        }
    }
}

inline void draw_dual_fighter(int screen_x, int screen_y) {
    draw_sprite_16x16(screen_x, screen_y, SPR_FIGHTER, 0.0f);
    draw_sprite_16x16(screen_x + 16, screen_y, SPR_FIGHTER, 0.0f);
}

inline void draw_explosion(int screen_x, int screen_y, int frame, bool is_player = false) {
    if (frame < 0 || frame > 3) return;
    const uint8_t (*matrices[4])[16] = {SPR_EXP_0, SPR_EXP_1, SPR_EXP_2, SPR_EXP_3};
    if (is_player) {
        const uint8_t (*mat)[16] = matrices[frame];
        for (int r = 0; r < 16; r++) {
            for (int c = 0; c < 16; c++) {
                uint8_t cidx = mat[r][c];
                if (cidx == 0) continue;
                draw_fill_rect(screen_x - 4 + c * 2, screen_y - 4 + r * 2, 2, 2, SPR_PALETTE[cidx % 8]);
            }
        }
    } else {
        draw_sprite_16x16(screen_x, screen_y, matrices[frame], 0.0f);
    }
}

/* ─── STARFIELD ENGINE ──────────────────────────────────────────────────── */
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

    void render() {
        for (int i = 0; i < MAX_STARS; i++) {
            int sx = PLAY_X + (int)stars[i].x;
            int sy = PLAY_Y + (int)stars[i].y;

            int cphase = (stars[i].color_cycle / 15 + stars[i].base_color) % 4;
            uint32_t c = C_WHITE;
            switch (cphase) {
                case 0: c = C_WHITE; break;
                case 1: c = C_CYAN; break;
                case 2: c = C_YELLOW; break;
                case 3: c = C_BLUE; break;
                default: c = C_WHITE; break;
            }

            draw_pixel(sx, sy, c);
            if (stars[i].speed > 1.2f) {
                draw_pixel(sx, sy + 1, c);
            }
        }
    }
};

/* ─── TRACTOR BEAM PROCEDURAL RENDERER ──────────────────────────────────── */
inline void render_tractor_beam(float top_x, float top_y, float length, int frame) {
    if (length <= 4.0f) return;

    float bottom_y = top_y + length;
    float max_half_w = 42.0f * (length / 150.0f);
    if (max_half_w > 42.0f) max_half_w = 42.0f;

    int num_rings = (int)(length / 12.0f);
    if (num_rings < 2) num_rings = 2;

    for (int i = 0; i < num_rings; i++) {
        float frac = (float)i / (float)num_rings;
        float y = top_y + frac * length;
        float half_w = 4.0f + frac * (max_half_w - 4.0f);

        float wave = sinf((float)frame * 0.3f + frac * 8.0f) * 3.0f;
        half_w += wave;

        int col_idx = (i + frame / 3) % 3;
        uint32_t col = C_CYAN;
        if (col_idx == 1) col = C_BLUE;
        else if (col_idx == 2) col = C_WHITE;

        int rx1 = PLAY_X + (int)(top_x - half_w);
        int rx2 = PLAY_X + (int)(top_x + half_w);
        int ry = PLAY_Y + (int)y;
        draw_line(rx1, ry, rx2, ry, col);

        draw_pixel(rx1 - 1, ry, RGB(120, 240, 255));
        draw_pixel(rx2 + 1, ry, RGB(120, 240, 255));
    }

    draw_line(PLAY_X + (int)top_x - 3, PLAY_Y + (int)top_y,
              PLAY_X + (int)(top_x - max_half_w), PLAY_Y + (int)bottom_y, RGB(0, 200, 255));
    draw_line(PLAY_X + (int)top_x + 3, PLAY_Y + (int)top_y,
              PLAY_X + (int)(top_x + max_half_w), PLAY_Y + (int)bottom_y, RGB(0, 200, 255));
}

#endif // SPRITES_HPP
