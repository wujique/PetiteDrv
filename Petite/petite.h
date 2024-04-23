#ifndef __PETITE_H__
#define __PETITE_H__

#include "log.h"
#include "maths.h"
#include "petite_def.h"
#include "stimer.h"
#include "partition.h"
#include "vfs.h"

#include "mem/p_malloc.h"

#include "mem/p_kfifo.h"
#include "mem/p_list.h"


#include "petite_font.h"


#include "bus/bus_i2c.h"
#include "bus/bus_vi2c.h"

#include "bus/bus_spi.h"
#include "bus/bus_vspi.h"

#include "bus/bus_uart.h"

#define LOG_PETITE_TAG "PETITE"
#define LogPetite(l,args...) petite_log(l, LOG_PETITE_TAG, NULL,__FUNCTION__, __LINE__, ##args);

/** 将一个函数加入petite任务轮询 */
int petite_add_loop(char *name, void *cb, uint32_t periodic);

#endif

