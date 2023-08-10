用mklittlefs打包文件为littlefs，
然后编译到固件中，并且通过外部分散加载文件将其放到外部QSPI Flash。
在程序中将其挂载为littlefs，并且QSPI处于map模式，所以此littlefs文件系统为只读。

mklittlefs和说明在:
	PetiteDrv\tool\mklittlefs

在本目录进入命令行后执行：
../../../tool/mklittlefs/mklittlefs.exe -c ./src -b 4096 -p 256 -s 0x300000 elfs.bin


1. 打包littlefs镜像
2. 添加elfs.s，添加到mdk工程，编译将bin链接到代码。
3. 修改分散加载文件，将elfs放到外部QSPI flash
4. 在C代码中引用符号。
5. 通过符号加载littlefs。

提示：下载很慢，确认bin已经下载到QSPI Flash后，
就可以将elfs.s从工程删除，否则每次下载，很浪费时间