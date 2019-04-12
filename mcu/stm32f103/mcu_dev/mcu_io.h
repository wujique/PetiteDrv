#ifndef _MCU_BSP_H_
#define _MCU_BSP_H_

#include "mcu_timer.h"

typedef enum{
	MCU_PORT_NULL = 0x00,
	MCU_PORT_A,
	MCU_PORT_B,
	MCU_PORT_C,
	MCU_PORT_D,
	MCU_PORT_E,
	MCU_PORT_F,
	MCU_PORT_G,
	MCU_PORT_MAX
}MCU_PORT;


extern const GPIO_TypeDef *Stm32PortList[MCU_PORT_MAX];

extern void mcu_io_init(void);
extern void mcu_io_config_in(MCU_PORT port, u16 pin);
extern void mcu_io_config_out(MCU_PORT port, u16 pin);
extern s32 mcu_io_config_timer(MCU_PORT port, u16 pin, McuTimerNum timer);

extern void mcu_io_output_setbit(MCU_PORT port, u16 pin);
extern void mcu_io_output_resetbit(MCU_PORT port, u16 pin);
extern u8 mcu_io_input_readbit(MCU_PORT port, u16 pin);
extern u16 mcu_io_input_readport(MCU_PORT port);


#endif

