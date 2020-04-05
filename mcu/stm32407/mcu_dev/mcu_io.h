#ifndef _MCU_BSP_H_
#define _MCU_BSP_H_


extern const GPIO_TypeDef *Stm32PortList[MCU_PORT_MAX];

extern void mcu_io_init(void);
extern void mcu_io_config_in(MCU_PORT port, MCU_IO pin);
extern void mcu_io_config_out(MCU_PORT port, MCU_IO pin);
extern s32 mcu_io_config_timer(MCU_PORT port, MCU_IO pin, McuTimerNum timer);
extern void mcu_io_output_setbit(MCU_PORT port, MCU_IO pin);
extern void mcu_io_output_resetbit(MCU_PORT port, MCU_IO pin);
extern u8 mcu_io_input_readbit(MCU_PORT port, MCU_IO pin);
extern u16 mcu_io_input_readport(MCU_PORT port);

#endif

