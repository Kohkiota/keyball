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

//////////////////////////////////////////////////////////////////////////////
// スクロール状態
//
// スクロールモードを ON にしたい要求元は 4 つある。
//   scrl_toggled        … Kb6 (SCRL_TO) の完全トグル
//   scrl_mo_held        … Kb7 (SCRL_MO) を物理的に押している
//   scrl_session_active … Kb7 のスクロールセッション継続中（後述）
//   最上位レイヤーが 3  … 既存のレイヤー 3 自動スクロール
//
// この 4 つを keyball_set_scroll_mode() で個別に叩くと壊れる。QMK の
// layer_state_set() には「状態が変わったか」の判定が無いので
// (action_layer.c)、AML の点灯 / 消灯や LT のホールド / 解放のたびに
// layer_state_set_user() が呼ばれる。そこでレイヤーだけを見て上書きすると、
// Kb6 / Kb7 のスクロールが無言で解除されてしまう。
// そこで要求元を OR した結果だけを scroll_sync() で反映し、
// keyball_set_scroll_mode() の呼び出しをこの 1 箇所に集約する。
static bool     scrl_toggled        = false; // Kb6 の完全トグルが ON か
static bool     scrl_mo_held        = false; // Kb7 を物理的に押している
static bool     scrl_session_active = false; // セッション継続中（タイマー稼働中）
static uint16_t scrl_session_timer  = 0;     // swap_timer と同じ理由で sentinel に 0 を使わない

// 現在の要求元からスクロールモードを合成して反映する。
// layer_state_set_user() から呼ぶ場合、グローバルの layer_state はまだ古い値
// なので、必ず引数で渡された state を使うこと。
static void scroll_sync(layer_state_t state) {
    keyball_set_scroll_mode(scrl_toggled || scrl_mo_held || scrl_session_active || get_highest_layer(state) == 3);
}

layer_state_t layer_state_set_user(layer_state_t state) {
    scroll_sync(state);
    return state;
}

//////////////////////////////////////////////////////////////////////////////
// Swapper
//
// 下記 10 キーを 1 つの状態機械で扱い、修飾キーを押しっぱなしのまま連打
// できるようにする。
//   Alt 系:  A(KC_TAB), S(A(KC_TAB))           … タスク切り替え
//            A(KC_LEFT), A(KC_RGHT)            … ブラウザの戻る / 進む
//   Win 系:  G(KC_LEFT/RGHT/UP/DOWN)           … ウィンドウスナップ
//   Ctrl 系: C(KC_TAB), S(C(KC_TAB))           … ブラウザのタブ移動
//
// 同じ系の中はもちろん、Alt 系 <-> Win 系 <-> Ctrl 系をまたいでも、最初の
// 1 打から正常に動く。系をまたぐ時は旧 mod を外してから新 mod を握るので、
// Alt+Ctrl や Alt+Win のように修飾キーが二重に残ることはない。
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
static uint16_t swap_timer = 0;
// タイマー稼働フラグ。0 を「停止」の sentinel に兼用してはいけない。
// timer_read() の戻り値そのものが 0 になりうるうえ、sentinel を避けようと
// `timer_read() | 1` にすると偶数時刻のとき保存値が 1ms 未来になり、
// timer_elapsed() = TIMER_DIFF_16(timer_read(), last) = (uint16_t)(a - b) が
// 同じ 1ms 内で 65535 に回り込んで即 SWAP_TIMEOUT 超過と誤判定される。
static bool swap_timer_running = false;
#ifdef POINTING_DEVICE_AUTO_MOUSE_ENABLE
// セッション開始時に AML が点灯していたか。消灯していたなら AML は維持しない
// （ボールに触っていないのに Alt+Tab だけでマウスレイヤーが点くのを防ぐ）。
static bool swap_aml = false;
#endif

static uint8_t swap_group(uint16_t keycode) {
    switch (keycode) {
        // Alt 系
        case A(KC_TAB):
        case S(A(KC_TAB)):
        case A(KC_LEFT):
        case A(KC_RGHT):
            return MOD_BIT(KC_LALT);
        // Win 系
        case G(KC_LEFT):
        case G(KC_RGHT):
        case G(KC_UP):
        case G(KC_DOWN):
            return MOD_BIT(KC_LGUI);
        // Ctrl 系
        case C(KC_TAB):
        case S(C(KC_TAB)):
            return MOD_BIT(KC_LCTL);
    }
    return 0;
}

static void swap_end(void) {
    if (swap_mods) {
        unregister_mods(swap_mods);
        swap_mods = 0;
    }
    swap_timer_running = false;
#ifdef POINTING_DEVICE_AUTO_MOUSE_ENABLE
    swap_aml = false;
#endif
}

//////////////////////////////////////////////////////////////////////////////
// スクロールセッション（Kb7 = SCRL_MO の拡張）
//
// 従来の Kb7 は「押している間だけスクロール」。ここではそれに加えて、
// 実際にスクロール入力が出たあとに Kb7 を離しても、指を離したまま
// SCROLL_SESSION_TIMEOUT ミリ秒だけスクロールを続けられるようにする。
// スクロール入力があるたびタイムアウトを打ち直すので、途中で少し読んでから
// 再びボールを回してもセッションは継続する。
//
// セッション開始条件を「Kb7 を押した」ではなく「Kb7 を押している間に実際に
// スクロール入力が出た」にしてあるので、通常のポインタ移動と混同しない。
// 検出は pointing_device_task_user() で rep.h / rep.v を見る。keyball は
// pointing_device_task_kb() を定義していないため、この関数には
// pointing_device_driver_get_report() がスクロール量を入れた後のレポートが
// 渡ってくる（pointing_device.c:343 -> :364）。h / v はスクロールモード中しか
// 非 0 にならないので、ポインタ移動では発火しない。
//
// セッションの開始 / 終了は scrl_session_active を動かして scroll_sync() を
// 呼ぶだけでよい。Kb6 のトグル中やレイヤー 3 の自動スクロール中に
// セッションが切れてもスクロールが落ちないのは scroll_sync() が
// 要求元を OR しているため（以前の scrl_session_may_release() は不要）。

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
// セッションは上記 10 キーを押した時にしか始まらないので、維持対象は
// 実質この 10 キーだけになる。素の KC_TAB や素の矢印は対象外
// （従来どおり auto_mouse_reset_trigger() で AML をリセットする）。
//
// ただし tracker を握ると AML は「維持」だけでなく「点灯」もしてしまう
// （pointing_device_task_auto_mouse() が is_auto_mouse_active() を見て
// layer_on する）。ボールに触っていないのに Alt+Tab でマウスレイヤーが
// 点くのを避けるため、swapper 側はセッション開始時に AML が点灯していた
// 場合（swap_aml）だけ維持対象にする。
//
// increment / decrement は必ず 1 対 1 に保つ必要があるので、状態が変わった
// 時だけ呼ぶ。
static bool aml_held = false;

static void aml_hold(bool on) {
    if (on == aml_held) {
        return;
    }
    aml_held = on;
    if (on) {
        auto_mouse_keyevent(true);
        return;
    }
    // レイヤー 3 の AML_I50 / AML_D50 / KBC_RST は set_auto_mouse_timeout() /
    // set_auto_mouse_enable() 経由で auto_mouse_reset() を呼び、tracker を
    // 0 に戻してしまう。そのまま decrement すると tracker が -1 になり、
    // is_auto_mouse_active() は「非 0 なら真」なので（次のキーイベントで
    // クランプされるまで）AML が張り付く。0 のときは触らない。
    if (get_auto_mouse_key_tracker() > 0) {
        auto_mouse_keyevent(false);
    }
}

// AML 上で押しても AML が解除されないキー。keyball.c の is_mouse_record_kb()
// から呼ばれる（同関数は SCRL_MO を先に true にしたうえでここへ落ちてくる）。
//
// これが無いと、これらの素のキーコードは process_auto_mouse() の default: に
// 落ちて auto_mouse_reset_trigger() が layer_off(AML) を実行する。その後
// process_record_handler() -> store_or_get_action() が press 時に
// layer_switch_get_layer() で「AML を落とした後の」layer_state を読むため
// （action.c:293 -> :327, action_layer.c:327）、レイヤー 0 のキーとして
// 解決されてしまう。AML 上の PageUp が base の文字になるのはこれが原因。
//
// true を返すと auto_mouse_reset_trigger() の代わりに auto_mouse_keyevent()
// が呼ばれ、layer_off されず tracker が立つので、押している間は AML が維持され、
// 離せば通常の 10 秒タイマーへ戻る。
//
// ※ ここに他の通常キーを増やさないこと。増やすとそのキーで AML を抜けられなくなる
//   （is_session_break_key() もマウス系キーを除外するので、二重に抜け道が無くなる）。
bool is_mouse_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case KC_PGUP:
        case KC_PGDN:
        case KC_HOME:
        case KC_END:
            return true;
    }
    return false;
}
#endif

// このキーの押下でセッション（swapper / スクロール）を終わらせるか。
// 判定は process_auto_mouse() が AML をリセットするキーの集合、つまり同関数の
// default 分岐に落ちるキーと一致させている。除外するのは:
//   - 素の修飾キー / 修飾付きキーコード … そもそも AML を触らない。
//     Ctrl + スクロールのズームを壊さないためにも必ず除外する
//   - MT / LT のホールド                … mod / レイヤーとしての打鍵
//   - マウス系のキー                    … AML を維持する側のキー
static bool is_session_break_key(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case KC_LEFT_CTRL ... KC_RIGHT_GUI:
        case QK_MODS ... QK_MODS_MAX:
            return false;
#ifndef NO_ACTION_TAPPING
        case QK_MOD_TAP ... QK_MOD_TAP_MAX:
        case QK_LAYER_TAP ... QK_LAYER_TAP_MAX:
            if (!record->tap.count) {
                return false;
            }
            break;
#endif
    }
#ifdef POINTING_DEVICE_AUTO_MOUSE_ENABLE
    // IS_MOUSE_KEYCODE は process_auto_mouse() の is_mouse_record() が使う
    // IS_MOUSEKEY と同一（keycode.h で別名定義されている）。
    if (IS_MOUSE_KEYCODE(keycode) || is_mouse_record_kb(keycode, record)) {
        return false;
    }
#endif
    return true;
}

void housekeeping_task_user(void) {
    if (swap_timer_running && timer_elapsed(swap_timer) > SWAP_TIMEOUT) {
        swap_end();
    }
    // スクロールセッションのタイムアウト。Kb7 を押している間は従来の
    // momentary 動作なので計測しない。
    if (scrl_session_active && !scrl_mo_held && timer_elapsed(scrl_session_timer) > SCROLL_SESSION_TIMEOUT) {
        scrl_session_active = false;
        scroll_sync(layer_state);
    }
#ifdef POINTING_DEVICE_AUTO_MOUSE_ENABLE
    aml_hold((swap_mods != 0 && swap_aml) || scrl_session_active);
#endif
}

// スクロール入力の検出とセッションの延長。
report_mouse_t pointing_device_task_user(report_mouse_t rep) {
    if ((rep.h != 0 || rep.v != 0) && keyball_get_scroll_mode() && (scrl_mo_held || scrl_session_active)) {
        scrl_session_active = true;
        scrl_session_timer  = timer_read();
    }
    return rep;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    // Swapper は既存処理より手前で判定する
    uint8_t want = swap_group(keycode);
    if (want) {
        if (record->event.pressed) {
#ifdef POINTING_DEVICE_AUTO_MOUSE_ENABLE
            if (swap_mods == 0) {
                // AML が点灯している時に始まったセッションだけ AML を維持する
                swap_aml = layer_state_is(AUTO_MOUSE_TARGET_LAYER);
            }
#endif
            if (swap_mods != want) {
                // Alt 系 <-> Win 系 の乗り換え。旧 mod を外してから新 mod を握る
                if (swap_mods) {
                    unregister_mods(swap_mods);
                }
                register_mods(want);
                swap_mods = want;
            }
            swap_timer_running = false; // 押している間はタイムアウトを止める
        } else {
            // 離したらタイムアウト計測開始
            swap_timer         = timer_read();
            swap_timer_running = true;
        }
        // ここは return true。QMK の ACT_LMODS がキーコード側の修飾を
        // weak mods で乗せて送ってくれるので、S(A(KC_TAB)) の Shift も
        // 自動で付く。register_mods() で握った Alt / GUI は real mods なので
        // キーを離す時の del_weak_mods() では消えない。
        return true;
    }

    // SCRL_MO（Kb 7）を横取りしてスクロールセッションを実装する。
    // keyball 標準の処理は `keyball_set_scroll_mode(record->event.pressed)`
    // だけなので、押下側は同じ動作をこちらで行う。離した側だけを変える。
    if (keycode == SCRL_MO) {
        if (record->event.pressed) {
            swap_end(); // swapper の対象外キーなのでセッションは終了する
            scrl_mo_held        = true;
            scrl_session_active = false; // 新しい操作の開始
        } else {
            scrl_mo_held = false;
            if (scrl_session_active) {
                // スクロール実績あり → 指を離してもセッションを継続し、
                // ここから SCROLL_SESSION_TIMEOUT を数え直す
                scrl_session_timer = timer_read();
            }
            // 一度もスクロールしていなければ scroll_sync() が従来どおり即解除する
        }
        scroll_sync(layer_state);
        return false; // Keyball標準の SCRL_MO 処理には渡さない
    }

    // SCRL_TO（Kb 6）を横取りして、スクロールトグルと AML（Auto Mouse Layer）の
    // レイヤー固定を連動させる。
    //   スクロールON  → auto_mouse_toggle() でレイヤーを固定し、タイムアウト解除されないようにする
    //   スクロールOFF → 固定を解除し、auto_mouse_layer_off() で通常のタイムアウト復帰へ戻す
    //
    // トグルの状態は scrl_toggled だけが持つ。keyball_get_scroll_mode() は
    // Kb7 やレイヤー 3 でも動き、レイヤー遷移でも変わるので、次の状態を決める
    // 材料にしてはいけない（これが「Kb6 を押しても OFF にならない」原因だった）。
    if (keycode == SCRL_TO && record->event.pressed) {
        swap_end(); // swapper の対象外キーなのでセッションは終了する

        bool next           = !scrl_toggled;
        scrl_toggled        = next;
        scrl_session_active = false; // トグルを常に優先する
        scroll_sync(layer_state);

#ifdef POINTING_DEVICE_AUTO_MOUSE_ENABLE
        // 状態ズレ防止のため、get_auto_mouse_toggle() で現在状態を見てから揃える
        if (get_auto_mouse_toggle() != next) {
            auto_mouse_toggle();
        }
        if (!next) {
            // 固定を解除したうえでレイヤーを下ろし、通常のタイムアウト復帰へ戻す
            auto_mouse_layer_off();
        }
#endif
        return false; // Keyball標準の SCRL_TO 処理には渡さない
    }

    // 対象外キーの押下で swapper とスクロールセッションの両方を終了する。
    // キー自体は握り潰さずそのまま通す。
    if (record->event.pressed && is_session_break_key(keycode, record)) {
        swap_end();
        if (scrl_session_active) {
            scrl_session_active = false;
            scroll_sync(layer_state);
        }
#ifdef POINTING_DEVICE_AUTO_MOUSE_ENABLE
        // AML はこの場で下ろす。process_auto_mouse() は process_record_user()
        // より先に走る（quantum.c の process_record_quantum）ので、あちらが
        // 判定する時点では tracker がまだ立っており AML が落ちない。一方で
        // キーの action が解決されるのはこの関数より後なので、ここで下ろせば
        // レイヤー 0 のキーとして解決される（Alt+Tab 直後の d が ↑ になる、
        // スクロール直後の 2 秒間だけ別のキーが入る、といった症状の対策）。
        // 条件は process_auto_mouse() の default 分岐と同じにそろえてある。
        aml_hold(false);
        if (!is_auto_mouse_active()) {
            auto_mouse_reset_trigger(true);
        }
#endif
    }

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

    // 現在の要求元（Kb6 のトグル / Kb7 / レイヤー 3）からスクロールを再同期する
    scroll_sync(layer_state);
}
