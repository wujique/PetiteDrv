#ifndef _DEV_LCD_H_
#define _DEV_LCD_H_


#include "petite_def.h"
#include "petite.h"



extern s32 display_lcd_put_string(DevLcdNode *lcd, petite_font_t *font, int x, int y, char *s, unsigned colidx);


#endif

