
/**
 * @file component_src.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-12-09
 * 
 * @copyright Copyright (c) 2023
 * 
 */
/*
    mdk 也就是keil，工程管理不方便。
    因此把用到的源码包含在一个.c。
    这样做，有不好之处，就是编译后，只有一个component_src文件，
    在map中也看不到其他文件的情况。
*/

#include "log.h"

/**
 * 存储类
*/
/**---------------------fatfs----------------------------*/
#include "..\..\components\FatFs\diskio.c"
#include "..\..\components\FatFs\ff.c"
#include "..\..\components\FatFs\ff_gen_drv.c"
#include "..\..\components\FatFs\ffunicode.c"
#include "..\..\components\FatFs\option\cc936.c"
#include "..\..\components\FatFs\option\syscall.c"
/**---------------------littlefs---------------------------*/
#include "../../components/littlefs/lfs.c"
#include "../../components/littlefs/lfs_util.c"
/**--------------------FlashDB-------------------------*/
//port
#include "../../components/FlashDB/porting/fal.c"
#include "../../components/FlashDB/porting/flashdb_demo.c"
//samples
#include "../../components/FlashDB/samples/kvdb_basic_sample.c"
#include "../../components/FlashDB/samples/kvdb_type_blob_sample.c"
#include "../../components/FlashDB/samples/kvdb_type_string_sample.c"
#include "../../components/FlashDB/samples/tsdb_sample.c"
//src
#include "../../components/FlashDB/src/fdb.c"
#include "../../components/FlashDB/src/fdb_file.c"
#include "../../components/FlashDB/src/fdb_kvdb.c"
#include "../../components/FlashDB/src/fdb_tsdb.c"
#include "../../components/FlashDB/src/fdb_utils.c"
//tests
#include "../../components/FlashDB/tests/fdb_kvdb_tc.c"
#include "../../components/FlashDB/tests/fdb_tsdb_tc.c"

/**
 * 显示类
 * 
 */
/**-------------------petite font------------------------*/
#include "..\..\components\Petitefont\font_6x12.c"
#include "..\..\components\Petitefont\font_8x8.c"
#include "..\..\components\Petitefont\font_asc_6x12.c"
#include "..\..\components\Petitefont\font_8x16.c"
#include "..\..\components\Petitefont\conv.c"

#include "..\..\components\Petitefont\font_unicode.c"
#include "..\..\components\Petitefont\font_wjq.c"
#include "..\..\components\Petitefont\petite_font.c"
#include "..\..\components\Petitefont\font_ymy.c"

/**-------------------emenu------------------------*/
//#include "../../components/emenu/emenu.c"
//#include "../../components/emenu/emenu_test.c"

/**-------------------qfsm-----------------------*/
#include "../../components/qfsm/qfsm.c"
/**-------------------free rtos------------------------*/
#include "..\freertos_port\port.c"
//#include "..\..\components\FreeRTOS\Source\portable\MemMang\heap_4.c"

#include "..\..\components\FreeRTOS\Source\timers.c"
#include "..\..\components\FreeRTOS\Source\tasks.c"
#include "..\..\components\FreeRTOS\Source\stream_buffer.c"
#include "..\..\components\FreeRTOS\Source\queue.c"
#include "..\..\components\FreeRTOS\Source\list.c"
#include "..\..\components\FreeRTOS\Source\event_groups.c"
#include "..\..\components\FreeRTOS\Source\croutine.c"

#include "../../components/FreeRTOS/Source/CMSIS_RTOS_V2/cmsis_os2.c"