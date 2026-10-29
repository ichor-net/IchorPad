// Renders the hardcoded lighting profile from lighting_profile.h.
#include "quantum.h"
#include "lighting_profile.h"

static uint32_t hit_time[MATRIX_ROWS][MATRIX_COLS];   // last press time per switch, 0 = idle
static uint32_t enc_led_time[2][ENC_LED_COUNT];       // last cursor hit per encoder LED, 0 = idle
static uint8_t  enc_pos[2];                           // encoder cursor position

static void set_hsv(uint8_t led, uint8_t h, uint8_t s, uint8_t v) {
    RGB rgb = hsv_to_rgb((HSV){h, s, v}); // go through HSV so brightness is capped
    rgb_matrix_set_color(led, rgb.r, rgb.g, rgb.b);
}

// Brightness for something that was triggered at *t: ACTIVE_VAL fading to `base`
// over `span` ms. Clears the timer once it has finished.
static uint8_t pulse(uint32_t *t, uint32_t span, uint8_t base) {
    if (!*t) return base;
    uint32_t age = timer_elapsed32(*t);
    if (age >= span) { *t = 0; return base; }
    return (uint8_t)(ACTIVE_VAL + ((int32_t)base - (int32_t)ACTIVE_VAL) * (int32_t)age / (int32_t)span);
}

static bool in_range(uint8_t led, uint8_t lo, uint8_t hi) {
    return led != NO_LED && led >= lo && led < hi;
}

bool rgb_matrix_indicators_advanced_kb(uint8_t led_min, uint8_t led_max) {
    if (!rgb_matrix_indicators_advanced_user(led_min, led_max)) return false;

#ifdef PROFILE_DEBUG_ORDER
    // Wiring check: each LED shows a different hue by its index.
    for (uint8_t i = led_min; i < led_max; i++) set_hsv(i, (uint8_t)(i * 17), 255, 80);
    return false;
#endif

    uint8_t layer = get_highest_layer(layer_state);
    if (layer >= PROFILE_LAYERS) layer = PROFILE_LAYERS - 1;

    // 1. Key LEDs: per-layer background colour, flash on press
    for (uint8_t r = 0; r < 3; r++) {
        for (uint8_t c = 0; c < 3; c++) {
            uint8_t led = g_led_config.matrix_co[r][c];
            if (!in_range(led, led_min, led_max)) continue;
            hs_t col = PROFILE_BG[layer][r][c];
            set_hsv(led, col.h, col.s, pulse(&hit_time[r][c], FADE_MS, BG_VAL));
        }
    }

    // 2. Encoder LEDs: show the active layer colour; a bright cursor steps
    //    through them when the knob turns (leaving a fading trail), and the
    //    whole group flashes when the knob is clicked.
    hs_t lc = PROFILE_LAYER_COLOR[layer];
    for (uint8_t e = 0; e < 2; e++) {
        for (uint8_t s = 0; s < ENC_LED_COUNT; s++) {
            uint8_t led = ENC_LEDS[e][s];
            if (!in_range(led, led_min, led_max)) continue;
            uint8_t v     = pulse(&enc_led_time[e][s], ENC_SHOW_MS, ENC_IDLE_VAL);
            uint8_t click = pulse(&hit_time[ENC_CLICK_ROW][ENC_CLICK_COL[e]], FADE_MS, ENC_IDLE_VAL);
            set_hsv(led, lc.h, lc.s, v > click ? v : click);
        }
    }
    return false;
}

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
    if (!process_record_user(keycode, record)) return false;

    // With ENCODER_MAP_ENABLE, QMK delivers each encoder tick as a record
    // (type ENCODER_CW/CCW_EVENT, encoder number in key.col), not through
    // encoder_update_kb, so the cursor is driven from here.
    if (IS_ENCODEREVENT(record->event)) {
        uint8_t index = record->event.key.col;
        if (record->event.pressed && index < 2) {
            bool cw = (record->event.type == ENCODER_CW_EVENT);
            enc_pos[index] = cw ? (uint8_t)((enc_pos[index] + 1) % ENC_LED_COUNT)
                                : (uint8_t)((enc_pos[index] + ENC_LED_COUNT - 1) % ENC_LED_COUNT);
            enc_led_time[index][enc_pos[index]] = timer_read32();
        }
        return true; // let the encoder map (VIA-set keycodes) run as normal
    }

    // Normal key (or encoder click) press: start the flash for that switch.
    if (record->event.pressed &&
        record->event.key.row < MATRIX_ROWS && record->event.key.col < MATRIX_COLS) {
        hit_time[record->event.key.row][record->event.key.col] = timer_read32();
    }
    return true;
}
