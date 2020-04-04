#ifndef __MCU_I2C_H__
#define __MCU_I2C_H__

extern void SCCB_GPIO_Config(I2C_TypeDef* I2CDef);
extern uint8_t bus_sccb_writereg(I2C_TypeDef* I2CDef, uint8_t DeviceAddr, uint16_t Addr, uint8_t Data);
extern uint8_t bus_sccb_readreg(I2C_TypeDef* I2CDef, uint8_t DeviceAddr, uint16_t Addr);

#endif
