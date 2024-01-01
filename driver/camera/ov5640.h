#ifndef _OV5640_H
#define _OV5640_H


#define OV5640_I2C_CLK			20
#define OV5640_ID               0X5640  
#define OV5640_ADDR        		0X3c		
#define OV5640_CHIPIDH          0X300A  	
#define OV5640_CHIPIDL          0X300B  


#define   JPEG_BUF_SIZE_MAX     400*1024    

#define   QQVGA_160_120    0
#define   QCIF_176_144     1
#define   QVGA_320_240     2
#define   WQVGA_400_240    3
#define   CIF_352_288      4
#define   VGA_640_480      5
#define   SVGA_800_600     6

//#define   XSIZE      1024       
//#define   YSIZE      600
#define   XSIZE      480       
#define   YSIZE      272
 

extern const uint16_t jpeg_size_tbl[][2];
extern const uint32_t jpeg_buf_max_size[];


int OV5640_Init(void);  
void OV5640_JPEG_Mode(void);
void OV5640_RGB565_Mode(void);
void OV5640_Exposure(uint8_t exposure);
void OV5640_Light_Mode(uint8_t mode);
void OV5640_Color_Saturation(uint8_t sat);
void OV5640_Brightness(uint8_t bright);
void OV5640_Contrast(uint8_t contrast);
void OV5640_Sharpness(uint8_t sharp);
void OV5640_Special_Effects(uint8_t eft);
void OV5640_Test_Pattern(uint8_t mode);
void OV5640_Flash_Lamp(uint8_t sw);
uint8_t OV5640_OutSize_Set(uint16_t offx,uint16_t offy,uint16_t width,uint16_t height);
uint8_t OV5640_Focus_Init(void);
uint8_t OV5640_Auto_Focus(void);
void rgb565_test(void);
void jpeg_test(uint8_t jpg_size);

int OV5640_SET_SCPLL(uint8_t type);

#endif





















