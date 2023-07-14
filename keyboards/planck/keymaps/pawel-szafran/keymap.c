/* Copyright 2023 Pawel Szafran
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include QMK_KEYBOARD_H

#include "oneshot.h"
#include "swapper.h"

#define HOME G(KC_LEFT)
#define END G(KC_RGHT)
#define FWD G(KC_RBRC)
#define BACK G(KC_LBRC)
#define TABL G(S(KC_LBRC))
#define TABR G(S(KC_RBRC))
#define PANL C(S(KC_LBRC))
#define PANR C(S(KC_RBRC))
#define SFT_TAB S(KC_TAB)
#define SW_WIN G(KC_GRV)
#define LA_SYM MO(SYM)
#define LA_NAV MO(NAV)

enum planck_layers {
  DEF,
  SYM,
  NAV,
  NUM
};

enum planck_keycodes {
  // Custom oneshot mod implementation with no timers.
  OS_SHFT = SAFE_RANGE,
  OS_CTRL,
  OS_ALT,
  OS_CMD,

  SW_APP,  // Switch to next app (cmd-tab)

  EX_LA,
  EX_RA,
  EX_RFA,
  EX_PIPE,
  EX_INS,
  EX_W,
  EX_S,
  EX_DEF,
};

#define LOWER MO(_LOWER)
#define RAISE MO(_RAISE)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [DEF] = LAYOUT_planck_grid(
      KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    XXXXXXX, XXXXXXX, KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,   
      KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    XXXXXXX, XXXXXXX, KC_H,    KC_J,    KC_K,    KC_L,    KC_QUOT,
      KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    XXXXXXX, XXXXXXX, KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,
      XXXXXXX, XXXXXXX, KC_ESC,  LA_NAV,  KC_LSFT, XXXXXXX, XXXXXXX, KC_SPC,  LA_SYM,  KC_ENT,  XXXXXXX, XXXXXXX 
  ),

  [SYM] = LAYOUT_planck_grid(
      KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC, XXXXXXX, XXXXXXX, KC_CIRC, KC_AMPR, KC_ASTR, KC_SCLN, XXXXXXX,
      KC_TILD, KC_MINS, KC_UNDS, KC_COLN, KC_EQL,  XXXXXXX, XXXXXXX, KC_PLUS, OS_CMD,  OS_ALT,  OS_CTRL, OS_SHFT,
      KC_GRV,  KC_LBRC, KC_LCBR, KC_LPRN, KC_PIPE, XXXXXXX, XXXXXXX, XXXXXXX, KC_RPRN, KC_RCBR, KC_RBRC, KC_BSLS,
      XXXXXXX, XXXXXXX, _______, _______, _______, XXXXXXX, XXXXXXX, _______, _______, _______, XXXXXXX, XXXXXXX 
  ),

  [NAV] = LAYOUT_planck_grid(
      PANL,    PANR,    TABL,    TABR,    KC_VOLU, XXXXXXX, XXXXXXX, SW_APP,  KC_TAB,  SFT_TAB, SW_WIN,  KC_DEL,
      OS_SHFT, OS_CTRL, OS_ALT,  OS_CMD,  KC_VOLD, XXXXXXX, XXXXXXX, KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, CW_TOGG,
      QK_BOOT, XXXXXXX, BACK,    FWD,     KC_MPLY, XXXXXXX, XXXXXXX, HOME,    KC_PGDN, KC_PGUP, END,     XXXXXXX,
      XXXXXXX, XXXXXXX, _______, _______, _______, XXXXXXX, XXXXXXX, KC_BSPC, _______, _______, XXXXXXX, XXXXXXX 
  ),

  [NUM] = LAYOUT_planck_grid(
      KC_7,    KC_5,    KC_3,    KC_1,    KC_9,    XXXXXXX, XXXXXXX, KC_8,    KC_0,    KC_2,    KC_4,    KC_6,
      OS_SHFT, OS_CTRL, OS_ALT,  OS_CMD,  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, OS_CMD,  OS_ALT,  OS_CTRL, OS_SHFT,
      EX_LA,   EX_RA,   EX_RFA,  EX_PIPE, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, EX_INS,  EX_W,    EX_S,    EX_DEF,
      XXXXXXX, XXXXXXX, _______, _______, _______, XXXXXXX, XXXXXXX, _______, _______, _______, XXXXXXX, XXXXXXX 
  )
};

bool is_oneshot_cancel_key(uint16_t keycode) {
    switch (keycode) {
    case LA_SYM:
    case LA_NAV:
        return true;
    default:
        return false;
    }
}

bool is_oneshot_ignored_key(uint16_t keycode) {
    switch (keycode) {
    case LA_SYM:
    case LA_NAV:
    case KC_LSFT:
    case OS_SHFT:
    case OS_CTRL:
    case OS_ALT:
    case OS_CMD:
        return true;
    default:
        return false;
    }
}

bool sw_app_active = false;

oneshot_state os_shft_state = os_up_unqueued;
oneshot_state os_ctrl_state = os_up_unqueued;
oneshot_state os_alt_state = os_up_unqueued;
oneshot_state os_cmd_state = os_up_unqueued;

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    update_swapper(
        &sw_app_active, KC_LGUI, KC_TAB, SW_APP,
        keycode, record
    );

    update_oneshot(
        &os_shft_state, KC_LSFT, OS_SHFT,
        keycode, record
    );
    update_oneshot(
        &os_ctrl_state, KC_LCTL, OS_CTRL,
        keycode, record
    );
    update_oneshot(
        &os_alt_state, KC_LALT, OS_ALT,
        keycode, record
    );
    update_oneshot(
        &os_cmd_state, KC_LCMD, OS_CMD,
        keycode, record
    );

    switch (keycode) {
    case EX_LA:
        if (record->event.pressed) {
            SEND_STRING(" <- ");
        }
        break;
    case EX_RA:
        if (record->event.pressed) {
            SEND_STRING(" -> ");
        }
        break;
    case EX_RFA:
        if (record->event.pressed) {
            SEND_STRING(" => ");
        }
        break;
    case EX_PIPE:
        if (record->event.pressed) {
            SEND_STRING("|> ");
        }
        break;
    case EX_INS:
        if (record->event.pressed) {
            SEND_STRING("|s if\n");
        }
        break;
    case EX_W:
        if (record->event.pressed) {
            SEND_STRING("~w(");
        }
        break;
    case EX_S:
        if (record->event.pressed) {
            SEND_STRING("~s<");
        }
        break;
    case EX_DEF:
        if (record->event.pressed) {
            SEND_STRING(" \\\\ ");
        }
        break;
    }

    return true;
}

layer_state_t layer_state_set_user(layer_state_t state) {
    return update_tri_layer_state(state, SYM, NAV, NUM);
}

