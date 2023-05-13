[GitHub - earlephilhower/mklittlefs: Utility for creating littlefs for upload on the ESP8266](https://github.com/earlephilhower/mklittlefs)

mklittlefs用于将文件打包成littlefs格式。

在win平台直接用mklittlefs.exe进行打包。

在当前目录打开powershell（cmd命令行也行），执行下面格式命令：

```
.\mklittlefs.exe -c <要打包的目录>  -b <blocksize> -p <pagesize> -s <partition size> <image_file>
```

例如：

```
.\mklittlefs.exe -c F:\PetiteDrv\mtd0 -b 4096 -p 256 -s 0x400000 mtd0.bin
```

命令的意思是：用mklittlefs.exe，将F:\PetiteDrv\mtd0文件夹中的文件打包为mtd0.bin。

mtd0.bin是littlefs格式镜像，

按照block size=4096Byte，page size=256的格式打包，这两个参数要和你准备将这个mtd0.bin下载到的Flash一致。

-s 0x400000则表明**littlefs镜像**是4M，这个也要和你的嵌入式设备的**分区**一致。