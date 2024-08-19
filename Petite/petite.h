#ifndef __PETITE_H__
#define __PETITE_H__

/**
参考
https://cloud.tencent.com/developer/article/2005074

简单说一下这里的思路：
先判断是不是IAR，在判断armcc的版本
在排除了 Arm Compiler 6 的前提下，根据 __clang__ 来判断当前编译器是否为 LLVM（即：__IS_COMPILER_LLVM__）；
在排除了 LLVM、Arm Compiler 和IAR的前提下，根据 __GNUC__ 来判断当前编译器是否为 GCC

目前已知的获取渠道包括但不限于：
从本文抄下来
包含获取perf_counter 并包含 perf_counter.h 
在存在 arm-2d 的情况下，直接包含 arm_2d.h 或者 arm_2d_utils.h
 */

enum{
    _petite_COMPILER_ARM_CC_5 = 1,
    _petite_COMPILER_ARM_CC_6,
    _petite_COMPILER_IAR,
    _petite_COMPILER_LLVM,
    _petite_COMPILER_GCC,
};

//! \note for IAR
#undef __IS_COMPILER_IAR__
#if defined(__IAR_SYSTEMS_ICC__)
#   define __IS_COMPILER_IAR__                  1
#define __PETITE_COMPILER_IS__  _petite_COMPILER_IAR
#endif

//! \note for arm compiler 5
#undef __IS_COMPILER_ARM_COMPILER_5__
#if ((__ARMCC_VERSION >= 5000000) && (__ARMCC_VERSION < 6000000))
#   define __IS_COMPILER_ARM_COMPILER_5__       1
#define __PETITE_COMPILER_IS__  _petite_COMPILER_ARM_CC_5
#endif
//! @}

//! \note for arm compiler 6

#undef __IS_COMPILER_ARM_COMPILER_6__
#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
#   define __IS_COMPILER_ARM_COMPILER_6__       1
#define __PETITE_COMPILER_IS__  _petite_COMPILER_ARM_CC_6
#endif

#undef __IS_COMPILER_ARM_COMPILER__
#if defined(__IS_COMPILER_ARM_COMPILER_5__) && __IS_COMPILER_ARM_COMPILER_5__   \
||  defined(__IS_COMPILER_ARM_COMPILER_6__) && __IS_COMPILER_ARM_COMPILER_6__

#   define __IS_COMPILER_ARM_COMPILER__         1

#endif

#undef  __IS_COMPILER_LLVM__
#if defined(__clang__) && !__IS_COMPILER_ARM_COMPILER_6__
#   define __IS_COMPILER_LLVM__                 1
#define __PETITE_COMPILER_IS__  _petite_COMPILER_LLVM
#else
//! \note for gcc
#   undef __IS_COMPILER_GCC__
#   if defined(__GNUC__) && !(  defined(__IS_COMPILER_ARM_COMPILER__)           \
                            ||  defined(__IS_COMPILER_LLVM__)                   \
                            ||  defined(__IS_COMPILER_IAR__))
#       define __IS_COMPILER_GCC__              1
#define __PETITE_COMPILER_IS__  _petite_COMPILER_GCC
#   endif
//! @}
#endif

#if __IS_COMPILER_ARM_COMPILER_6__
#   ifndef __MICROLIB
__asm(".global __ARM_use_no_argv\n\t");
#   endif
#endif

#if __IS_COMPILER_ARM_COMPILER_6__
    __asm(".global __use_no_semihosting");
#elif __IS_COMPILER_ARM_COMPILER_5__
    #pragma import(__use_no_semihosting)
#endif

/*------------------------------------------------------*/
#include "log.h"
#include "maths.h"
#include "petite_def.h"
#include "stimer.h"
#include "partition.h"
#include "vfs.h"

#include "mem/p_malloc.h"

#include "mem/p_kfifo.h"
#include "mem/p_list.h"

#include "bus/bus_i2c.h"
#include "bus/bus_vi2c.h"

#include "bus/bus_spi.h"
#include "bus/bus_vspi.h"

#include "bus/bus_uart.h"

#define LOG_PETITE_TAG "PETITE"
#define LogPetite(l,args...) petite_log(l, LOG_PETITE_TAG, NULL,__FUNCTION__, __LINE__, ##args);


int petite_add_loop(char *name, void *cb, uint32_t periodic);

#endif

