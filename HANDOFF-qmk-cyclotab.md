# 作業引き継ぎメモ: feature/qmk-cyclotab

このファイルは実験ブランチ `feature/qmk-cyclotab` 専用の作業メモです。
main には入れません。作業完了後に削除してください。

---

## 1. 完了済み

### Step 1: QMK 0.22.14 → 0.34.5（commit `fe58df2`, `852b1d8`）

- `.github/workflows/build-firmware.yml` が `inputs.qmk_version` を
  checkout action に渡していなかったバグを修正。
- QMK version を **0.34.5**（現行安定版・2026-09-26）に固定。
  - `.github/actions/checkout-qmk_firmware/action.yml` の default
  - `.github/workflows/build-firmware.yml` の default
  - `.github/workflows/build-all.yml` / `build-user.yml` の呼び出し
- **Docker image SHA は一切変更していない**
  （`ghcr.io/qmk/qmk_cli@sha256:16c4916e95b99bf88d27b15aec8db409ee17265d1710287fde248c6666508966`）。
- `build-all.yml` の push branch filter を `'*'` → `'**'` に変更。
  `'*'` は `/` を含む branch 名にマッチしないため、`feature/...` への
  push で CI が起動しなかった。

#### QMK 移行で必要だった互換修正

| 内容 | 対象ファイル |
|---|---|
| keyboard 判定が `keyboard.json` 必須になった | `keyball{39,44,46,61}/info.json`, `one47/info.json` を `keyboard.json` にリネーム |
| 旧 GPIO API 廃止 (`setPinOutput` 等) | `drivers/pmw3360/pmw3360.c`, `lib/duplexmatrix/duplexmatrix.c`, `keyball46/keyball46.c`, `one47/one47.c` |
| `pointing_device_driver_init()` が `bool` を返すように | `lib/keyball/keyball.c` |
| `RGBLED_NUM` → `RGBLIGHT_LED_COUNT` | 各 `keyball*/config.h`, `one47/config.h` |
| `KC_BTNn` / `KC_MS_BTNn` / `RGB_xxx` の別名が削除 | **新規** `lib/keyball/keyball_qmk_compat.h`（`keyball.h` から include）<br>※数値は不変なので Remap/VIA の EEPROM keymap はそのまま有効 |
| `debounce()` から `num_rows` 引数が削除 | `lib/duplexmatrix/duplexmatrix.c` |
| `isLeftHand` → `is_keyboard_left()` | `lib/duplexmatrix/duplexmatrix.c` |
| `keyboard.json` の `"url": ""` が schema 違反でファイルごと無視され VENDOR_ID が消える | `keyball46/keyboard.json` から `"url"` を削除 |

#### CI コンテナ側の問題と対処（image は変更せず）

pinned image に入っている **古い `qmk_cli`** が `milc.set_metadata()` を呼ぶが、
QMK 0.34.5 の `requirements.txt` は `milc>=1.9.0` を要求し pip が milc 2.x を
入れるため、`AttributeError: module 'milc' has no attribute 'set_metadata'` で落ちる。

→ `build-firmware.yml` の "Install Python dependencies" に
`python3 -m pip install --break-system-packages --upgrade qmk` を追加して対処。

また QMK 0.34.5 には `lib/python/qmk/math.py` が存在しないため、
"Patch QMK for Python 3.14 (ast.Num)" ステップにファイル存在チェックを追加。

### Step 2: Cyclotab 導入

- `build-firmware.yml` に `qmk_modules_version` input を追加し、
  **固定 commit** `8c55ac1c5d547d1ff324ae2834b26f2075222c97`
  (getreuer/qmk-modules, 2026-09-16) を `__qmk__/modules/getreuer` に checkout。
- `keyball44/keymaps/via/keymap.json` を新規作成（`{"modules": ["getreuer/cyclotab"]}`）。
  QMK は `keymap.json` と `keymap.c` の共存をサポートしており、
  生成された keymap.c の末尾で既存 `keymap.c` が include される。
  既存の `config.h` / `rules.mk` / dynamic keymap には影響なし。
- `keyball44/keymaps/via/config.h` に `#define CYCLOTAB_TIMEOUT 2000`
  （`<<< Cyclotab timeout setting >>>` コメント付き）。
- Cyclotab 本体は**未改造**。デフォルトの `CYCLOTAB_KEYS` が `A(KC_TAB)` で、
  `S(A(KC_TAB))` も自動でトリガ対象になるため、要件（Alt+Tab / Shift+Alt+Tab）を
  そのまま満たす。Remap 側で `LALT(KC_TAB)` と `LSFT(LALT(KC_TAB))` を割り当てる。

---

## 2. ROM サイズ（決着済み）

atmega32u4 の上限は **28672 bytes**。ローカル実測（avr-gcc 12.1.0）:

| 構成 | サイズ | 空き |
|---|---|---|
| main / QMK 0.22.14 | 27280 | 1392 |
| QMK 0.34.5 のみ | 27348 | 1324 |
| + Cyclotab | 28212 | 460 |
| + Win Swapper | 28306 | 366 |
| + Combo | 30268 | **-1596** |
| + AML | 30388 | **-1716** |
| **上記から RGBLIGHT_ENABLE = no**（採用） | **27578** | **1094** |

Combo だけが 1962 byte を消費する。エンジン本体のコストなので、
**コンボの数を減らしても減らない**（6 個 → 3 個で 18 byte しか減らなかった）。

### 削減候補の実測結果（推定ではなく実測）

| 候補 | 節約 | 判定 |
|---|---|---|
| `RGBLIGHT_ENABLE = no` | **2812 byte** | ✅ 採用。全機能が入り 1094 byte 空く |
| Combo を 6 個 → 3 個 | 18 byte | ほぼ無意味 |
| `OLED_FONT_END` 縮小 | **0 byte** | 無意味。logofont.c の配列は範囲に関係なく丸ごとリンクされる |
| `NO_ACTION_ONESHOT` | 738 byte | 単体では不足 |
| `LAYER_STATE_8BIT` | 0 byte | 無意味 |

LTO・`BOOTMAGIC`/`EXTRAKEY`/`CONSOLE`/`COMMAND`/`NKRO`/`MOUSEKEY`/
`SPACE_CADET`/`GRAVE_ESC`/`MAGIC` の無効化は `keyball44/rules.mk` で
**既にすべて適用済み**。これ以上のタダの削減余地は無い。

採用した変更は `keymaps/via/rules.mk` の `RGBLIGHT_ENABLE = no` のみ。
代償は底面 RGB LED が消灯し、レイヤー 3 の `RGB_*` キーが無反応になること
（OLED は従来どおり動作）。スクロール設定・AML ルールには触れていない。

---

## 3. Step 3〜5（実装済み）

### Step 3: Win + 矢印 Swapper（`keyball44/keymaps/via/keymap.c`）

要件は **`G(KC_LEFT/RGHT/UP/DOWN)` の 4 キーを自由に押し替えても GUI を
保持し続ける**こと（「最初だけ Win+矢印、あとは素の矢印」ではない）。

#### stock Cyclotab で代用できないことの確認結果

`modules/getreuer/cyclotab/cyclotab.c` を確認した結論: **構造上不可能**。

1. セッション継続判定 `is_trigger_keycode()` は「現在の `active_key`」と
   「`S(active_key)`」の 2 つとしか照合しない。`CYCLOTAB_KEYS` に 4 方向を
   並べても、`G(KC_LEFT)` セッション中の `G(KC_RGHT)` は別キー扱いになる。
2. 例外的に継続を許す `switch` の case は**素の** `KC_LEFT/RGHT/UP/DOWN`。
   `G(KC_RGHT)` = `0x084F` は `KC_RGHT` = `0x004F` と一致しない。
3. 結果、方向を変えた最初の 1 打が `release_active()` で GUI を解放し、
   さらに `return !record->event.pressed` で握り潰される。

拡張フックは weak な `cyclotab_timeout()` だけで、継続条件を差し替える手段は
無い。CI は getreuer/qmk-modules を固定 commit から clone するので
`cyclotab.c` 自体の改造も不可。

→ **独立 Win Swapper を残す**（要件だけを満たす最小実装）。

一方 `A(KC_TAB)` ↔ `S(A(KC_TAB))` の往復は上記 1. の `S(active_key)` 照合で
成立するので、**タスク切り替えは stock Cyclotab のまま**で要件を満たす。

#### 実装

- 対象: `G(KC_LEFT)`, `G(KC_RGHT)`, `G(KC_UP)`, `G(KC_DOWN)` を 1 つの
  case グループにまとめ、4 方向のどれに押し替えても `winswap_active` を
  保ったまま GUI を握り続ける。
- 押下時: 未保持なら `register_mods(MOD_BIT(KC_LGUI))`、
  `tap_code(keycode & 0xFF)` で矢印だけ送り `return false`。
- 離した時: `winswap_timer = timer_read() | 1` でタイムアウト計測開始
  （押している間は 0 で停止）。
- 対象外のキー押下: `winswap_release()` して `return true`（キーは通す）。
- タイムアウト: `housekeeping_task_user()` で判定。QMK 0.34.5 の
  `housekeeping_task()` は `_modules()` → `_kb()` → `_user()` を順に呼ぶので、
  keyball.c が `housekeeping_task_kb` を持っていても `_user` は呼ばれる（確認済み）。
- `<<< Win swapper timeout setting >>>` 付きで `#define WINSWAP_TIMEOUT 2000`

### Step 4: Combo

- `keymaps/via/rules.mk`: `COMBO_ENABLE = yes`
- `keymaps/via/config.h`: `#define COMBO_TERM 30`, `#define COMBO_ONLY_FROM_LAYER 0`
- `keymap.c`: 左 `KC_BTN1` = J+K / D+F、右 `KC_BTN2` = K+L / S+D、
  中 `KC_BTN3` = J+L / S+F

### Step 5: AML

- `keyboard_post_init_user()` に `set_auto_mouse_timeout(10000);`
  - `keyball.c` の `keyboard_post_init_kb()` が EEPROM 値で
    `set_auto_mouse_timeout()` した**後**に `_user()` を呼ぶので上書きが効く。
  - 注意: レイヤー 3 の `AML_I50`/`AML_D50` を押すと keyball 側の
    `AML_TIMEOUT_MAX = 1000` に丸められる。再起動すれば 10000 に戻る。

#### 【重要】`is_mouse_record_user()` は使えない

当初の計画では `is_mouse_record_user()` に対象キーを並べる予定だったが、
**この 6 キーでは絶対に呼ばれない**ことが判明した。

`quantum/pointing_device/pointing_device_auto_mouse.c` の
`process_auto_mouse()` は switch の**先頭**で

```c
case KC_LEFT_CTRL ... KC_RIGHT_GUI:
case QK_MODS ... QK_MODS_MAX:
    break;
```

としている。`QK_MODS` の範囲は `0x0100`〜`0x1FFF` で、
`A(KC_TAB)`=0x042B / `S(A(KC_TAB))`=0x062B / `G(KC_LEFT)`=0x0850 /
`G(KC_RGHT)`=0x084F / `G(KC_UP)`=0x0852 / `G(KC_DOWN)`=0x0851 は
**全部この範囲に入る**ので、`default:` の `is_mouse_record()` まで到達しない。
（同ファイル内の `// QK_MODS goes to default` というコメントと
doxygen の説明は実装と食い違っている。コードが正。）

副作用として「押しても AML がリセットされない」こと自体は素の QMK で既に
成立している。足りないのは **10 秒タイマーの更新**のほう。

#### 採用した方式: セッション連動で `auto_mouse_keyevent()` を握る

```c
static bool aml_held = false;
static void aml_hold(bool on) {
    if (on != aml_held) { aml_held = on; auto_mouse_keyevent(on); }
}
// housekeeping_task_user() 内
aml_hold(winswap_active || cyclotab_active_key() != KC_NO);
```

- `auto_mouse_keyevent(true)` は `mouse_key_tracker` を +1 する。
  tracker が非 0 の間 `is_auto_mouse_active()` が true になり、
  `pointing_device_task_auto_mouse()` が毎周期 `timer.active` を打ち直すので
  **セッション中は 10 秒が減らず、解放時点から 10 秒が再スタート**する。
- Cyclotab セッションも Win Swapper セッションも
  `A(KC_TAB)` / `S(A(KC_TAB))` / `G(KC_LEFT/RGHT/UP/DOWN)` の 6 キーを
  押した時にしか始まらないので、維持対象は実質この 6 キーだけになる。
- **素の `KC_TAB` / 素の矢印には一切ルールを追加していない**
  （従来どおり `auto_mouse_reset_trigger()` で AML をリセットする）。
- increment / decrement を必ず 1 対 1 に保つため状態変化時のみ呼ぶ。
  Cyclotab がキーイベントを握り潰しても tracker がずれない。
- Cyclotab の状態は public getter `cyclotab_active_key()` で読む
  （`#include "cyclotab.h"`）。

### 既知の制限・副作用

1. **Cyclotab セッション中の `G(矢印)` 初回打鍵が握り潰される。**
   Cyclotab は module として `process_record_kb/user` より先に走り、
   非対象キーを `release_active()` したうえで消費するため。Alt+Tab 直後
   2 秒以内に Win+矢印を押した場合のみ発生し、もう一度押せば効く。
   これは Cyclotab 本来の「他キーで選択を確定する」設計そのもの。
   （逆方向の Win Swapper → Alt+Tab は正常に動く。）
2. **AML が出ていない状態からこの 6 キーを押すと AML が点く。**
   `auto_mouse_keyevent()` で tracker を握る以上避けられない。
   レイヤー 1 は AML の対象レイヤーであると同時に `LT(1,KC_SPC)` の
   レイヤーでもあるため、Space 長押しから Win+矢印を打つと、Space を
   離した後もレイヤー 1 が 10 秒ほど残る。

---

## 4. ローカルビルド環境（CI を待たずに検証できる）

`gh` が無く、GitHub Actions は 1 回 5 分程度かかるため、ローカルに AVR
ツールチェーンを用意してある。**scratchpad は揮発するので次セッションでは
作り直しが必要**（手順は以下）。

```bash
SP=<scratchpad>      # 例: /tmp/claude-.../scratchpad
mkdir -p $SP && cd $SP

# 1. AVR toolchain（sudo 不要。14.x は glibc 2.36 が要るので 12.1.0 を使う）
curl -sL -o avr-gcc12.tar.bz2 \
  https://github.com/ZakKemble/avr-gcc-build/releases/download/v12.1.0-1/avr-gcc-12.1.0-x64-linux.tar.bz2
tar xjf avr-gcc12.tar.bz2

# 2. QMK 本体（shallow）＋ AVR に必要な submodule だけ
git clone --depth 1 --branch 0.34.5 https://github.com/qmk/qmk_firmware.git qmk-0.34.5
cd qmk-0.34.5 && git submodule update --init --depth 1 lib/lufa lib/printf && cd ..

# 3. community modules を固定 commit で
git clone https://github.com/getreuer/qmk-modules.git qmk-0.34.5/modules/getreuer
git -C qmk-0.34.5/modules/getreuer checkout 8c55ac1c5d547d1ff324ae2834b26f2075222c97

# 4. python 側
python3 -m pip install --user -r qmk-0.34.5/requirements.txt
python3 -m pip install --user qmk
```

ビルド（keyboards/keyball は symlink だと `qmk` が認識しないので実体コピー）:

```bash
export PATH="$SP/avr-gcc-12.1.0-x64-linux/bin:$HOME/.local/bin:$PATH"
export QMK_HOME="$SP/qmk-0.34.5"
rm -rf $QMK_HOME/keyboards/keyball
cp -a /home/komai/keyball/qmk_firmware/keyboards/keyball $QMK_HOME/keyboards/keyball
cd $QMK_HOME && qmk compile -j 4 -kb keyball/keyball44 -km via
```

CI と同じ `ln -s` 方式でも 0.34.5 で認識されることは確認済み
（最初に symlink で失敗したのは `keyboard.json` リネーム前だったのが原因）。

`Layout macro should not be defined within ".h" files.` は警告で、ビルドは通る
（レイアウトを keyboard.json に移す DD 化は今回のスコープ外）。

CI matrix 19 通りを一括で試すスクリプトも同じ要領で回せる。
**2 個同時に走らせると `keyboards/keyball` の差し替えが競合するので厳禁。**

---

## 5. GitHub Actions の確認方法（`gh` 無し）

`gh` はこの環境に入っていない。リポジトリは public なので未認証 REST API で
run の成否だけは取れる（ただし **未認証は 60 req/hour**。ログ本文と artifact の
ダウンロードは認証が要るので取得できない）。

```bash
# 最新 run
curl -s "https://api.github.com/repos/Kohkiota/keyball/actions/runs?branch=feature%2Fqmk-cyclotab&per_page=3"

# job ごとの成否
curl -s "https://api.github.com/repos/Kohkiota/keyball/actions/runs/<RUN_ID>/jobs?per_page=100"
```

失敗ログの中身が要る場合はユーザーに貼ってもらうこと。
`gh auth login` が使えるようになるならそちらが早い。

---

## 6. 固定している版数まとめ

| 対象 | 値 |
|---|---|
| QMK firmware | `0.34.5` |
| getreuer/qmk-modules | `8c55ac1c5d547d1ff324ae2834b26f2075222c97` |
| Docker image（**変更禁止・未変更**） | `ghcr.io/qmk/qmk_cli@sha256:16c4916e95b99bf88d27b15aec8db409ee17265d1710287fde248c6666508966` |
