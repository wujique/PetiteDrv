#ifndef __MCU_SPI_H_
#define __MCU_SPI_H_

extern s32 mcu_hspi_init(const DevSpi *dev);
extern s32 mcu_hspi_open(DevSpiNode *node, SPI_MODE mode, u16 KHz);
extern s32 mcu_hspi_close(DevSpiNode *node);
extern s32 mcu_hspi_transfer(DevSpiNode *node, u8 *snd, u8 *rsv, s32 len);

#endif


