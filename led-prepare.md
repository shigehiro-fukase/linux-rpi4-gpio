# Lチカの準備

Raspberry Pi 4BにはオンボードLEDが2つ (緑と赤) が付いており、緑のほうはGPIO 42から制御できるようになっています。Linuxが起動した直後はSDカードのアクセスインジケーターとして使用するよう設定されています。

<div><p><details>
<summary>初期状態</summary>
<div><p>
<code>
pi@raspberrypi:~$ cat /sys/class/leds/led0/trigger
none rc-feedback kbd-scrolllock kbd-numlock kbd-capslock kbd-kanalock kbd-shiftlock kbd-altgrlock kbd-ctrllock kbd-altlo
ck kbd-shiftllock kbd-shiftrlock kbd-ctrlllock kbd-ctrlrlock timer oneshot heartbeat backlight gpio cpu cpu0 cpu1 cpu2 c
pu3 default-on input panic actpwr mmc1 [mmc0] rfkill-any rfkill-none rfkill0 rfkill1
</code>
</p></div><br>
</details></p></div>

SOLID-OS側で操作したい場合、Linuxでの点滅イベントを無し（`none`）に設定する必要があります。

Linuxのターミナルで以下のように入力します。
```
$ sudo sh -c 'echo none > /sys/class/leds/led0/trigger'
```

注）起動している Linux（raspios） の バージョンによってはパスが異なることがあります。

```
$ sudo sh -c 'echo none > /sys/class/leds/ACT/trigger'
```


<div><p><details>
<summary>設定変更後の状態</summary>
<div><p>
<code>
pi@raspberrypi:~$ cat /sys/class/leds/led0/trigger
[none] rc-feedback kbd-scrolllock kbd-numlock kbd-capslock kbd-kanalock kbd-shiftlock kbd-altgrlock kbd-ctrllock kbd-alt
lock kbd-shiftllock kbd-shiftrlock kbd-ctrlllock kbd-ctrlrlock timer oneshot heartbeat backlight gpio cpu cpu0 cpu1 cpu2
 cpu3 default-on input panic actpwr mmc1 mmc0 rfkill-any rfkill-none rfkill0 rfkill1
</code>
</p></div>
</details></p></div>

これで、LED (緑) をSOLID-OS側で操作する準備が完了します。



