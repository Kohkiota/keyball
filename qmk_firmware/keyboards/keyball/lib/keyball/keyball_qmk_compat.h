/*
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

//////////////////////////////////////////////////////////////////////////////
// QMK keycode name compatibility
//
// Keyball's sources were written against QMK 0.22.x.  Newer QMK renamed some
// keycode aliases, but *not* their numeric values.  The aliases below keep the
// existing Keyball keymaps compiling unchanged, and because the numeric values
// are identical, keymaps already stored in EEPROM by VIA / Remap keep working.
//
//   KC_MS_BTNn / KC_BTNn -> MS_BTNn     (renamed in QMK 0.24.0)
//   RGB_xxx              -> UG_xxx      (renamed in QMK 0.28.0)
//
// Note: RGB_M_* (RGBLIGHT mode selection) aliases still exist upstream, so
// they are intentionally not listed here.

// Mouse buttons.
#define KC_MS_BTN1 MS_BTN1
#define KC_MS_BTN2 MS_BTN2
#define KC_MS_BTN3 MS_BTN3
#define KC_MS_BTN4 MS_BTN4
#define KC_MS_BTN5 MS_BTN5
#define KC_MS_BTN6 MS_BTN6
#define KC_MS_BTN7 MS_BTN7
#define KC_MS_BTN8 MS_BTN8

#define KC_BTN1 MS_BTN1
#define KC_BTN2 MS_BTN2
#define KC_BTN3 MS_BTN3
#define KC_BTN4 MS_BTN4
#define KC_BTN5 MS_BTN5
#define KC_BTN6 MS_BTN6
#define KC_BTN7 MS_BTN7
#define KC_BTN8 MS_BTN8

// RGBLIGHT (underglow).
#define RGB_TOG  UG_TOGG
#define RGB_MOD  UG_NEXT
#define RGB_RMOD UG_PREV
#define RGB_HUI  UG_HUEU
#define RGB_HUD  UG_HUED
#define RGB_SAI  UG_SATU
#define RGB_SAD  UG_SATD
#define RGB_VAI  UG_VALU
#define RGB_VAD  UG_VALD
#define RGB_SPI  UG_SPDU
#define RGB_SPD  UG_SPDD
