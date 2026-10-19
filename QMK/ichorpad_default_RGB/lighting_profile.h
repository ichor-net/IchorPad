// Hardcoded lighting profile for IchorPad (15x SK6812 Mini-E).
// All tunable values live here.
//
// LED wiring order (0-based index = position in the data chain):
//   0-2   under encoder 0 (left knob in the vertical orientation)
//   3-5   under encoder 1 (right knob)
//   6-14  key LEDs, serpentine: row 0 right-to-left, row 1 left-to-right,
//         row 2 right-to-left (see rgb_matrix.layout in keyboard.json)
#pragma once
#include "quantum.h"

#define PROFILE_LAYERS 4     // VIA's default layer count
#define BG_VAL         70    // key idle brightness (0-255)
#define ACTIVE_VAL     200   // pressed / cursor peak brightness (0-255)
#define ENC_IDLE_VAL   50    // encoder LEDs idle brightness (they show the layer colour)
#define FADE_MS        200   // key press and encoder click fade-out time
#define ENC_SHOW_MS    200   // encoder cursor fade-out time
#define WHITE { 0, 0 }

typedef struct { uint8_t h, s; } hs_t;
#define HS(h, s) { h, s }

// Key background colour per layer, per key [matrix row][matrix col].
// QMK hue scale: 0 = red, 85 = green, 170 = blue.
static const hs_t PROFILE_BG[PROFILE_LAYERS][3][3] = {
    { { HS(116, 99), HS(116, 99), HS(116, 99) }, { HS(116, 99), HS(116, 99), HS(116, 99) }, { HS(116, 99), HS(116, 99), HS(116, 99) } }, // layer 0: pastel teal
    { { HS(191, 99),  HS(191, 99),  HS(191, 99)  }, { HS(191, 99),  HS(191, 99),  HS(191, 99) }, { HS(191, 99),  HS(191, 99), HS(191, 99) } }, // layer 1: green
    { { HS(30, 99),  HS(30, 99),  HS(30, 99)  }, { HS(30, 99),  HS(30, 99),  HS(30, 99)  }, { HS(30, 99),  HS(30, 99),  HS(30, 99)  } }, // layer 2: orange
    { { WHITE, WHITE, WHITE }, { WHITE, WHITE, WHITE }, { WHITE, WHITE, WHITE } }, // layer 3: white
};

// Active-layer colour shown on all six encoder LEDs.
static const hs_t PROFILE_LAYER_COLOR[PROFILE_LAYERS] = { HS(116, 99), HS(191, 99), HS(30, 99), WHITE };

#define ENC_LED_COUNT 3
// LED indices under each encoder; the cursor steps through them in this order.
static const uint8_t ENC_LEDS[2][ENC_LED_COUNT] = { { 0, 1, 2 }, { 3, 4, 5 } };

// Encoder push-switch matrix position (row 3), so a click can flash its LEDs.
#define ENC_CLICK_ROW 3
static const uint8_t ENC_CLICK_COL[2] = { 0, 2 };
