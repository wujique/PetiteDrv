#ifndef _MCU_DCMI_H_
#define _MCU_DCMI_H_

#define DCMI_FLAG_BUF0 	0X01
#define DCMI_FLAG_BUF1	0X02
#define DCMI_FLAG_FRAME	0X04


extern void BUS_DCMI_Config(u8 mode, u16 pck, u16 vs, u16 hs);
extern void BUS_DCMI_DMA_Init(u8 mode, u32 Memory0BaseAddr, u32 Memory1BaseAddr, u32 BufferSize);
extern void MCO1_Init(void);

#endif

