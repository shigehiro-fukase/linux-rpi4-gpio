
## mod-gpio-router

Linux 用のドライバモジュール。
基板上の GPIO の入力ポートを割り込みで監視し、状態を出力ポートに反映します。
主要なコードは[`mod-gpio-router/driver.c`](./mod-gpio-router/driver.c)にあります。

### 準備

初期状態では以下の設定になっています。
| 設定 | 意味 |
|------|------|
| GPIO 入力 | `16`
| GPIO 出力 | `4`

注）出力に `42`（緑のLED）を用いる場合は Linux との競合を避けるために
[Lチカの準備](led-prepare.md)を参照してください。

### 操作

モジュールをロードすると、
基板上の GPIO の入力ポートを割り込みで監視し、状態を出力ポートに反映します。

モジュールのロード（`insmod`）時にパラメータ指定することができます。

機能を停止するにはモジュールのアンロード（`rmmod`）をしてください。

### パラメータ

`insmod` コマンド実行時に `パラメータ名=値` 形式で指定します。

指定例）
```
$ sudo insmod gpio-router.ko gpi=16 gpo=42 gpi_opt="rf" reverse=true verbose=true
```

値の確認例）
```
$ grep "" -r /sys/module/gpio_router/parameters/
/sys/module/gpio_router/parameters/gpo:42
/sys/module/gpio_router/parameters/reverse:Y
/sys/module/gpio_router/parameters/verbose:Y
/sys/module/gpio_router/parameters/gpi:16
/sys/module/gpio_router/parameters/gpi_opt:rf
```
パス名はハイフン（`'-'`）ではなく、アンダースコア（`'_'`）なので注意。

#### `gpi` : GPIO 入力番号変更

入力に用いる GPIO ポート番号を数値で指定します。

#### `gpi_opt` : GPIO 入力設定

GPIO 入力の設定を文字列で指定します。

設定文字列は以下の組み合わせです。（大文字小文字は区別されません）
| 文字列 | 意味 |
|--------|------|
| `R`    | Rising Edge Detect Enable
| `F`    | Falling Edge Detect Enable
| `H`    | High Detect Enable
| `L`    | Low Detect Enable

例）
- 立ち上がりエッジと立下りエッジの割り込みを有効化
  - `RF`

GPIO 16 に設定する例
```
$ sudo insmod gpio-router.ko gpi=16 gpi_opt="rf"
```

##### Note 1）設定値について

Linux カーネルの `<linux/interrupt.h>` で定義された次の値にのみ対応しています。
```
#define IRQF_TRIGGER_RISING     0x00000001
#define IRQF_TRIGGER_FALLING    0x00000002
#define IRQF_TRIGGER_HIGH       0x00000004
#define IRQF_TRIGGER_LOW        0x00000008
```

BCM2711 のレジスタ設定値とは直接対応ではありません。

##### Note 2）GPIO のプルアップ／プルダウン設定について

本モジュールでは設定できません。
Linux のコマンド（raspios付属の `raspi-gpio`）で設定してください。

GPIO 16 をプルアップに設定する例
```
$ raspi-gpio set 16 pu
```
`pu` 部分は

- プルダウン： `pd`
- プル無し： `pn`

など。設定方法の詳細は `raspi-gpio help` でご確認ください。


#### `gpo` : GPIO 出力番号変更

出力に用いる GPIO ポート番号を数値で指定します。

GPIO 42 に設定する例
```
$ sudo insmod gpio-router.ko gpo=42
```

#### `reverse` : 出力反転設定

出力する際に入力ポートの状態を反転するかどうかを bool 値で指定します。

有効 に設定する例
```
$ sudo insmod gpio-router.ko reverse=true
```

#### `verbose` : 冗長プリント出力設定

冗長なプリント(`printk`）出力をするかどうかを bool 値で指定します。
出力内容は `sudo dmesg` コマンドで確認してください。

有効 に設定する例
```
$ sudo insmod gpio-router.ko verbose=true
```


## 参考資料
本プログラムは下記資料を参考にしています。

- [組み込みLinuxデバイスドライバの作り方 - iwatake2222著, Qiita](https://qiita.com/iwatake2222/items/1fdd2e0faaaa868a2db2)
  - [09_02](https://github.com/iwatake2222/DeviceDriverLesson/blob/master/09_02/myDeviceDriver.c)


