## PetiteDrv


 PetiteDrv想做什么？

PetiteDrv面向的是单片机，少资源的系统，基于freertos。

PetiteDrv尝试做一些设备管理和partition管理。

PetiteDrv尝试做一些能随便用，容易用的组件。

PetiteDrv收集一些开源组件，优化，让其在少资源的单片机上跑起来。

PetiteDrv中的代码，尽量少依赖，以便你拷贝到你的工程，而不需要使用整个PetiteDrv。

PetiteDrv尝试定义跟MCU的接口，以便很容易适配到新的MCU，而不是等MCU厂家来适配才可用。

PetiteDrv不干涉编译，ubuntu or win，gcc or  armcc， mdk or cmake or ninja，跟我们关系不大，我们只是代码。

PetiteDrv尝试做一些编译，例如 基于gn + ninja+ gcc 编译STM32。



**经过一年多ESP32项目后发现，PetiteDrv ，可能是想做一个面对普通单片机的IDF**

## Directory Structure
|Dir|Decription|
|-|-|
|board|各种板的应用和测试程序，相当于应用层|
|components|各种组件|
|driver|设备驱动：摄像头、LCD、语音codec、触摸屏、矩阵按键蜂鸣器等。|
|mcu| 芯片的HAL库，将芯片库重新封的mcu接口。           |
|panel|中间件：wifi管理、显示、菜单界面、语音播放|
|Petite|Petite核心、总线管理（I2C&SPI等），内存管理（动态分配，链表等），分区管理。|
|tool| 各种脚本工具，例如mklittlefs                                 |

## Contact us
Website:
<www.wujique.com>



