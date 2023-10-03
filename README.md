# linux-rpi4-gpio


## [mod-gpio-blinker](mod-gpio-blinker.md)
 
Linux 用のドライバモジュール。
基板上の GPIO 出力ピンへ周期的に ON/OFF を出力します。

## [mod-gpio-router](mod-gpio-router.md)

Linux 用のドライバモジュール。
基板上の GPIO の入力ピンを割り込みで監視し、状態を出力ピンに反映します。


## HOW-TO-USE

各ディレクトリ内で `make` してモジュール `.ko` を作成してください。

Raspberry Pi 4B, Linux（raspios, bullseye, arm64）上で
`aarch64-linux-gnu-gcc` (Version 10.x) を使ってセルフビルドする想定です。

### Linux 用のドライバモジュールの操作

以下、モジュールを `mod-xxx.ko` とします。

#### ロード

```
$ sudo insmod mod-xxx.ko
$ lsmod | grep mod-xxx
$ sudo dmesg | tail
```

#### ロード（パラメータ指定付き）

```
$ sudo insmod mod-xxx.ko param1=10 param2=20 param3="abcde"
$ lsmod | grep mod-xxx
$ sudo dmesg | tail
```

#### パラメータの確認

```
$ grep "" -r /sys/module/mod_xxx/parameters/
```

#### アンロード

```
$ sudo rmmod mod-xxx
$ lsmod | grep mod-xxx
$ sudo dmesg | tail
```

