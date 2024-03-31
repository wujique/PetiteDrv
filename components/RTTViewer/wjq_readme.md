
# 使用J-Link RTT Viewer替代串口输出日志。

## 方案：
1. 从jlink资料中移植rtt viewer到工程。
2. 使用DAPLINK配合pyocd。

## 参考
https://blog.csdn.net/qq_36973838/article/details/131541025
https://blog.csdn.net/qq_36973838/article/details/131564806

## 下载安装JLINK软件
https://www.segger.com/downloads/jlink/#J-LinkSoftwareAndDocumentationPack

## 移植

## 安装puocd
直接在VSCODE中打开终端，WIN一般默认是powershell.
pip install pyocd

某些以来在外网，可能下载失败。请多试试。或者使用VPN下载。

安装完后，查看安装路径 pip show pyocd
在路径下，Python311\Scripts，有一个pyocd.exe

在终端中cd到目录。
路径太长，用'Function Prompt { "$( ( get-item $pwd ).Name )>" }'减少
执行./pyocd list 查看调试器
--
Script> .\pyocd.exe list
  #   Probe/Board              Unique ID      Target  
------------------------------------------------------
  0   X893 ARM CMSIS-DAP HID   0001A0000000   n/a
--

查看安装依赖包
./pyocd.exe pack find STM32H750VB
./pyocd.exe pack install STM32H750VB

这个包，起始就是keil的pack。
在pyocd中安装很慢，如果你有pack，直接安装
https://blog.csdn.net/sinat_36568888/article/details/127845304

---
Scripts>./pyocd.exe pack find stm32f407
0001273 I No pack index present, downloading now... [pack_cmd]
  Part            Vendor               Pack                 Version   Installed
---------------------------------------------------------------------------------
  STM32F407IEHx   STMicroelectronics   Keil.STM32F4xx_DFP   2.17.1    False
  STM32F407IETx   STMicroelectronics   Keil.STM32F4xx_DFP   2.17.1    False
  STM32F407IGHx   STMicroelectronics   Keil.STM32F4xx_DFP   2.17.1    False
  STM32F407IGTx   STMicroelectronics   Keil.STM32F4xx_DFP   2.17.1    False
  STM32F407VETx   STMicroelectronics   Keil.STM32F4xx_DFP   2.17.1    False
  STM32F407VGTx   STMicroelectronics   Keil.STM32F4xx_DFP   2.17.1    False
  STM32F407ZETx   STMicroelectronics   Keil.STM32F4xx_DFP   2.17.1    False
  STM32F407ZGTx   STMicroelectronics   Keil.STM32F4xx_DFP   2.17.1    False
Script> .\pyocd.exe pack install STM32F407VGTx
Downloading packs (press Control-C to cancel):
    Keil.STM32F4xx_DFP.2.17.1
Downloading descriptors (001/001)
---

查看已安装包
pyocd pack -s

查看目标列表
pyocd list --targets

在MDK编译，下载后。

链接目标
./pyocd.exe rtt -t stm32f407vgtx

为什么要连好久？
出现这个才能看到RTT消息
0018035 I Reading from up channel 0 ("Terminal") [rtt_cmd]
0018040 I Writing to down channel 0 ("Terminal") [rtt_cmd]


如果要重新下载程序，先用ctrl+c结束RTT


------------------------------------
20240331
有丢日志情况，原因未知。
串口和RTT同时输出，有死机情况。

