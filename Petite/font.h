/*
 *  font.h -- `Soft' font definitions
 *
 *  Created 1995 by Geert Uytterhoeven
 *
 *  This file is subject to the terms and conditions of the GNU General Public
 *  License.  See the file COPYING in the main directory of this archive
 *  for more details.
 */

#ifndef _VIDEO_FONT_H
#define _VIDEO_FONT_H

/* github 上有一个 DisplayCore, 计划参考它来做字库管理 */
struct fbcon_font_desc 
{
    char *name;//字库名称
	char *path;//字库路径
    u16 width, height;//字库宽度与高度
	u16 size;//每个字符字节数
};

extern struct fbcon_font_desc font_vga_8x8;
extern struct fbcon_font_desc font_vga_8x16;
extern struct fbcon_font_desc font_vga_6x12;

extern s32 font_get_asc(char *font, u8 *ch, u8 *buf);
extern s32 font_get_hz(char *font, u8 *ch, u8 *buf);
extern s32 font_get_hw(char *font, u16 *h, u16 *w);

#endif /* _VIDEO_FONT_H */

