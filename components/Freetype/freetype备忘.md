参考：https://blog.csdn.net/Ranchaun/article/details/106412048

1 增大任务栈 8*1024 和动态分配内存 100K 就可以正常获取汉字点阵了。

> 如此大的内存消耗，根本不可能在单片机中使用。最好在PC用freetype先转成bitmap。

MDK优化等级也要设置为O1

2 检测，发现在open face中用过了29+32K动态申请，尝试分析优化。



freetype配置,  依赖vfs.c和alloc.c：

ftoption.h

ftmodule.h

freetype的接口和函数都比较绕，顺藤摸瓜也有点麻烦。

内存分配在：ftutil.c

FT_Stream_EnterFrame 函数中申请的内存较多----------》

FT_Stream_ExtractFrame定义宏FT_FRAME_EXTRACT， 两次大内存都是FT_Stream_ExtractFrame申请的。

现在用的是truetype字库

> tt_face_load_cmap
> FT_Stream_ExtractFrame:29514
> FT_Stream_ReadFields:6
> FT_Stream_ReadFields:32
> FT_Stream_ReadFields:36
> FT_Stream_ReadFields:36
> FT_Stream_ReadFields:78
> FT_Stream_ReadFields:8
> tt_face_load_loca
> FT_Stream_ExtractFrame:32452

大动态内存都是tt_face_init申请的。