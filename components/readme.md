
## 计划整理
1. libqrcode，二维码生成库。
2. T9拼音输入
3. 笔画输入法

## 已经整理的组件
AT
	AT指令框架，用来和ES8266等模块进行交互。
cjson
	json构建于解析
CmBacktrace
	hardfault后堆栈信息跟踪，用来调试死机问题。
CMD
	命令行shell，之前移植uboot的，目前基本不用了。
emenu
	简易菜单，通常用于128*64 COG lcd上。
Fatfs
	文件系统，用于读SD卡文件。
FlashDB
	一种 KV数据库
FreeRTOS
	RTOS系统
Helix
	MP3解码库
littlefs
	文件系统， ARM官方出品，在SPI FLASH上优先使用本文件系统
lua
	一种脚本语言
lvgl
	UI
lwip
	网络库
lwrb
	环形缓冲区
Petitefont
	我们整理的字库组件，接口参考lvgl
spiffs
	一种性能较低的文件系统，已经不用，被littlefs替代。
tinyttf
	一种很轻的矢量字库，在LVGL9中已经包含，提取出来单独使用
zbar
	二维码解码库
zint
	条码二维码生成库

