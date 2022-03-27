本工程基于屋脊雀cuckoo开发板，芯片为STM32H7B0VBT。

LCD测试用的图片、LVGL代码（lv_hal_tick.c除外）、LVGL demo图片都保存在QSPI FLASH。
具体请查看h7b0_mdk_demo.sct。

在MDK的Option中C/C++中，定义了NDEBUG宏，用于避免LVGL的断言错误。

启动代码startup_stm32h7b0xx.s中的栈和堆要注意配置，很容易出现溢出死机。

QSPI 时钟配置为140M，超出芯片规格书说明133M，但是实测能运行。




