# Aircondition_On_ESP32
### 一个基于ESP32的远程空调控制方案
学校新装的空调居然不支持连接Wifi，~~生活在2023年的我感到非常不爽~~，所以一怒之下做出了这个小项目。<br>
用的红外模块是某宝上的BQ-TTL-IRTM，这个通过串口和单片机通信，可以直接发送一堆hex code，非常方便，就是有点吃闪存，~~这也是不用ESP8266的主要原因。~~<br>
我手动记录下了几个常用的hex code，写到[codes.conf](https://github.com/LJY32/Aircondition_On_ESP32/blob/main/data/codes.conf)里，使用的时候再读取，发送。
#
这个方法理论上来说应该通吃市面上所有红外控制的设备<br>
你也可以搓一个放大电路，把红外发射管放在屋子中央，实现一个非常抽象的全屋控制。
