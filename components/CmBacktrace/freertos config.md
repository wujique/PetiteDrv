##　进行配置

[FreeRTOS-移植Cmbacktrace - 一地鸡毛- - 博客园 (cnblogs.com)](https://www.cnblogs.com/ydmblog/p/15308372.html)

#### main函数：

```
#include "cm_backtrace.h"`
cm_backtrace_init("CmBacktrace", "V1.0.0", "V1.0.0");`
```



#### cmb_cfg

```
/* print line, must config by user */
#define cmb_println(...)             printf(__VA_ARGS__);printf("\r\n"); /* e.g., printf(__VA_ARGS__);printf("\r\n") */
//#define cmb_println(...)  							BSP_UARTPrintf(DBUART, __VA_ARGS__);BSP_UARTPrintf(DBUART,"\r\n");
/* enable bare metal(no OS) platform */
/* #define CMB_USING_BARE_METAL_PLATFORM */
/* enable OS platform */
/* #define CMB_USING_OS_PLATFORM */
#define CMB_USING_OS_PLATFORM
/* OS platform type, must config when CMB_USING_OS_PLATFORM is enable */
/* #define CMB_OS_PLATFORM_TYPE           CMB_OS_PLATFORM_RTT or CMB_OS_PLATFORM_UCOSII or CMB_OS_PLATFORM_UCOSIII or CMB_OS_PLATFORM_FREERTOS */
#define CMB_OS_PLATFORM_TYPE           CMB_OS_PLATFORM_FREERTOS
/* cpu platform type, must config by user */
#define CMB_CPU_PLATFORM_TYPE          CMB_CPU_ARM_CORTEX_M4/* CMB_CPU_ARM_CORTEX_M0 or CMB_CPU_ARM_CORTEX_M3 or CMB_CPU_ARM_CORTEX_M4 or CMB_CPU_ARM_CORTEX_M7 */
/* enable dump stack information */
/* #define CMB_USING_DUMP_STACK_INFO */
#define CMB_USING_DUMP_STACK_INFO
/* language of print information */
/* #define CMB_PRINT_LANGUAGE             CMB_PRINT_LANGUAGE_ENGLISH(default) or CMB_PRINT_LANGUAGE_CHINESE */
#define CMB_PRINT_LANGUAGE	CMB_PRINT_LANGUAGE_ENGLISH
#endif /* _CMB_CFG_H_ */
```

#### task.c
添加如下函数

```
/*< Support For CmBacktrace >*/
uint32_t * vTaskStackAddr()
{
    return pxCurrentTCB->pxStack;
}

uint32_t vTaskStackSize()
{
    #if ( portSTACK_GROWTH > 0 )
    
    return (pxNewTCB->pxEndOfStack - pxNewTCB->pxStack + 1);
    
    #else /* ( portSTACK_GROWTH > 0 )*/
    
    return pxCurrentTCB->uxSizeOfStack;
    
    #endif /* ( portSTACK_GROWTH > 0 )*/

}

char * vTaskName()
{
    return pxCurrentTCB->pcTaskName;
}
/*-----------------------------------------------------------*/
```




这时候时候会显示没有pxCurrentTCB->uxSizeOfStack

typedef struct tskTaskControlBlock中添加

```
#if( portSTACK_GROWTH <= 0)
UBaseType_t     uxSizeOfStack;      /*< Support For CmBacktrace >*/
#endif
```

static void prvInitialiseNewTask中添加

```
pxNewTCB->uxSizeOfStack = ulStackDepth;   /*< Support For CmBacktrace >*/
```


#### FreeRTOS.h

typedef struct xSTATIC_TCB中添加

```
#if(portSTACK_GROWTH <= 0)
        UBaseType_t     uxSizeOfStack;      /*< Support For CmBacktrace >*/
#endif /* ( portSTACK_GROWTH > 0 )*/
```



程序死机报错后，在工程Objects目录打开windows命令行（powershell），根据提示执行命令

例如：

F:\PetiteDrv\prog\PetiteDrv\components\CmBacktrace\tools\addr2line\win64\addr2line.exe -e h7b0_mdk_demo.axf -a -f 08000430 08011caa 08014170 08014740 08014a56 080188e6 0800de6a 0801714c

注意，只有在MDK配置，Output配置中选型输出Debug Information，才能通过addr2line找到对应代码。

