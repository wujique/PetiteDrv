# 基于 GN+Ninja+ARMCC的STM32F407软件开发实践。

## 编译环境
GN+NINJA+ARMCC = CMAKE+MAKE+GCC

## 实践内容
1. 如何基于ninja编译程序。
2. 包含源码，库到工程。
3. 生成elf、bin、map。
4. backtrace、coredump
5. 如何调用py脚本添加编译功能。

##　参考
使用GN+Ninja替代MDK进行编译
https://blog.csdn.net/define_me_freedom/article/details/104194338
gn+ninja代替MDK编译工程
https://blog.csdn.net/ITsenlin/article/details/120773729
GN语法和操作
https://blog.csdn.net/yujiawang/article/details/72627138
参考鸿蒙 GN和ninja都可以从这里下载
https://repo.huaweicloud.com/harmonyos/compiler/

ninja仓库
https://github.com/ninja-build/ninja/releases
gn仓库
https://github.com/timniederhausen/gn


## 编译命令
构建
.\gn.exe gen out/demo
编译
.\ninja.exe -C out/demo
清除
.\ninja.exe -C out/demo -t clean

烧录程序用pyocd
https://github.com/pyocd/pyOCD
参考
https://blog.csdn.net/lbaihao/article/details/127345614
文档
https://pyocd.io/docs/target_support.html

直接在vscode打开终端，安装pyocd,当然，前提是电脑有python环境。

查看版本
pyocd --version
如果运行失败，请用
python -m pyocd --version
电路板通过daplink接上电脑，list
```
PS E:\work\PetiteDrv\prog\PetiteDrv> python -m pyocd list
  #   Probe/Board     Unique ID                                          Target  
---------------------------------------------------------------------------------
  0   ARM CMSIS-DAP   5050000007870cf300000000000000000000000097969902   n/a
```

查看支持的芯片, 如果不支持，自行安装pack
python -m pyocd list --targets

烧录
```
PS E:\work\PetiteDrv\prog\PetiteDrv> python -m pyocd flash --target stm32f407vg .\board\pochard_gn_ninja\out\demo\bin\build_target.axf
0001088 W Board ID 5050 is not recognized [mbed_board]
0001631 I Loading E:\work\PetiteDrv\prog\PetiteDrv\board\pochard_gn_ninja\out\demo\bin\build_target.axf [load_cmd]
[=====                                             ]  11%
```

