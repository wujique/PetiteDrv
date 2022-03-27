## PetiteDrv
This project is extends from wujique/stm32f407。
The program architecture is not significantly different。
Only modified the directory structure for better portability。
Now, PetiteDrv is port to WUJIQUE(STM32F407) and Canary(STM32F103).
It will port to more chip in the future, E.g FSL MK60, K210, ESP32, RT1052...

## Directory Structure
|Dir|Decription|
|-|-|
|board|Configuration and application of different hardware, including project files and app|
|driver|Peripheral drive, such as LCD driver|
|mcu|mcu lib, such as StLib, May include a secondary encapsulation layer|
|panel|some code between app and driver|
|Petite|PetiteDrv Module：bus,font,mem|
|Utilities|Third party library: freertos,fatfs,lvgl,littlefs,zbar,zint|

## Petite

#### bus

i2c bus, lcd bus, spi bus, uart bus, vi2c bus, vspi bus

#### font

#### mem

alloc, kfifo, list, linkfifo

## Main feature
Please find the detail from :board_sysconf.c/board_sysconf.h, and Petite.
1. SPI Device and SPI CH
2. LCD bus & lcd driver & lcd device
3. I2C CH
4. VFS
5. emenu

## Contact us
Website:
<www.wujique.com>



