/*
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

#include QMK_KEYBOARD_H
#include "quantum.h"

#ifdef POINTING_DEVICE_ENABLE
#    include "pointing_device.h"
#endif

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  // keymap for default (VIA)
  [0] = LAYOUT_universal(
    KC_ESC   , KC_Q     , KC_W     , KC_E     , KC_R     , KC_T     ,                                        KC_Y     , KC_U     , KC_I     , KC_O     , KC_P     , KC_DEL   ,
    KC_TAB   , KC_A     , KC_S     , KC_D     , KC_F     , KC_G     ,                                        KC_H     , KC_J     , KC_K     , KC_L     , KC_SCLN  , S(KC_7)  ,
    KC_LSFT  , KC_Z     , KC_X     , KC_C     , KC_V     , KC_B     ,                                        KC_N     , KC_M     , KC_COMM  , KC_DOT   , KC_SLSH  , KC_INT1  ,
              KC_LALT,KC_LGUI,LCTL_T(KC_LNG2)     ,LT(1,KC_SPC),LT(3,KC_LNG1),                  KC_BSPC,LT(2,KC_ENT), RCTL_T(KC_LNG2),     KC_RALT  , KC_PSCR
  ),

  [1] = LAYOUT_universal(
    SSNP_FRE ,  KC_F1   , KC_F2    , KC_F3   , KC_F4    , KC_F5    ,                                         KC_F6    , KC_F7    , KC_F8    , KC_F9    , KC_F10   , KC_F11   ,
    SSNP_VRT ,  _______ , _______  , KC_UP   , KC_ENT   , KC_DEL   ,                                         KC_PGUP  , KC_BTN1  , KC_UP    , KC_BTN2  , KC_BTN3  , KC_F12   ,
    SSNP_HOR ,  _______ , KC_LEFT  , KC_DOWN , KC_RGHT  , KC_BSPC  ,                                         KC_PGDN  , KC_LEFT  , KC_DOWN  , KC_RGHT  , _______  , _______  ,
                  _______  , _______ , _______  ,         _______  , _______  ,                   _______  , _______  , _______       , _______  , _______
  ),

  [2] = LAYOUT_universal(
    _______  ,S(KC_QUOT), KC_7     , KC_8    , KC_9     , S(KC_8)  ,                                         S(KC_9)  , S(KC_1)  , S(KC_6)  , KC_LBRC  , S(KC_4)  , _______  ,
    _______  ,S(KC_SCLN), KC_4     , KC_5    , KC_6     , KC_RBRC  ,                                         KC_NUHS  , KC_MINS  , S(KC_EQL), S(KC_3)  , KC_QUOT  , S(KC_2)  ,
    _______  ,S(KC_MINS), KC_1     , KC_2    , KC_3     ,S(KC_RBRC),                                        S(KC_NUHS),S(KC_INT1), KC_EQL   ,S(KC_LBRC),S(KC_SLSH),S(KC_INT3),
                  KC_0     , KC_DOT  , _______  ,         _______  , _______  ,                   KC_DEL   , _______  , _______       , _______  , _______
  ),

  [3] = LAYOUT_universal(
    RGB_TOG  , AML_TO   , AML_I50  , AML_D50  , _______  , _______  ,                                        RGB_M_P  , RGB_M_B  , RGB_M_R  , RGB_M_SW , RGB_M_SN , RGB_M_K  ,
    RGB_MOD  , RGB_HUI  , RGB_SAI  , RGB_VAI  , _______  , SCRL_DVI ,                                        RGB_M_X  , RGB_M_G  , RGB_M_T  , RGB_M_TW , _______  , _______  ,
    RGB_RMOD , RGB_HUD  , RGB_SAD  , RGB_VAD  , _______  , SCRL_DVD ,                                        CPI_D1K  , CPI_D100 , CPI_I100 , CPI_I1K  , _______  , KBC_SAVE ,
                  QK_BOOT  , KBC_RST  , _______  ,        _______  , _______  ,                   _______  , _______  , _______       , KBC_RST  , QK_BOOT
  ),
};
// clang-format on

#ifdef COMBO_ENABLE
// ホームポジションからマウスボタンを押すためのコンボ。
// 右手 J/K/L と左手 S/D/F の 2 セットを用意し、どちらでも同じボタンを出す。
//   左クリック  : J+K, D+F
//   右クリック  : K+L, S+D
//   中クリック  : J+L, S+F
enum combo_events {
    CMB_JK_BTN1,
    CMB_DF_BTN1,
    CMB_KL_BTN2,
    CMB_SD_BTN2,
    CMB_JL_BTN3,
    CMB_SF_BTN3,
    COMBO_LENGTH,
};
uint16_t COMBO_LEN = COMBO_LENGTH;

const uint16_t PROGMEM combo_jk[] = {KC_J, KC_K, COMBO_END};
const uint16_t PROGMEM combo_df[] = {KC_D, KC_F, COMBO_END};
const uint16_t PROGMEM combo_kl[] = {KC_K, KC_L, COMBO_END};
const uint16_t PROGMEM combo_sd[] = {KC_S, KC_D, COMBO_END};
const uint16_t PROGMEM combo_jl[] = {KC_J, KC_L, COMBO_END};
const uint16_t PROGMEM combo_sf[] = {KC_S, KC_F, COMBO_END};

combo_t key_combos[] = {
    [CMB_JK_BTN1] = COMBO(combo_jk, KC_BTN1),
    [CMB_DF_BTN1] = COMBO(combo_df, KC_BTN1),
    [CMB_KL_BTN2] = COMBO(combo_kl, KC_BTN2),
    [CMB_SD_BTN2] = COMBO(combo_sd, KC_BTN2),
    [CMB_JL_BTN3] = COMBO(combo_jl, KC_BTN3),
    [CMB_SF_BTN3] = COMBO(combo_sf, KC_BTN3),
};
#endif

layer_state_t layer_state_set_user(layer_state_t state) {
    // Auto enable scroll mode when the highest layer is 3
    keyball_set_scroll_mode(get_highest_layer(state) == 3);
    return state;
}

//////////////////////////////////////////////////////////////////////////////
// Swapper
//
// 下記 6 キーを 1 つの状態機械で扱い、修飾キーを押しっぱなしのまま連打
// できるようにする。
//   Alt 系: A(KC_TAB), S(A(KC_TAB))            … タスク切り替え
//   Win 系: G(KC_LEFT/RGHT/UP/DOWN)            … ウィンドウスナップ
//
// 同じ系の中はもちろん、Alt 系 <-> Win 系をまたいでも、最初の 1 打から
// 正常に動く。系をまたぐ時は旧 mod を外してから新 mod を握るので、
// 修飾キーが二重に残ることはない。
// 保持中の mod は、最後にキーを離してから SWAP_TIMEOUT ミリ秒が経過するか、
// 対象外のキーが押された時点で解放する（対象外キー自体は消費しない）。
//
// stock Cyclotab を使わない理由:
//   1. セッション継続の判定 is_trigger_keycode() は「現在の active_key」と
//      「S(active_key)」の 2 つとしか照合しない。CYCLOTAB_KEYS に 4 方向を
//      並べても、G(KC_LEFT) セッション中の G(KC_RGHT) は別キー扱いになる。
//      継続を許す switch の case も素の KC_LEFT/RGHT/UP/DOWN なので、
//      G(KC_RGHT)=0x084F は KC_RGHT=0x004F と一致しない。
//      → Win 系 4 方向の自由往復は構造上不可能。
//   2. セッション中に対象外キーを押すと release_active() のうえ
//      `return !record->event.pressed` で必ず握り潰される。これは
//      process_record_cyclotab() にハードコードされており、公開 API では
//      変えられない（cyclotab.h の cyclotab_clear() は宣言のみで実装が無い）。
//   3. Cyclotab は community module なので process_record_kb/user より先に
//      走る。これを出し抜けるのは pre_process_record_user だけだが、
//      そこで全キーを横取りするのは Cyclotab を通さないのと同じ。
static uint8_t  swap_mods  = 0; // 保持中の mod（0 = セッション無し）
static uint16_t swap_timer = 0; // 0 = 計測停止（キーを押している間）

static uint8_t swap_group(uint16_t keycode) {
    switch (keycode) {
        case A(KC_TAB):
        case S(A(KC_TAB)):
            return MOD_BIT(KC_LALT);
        case G(KC_LEFT):
        case G(KC_RGHT):
        case G(KC_UP):
        case G(KC_DOWN):
            return MOD_BIT(KC_LGUI);
    }
    return 0;
}

static void swap_end(void) {
    if (swap_mods) {
        unregister_mods(swap_mods);
        swap_mods = 0;
    }
    swap_timer = 0;
}

#ifdef POINTING_DEVICE_AUTO_MOUSE_ENABLE
// AML 維持
//
// QMK の process_auto_mouse() は switch の先頭で
//   case QK_MODS ... QK_MODS_MAX: break;
// としているため、A(KC_TAB) や G(KC_LEFT) のような修飾付きキーコードは
// is_mouse_record_kb/user() まで到達しない（0x0100〜0x1FFF が QK_MODS）。
// つまり is_mouse_record_user() にこの 6 キーを並べても呼ばれない。
// 一方で「AML をリセットもしない」ので、解除されないこと自体は素の QMK で
// 既に満たされている。足りないのは 10 秒タイマーの更新のほう。
//
// そこで auto_mouse_keyevent() で mouse_key_tracker を直接握る。
// tracker が非 0 の間は is_auto_mouse_active() が true になり、
// pointing_device_task_auto_mouse() が毎周期 timer.active を打ち直すため、
// セッション中は 10 秒が減らず、解放した時点から 10 秒が再スタートする。
//
// セッションは上記 6 キーを押した時にしか始まらないので、維持対象は
// 実質この 6 キーだけになる。素の KC_TAB や素の矢印は対象外
// （従来どおり auto_mouse_reset_trigger() で AML をリセットする）。
//
// increment / decrement は必ず 1 対 1 に保つ必要があるので、状態が変わった
// 時だけ呼ぶ。
static bool aml_held = false;

static void aml_hold(bool on) {
    if (on != aml_held) {
        aml_held = on;
        auto_mouse_keyevent(on);
    }
}
#endif

void housekeeping_task_user(void) {
    if (swap_timer && timer_elapsed(swap_timer) > SWAP_TIMEOUT) {
        swap_end();
    }
#ifdef POINTING_DEVICE_AUTO_MOUSE_ENABLE
    aml_hold(swap_mods != 0);
#endif
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    // Swapper は既存処理より手前で判定する
    uint8_t want = swap_group(keycode);
    if (want) {
        if (record->event.pressed) {
            if (swap_mods != want) {
                // Alt 系 <-> Win 系 の乗り換え。旧 mod を外してから新 mod を握る
                if (swap_mods) {
                    unregister_mods(swap_mods);
                }
                register_mods(want);
                swap_mods = want;
            }
            swap_timer = 0; // 押している間はタイムアウトを止める
        } else {
            swap_timer = timer_read() | 1; // 離したらタイムアウト計測開始
        }
        // ここは return true。QMK の ACT_LMODS がキーコード側の修飾を
        // weak mods で乗せて送ってくれるので、S(A(KC_TAB)) の Shift も
        // 自動で付く。register_mods() で握った Alt / GUI は real mods なので
        // キーを離す時の del_weak_mods() では消えない。
        return true;
    }
    if (record->event.pressed) {
        swap_end(); // 対象外キーは通す（消費しない）
    }

#ifdef POINTING_DEVICE_AUTO_MOUSE_ENABLE
    // SCRL_TO（Kb 6）を横取りして、スクロールトグルと AML（Auto Mouse Layer）の
    // レイヤー固定を連動させる。
    //   スクロールON  → auto_mouse_toggle() でレイヤーを固定し、タイムアウト解除されないようにする
    //   スクロールOFF → 固定を解除し、auto_mouse_layer_off() で通常のタイムアウト復帰へ戻す
    // 状態ズレ防止のため、get_auto_mouse_toggle() で現在状態を見てから切り替える。
    if (keycode == SCRL_TO && record->event.pressed) {
        bool next = !keyball_get_scroll_mode();

        keyball_set_scroll_mode(next);

        if (next) {
            // スクロールON：レイヤー固定（未固定なら固定する）
            if (!get_auto_mouse_toggle()) {
                auto_mouse_toggle();
            }
        } else {
            // スクロールOFF：固定を解除（固定中なら解除する）してレイヤーを下ろす
            if (get_auto_mouse_toggle()) {
                auto_mouse_toggle();
            }
            auto_mouse_layer_off();
        }

        return false; // Keyball標準の SCRL_TO 処理には渡さない
    }
#endif

    return true;
}

void keyboard_post_init_user(void) {
    // スクロールスナップを必ず Free に初期化する（縦ロック防止）
    keyball_set_scrollsnap_mode(KEYBALL_SCROLLSNAP_MODE_FREE);

#ifdef POINTING_DEVICE_AUTO_MOUSE_ENABLE
    // 起動時に AML（Auto Mouse Layer）を必ず有効化する
    set_auto_mouse_enable(true);

    // AML のタイムアウトを 10 秒にする。
    // keyball.c の keyboard_post_init_kb() が EEPROM 値で
    // set_auto_mouse_timeout() した「後」にこの関数が呼ばれるので、ここでの
    // 上書きが最後に効く。
    // 注意: レイヤー 3 の AML_I50 / AML_D50 を押すと keyball 側の上限
    // （AML_TIMEOUT_MAX = 1000）に丸められる。再起動すれば 10 秒に戻る。
    // <<< AML timeout setting >>>
    set_auto_mouse_timeout(10000);
#endif
}


#ifdef OLED_ENABLE

#    include "lib/oledkit/oledkit.h"

void oledkit_render_info_user(void) {
    keyball_oled_render_keyinfo();
    keyball_oled_render_ballinfo();
    keyball_oled_render_layerinfo();
}
#endif

// スリープ復帰、再起動時のスクロール不安定問題の対処
void suspend_wakeup_init_user(void) {
    // USB / センサー安定待ち（復帰直後対策）
    wait_ms(200);

    // pointing device（Keyball）の再初期化
#ifdef POINTING_DEVICE_ENABLE
    pointing_device_init();
#endif

    // 念のためスクロールスナップも初期化（縦スクロール死対策）
    keyball_set_scrollsnap_mode(KEYBALL_SCROLLSNAP_MODE_FREE);

    // レイヤー状態に応じてスクロールモードを再同期
    layer_state_set_user(layer_state);
}
