#ifndef __MCU_I2C_H__
#define __MCU_I2C_H__

extern void SCCB_GPIO_Config(void);
extern uint8_t bus_sccb_writereg(uint8_t DeviceAddr, uint16_t Addr, uint8_t Data);
extern uint8_t bus_sccb_readreg(uint8_t DeviceAddr, uint16_t Addr);

#endif

