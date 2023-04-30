SD卡超时错误问题

[一个关于SD卡读写超时的话题 (stmicroelectronics.cn)](https://shequ.stmicroelectronics.cn/thread-628998-1-1.html)

在基于STM32 MCU的应用开发中，经常会有人用到SD卡。有人发现在对SD卡进行读写时，偶尔会遇到SD卡读写失败的情况。尤其是单独对SD卡进行读写正常、或者非多任务环境下运行正常，而当SD读写跟其它程序代码混在一起或在多任务环境下运行时就容易发生异常。比方如下情形：

**1、**使用STM32F4的SDIO，用CubeMX配置。用polling模式的时候，出现SD卡写的时候卡死的情况。后来用DMA模式就好了。

**2、**STM32F746DISCO开发板和官方1.3.0例程基础上使用了FreeRTOS系统，改为DMA传输后还是不稳定。后来把SDIO相关的中断优先级别调高，SDIO中断设为3，DMA发送和接收的调为4，比FreeRTOS的系统用户级别高，解决问题。

**3、**在FreeRTOS+FatFs+STM32F429的基础上。

A.单个任务操作，SD卡读写没有问题

B.多个任务，SD卡和U盘两个任务，优先级别一样，SD卡无法读写，U盘可以读写，调中断级别，调整堆栈大小都没有用。

C.还是多任务，SD卡和U盘两个任务，甚至更多任务，只要把SD卡任务的优先级别调为最高，则SD卡，U盘读写都正常。

**4、**使用STM32官方SDIO的SD CARD驱动的时候,单任务的时候没有出现错误,使用多任务的时候经常出现问题,表现为读取文件的时候没有问题,经常点击屏幕的时候就会出问题,最后通过仿真找到出错点就是读取数据向SD卡发送CD17命令时出现SD_RX_OVERRUN这个错误,产生原因为读取FIFO溢出. 通过仿真与测试发现问题主要出现在读取过程中不能打断。

在两个读取函数里面SD_ErrorSD_ReadMultiBlocks(u32addr, u32 *readbuff, u16 BlockSize, u32NumberOfBlocks);SD_Error SD_ReadBlock(u32addr, u32 *readbuff, u16BlockSize); 加上临界保护可以消除问题。同样写操作的时候也一样需要加临界保护。

以上罗列的情形，根本上讲因为SD卡读写过程中发生了超时错误导致读写异常。应对措施视具体应用要求可能有所差异，最终目的都是为了避免SD卡读写过程中的超时错误。比如将查询模式改为DMA模式、调整相关SD卡读写过程中所涉及中断的优先级、提升SD卡读写任务的优先级、将SD卡读写程序放在临界保护区运行或者在读写SD卡操作时关闭中断等。

我们可以从SD卡相关协议得知，在对SD卡进行读写操作时是有超时保护及约束机制的。对于标准卡和高容量卡的读写超时定义还有所差异。细节请参考SD卡相关规格书。

SD Specifications ----Physical LayerSimplified Specification。

