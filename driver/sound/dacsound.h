#ifndef _DEV_DACSOUND_H_
#define _DEV_DACSOUND_H_

#include "mcu_timer.h"
#include "mcu_io.h"

/* DAC播放语音设备*/
typedef struct _strDevDacSound
{
	MCU_PORT Port;
	MCU_IO	 Pio;

	McuTimerNum Timer;
	McuTimerCh Ch;
}DevDacSound;

extern s32 dev_dacsound_init(DevDacSound *Dev);
extern s32 dev_dacsound_open(void);
extern s32 dev_dacsound_dataformat(u32 Freq, u8 Standard, u8 Format);
extern s32 dev_dacsound_setbuf(u16 *buffer0,u16 *buffer1,u32 len);
extern s32 dev_dacsound_transfer(u8 sta);
extern s32 dev_dacsound_close(void);


#endif
