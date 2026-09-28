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

## 2. 【最重要】残っている問題: ROM サイズ

atmega32u4 の上限は **28672 bytes**。ローカル計測（avr-gcc 12.1.0）:

| 構成 | サイズ | 空き |
|---|---|---|
| main / QMK 0.22.14（現状バックアップ） | 27280 | 1392 |
| QMK 0.34.5 のみ | 27348 | **1324** |
| QMK 0.34.5 + Cyclotab | 28212 | **460** |

**QMK 更新自体のコストは +68 byte しかない**（つまり古い QMK に戻しても解決しない）。
問題は元々 main の時点で 95% 使用済みだったこと。

Cyclotab で 864 byte 消費し、残り 460 byte。
ここに Win Swapper（推定 200〜400 byte）と Combo（推定 700〜1200 byte）は
**入らない見込み**。

### 次セッションで最初にやること

1. Win Swapper と Combo を実装して実測する（下記ローカルビルド環境で数十秒）。
2. 溢れた場合、機能を削らずに済む手段は無いので、
   **どれを削るかはユーザーに選んでもらう**。候補:
   - `keymaps/via/rules.mk` の `RGBLIGHT_ENABLE = yes` を `no` に（約 2〜3KB 節約 / 底面 LED が消灯）
   - OLED フォント範囲 `OLED_FONT_END` の縮小（OLED 表示が変わる）
   - Combo 数を減らす（例: J+K / K+L / J+L の 3 つだけにする）
   - `#define NO_ACTION_ONESHOT`（Remap で OSM を割り当てられなくなる）

   ※ スクロール設定・AML ルールは変更禁止なのでここからは削らないこと。

---

## 3. 未実装（Step 3 以降）

作業順は当初指示どおり: Win Swapper → build → Combo → build → AML → build。

### Step 3: Win + 矢印 Swapper（`keyball44/keymaps/via/keymap.c`）

独立実装。Cyclotab には混ぜない。設計は確定済み:

- 対象 keycode: `LGUI(KC_LEFT)`, `LGUI(KC_RGHT)`, `LGUI(KC_UP)`, `LGUI(KC_DOWN)`
- 押下時: 未保持なら `register_mods(MOD_BIT(KC_LGUI))`、
  `tap_code(keycode & 0xFF)` で矢印だけ送る、`return false`
- 離した時: `timer = timer_read() | 1` でタイムアウト計測開始（押している間は 0 で停止）
- それ以外のキー: `unregister_mods(MOD_BIT(KC_LGUI))` して `return true`（キーは通す）
- タイムアウト: `housekeeping_task_user()` で `timer_expired()` を判定
  - **確認済み**: QMK 0.34.5 の `housekeeping_task()` は
    `housekeeping_task_modules()` → `_kb()` → `_user()` を並列に呼ぶので、
    keyball.c が `housekeeping_task_kb` を上書きしていても `_user` は呼ばれる。
- `<<< Win swapper timeout setting >>>` コメント付きで `#define WINSWAP_TIMEOUT 2000`

既存 `process_record_user`（SCRL_TO の横取り）の**手前**に swapper 判定を置く。

### Step 4: Combo

- `keymaps/via/rules.mk`: `COMBO_ENABLE = yes`
- `keymaps/via/config.h`: `#define COMBO_TERM 30`, `#define COMBO_ONLY_FROM_LAYER 0`
  （`COMBO_ONLY_FROM_LAYER` は QMK 0.34.5 の `process_combo.c:577` に存在確認済み）
- `keymap.c`: `COMBO_LEN` と `combo_t key_combos[]` を定義
  - 左クリック `MS_BTN1`: J+K, D+F
  - 右クリック `MS_BTN2`: K+L, S+D
  - 中クリック `MS_BTN3`: J+L, S+F
  - ※ `KC_BTN1` は compat header 経由で `MS_BTN1` になるのでどちらでも可

### Step 5: AML

- `keyboard_post_init_user()` に `set_auto_mouse_timeout(10000);` を追加。
  - **確認済み**: `keyball.c` の `keyboard_post_init_kb()` が EEPROM から
    `set_auto_mouse_timeout()` した**後**に `keyboard_post_init_user()` を呼ぶので、
    user 側の上書きが最後に効く。Keyball 側 EEPROM 実装の改造は不要。
  - 注意（報告事項）: layer 3 の `AML_I50`/`AML_D50` を押すと keyball 側の
    `AML_TIMEOUT_MAX = 1000` に丸められる。再起動すれば 10000 に戻る。
- AML 維持対象は `is_mouse_record_user()` で実装
  （keyball.c の `is_mouse_record_kb()` が呼んでくれる）。
  対象: `A(KC_TAB)`, `S(A(KC_TAB))`, `LGUI(KC_LEFT/RGHT/UP/DOWN)`,
  `KC_LEFT/KC_RGHT/KC_UP/KC_DOWN`。
  - 指示の「Left / Right / Up / Down」が素の矢印か Win+矢印かが曖昧だったため
    両方入れる方針。不要ならユーザー確認のうえ削る。
  - **それ以外のキーは絶対に追加しない。**

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
