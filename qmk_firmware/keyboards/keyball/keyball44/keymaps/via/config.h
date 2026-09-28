/*
This is the c configuration file for the keymap

Copyright 2022 @Yowkees
Copyright 2022 MURAOKA Taro (aka KoRoN, @kaoriya)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#ifdef RGBLIGHT_ENABLE
//#    define RGBLIGHT_EFFECT_BREATHING
//#    define RGBLIGHT_EFFECT_RAINBOW_MOOD
//#    define RGBLIGHT_EFFECT_RAINBOW_SWIRL
//#    define RGBLIGHT_EFFECT_SNAKE
//#    define RGBLIGHT_EFFECT_KNIGHT
//#    define RGBLIGHT_EFFECT_CHRISTMAS
#    define RGBLIGHT_EFFECT_STATIC_GRADIENT
//#    define RGBLIGHT_EFFECT_RGB_TEST
//#    define RGBLIGHT_EFFECT_ALTERNATING
//#    define RGBLIGHT_EFFECT_TWINKLE
#endif

#define TAP_CODE_DELAY 5

#define POINTING_DEVICE_AUTO_MOUSE_ENABLE
#define AUTO_MOUSE_DEFAULT_LAYER 1

#define DYNAMIC_KEYMAP_LAYER_COUNT 6

//////////////////////////////////////////////////////////////////////////////
// Cyclotab (getreuer/cyclotab community module) — Alt+Tab swapper
//
// Triggered by A(KC_TAB) (next window) and S(A(KC_TAB)) (previous window),
// assigned from Remap / VIA.  Alt stays held after the key is released so the
// taps can be chained, and is released after this timeout (milliseconds).
//
// <<< Cyclotab timeout setting >>>
#define CYCLOTAB_TIMEOUT 2000

//////////////////////////////////////////////////////////////////////////////
// Win + arrow swapper (keymap.c)
//
// Triggered by LGUI(KC_LEFT) / LGUI(KC_RGHT) / LGUI(KC_UP) / LGUI(KC_DOWN),
// assigned from Remap / VIA.  Win stays held after the key is released so the
// taps can be chained, and is released after this timeout (milliseconds) or as
// soon as any other key is pressed.
//
// <<< Win swapper timeout setting >>>
#define WINSWAP_TIMEOUT 2000

#define TAPPING_TERM 145

// ホールド後でも他キー入力が無ければタップに戻す
// #define RETRO_TAPPING

// 他のキーが押された瞬間ホールド
// #define PERMISSIVE_HOLD

// 他のキーが押されている状態で離した時ホールド
// #define HOLD_ON_OTHER_KEY_PRESS