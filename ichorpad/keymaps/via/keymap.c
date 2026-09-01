// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /*
     * ┌───┬───┬───┬
     * │ 9 │ 6 │ 3 │
     * ├───┼───┼───┼
     * │ 8 │ 5 │ 2 │
     * ├───┼───┼───┼
     * │ 7 │ 4 │ 1 │
     * ├───┼───┼───┼
     * │ 0 │ / │ / │
     * └───┴───┴───┴
     */
    [0] = LAYOUT_ortho_4x3(
        KC_P9,   KC_P6,   KC_P3,
        KC_P8,   KC_P5,   KC_P2,
        KC_P7,   KC_P4,   KC_P1,
        KC_P0
    )
};
