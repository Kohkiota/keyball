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

atmega32u4 の上限は **28672 bytes**。最終構成は **27078 / 28672（94%、空き 1594 byte）**。

| 構成 | RGB off | RGB on |
|---|---|---|
| main / QMK 0.22.14 | 27280 | — |
| QMK 0.34.5 のみ | 27348 | — |
| + Cyclotab | 28212 | — |
| + 独立 Win Swapper | 28306 | — |
| + Combo | 30268 | — |
| + AML | 30388 | — |
| Cyclotab + 独立 Swapper（旧案） | 27578 (1094空) | 30328 (1656超) |
| bridge 案（pre_process 補正・不採用） | 27738 (934空) | 30468 (1796超) |
| **統合 Swapper（採用）** | **27078 (1594空)** | 29746 (1074超) |
| 参考: 統合 + Combo 無効 | 25104 | 27786 (886空) |
| 参考: 統合 + 軽量自作Combo（試作・不採用） | 25280 | 27986 (686空) |

### 削減候補の実測結果（推定ではなく実測）

| 候補 | 節約 | 判定 |
|---|---|---|
| `RGBLIGHT_ENABLE = no` | **2750 byte** | ✅ 採用（RGB はほぼ未使用のため） |
| Cyclotab 廃止＋Swapper 統合 | **500 byte** | ✅ 採用 |
| Combo を 6 個 → 3 個 | 18 byte | ほぼ無意味（コストはエンジン本体 1962〜1974 byte） |
| `OLED_FONT_END` 縮小 | **0 byte** | 無意味。logofont.c の配列は範囲に関係なく丸ごとリンクされる |
| `NO_ACTION_ONESHOT` | 738 byte | 不採用（OSM が割り当てられなくなる） |
| `LAYER_STATE_8BIT` | 0 byte | 無意味 |
| 軽量自作 Combo（176 byte）に置換 | 1798 byte | **不採用**（下記 3. 参照） |

LTO・`BOOTMAGIC`/`EXTRAKEY`/`CONSOLE`/`COMMAND`/`NKRO`/`MOUSEKEY`/
`SPACE_CADET`/`GRAVE_ESC`/`MAGIC` の無効化は `keyball44/rules.mk` で
**既にすべて適用済み**。これ以上のタダの削減余地は無い。

---

## 3. 最終構成

`keyball44/keymaps/via/` のみ変更。スクロール関連ロジックと Remap/VIA の
Dynamic Keymap には一切触れていない。

### 統合 Swapper（`keymap.c`）

6 キーを 1 つの状態機械で扱う。

| キー | 保持する mod |
|---|---|
| `A(KC_TAB)`, `S(A(KC_TAB))` | Alt |
| `G(KC_LEFT/RGHT/UP/DOWN)` | GUI |

- `swap_group()` 1 回の判定と `swap_mods` 1 変数だけ。系をまたぐ時は
  旧 mod を外してから新 mod を握るので **modifier の二重残留が構造的に起きない**。
- 対象キーは `return true`。QMK の `ACT_LMODS`（`action.c:410`）が
  キーコード側の修飾を **weak mods** で乗せるので `S(A(KC_TAB))` の Shift も
  自動で付き、`register_mods()` で握った real mods はキー解放時の
  `del_weak_mods()` では消えない。
- 対象外キーは `swap_end()` したうえで **消費せず通す**。
- 解放条件: 最後にキーを離してから `SWAP_TIMEOUT`（2000ms）経過、
  または対象外キーの押下。タイムアウト判定は `housekeeping_task_user()`。

#### stock Cyclotab を使わない理由（cyclotab.c 実物を確認済み）

1. 継続判定 `is_trigger_keycode()` は「現在の `active_key`」と
   「`S(active_key)`」の 2 つとしか照合しない。`CYCLOTAB_KEYS` に 4 方向を
   並べても `G(KC_LEFT)` セッション中の `G(KC_RGHT)` は別キー扱い。
   継続を許す `switch` の case も**素の** `KC_LEFT/RGHT/UP/DOWN` なので、
   `G(KC_RGHT)`=`0x084F` は `KC_RGHT`=`0x004F` と一致しない。
   → Win 系 4 方向の自由往復は構造上不可能。
2. セッション中に対象外キーを押すと `release_active()` のうえ
   `return !record->event.pressed` で**必ず握り潰される**。
   `process_record_cyclotab()` にハードコードされており公開 API で変えられない。
3. `cyclotab.h` は `cyclotab_clear()` を宣言しているが
   **リポジトリのどのコミットにも実装が無い**（`cyclotab.c` を触った commit は
   `ead180e` の 1 つだけ）。呼べばリンクエラー。
4. community module なので `process_record_kb/user` より先に走る。
   出し抜けるのは `pre_process_record_user` だけだが、そこで全キーを
   横取りするのは module を通さないのと同じ。

### Combo（`rules.mk` / `config.h` / `keymap.c`）

QMK 標準 Combo をそのまま使う（`COMBO_ENABLE = yes`）。

- `COMBO_TERM 30`, `COMBO_ONLY_FROM_LAYER 0`
- 左 `KC_BTN1` = J+K / D+F、右 `KC_BTN2` = K+L / S+D、中 `KC_BTN3` = J+L / S+F

#### `COMBO_ONLY_FROM_LAYER 0` は「レイヤー 0 でしか効かない」ではない

名前に反して**レイヤーを制限する設定ではない**。`process_combo.c` の該当箇所は

```c
#ifdef COMBO_ONLY_FROM_LAYER
    /* Only check keycodes from one layer. */
    keycode = keymap_key_to_keycode(COMBO_ONLY_FROM_LAYER, record->event.key);
#else
    ... combo_ref_from_layer(get_highest_layer(...)) ...
#endif
```

だけで、`process_combo()` 全体を通して**レイヤーを見た分岐は存在しない**
（`COMBO_ONLY_FROM_LAYER` の出現箇所は `#ifndef`(32行) / `#ifdef`(577行) /
この書き換え(579行) の 3 つのみ）。つまり「今どのレイヤーにいても、押された
物理キーをレイヤー 0 のキーコードに読み替えて Combo 判定する」機能であり、
**全レイヤーで Combo が有効**になる。

ビルド済みバイナリでも確認済み: `keymap_key_to_keycode` の呼び出しのうち
1 箇所が `ldi r24, 0x00`（レイヤー = 即値 0）で呼んでおり、そこへ分岐して
くる条件は `QK_COMBO_ON/OFF/TOGGLE` のキーコード比較
（`cpi 0x51/0x52` + `sbci 0x7C`）の 3 つだけ。`get_highest_layer` や
`layer_state` の読み出しは経路上に無い。

レイヤー 0 を Remap で並べ替えると Combo もそれに追従する
（`keymap_key_to_keycode` は VIA の dynamic keymap を引くため）。

軽量自作 Combo（176 byte、1798 byte 節約）も試作して実測したが**不採用**。
レイヤー 0 基準で全レイヤーから使う設計にすると、対象 6 物理位置に
全レイヤーで 30ms の出力保留が入り（特に AML の左クリックが 30ms 遅れる）、
さらに再注入が `action_tapping_process()` を通らないため、その位置に
Mod-Tap / Layer-Tap を割り当てられなくなる。安定性・互換性を優先した。

### AML（`keymap.c`）

- `keyboard_post_init_user()` で `set_auto_mouse_timeout(10000);`
  - `keyball.c` の `keyboard_post_init_kb()` が EEPROM 値で
    `set_auto_mouse_timeout()` した**後**に `_user()` を呼ぶので上書きが効く。
  - 注意: レイヤー 3 の `AML_I50`/`AML_D50` を押すと keyball 側の
    `AML_TIMEOUT_MAX = 1000` に丸められる。再起動すれば 10000 に戻る。

#### `is_mouse_record_user()` は使えない（重要）

`process_auto_mouse()` は switch の**先頭**で

```c
case KC_LEFT_CTRL ... KC_RIGHT_GUI:
case QK_MODS ... QK_MODS_MAX:
    break;
```

としている。`QK_MODS` の範囲は `0x0100`〜`0x1FFF` で、
`A(KC_TAB)`=0x042B / `S(A(KC_TAB))`=0x062B / `G(KC_LEFT)`=0x0850 /
`G(KC_RGHT)`=0x084F / `G(KC_UP)`=0x0852 / `G(KC_DOWN)`=0x0851 は
**全部この範囲に入る**ので `default:` の `is_mouse_record()` まで到達しない。
（同ファイルの `// QK_MODS goes to default` というコメントと doxygen の説明は
実装と食い違っている。コードが正。）

副作用として「押しても AML がリセットされない」こと自体は素の QMK で既に
成立している。足りないのは **10 秒タイマーの更新**のほう。

そこで `auto_mouse_keyevent()` で `mouse_key_tracker` を直接握る。

```c
static bool aml_held = false;
static void aml_hold(bool on) {
    if (on != aml_held) { aml_held = on; auto_mouse_keyevent(on); }
}
// housekeeping_task_user() 内
aml_hold(swap_mods != 0);
```

tracker が非 0 の間 `is_auto_mouse_active()` が true になり、
`pointing_device_task_auto_mouse()` が毎周期 `timer.active` を打ち直すので
**セッション中は 10 秒が減らず、解放時点から 10 秒が再スタート**する。
セッションは上記 6 キーでしか始まらないので、維持対象は実質この 6 キーだけ。
**素の `KC_TAB` / 素の矢印にはルールを追加していない**。

### QMK 0.34.5 の呼び出し順（確認済み）

```
action_exec()                          action.c:133
└─ pre_process_record_quantum()        quantum.c:277
   └─ _modules() → _kb() → _user() → process_combo()
└─ process_record_quantum()            quantum.c:296
   └─ ... process_auto_mouse() → process_record_modules()
      → process_record_kb() → process_record_user() → process_action()
housekeeping_task()                    keyboard.c:425
└─ _modules() → _kb() → _user()        _user は常に呼ばれる
```

### 既知の副作用

**AML が出ていない状態からこの 6 キーを押すと AML が点く。**
`auto_mouse_keyevent()` で tracker を握る以上、公開 API の範囲では回避不可。
レイヤー 1 は AML の対象レイヤーであると同時に `LT(1,KC_SPC)` のレイヤーでも
あるため、Space 長押しから Win+矢印を打つと、Space を離した後もレイヤー 1 が
10 秒ほど残る。

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

# 3. python 側（PEP 668 で --user が拒否されるので venv を使う）
python3 -m venv venv
./venv/bin/pip install -r qmk-0.34.5/requirements.txt
./venv/bin/pip install qmk
```

ビルド（keyboards/keyball は symlink だと `qmk` が認識しないので実体コピー）:

```bash
export PATH="$SP/avr-gcc-12.1.0-x64-linux/bin:$SP/venv/bin:$PATH"
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
| Docker image（**変更禁止・未変更**） | `ghcr.io/qmk/qmk_cli@sha256:16c4916e95b99bf88d27b15aec8db409ee17265d1710287fde248c6666508966` |
