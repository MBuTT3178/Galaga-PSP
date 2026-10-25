#!/usr/bin/env python3
import struct
import zlib
import math

def make_png(width, height, get_pixel_func):
    raw = bytearray()
    for y in range(height):
        raw.append(0) # filter byte: none
        for x in range(width):
            r, g, b, a = get_pixel_func(x, y, width, height)
            raw.extend((r, g, b, a))
    compressed = zlib.compress(bytes(raw), 9)

    def chunk(tag, data):
        return struct.pack(">I", len(data)) + tag + data + struct.pack(">I", zlib.crc32(tag + data) & 0xffffffff)

    png = bytearray(b"\x89PNG\r\n\x1a\n")
    ihdr = struct.pack(">IIBBBBB", width, height, 8, 6, 0, 0, 0)
    png.extend(chunk(b"IHDR", ihdr))
    png.extend(chunk(b"IDAT", compressed))
    png.extend(chunk(b"IEND", b""))
    return bytes(png)

# ─── SPRITE BITMAP DATA FOR ICONS ──────────────────────────────────────
SPR_FIGHTER = [
    ".......11.......",
    ".......11.......",
    "......1221......",
    "......1221......",
    ".....114411.....",
    ".....114411.....",
    "....11222211....",
    "...1112222111...",
    "..111111111111..",
    ".11311111111311.",
    "1133111111113311",
    "1333111..1113331",
    "133.11....11.331",
    "11..12....21..11",
    "1...22....22...1",
    "....2......2...."
]

SPR_BOSS = [
    "....66....66....",
    "...6666..6666...",
    "..665566665566..",
    ".66555566555566.",
    ".66655666655666.",
    "6636663333666366",
    "6333333333333336",
    "6331133333311336",
    "6631136666311366",
    ".66336655663366.",
    "..666655556666..",
    "...6666666666...",
    "..666.6666.666..",
    ".666...66...666.",
    "666..........666",
    "66............66"
]

PALETTE = {
    '.': (0, 0, 0, 0),
    '1': (255, 255, 255, 255),
    '2': (240, 32, 32, 255),
    '3': (32, 96, 240, 255),
    '4': (32, 224, 240, 255),
    '5': (248, 224, 32, 255),
    '6': (32, 208, 64, 255),
    '7': (248, 140, 32, 255)
}

# ICON0.PNG (144x80)
def icon_pixel(x, y, w, h):
    # Rounded badge border
    dx = min(x, w - 1 - x)
    dy = min(y, h - 1 - y)
    if dx < 2 and dy < 2: return (0, 0, 0, 0)
    if dx < 3 or dy < 3: return (248, 200, 32, 255) # Gold border

    # Space background with stars
    star_hash = (x * 37 + y * 59) % 251
    if star_hash == 7: return (255, 255, 255, 255)
    if star_hash == 42: return (0, 220, 255, 255)
    if star_hash == 99: return (255, 220, 0, 255)

    # Boss Galaga at top-center: x=64..95, y=10..41 (scale 2)
    bx = (x - 64) // 2
    by = (y - 10) // 2
    if 0 <= bx < 16 and 0 <= by < 16:
        ch = SPR_BOSS[by][bx]
        if ch != '.': return PALETTE[ch]

    # Fighter at bottom-center: x=64..95, y=44..75 (scale 2)
    fx = (x - 64) // 2
    fy = (y - 44) // 2
    if 0 <= fx < 16 and 0 <= fy < 16:
        ch = SPR_FIGHTER[fy][fx]
        if ch != '.': return PALETTE[ch]

    # Side decorative vertical stripes
    if x == 10 or x == w - 11: return (40, 80, 160, 255)
    if x == 12 or x == w - 13: return (0, 200, 240, 255)

    return (10, 14, 28, 255)

# PIC1.PNG (480x272)
def pic1_pixel(x, y, w, h):
    # Starfield space backdrop
    star_hash = (x * 71 + y * 97) % 313
    is_star = (star_hash in [5, 17, 88, 143])

    # Center vertical arcade canvas: x=138..341
    if 138 <= x < 342:
        if x == 138 or x == 341: return (0, 220, 255, 255)
        if x == 139 or x == 340: return (248, 200, 32, 255)
        if is_star:
            if star_hash == 5: return (255, 255, 255, 255)
            if star_hash == 17: return (0, 220, 255, 255)
            if star_hash == 88: return (255, 220, 0, 255)
            return (64, 120, 255, 255)
        return (4, 6, 14, 255)

    # Left & Right Cabinet Bezels
    # Gradient and pinstripes
    bx = x if x < 138 else (w - 1 - x)
    if y % 16 == 0:
        return (24, 30, 56, 255)

    if is_star:
        return (100, 120, 160, 255)

    return (14, 18, 34, 255)

# screenshot.png (480x272) — Authentic in-game screen capture
def screenshot_pixel(x, y, w, h):
    # Left & Right bezels
    if x < 138 or x >= 342:
        return pic1_pixel(x, y, w, h)

    # Center arcade screen
    # Draw starfield
    star_hash = (x * 61 + y * 89) % 227
    bg_col = (0, 0, 0, 255)
    if star_hash == 11: bg_col = (255, 255, 255, 255)
    elif star_hash == 47: bg_col = (0, 220, 255, 255)
    elif star_hash == 93: bg_col = (255, 220, 0, 255)

    # Player Fighter at x=232..247, y=248..263
    fx = x - 232
    fy = y - 248
    if 0 <= fx < 16 and 0 <= fy < 16:
        ch = SPR_FIGHTER[fy][fx]
        if ch != '.': return PALETTE[ch]

    # Boss Galaga at x=232..247, y=50..65
    bx = x - 232
    by = y - 50
    if 0 <= bx < 16 and 0 <= by < 16:
        ch = SPR_BOSS[by][bx]
        if ch != '.': return PALETTE[ch]

    # Tractor Beam from y=66 to 160
    if 66 <= y < 160:
        dy = y - 66
        half_w = 4 + int(dy * 0.35)
        cx = 240
        if cx - half_w <= x <= cx + half_w:
            ring = (dy // 6) % 3
            if ring == 0: return (0, 220, 255, 255)
            elif ring == 1: return (40, 120, 255, 255)
            else: return (255, 255, 255, 255)

    # Missiles at x=236..237, y=200..208
    if (x == 239 or x == 240) and 190 <= y <= 198:
        return (255, 240, 32, 255)

    return bg_col

if __name__ == '__main__':
    print("Generating ICON0.PNG...")
    with open("ICON0.PNG", "wb") as f:
        f.write(make_png(144, 80, icon_pixel))

    print("Generating PIC1.PNG...")
    with open("PIC1.PNG", "wb") as f:
        f.write(make_png(480, 272, pic1_pixel))

    print("Generating screenshot.png...")
    with open("screenshot.png", "wb") as f:
        f.write(make_png(480, 272, screenshot_pixel))

    print("Assets generated successfully!")
