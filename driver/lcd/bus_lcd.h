#ifndef _DEV_LCDBUS_H_
#define _DEV_LCDBUS_H_


#include "drv_lcd.h"


typedef struct {

    s32 (*bl)(DevLcdNode *node, u8 sta);
    s32 (*rst)(DevLcdNode *node, u8 sta);
    DevLcdNode *(*open)(DevLcdNode *name);
    s32 (*close)(DevLcdNode *node);
    s32 (*write_data)(DevLcdNode *node, u8 *data, u32 len);
    s32 (*w_data)(DevLcdNode *lcd, u16 color, u32 len);
    s32 (*read_data)(DevLcdNode *node, u8 *data, u32 len);
    s32 (*write_cmd)(DevLcdNode *node, u16 cmd);
}BusLcdDrv;


extern BusLcdDrv BusLcd8080Drv;
extern BusLcdDrv BusLcdSpiDrv;
extern BusLcdDrv BusLcdI2cDrv;
#endif

