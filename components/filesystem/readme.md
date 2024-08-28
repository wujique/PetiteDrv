### FatFs
Fat格式的文件系统。
如果挂载SD卡，选用这个系统。
### littlefs
ARM mbed项目的一个组件。
带掉电保护和擦写均衡。
但有一个明显缺陷，修改一个文件的头部数据，在文件同步的时候，会将整个文件重新拷贝一次。
耗时：文件大小/块大小*块擦写时间，spi nor flash块擦写时间接近100ms
例如，一个400K的文件，100个sector，整理一遍要100*100ms=10s

### SPIFFS
乐鑫IDF用的比较多。细节没研究过，感觉读写很慢。
https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/api-reference/storage/spiffs.html
官方仓库
https://github.com/pellepl/spiffs

### PetitFatFs
http://elm-chan.org/fsw/ff/00index_p.html
Petit FatFs 是FatFs的精简版，比较适用于低端8位单片机中。可以用在小RAM的单片机中，RAM可以小于扇区的RAM（512bytes)中。

  Features：

 （1）Verysmall RAM consumption (44 bytes work area + certain stack).

 （2）Verysmall code size (2K-4K bytes).

 （3）SupportsFAT32.

 （4）Singlevolume and Single file.

 （5）Filewrite function with some restrictions.