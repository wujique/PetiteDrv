#ifndef __BUS_VSPI_H_
#define __BUS_VSPI_H_

extern s32 bus_vspi_init(const DevSpi *dev);
extern s32 bus_vspi_open(DevSpiNode *node, SPI_MODE mode, u16 pre);
extern s32 bus_vspi_close(DevSpiNode *node);
extern s32 bus_vspi_transfer(DevSpiNode *node, u8 *snd, u8 *rsv, s32 len);

#endif

