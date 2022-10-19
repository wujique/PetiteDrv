#ifndef _DEV_LCDBUS_H_
#define _DEV_LCDBUS_H_

//#include "mem/p_list.h"

//#include "petite_def.h"
#include "drv_lcd.h"

extern s32 bus_lcd_bl(DevLcdNode *node, u8 sta);
extern s32 bus_lcd_rst(DevLcdNode *node, u8 sta);
extern DevLcdNode *bus_lcd_open(DevLcdNode *name);
extern s32 bus_lcd_close(DevLcdNode *node);
extern s32 bus_lcd_write_data(DevLcdNode *node, u8 *data, u32 len);
extern s32 bus_lcd_flush_data(DevLcdNode *node, u8 *data, u32 len);
extern s32 bus_lcd_flush_wait(DevLcdNode *node);
extern s32 bus_lcd_read_data(DevLcdNode *node, u8 *data, u32 len);
extern s32 bus_lcd_write_cmd(DevLcdNode *node, u16 cmd);
#endif

