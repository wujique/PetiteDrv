#ifndef __MCU_TIMER_H__
#define __MCU_TIMER_H__

/*
	硬定时器定义
	407 有多少个定时器?
*/
typedef enum
{
	MCU_TIMER_NULL = 0,
	MCU_TIMER_1,
	MCU_TIMER_2,
	MCU_TIMER_3,
	MCU_TIMER_4,
	MCU_TIMER_5,
	MCU_TIMER_6,
	MCU_TIMER_7,
	MCU_TIMER_8,
	MCU_TIMER_MAX,
}McuTimerNum;

typedef enum
{
	MCU_TIMER_1US = 1,
	MCU_TIMER_10US,
	MCU_TIMER_100US,
	MCU_TIMER_500US,
	MCU_TIMER_100NS,
	MCU_TIMER_DEF_MAX
}McuTimerTickDef;
	
typedef enum
{
	MCU_TIMER_RE = 0,/*重复*/
	MCU_TIMER_ONE,/*单次*/
}McuTimerReType;
	
typedef enum
{
	MCU_TIMER_CH1 = 0,
	MCU_TIMER_CH2,
	MCU_TIMER_CH3,
	MCU_TIMER_CH4
}McuTimerCh;

typedef enum
{
	MCU_TIMER_CAP_RISING = 0,
	MCU_TIMER_CAP_FALLING,
	MCU_TIMER_CAP_BOTHEDGE,
}McuTimerCapEdge;


extern s32 mcu_timer_init(McuTimerNum timer);
extern s32 mcu_timer_config(McuTimerNum timer, McuTimerTickDef tickdef, u32 tick, void (*callback)(void), u8 type);
extern s32 mcu_timer_start(McuTimerNum timer);
extern s32 mcu_timer_stop(McuTimerNum timer);

extern s32 mcu_timer_pwm_init(McuTimerNum timer, McuTimerTickDef tickdef, u32 tick, u8 duty, McuTimerCh ch);

extern s32 mcu_timer_cap_init(McuTimerNum timer, McuTimerTickDef tickdef, McuTimerCh ch, McuTimerCapEdge edge);
extern u32 mcu_timer_get_cap(McuTimerNum timer, McuTimerCh ch);

#endif
