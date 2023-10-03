
## mod-gpio-blinker

Linux 用のドライバモジュール。
基板上の GPIO 出力ピンへ周期的に ON/OFF を出力します。
周期的なタイミングを発生させるために Linux カーネルの機能 [hrtimers][1] を使用します。
主要なコードは[`mod-gpio-blinker/driver.c`](./mod-gpio-blinker/driver.c)にあります。

### 準備

デフォルト値では以下の設定になっています。
| 設定 | 意味 |
|------|------|
| GPIO 出力 | `42` （緑のLED）
| 周期      | `1000000000` ns （1秒）

注）緑のLED は Linuxでも使用しています。
競合を避けるために [Lチカの準備](led-prepare.md)を参照してください。

### 操作

モジュールをロードすると、カーネルのタイマー処理を実行して
基板上の GPIO の出力ポートに ON/OFF 出力します。

モジュールのロード（`insmod`）時にパラメータ指定することができます。

機能を停止するにはモジュールのアンロード（`rmmod`）をしてください。


### パラメータ

`insmod` コマンド実行時に `パラメータ名=値` 形式で指定します。

指定例）
```
$ sudo insmod gpio-blinker.ko gpo=42 nsec=2000000000 verbose=true
```

値の確認例）
```
$ grep "" -r /sys/module/gpio_blinker/parameters/
sudo insmod gpio-blinker.ko gpo=42 nsec=2000000000
grep "" -r /sys/module/gpio_blinker/parameters/
/sys/module/gpio_blinker/parameters/gpo:42
/sys/module/gpio_blinker/parameters/verbose:Y
/sys/module/gpio_blinker/parameters/nsec:2000000000
/sys/module/gpio_blinker/parameters/sec:0
```
パス名はハイフン（`'-'`）ではなく、アンダースコア（`'_'`）なので注意。

#### `gpo` : GPIO 出力番号変更

出力に用いる GPIO ポート番号を数値で指定します。

GPIO 4 に設定する例
```
$ sudo insmod gpio-router.ko gpo=4
```

#### `sec` `nsec` : 周期変更

タイマー周期を2つの数値で指定します。

- `sec` : 数値は秒(s) で指定します。（デフォルト値:`0`）
- `nsec` : 数値はナノ秒(ns) で指定します。（デフォルト値:`1000000000` ＝1秒）

モジュールのロード（`insmod`）時にタイマーを開始するためのパラメータとして使用されます。
それぞれ、Linux の `ktime_set(sec, nsec)` 関数の引数になります。
（つまり、周期を5秒にしたい場合、 `sec=5,nsec=1000000000` でも、`sec=0,nsec=5000000000` でも可能です）

20ms に設定する例
```
$ sudo insmod gpio-router.ko nsec=20000000
```

#### `verbose` : 冗長プリント出力設定

冗長なプリント(`printk`）出力をするかどうかを bool 値で指定します。
出力内容は `sudo dmesg` コマンドで確認してください。

有効 に設定する例
```
$ sudo insmod gpio-router.ko verbose=true
```

[1]: https://docs.kernel.org/timers/hrtimers.html


## 参考資料
本プログラムは下記資料を参考にしています。

GPIO

- [組み込みLinuxデバイスドライバの作り方 - iwatake2222著, Qiita](https://qiita.com/iwatake2222/items/1fdd2e0faaaa868a2db2)
  - [09_02](https://github.com/iwatake2222/DeviceDriverLesson/blob/master/09_02/myDeviceDriver.c)

Timers

- [hrtimers - subsystem for high-resolution kernel timers](https://docs.kernel.org/timers/hrtimers.html)

- [High Resolution Timer - Linux Device Driver Tutorial Part 27](https://embetronicx.com/tutorials/linux/device-drivers/using-high-resolution-timer-in-linux-device-driver/)
  - [High_Resolution_Timer](https://github.com/Embetronicx/Tutorials/tree/master/Linux/Device_Driver/High_Resolution_Timer)


