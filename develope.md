## 修改记录



#### 2023.05.17

增加 partition层，将存储设备（spi flash等）进行分区管理，并将分区和虚拟文件系统关联。

添加组件：

​	cm backtrace，FlashDB，littlefs，tiny ttf，

#### 2022.10.19

改bus_lcd，每个LCD BUS，通常只有一个LCD，因此，不需要做bus管理。

bus_lcd层，归到lcd驱动，只做函数封装，不再有bus管理。

可减少显示的不必要操作。

#### 2022.10.18

本次主要修改字库和显示。在cuckoo+cog lcd上验证了，其他还没做测试。

1. 获取点阵的格式，参考freetype的bitmap格式。
2. 修改display，支持用utf-8格式，代码讲utf-8转为gbk后再显示。

字库和显示的逻辑差不多就是这样了。

后面再慢慢实现：

1. unicode编码排布的字库。
2. 多国语言，big5，codepage。

#### 2022.04.19

本次修改主要适配摄像头，OV2640和OV5640，在H7B0和F407平台。

1. 增加OV5640驱动，实现RGB565模式
2. 摄像头分层：板相关的放board.c，MCU DCMI，摄像头3部分。
3. F407摄像头I2C改为VI2C，不用SCCB了。并加上互斥。
4. board wujique增加msp文件，放一些硬件相关配置代码。

### **PetiteDrv 2.0**

2022.03.27

1. 适配 cuckoo板（stm32h7b0vb），主要功能lvgl，音频播放。
2. board/wujique 删除网络、USB功能，源码未动。
3. STM32F407平台更新cmsis，以便使用新版本rtos
4. 整理音频，梳理模块，增加audio pipeline 粘合层。
   	涉及soundplay,WM8960,mcu_i2s改动，暂时屏蔽dacsound功能。
5. I2C总线加互斥。
6. 字库系统优化，减少获取一个字符点阵的开销。
7. fatfs更新，并且整理注册disk的接口。
8. freertos更新，使用cmsis v2接口。
9. 与应用相关的代码，全部放到board工程目录下。

## 开发计划



#### 平台适配

适配RT1052

---

#### 架构设计

音频架构设计，参考ESP ADF

摄像头架构设计

hardisk架构：不同的存储设备封装一套按照地址分配的接口。

AT指令解析架构，串口WIFI模块驱动

分三层：

1. AT WIFI接口层，提供接口给APP实现接入路由、连接、收发数据。
2. AT指令处理核心
3. 不同无线模块的AT指令处理和业务实现。

---

#### 基础模块

内存分配增加类似linux slab功能，解决大量申请小内存（几字节）时利用率低的问题。

---

#### 外设驱动

环境检测传感器系列

