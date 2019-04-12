#ifndef __BUS_VI2C_H_
#define __BUS_VI2C_H_


extern s32 mcu_vi2c_transfer(DevI2cNode *node, u8 addr, u8 rw, u8* data, s32 datalen);

#endif

