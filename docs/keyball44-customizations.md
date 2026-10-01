# Keyball fork customizations

このリポジトリが upstream Keyball から変えている点の保守用メモ。
実装の大半は **Keyball44 の `via` キーマップ**にある。キーマップの配置そのものは
Remap / VIA の Dynamic Keymap（EEPROM）側で管理しているので、ここには書かない。

表記: **[fork]** = このリポジトリ独自 / **[QMK]** = QMK 標準機能の設定

---

## 1. Build / QMK 環境

- QMK firmware を **`0.34.5` に固定** [fork]
  - `.github/actions/checkout-qmk_firmware/action.yml`
  - `.github/workflows/build-firmware.yml` / `build-all.yml` / `build-user.yml`
- build コンテナも **digest 固定**
  - `ghcr.io/qmk/qmk_cli@sha256:16c4916e...` (`build-firmware.yml`)
- floating の `latest` を使わない理由: upstream の QMK / コンテナが動くと、
  この fork の互換パッチや ROM サイズ（atmega32u4 は 28672 byte 上限で常に 95% 超）が
  黙って壊れるため。**上げるときは必ず全 CI matrix を通してから**。

### QMK 0.22.14 → 0.34.5 で必要だった互換修正 [fork]

| 内容 | 対象 |
|---|---|
| keyboard 判定が `keyboard.json` 必須になった | `keyball{39,44,46,61}/info.json`, `one47/info.json` をリネーム |
| 旧 GPIO API 廃止（`setPinOutput` → `gpio_set_pin_output` 等） | `drivers/pmw3360/pmw3360.c`, `lib/duplexmatrix/duplexmatrix.c`, `keyball46/keyball46.c`, `one47/one47.c` |
| `pointing_device_driver_init()` が `bool` を返すようになった | `lib/keyball/keyball.c` |
| `RGBLED_NUM` → `RGBLIGHT_LED_COUNT` | 各 `keyball*/config.h`, `one47/config.h` |
| `KC_BTNn` / `KC_MS_BTNn` / `RGB_xxx` の別名が削除 | **新規** `lib/keyball/keyball_qmk_compat.h`（`keyball.h` から include）<br>数値は不変なので Remap/VIA の EEPROM キーマップはそのまま有効。`RGB_M_*` は upstream に残っているので対象外 |
| `debounce()` から `num_rows` 引数が削除 | `lib/duplexmatrix/duplexmatrix.c` |
| `isLeftHand` → `is_keyboard_left()` | `lib/duplexmatrix/duplexmatrix.c` |

`pointing_device_driver_init()` は **`return true;` 固定**にしている。戻り値は
`pointing_device_init()` が `POINTING_DEVICE_STATUS_SUCCESS` / `_INIT_FAILED` として保存し、
`pointing_device_task()` が `_SUCCESS` 以外のとき早期 return するため、ボール無し側で
false を返すとその側の task が止まる。実際にボールがあるかは `keyball.this_have_ball` が持つ。

---

## 2. Split handedness hotfix [fork]

```c
#define SPLIT_HAND_MATRIX_GRID_LOW_IS_LEFT
```

対象: **keyball39 / keyball44 / keyball61** の各 `config.h`（`SPLIT_HAND_MATRIX_GRID` の直後）

理由: QMK が `SPLIT_HAND_MATRIX_GRID` の**既定極性を反転**させた。

| | opt-in マクロ | 既定（マクロ無し） |
|---|---|---|
| 0.22.14 | `..._LOW_IS_RIGHT` | `!peek_matrix_intersection()` = LOW は左 |
| 0.34.5 | `..._LOW_IS_LEFT` | `peek_matrix_intersection()` = LOW は右 |

マクロ名が改名され同時に既定が裏返ったため、どちらも定義していなかったこの fork は
移行で左右判定が逆転し、キー配置の左右崩れ・ポインタ X/Y 両軸の反転・スクロール方向の反転・
Remap の Ball availability 誤判定が一度に起きた。上記 define で旧挙動を明示している。

**対象外**: keyball46 は `keyball46.c` で `is_keyboard_left()` を自前実装し明示的に
`!peek_matrix_intersection(...)` を使っているため影響なし。one47 は非分割（`SPLIT_KEYBOARD = no`）。

---

## 3. Keyball44 unified Swapper [fork]

`keyball44/keymaps/via/keymap.c`。修飾キーを押しっぱなしにしたまま同じ操作を連打できる。
対象は 10 キーで、3 グループに分かれる。

| group | 保持する修飾キー | 対象キー | 用途 |
|---|---|---|---|
| Alt | Alt | `A(KC_TAB)`, `S(A(KC_TAB))` | タスク切り替え |
| Alt | Alt | `A(KC_LEFT)`, `A(KC_RGHT)` | ブラウザの戻る / 進む |
| Win | GUI | `G(KC_LEFT)`, `G(KC_RGHT)`, `G(KC_UP)`, `G(KC_DOWN)` | ウィンドウスナップ |
| Ctrl | Ctrl | `C(KC_TAB)`, `S(C(KC_TAB))` | ブラウザのタブ移動 |

仕様:

- 同一 group 内は修飾キーを保持したまま何度でも自由に往復できる
- group をまたぐときは**旧修飾キーを解放してから新修飾キーを保持**する。
  Alt+Ctrl や Alt+Win のように二重に残らない（保持中の mod は `swap_mods` 1 変数のみ）
- 対象外キーの押下で session 終了。**その対象外キー自体は握り潰さず通す**。
  終了と同時に AML もその場で下ろす（後述の `is_session_break_key()`）
- session 中は AML も維持する。ただし**開始時に AML が点灯していた場合だけ**（後述）
- 最後にキーを離してから `SWAP_TIMEOUT` 経過で修飾キーを解放

対象キーは `process_record_user()` で `return true` している。QMK の `ACT_LMODS` が
キーコード側の修飾を weak mods で乗せるので `S(A(KC_TAB))` の Shift や `S(C(KC_TAB))` の
Shift は自動で付き、`register_mods()` で握った real mods はキー解放時の `del_weak_mods()`
では消えない。

### stock Cyclotab community module を使っていない理由

getreuer/cyclotab では要件を満たせなかったため、独自の state machine にした。
セッション継続の判定が「現在の1キーとその Shift 版」しか見ないので Win 系 4 方向の
自由往復ができず、さらに session 中の対象外キーを必ず握り潰す仕様が
`process_record_cyclotab()` にハードコードされていて公開 API では変えられなかった。
**module は依存から外してある**（CI の checkout ステップも削除済み）。

---

## 4. Swapper のタイマー管理 [fork]

`swap_timer`（時刻）と `swap_timer_running`（稼働フラグ）を**分離**している。
0 を「停止」の sentinel に兼用したり `timer_read() | 1` で 0 を避けたりしてはいけない。
偶数 ms のとき保存値が 1ms 未来になり、`timer_elapsed()` の符号なし減算が 65535 に
回り込んで即タイムアウト扱いになる（session がランダムに即終了する実機バグの原因だった）。
`scrl_session_timer` も同じ理由で同じ方式。

---

## 5. AML (Auto Mouse Layer)

- 対象レイヤー = **Layer 1** [QMK] (`AUTO_MOUSE_DEFAULT_LAYER 1`)
- 起動時に有効化し、timeout を **10 秒**に設定 [fork]
  （`keyboard_post_init_user()` 内の `set_auto_mouse_timeout(10000)`。
  `keyball.c` の `keyboard_post_init_kb()` が EEPROM 値で設定した**後**に呼ばれるので上書きが効く）
- **Swapper session 中は AML を維持**（開始時に AML が点灯していた場合だけ）[fork]
- **Kb7 の scroll session 中も AML を維持** [fork]
- **PageUp / PageDown / Home / End を AML 維持対象として特別扱い** [fork]

### PageUp / PageDown / Home / End

`is_mouse_record_user()` でこの 4 キーだけ `true` を返している。

これが無いと、AML 表示中にこれらを押しても **Layer 0 の文字が入力される**。
素のキーコードは `process_auto_mouse()` の `default:` に落ち、
`auto_mouse_reset_trigger()` が press 時に `layer_off(AML)` を実行する。
action が解決されるのはその**後**（`process_record_handler()` → `store_or_get_action()` が
press 時に `layer_switch_get_layer()` で現在の `layer_state` を読む）ため、
AML が消えた状態で Layer 0 として解決されてしまう。
`true` を返すと代わりに `auto_mouse_keyevent()` が呼ばれ、`layer_off` されず
押している間 AML が維持され、離せば通常の 10 秒タイマーへ戻る。

> **ここに他の通常キーを追加しないこと。** 追加するとそのキーで AML を抜けられなくなる。

### session 中の AML 維持と、その抜け方

維持の実装は `aml_hold()` で `auto_mouse_keyevent()` を呼び、QMK の
`mouse_key_tracker` を握る方法（`is_auto_mouse_active()` が true になる）。
注意点が 3 つある。

**1. 「維持」は「点灯」も意味する。**
`pointing_device_task_auto_mouse()` は `is_auto_mouse_active()` が true なら
`layer_on(AML)` まで実行する。つまり tracker を握るとボールに触っていなくても
AML が点く。そのため swapper 側は session 開始時の
`layer_state_is(AUTO_MOUSE_TARGET_LAYER)` を `swap_aml` に覚えておき、
**点灯していた session だけ**維持対象にしている
（これが無いと Alt+Tab しただけでマウスレイヤーが点く）。

**2. tracker を握っている間は、素のキーで AML を抜けられない。**
`process_auto_mouse()` の `default:` は
`else if (!is_auto_mouse_active())` の条件付きなので、tracker が立っていると
`auto_mouse_reset_trigger()` が呼ばれない。放置すると session 中の打鍵が
**Layer 1 のキーコードとして解決される**（Alt+Tab 直後の `d` が `↑` になる、
Kb7 のスクロール直後 2 秒間だけ別のキーが入る）。
しかも `process_auto_mouse()` は `process_record_user()` より**先**に走るので
（`quantum.c` の `process_record_quantum`）、`swap_end()` だけでは間に合わない。

対策として `process_record_user()` で対象外キーの押下を見たら、その場で
session を終了 → `aml_hold(false)` → `!is_auto_mouse_active()` なら
`auto_mouse_reset_trigger(true)` までを実行する。キーの action が解決されるのは
この関数より後なので、これで Layer 0 のキーとして解決される。

対象外キーの判定 `is_session_break_key()` は `process_auto_mouse()` の
`default:` に落ちるキーと一致させてある。除外するのは次の 3 種類。

| 除外するもの | 理由 |
|---|---|
| 素の修飾キー（`KC_LEFT_CTRL`〜`KC_RIGHT_GUI`）、修飾付きキーコード（`QK_MODS`〜`QK_MODS_MAX`） | AML を触らないキー。**Ctrl + スクロールのズームを壊さないため** |
| MT / LT のホールド（`record->tap.count == 0`） | mod / レイヤーとしての打鍵 |
| マウス系のキー（`IS_MOUSE_KEYCODE` / `is_mouse_record_kb()`） | AML を維持する側のキー |

> `pre_process_record_user()` は使わない。tap / hold とコンボが確定する前に
> 呼ばれるので、Space 長押しや J/K/L/S/D/F で session が誤って切れる。

**3. tracker の increment / decrement は 1 対 1 に保つ。**
`set_auto_mouse_timeout()` / `set_auto_mouse_enable()`（= レイヤー 3 の
`AML_I50` / `AML_D50` / `KBC_RST`）は内部で `auto_mouse_reset()` を呼び
tracker を 0 に戻す。`aml_held == true` のままこれが起きると解除時に
tracker が **-1** になり、`is_auto_mouse_active()` は「非 0 なら真」なので
（次のキーイベントでクランプされるまで）AML が張り付く。
そのため解除は `get_auto_mouse_key_tracker() > 0` のときだけ行う。

### 注意: `AML_I50` / `AML_D50`

Keyball 標準の AML timeout 調整キーには **1 秒の上限**（`keyball.c` の
`AML_TIMEOUT_MAX = 1000`）がある。レイヤー 3 のこれらを押すと 10 秒設定が
1 秒側へ丸められる。**再起動すれば 10 秒に戻る。**

---

## 6. Mouse Combo [QMK 機能 / 設定は fork]

`COMBO_ENABLE = yes`。ホームポジションからマウスボタンを出す 6 個。

| 組み合わせ | 出力 |
|---|---|
| J+K, D+F | 左クリック (`KC_BTN1`) |
| K+L, S+D | 右クリック (`KC_BTN2`) |
| J+L, S+F | 中クリック (`KC_BTN3`) |

- `COMBO_TERM 30` — 2 キーがこの時間内に押されたら成立
- `COMBO_ONLY_FROM_LAYER 0` — **「Layer 0 でだけ発火する」という意味ではない。**
  `process_combo()` にレイヤーを見た分岐は存在せず、このマクロは
  「今どのレイヤーにいても、押された物理キーを Layer 0 のキーコードとして評価する」
  設定。つまり **Combo は全レイヤーで有効**。Remap で Layer 0 を並べ替えると
  （dynamic keymap を引くので）Combo もそれに追従する。

Combo エンジンは実測で約 1960 byte 使うが、コンボ数を減らしても減らない
（6 個 → 3 個で 18 byte しか変わらなかった。コストはエンジン本体）。

---

## 7. Scroll

### スクロール状態は `scroll_sync()` が 1 箇所で合成する [fork]

スクロールモードを ON にしたい要求元は 4 つある。

| 要求元 | 変数 / 条件 |
|---|---|
| Kb6 の完全トグル | `scrl_toggled` |
| Kb7 を物理的に押している | `scrl_mo_held` |
| Kb7 の scroll session 継続中 | `scrl_session_active` |
| レイヤー 3 の自動スクロール | 最上位レイヤーが 3 |

`keyball_set_scroll_mode()` を呼ぶのは **`scroll_sync()` だけ**。他の場所から
直接呼んではいけない。

理由は QMK の `layer_state_set()` に「状態が変わったか」の判定が無いこと
（`action_layer.c`）。`layer_on()` / `layer_off()` が呼ばれるたびに、結果の
layer_state が同じでも `layer_state_set_user()` が走る。そこでレイヤーだけを見て
`keyball_set_scroll_mode(最上位 == 3)` を書いていたため、

- AML の点灯（`pointing_device_task_auto_mouse()` の `layer_on`）
- AML のタイムアウト消灯
- `LT(1,KC_SPC)` / `LT(2,KC_ENT)` のホールドと解放

のたびに Kb6 / Kb7 のスクロールが無言で解除されていた。
**AML が消えた状態から Kb7 を押すと一瞬だけスクロールして以降カーソルが動く**、
**Kb6 が OFF にできない**、という症状の原因がこれ。
`keyball_set_scroll_mode()` は変化時に `KEYBALL_SCROLLBALL_INHIVITOR`（50ms）
ボール入力を捨てるので、ボールのカクつきにもなっていた。

`layer_state_set_user()` から呼ぶときは**引数の `state` を渡す**。
この時点ではグローバルの `layer_state` はまだ古い値。

### Kb6 / `SCRL_TO` — 完全トグル

押すたびにスクロールモードを ON / OFF する。
ON のとき AML のレイヤーを固定し、OFF で固定を解除して `auto_mouse_layer_off()` で
レイヤーを下ろす。固定は `get_auto_mouse_toggle()` で現在値を見てから `next` に
揃えるので、AML 側が `auto_mouse_reset()` で `is_toggled` を落としていてもズレない。

トグルの状態は **`scrl_toggled` だけ**が持ち、次の状態は `next = !scrl_toggled`。
`keyball_get_scroll_mode()` は Kb7 でもレイヤー 3 でも動くので、次の状態を決める
材料にしてはいけない（以前はこれを見ていたため、レイヤー遷移で scroll mode だけが
落ちると `next` が常に ON 側になり、Kb6 が OFF にならなくなっていた）。

**scroll session の timeout では解除されない。**

### Kb7 / `SCRL_MO` — scroll session [fork]

従来は「押している間だけスクロール」。これに加えて、

1. Kb7 を押してボールを回す → スクロール開始（従来どおり）
2. **実際にスクロール入力が出た後は、Kb7 を離しても session が継続**する
3. スクロール入力があるたび timeout を打ち直すので、途中で止めて読んでから再開できる
4. `SCROLL_SESSION_TIMEOUT` 無操作で自動的にスクロールモード解除
5. session 中は AML も維持（対象外キーを押すと session ごと終了して AML も下りる）

session の開始条件は「Kb7 を押した」ではなく「**Kb7 を押している間に実際にスクロール
入力が出た**」。検出は `pointing_device_task_user()` で `rep.h` / `rep.v` を見ている
（keyball は `pointing_device_task_kb()` を定義していないので、この関数には driver が
スクロール量を入れた後のレポートが渡る。`h`/`v` はスクロールモード中しか非 0 にならない）。
そのため**通常のポインタ移動では session が始まらない**。

競合回避:

- **Kb6 のトグル ON 中**と**レイヤー 3 の自動スクロール中**は、session が切れても
  スクロールは落ちない。`scroll_sync()` が要求元を OR しているため
  （以前の `scrl_session_may_release()` は廃止）
- Kb6 を押すと session をクリアするので、トグルが常に優先される
- suspend / wakeup では `scroll_sync()` で現在の要求元から再同期する。
  Kb6 の ON のままスリープしても、復帰後も ON のまま

---

## 8. RGB / OLED

- `RGBLIGHT_ENABLE = no`（`keyball44/keymaps/via/rules.mk`）[fork]
  - 実機に RGB LED が無く、Flash が足りなかったため。実測で約 2700 byte 節約になる
  - レイヤー 3 の `RGB_*` キーは無反応になる
- `OLED_ENABLE = yes` のまま。**OLED と RGB は無関係**で、RGB 無効化は OLED に影響しない

---

## 9. 主な変更ファイル

`git diff e9ce7ec..HEAD` の範囲（`e9ce7ec` = QMK 移行前の main）。

| パス | 役割 |
|---|---|
| `qmk_firmware/keyboards/keyball/keyball44/keymaps/via/keymap.c` | Swapper / AML 維持 / Combo 定義 / scroll session / SCRL_TO 連動 / `scroll_sync()` |
| `.../keyball44/keymaps/via/config.h` | `SWAP_TIMEOUT`, `SCROLL_SESSION_TIMEOUT`, `COMBO_TERM`, `COMBO_ONLY_FROM_LAYER`, `TAPPING_TERM`, AML 設定 |
| `.../keyball44/keymaps/via/rules.mk` | `COMBO_ENABLE = yes`, `RGBLIGHT_ENABLE = no` |
| `qmk_firmware/keyboards/keyball/keyball44/config.h` | handedness hotfix, `RGBLIGHT_LED_COUNT` |
| `qmk_firmware/keyboards/keyball/keyball39/config.h` | 同上 |
| `qmk_firmware/keyboards/keyball/keyball61/config.h` | 同上 |
| `qmk_firmware/keyboards/keyball/keyball46/config.h` | `RGBLIGHT_LED_COUNT` のみ（handedness は対象外） |
| `qmk_firmware/keyboards/keyball/one47/config.h` | `RGBLIGHT_LED_COUNT` |
| `qmk_firmware/keyboards/keyball/lib/keyball/keyball_qmk_compat.h` | **新規**。削除されたキーコード別名の復活 |
| `qmk_firmware/keyboards/keyball/lib/keyball/keyball.h` | compat header の include |
| `qmk_firmware/keyboards/keyball/lib/keyball/keyball.c` | `pointing_device_driver_init()` の `bool` 化 |
| `qmk_firmware/keyboards/keyball/lib/duplexmatrix/duplexmatrix.c` | GPIO API, `debounce()` 引数, `is_keyboard_left()` |
| `qmk_firmware/keyboards/keyball/drivers/pmw3360/pmw3360.c` | GPIO API |
| `qmk_firmware/keyboards/keyball/keyball46/keyball46.c`, `one47/one47.c` | GPIO API |
| `qmk_firmware/keyboards/keyball/*/info.json` → `keyboard.json` | 5 モデルをリネーム |
| `.github/workflows/build-firmware.yml` | QMK 版固定, Docker digest 固定, Python 依存の調整 |
| `.github/workflows/build-all.yml` / `build-user.yml`, `.github/actions/checkout-qmk_firmware/action.yml` | QMK 版固定, push branch filter |

---

## 10. Adjustable values

| 値 | 現在値 | ファイル | 何の時間 / 設定か |
|---|---|---|---|
| `SWAP_TIMEOUT` | `2000` ms | `keyball44/keymaps/via/config.h` | Swapper が修飾キーを保持し続ける時間（最後にキーを離してから） |
| `SCROLL_SESSION_TIMEOUT` | `2000` ms | `keyball44/keymaps/via/config.h` | Kb7 の scroll session が無操作で解除されるまでの時間 |
| `COMBO_TERM` | `30` ms | `keyball44/keymaps/via/config.h` | Combo 成立とみなす 2 キーの同時押し判定幅 |
| AML timeout | `10000` ms | `keyball44/keymaps/via/keymap.c` の `set_auto_mouse_timeout()` | マウスレイヤーが自動的に下がるまでの時間 |
| `TAPPING_TERM` | `145` ms | `keyball44/keymaps/via/config.h` | タップ / ホールドの判定境界 |
| `TAP_CODE_DELAY` | `5` ms | `keyball44/keymaps/via/config.h` | `tap_code()` の押下保持時間 |
| `AUTO_MOUSE_DEFAULT_LAYER` | `1` | `keyball44/keymaps/via/config.h` | AML の対象レイヤー |
| `DYNAMIC_KEYMAP_LAYER_COUNT` | `6` | `keyball44/keymaps/via/config.h` | Remap / VIA で使えるレイヤー数 |
| QMK version | `0.34.5` | `.github/workflows/*.yml`, `.github/actions/checkout-qmk_firmware/action.yml` | ビルドに使う QMK |

ROM は atmega32u4 の 28672 byte 上限に対して常に 95% 前後まで埋まっている。
何か足すときは必ずローカルまたは CI の `Check size` で確認すること。
