
// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later
//
// DISPLAY TEST keymap: draws a 4x3 key grid and two encoder dials on a 128x32 OLED.
// No raw HID in this one; it's just for getting the screen reacting.
//
// rules.mk needs:  OLED_ENABLE = yes   (RAW_HID_ENABLE is not needed yet)
 
#include QMK_KEYBOARD_H
#include <stdlib.h>
 
// ---------- tweakable constants ----------
#define ENCODER_STEPS_PER_REV 24   // detents per 360 degree turn of the encoder
#define DIAL_COUNT            2    // number of encoders to draw
 
#define GRID_ROWS 4
#define GRID_COLS 3
#define CELL_SIZE 7                // key cell is CELL_SIZE x CELL_SIZE pixels
#define CELL_PITCH 8               // distance between cells (cell + 1px gap)
#define GRID_X    2                // left edge of the grid
 
#define DIAL_RADIUS 14
#define DIAL_CY     16
static const uint8_t dial_cx[DIAL_COUNT] = {60, 108};   // centre x of each dial
 
// ---------- keymap (your original) ----------
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /*
        |MUTE|   |NO|
            |7|8|9|
            |4|5|6|
            |1|2|3|
    */
 
    [0] = LAYOUT_ortho_4x3(
        KC_P7,   KC_P8,   KC_P9,
        KC_P4,   KC_P5,   KC_P6,
        KC_P1,   KC_P2,   KC_P3,
        KC_MUTE, KC_NO
    )
};
 
#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [0] = { ENCODER_CCW_CW(KC_VOLU, KC_VOLD), ENCODER_CCW_CW(MS_WHLU, MS_WHLD)},
};
#endif
 
// ---------- input state ----------
static bool    key_down[GRID_ROWS][GRID_COLS];
static uint8_t enc_pos[DIAL_COUNT];          // 0 .. ENCODER_STEPS_PER_REV-1
static bool    redraw = true;                // only redraw when something changed
 
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    uint8_t row = record->event.key.row;
    uint8_t col = record->event.key.col;
    // Encoder-map events report out-of-range rows, so this bounds check skips them.
    if (row < GRID_ROWS && col < GRID_COLS) {
        key_down[row][col] = record->event.pressed;
        redraw = true;
    }
    return true;
}
 
#if defined(ENCODER_ENABLE)
bool encoder_update_user(uint8_t index, bool clockwise) {
    if (index < DIAL_COUNT) {
        enc_pos[index] = (enc_pos[index] + (clockwise ? 1 : ENCODER_STEPS_PER_REV - 1)) % ENCODER_STEPS_PER_REV;
        redraw = true;
    }
    return true;   // keep the encoder_map behaviour (volume / scroll)
}
#endif
 
// ===== drawing begin (only depends on oled_write_pixel / oled_clear) =====
 
// sin(0..90 degrees) * 127, integer-only so we avoid pulling in floating point on AVR
static const uint8_t PROGMEM sin_q[91] = {
      0,   2,   4,   7,   9,  11,  13,  15,  18,  20,  22,  24,  26,
     29,  31,  33,  35,  37,  39,  41,  43,  46,  48,  50,  52,  54,
     56,  58,  60,  62,  63,  65,  67,  69,  71,  73,  75,  76,  78,
     80,  82,  83,  85,  87,  88,  90,  91,  93,  94,  96,  97,  99,
    100, 101, 103, 104, 105, 107, 108, 109, 110, 111, 112, 113, 114,
    115, 116, 117, 118, 119, 119, 120, 121, 121, 122, 123, 123, 124,
    124, 125, 125, 125, 126, 126, 126, 127, 127, 127, 127, 127, 127
};
 
static int16_t isin(uint16_t deg) {          // -127..127
    int16_t sign = 1;
    deg %= 360;
    if (deg >= 180) { sign = -1; deg -= 180; }
    if (deg > 90) deg = 180 - deg;
    return sign * (int16_t)pgm_read_byte(&sin_q[deg]);
}
 
static int16_t icos(uint16_t deg) { return isin(deg + 90); }
 
static void px(int16_t x, int16_t y) {
    if (x < 0 || y < 0 || x >= OLED_DISPLAY_WIDTH || y >= OLED_DISPLAY_HEIGHT) return;
    oled_write_pixel(x, y, true);
}
 
static void draw_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1) {
    int16_t dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int16_t dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int16_t err = dx + dy;
    for (;;) {
        px(x0, y0);
        if (x0 == x1 && y0 == y1) break;
        int16_t e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}
 
static void draw_circle(int16_t cx, int16_t cy, int16_t r) {
    int16_t x = r, y = 0, d = 1 - r;
    while (x >= y) {
        px(cx + x, cy + y); px(cx + y, cy + x); px(cx - y, cy + x); px(cx - x, cy + y);
        px(cx - x, cy - y); px(cx - y, cy - x); px(cx + y, cy - x); px(cx + x, cy - y);
        y++;
        if (d <= 0) {
            d += 2 * y + 1;
        } else {
            x--;
            d += 2 * (y - x) + 1;
        }
    }
}
 
static void draw_cell(int16_t x, int16_t y, bool filled) {
    for (int16_t j = 0; j < CELL_SIZE; j++) {
        for (int16_t i = 0; i < CELL_SIZE; i++) {
            bool edge = (i == 0 || j == 0 || i == CELL_SIZE - 1 || j == CELL_SIZE - 1);
            if (filled || edge) px(x + i, y + j);
        }
    }
}
 
static void draw_dial(int16_t cx, int16_t cy, uint8_t pos) {
    draw_circle(cx, cy, DIAL_RADIUS);
    // 0 steps = pointing straight up, increasing clockwise
    uint16_t deg = ((uint32_t)pos * 360) / ENCODER_STEPS_PER_REV;
    int16_t len = DIAL_RADIUS - 2;
    int16_t tx = cx + (len * isin(deg)) / 127;
    int16_t ty = cy - (len * icos(deg)) / 127;
    draw_line(cx, cy, tx, ty);
}
 
static void draw_all(void) {
    oled_clear();
    for (uint8_t r = 0; r < GRID_ROWS; r++) {
        for (uint8_t c = 0; c < GRID_COLS; c++) {
            draw_cell(GRID_X + c * CELL_PITCH, r * CELL_PITCH, key_down[r][c]);
        }
    }
    for (uint8_t i = 0; i < DIAL_COUNT; i++) {
        draw_dial(dial_cx[i], DIAL_CY, enc_pos[i]);
    }
}
 
// ===== drawing end =====
 
#if defined(OLED_ENABLE)
bool oled_task_user(void) {
    if (redraw) {
        redraw = false;
        draw_all();
    }
    return false;
}
#endif