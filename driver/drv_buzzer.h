#ifndef __DEV_BUZZER_H__
#define __DEV_BUZZER_H__

#include "mcu.h"

/*蜂鸣器设备定义 */
typedef struct _strDevBuzzer
{
	MCU_PORT Port;
	MCU_IO	 Pio;

	McuTimerNum Timer;
	McuTimerCh Ch;
}DevBuzzer;


extern s32 dev_buzzer_init(DevBuzzer *Buzzer);
extern s32 dev_buzzer_open(void);
extern s32 dev_buzzer_close(void);



#endif

