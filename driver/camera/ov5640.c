

#include "mcu.h"
#include "log.h"
#include "petite_config.h"
#include "board_sysconf.h"
#include "bus_i2c.h"
#include "mcu.h"


#include "ov5640.h"

#include "ov5640cfg.c"
#include "ov5640af.c"			 	


const uint16_t jpeg_size_tbl[][2]=
{     
	160, 120,	//QQVGA
	176, 144,	//QCIF
	320, 240,	//QVGA
	400, 240,	//WQVGA
	352, 288,	//CIF
	640, 480,	//VGA
	800, 600,	//SVGA   
};

const uint32_t jpeg_buf_max_size[]=
{     
	20*1024,	//QQVGA
	30*1024,	//QCIF
	100*1024,	//QVGA
	100*1024,	//WQVGA
	100*1024,	//CIF
	200*1024,	//VGA
	400*1024,	//SVGA   
};

DevI2cNode *node;

static uint8_t OV5640_I2C_open(void)
{
	 node = bus_i2c_open(DEV_CAMERA_I2CBUS, 0xffffffff, OV5640_I2C_CLK);
	 return 0;
}

static uint8_t OV5640_I2C_close(void)
{
	 bus_i2c_close(node);
	 return 0;
}

static uint8_t OV5640_WriteReg(uint16_t reg, uint8_t Data)
{

	uint8_t res;

	uint8_t buf[3];


	buf[0] = reg>>8;
	buf[1] = reg&0xff;
	buf[2] = Data;
	res  = 	bus_i2c_transfer(node, OV5640_ADDR, MCU_I2C_MODE_W, buf, 3);
	
	return res;
}

static uint8_t OV5640_ReadReg(uint16_t reg)
{
	uint8_t buf[2];
	
	buf[0] = reg>>8;
	buf[1] = reg&0xff;

	bus_i2c_transfer(node, OV5640_ADDR, MCU_I2C_MODE_W, buf, 2);
	bus_i2c_transfer(node, OV5640_ADDR, MCU_I2C_MODE_R, buf, 1);

	return buf[0];

}
/*
	通过读ID判断是否是OV5640
	*/
int OV5640_Init(void)
{ 
	uint16_t i=0;
	uint16_t reg;
        
    HAL_Delay(30); 

	OV5640_I2C_open();
	reg=OV5640_ReadReg(OV5640_CHIPIDH);	
	reg<<=8;
	reg|=OV5640_ReadReg(OV5640_CHIPIDL);

	uart_printf("ov5640 read id:%x\r\n", reg);
	if(reg!=OV5640_ID) {
		OV5640_I2C_close();
		return -1;
	}  
	#if 1
	/* 初始化后，工作电流很大 */
	OV5640_WriteReg(0x3103,0X11);//system clock from pad, bit[1]
	OV5640_WriteReg(0X3008,0X82);	
	HAL_Delay(10);
        
	for(i=0;i<sizeof(ov5640_init_reg_tbl)/4;i++) {
		OV5640_WriteReg(ov5640_init_reg_tbl[i][0],ov5640_init_reg_tbl[i][1]);
	} 
	#endif
	OV5640_I2C_close();
	
	HAL_Delay(50); 
	// Test for flash light
	OV5640_Flash_Lamp(1);
	HAL_Delay(50); 
	OV5640_Flash_Lamp(0);        

	OV5640_RGB565_Mode();	
	OV5640_Focus_Init(); 	
	OV5640_Light_Mode(0);	   //set auto
	OV5640_Color_Saturation(3); //default
	OV5640_Brightness(4);	//default
	OV5640_Contrast(3);     //default
	OV5640_Sharpness(33);	//set auto
	OV5640_Auto_Focus();
	
	return 0;
}  

void OV5640_JPEG_Mode(void) 
{
	uint16_t i=0; 
	
	OV5640_I2C_open();
	for (i=0;i<(sizeof(OV5640_jpeg_reg_tbl)/4);i++) {
		OV5640_WriteReg(OV5640_jpeg_reg_tbl[i][0],OV5640_jpeg_reg_tbl[i][1]);  
	}  
	OV5640_I2C_close();
}

void OV5640_RGB565_Mode(void) 
{
	uint16_t i=0;
	OV5640_I2C_open();
	for (i=0;i<(sizeof(ov5640_rgb565_reg_tbl)/4);i++) {
		OV5640_WriteReg(ov5640_rgb565_reg_tbl[i][0],ov5640_rgb565_reg_tbl[i][1]); 
	} 
	OV5640_I2C_close();
} 

const static uint8_t OV5640_EXPOSURE_TBL[7][6]=
{
    0x10,0x08,0x10,0x08,0x20,0x10,//-3  
    0x20,0x18,0x41,0x20,0x18,0x10,//-2
    0x30,0x28,0x61,0x30,0x28,0x10,//-1 
    0x38,0x30,0x61,0x38,0x30,0x10,//0  
    0x40,0x38,0x71,0x40,0x38,0x10,//+1 
    0x50,0x48,0x90,0x50,0x48,0x20,//+2   
    0x60,0x58,0xa0,0x60,0x58,0x20,//+3    
};

//exposure: 0 - 6, 快门
void OV5640_Exposure(uint8_t exposure)
{
	OV5640_I2C_open();
        OV5640_WriteReg(0x3212,0x03);	//start group 3
        OV5640_WriteReg(0x3a0f,OV5640_EXPOSURE_TBL[exposure][0]); 
        OV5640_WriteReg(0x3a10,OV5640_EXPOSURE_TBL[exposure][1]); 
        OV5640_WriteReg(0x3a1b,OV5640_EXPOSURE_TBL[exposure][2]); 
        OV5640_WriteReg(0x3a1e,OV5640_EXPOSURE_TBL[exposure][3]); 
        OV5640_WriteReg(0x3a11,OV5640_EXPOSURE_TBL[exposure][4]); 
        OV5640_WriteReg(0x3a1f,OV5640_EXPOSURE_TBL[exposure][5]); 
        OV5640_WriteReg(0x3212,0x13); //end group 3
        OV5640_WriteReg(0x3212,0xa3); //launch group 3
        OV5640_I2C_close();
}

const static uint8_t OV5640_LIGHTMODE_TBL[5][7]=
{ 
	0x04,0X00,0X04,0X00,0X04,0X00,0X00,//Auto 
	0x06,0X1C,0X04,0X00,0X04,0XF3,0X01,//Sunny
	0x05,0X48,0X04,0X00,0X07,0XCF,0X01,//Office
	0x06,0X48,0X04,0X00,0X04,0XD3,0X01,//Cloudy
	0x04,0X10,0X04,0X00,0X08,0X40,0X01,//Home
}; 

// light mode:
//      0: auto
//      1: sunny
//      2: office
//      3: cloudy
//      4: home
void OV5640_Light_Mode(uint8_t mode)
{
	uint8_t i;

	OV5640_I2C_open();
	OV5640_WriteReg(0x3212,0x03);	//start group 3
	for(i=0;i<7;i++)
		OV5640_WriteReg(0x3400+i,OV5640_LIGHTMODE_TBL[mode][i]); 
	OV5640_WriteReg(0x3212,0x13); //end group 3
	OV5640_WriteReg(0x3212,0xa3); //launch group 3	
	OV5640_I2C_close();
}

const static uint8_t OV5640_SATURATION_TBL[7][6]=
{ 
	0X0C,0x30,0X3D,0X3E,0X3D,0X01,//-3 
	0X10,0x3D,0X4D,0X4E,0X4D,0X01,//-2	
	0X15,0x52,0X66,0X68,0X66,0X02,//-1	
	0X1A,0x66,0X80,0X82,0X80,0X02,//+0	
	0X1F,0x7A,0X9A,0X9C,0X9A,0X02,//+1	
	0X24,0x8F,0XB3,0XB6,0XB3,0X03,//+2
	0X2B,0xAB,0XD6,0XDA,0XD6,0X04,//+3
}; 


// Color Saturation: 
//   sat:  0 - 6 
void OV5640_Color_Saturation(uint8_t sat)
{ 
	uint8_t i;

	OV5640_I2C_open();
	OV5640_WriteReg(0x3212,0x03);	//start group 3
	OV5640_WriteReg(0x5381,0x1c);
	OV5640_WriteReg(0x5382,0x5a);
	OV5640_WriteReg(0x5383,0x06);
	for(i=0;i<6;i++)  
		OV5640_WriteReg(0x5384+i,OV5640_SATURATION_TBL[sat][i]);    
	OV5640_WriteReg(0x538b, 0x98);
	OV5640_WriteReg(0x538a, 0x01);
	OV5640_WriteReg(0x3212, 0x13); //end group 3
	OV5640_WriteReg(0x3212, 0xa3); //launch group 3	
	OV5640_I2C_close();
}

//Brightness
//     bright:  0 - 8
void OV5640_Brightness(uint8_t bright)
{
	uint8_t brtval;
	if(bright<4)brtval=4-bright;
	else brtval=bright-4;
	OV5640_I2C_open();
	OV5640_WriteReg(0x3212,0x03);	//start group 3
	OV5640_WriteReg(0x5587,brtval<<4);
	if(bright<4)OV5640_WriteReg(0x5588,0x09);
	else OV5640_WriteReg(0x5588,0x01);
	OV5640_WriteReg(0x3212,0x13); //end group 3
	OV5640_WriteReg(0x3212,0xa3); //launch group 3
	OV5640_I2C_close();
}

//Contrast:
//     contrast:  0 - 6
void OV5640_Contrast(uint8_t contrast)
{
	uint8_t reg0val=0X00;
	uint8_t reg1val=0X20;
	switch(contrast)
	{
		case 0://-3
			reg1val=reg0val=0X14;	 	 
			break;	
		case 1://-2
			reg1val=reg0val=0X18; 	 
			break;	
		case 2://-1
			reg1val=reg0val=0X1C;	 
			break;	
		case 4://1
			reg0val=0X10;	 	 
			reg1val=0X24;	 	 
			break;	
		case 5://2
			reg0val=0X18;	 	 
			reg1val=0X28;	 	 
			break;	
		case 6://3
			reg0val=0X1C;	 	 
			reg1val=0X2C;	 	 
			break;	
	} 
	OV5640_I2C_open();
	OV5640_WriteReg(0x3212,0x03); //start group 3
	OV5640_WriteReg(0x5585,reg0val);
	OV5640_WriteReg(0x5586,reg1val); 
	OV5640_WriteReg(0x3212,0x13); //end group 3
	OV5640_WriteReg(0x3212,0xa3); //launch group 3
	OV5640_I2C_close();
}
// Sharpness:
//    sharp: 0 - 33   (0: close , 33: auto , other: Sharpness)

void OV5640_Sharpness(uint8_t sharp)
{
	OV5640_I2C_open();
	if (sharp<33) {
		OV5640_WriteReg(0x5308,0x65);
		OV5640_WriteReg(0x5302,sharp);
	} else {
		OV5640_WriteReg(0x5308,0x25);
		OV5640_WriteReg(0x5300,0x08);
		OV5640_WriteReg(0x5301,0x30);
		OV5640_WriteReg(0x5302,0x10);
		OV5640_WriteReg(0x5303,0x00);
		OV5640_WriteReg(0x5309,0x08);
		OV5640_WriteReg(0x530a,0x30);
		OV5640_WriteReg(0x530b,0x04);
		OV5640_WriteReg(0x530c,0x06);
	}
	OV5640_I2C_close();
}

const static uint8_t OV5640_EFFECTS_TBL[7][3]=
{ 
	0X06,0x40,0X10,//正常 
	0X1E,0xA0,0X40,//冷色
	0X1E,0x80,0XC0,//暖色
	0X1E,0x80,0X80,//黑白
	0X1E,0x40,0XA0,//泛黄 
	0X40,0x40,0X10,//反色
	0X1E,0x60,0X60,//偏绿
}; 
//特效设置
//0:正常    
//1,冷色
//2,暖色   
//3,黑白
//4,偏黄
//5,反色
//6,偏绿	    
void OV5640_Special_Effects(uint8_t eft)
{ 
	OV5640_I2C_open();
	OV5640_WriteReg(0x3212,0x03); //start group 3
	OV5640_WriteReg(0x5580,OV5640_EFFECTS_TBL[eft][0]);
	OV5640_WriteReg(0x5583,OV5640_EFFECTS_TBL[eft][1]);// sat U
	OV5640_WriteReg(0x5584,OV5640_EFFECTS_TBL[eft][2]);// sat V
	OV5640_WriteReg(0x5003,0x08);
	OV5640_WriteReg(0x3212,0x13); //end group 3
	OV5640_WriteReg(0x3212,0xa3); //launch group 3
	OV5640_I2C_close();
}
//测试序列
//mode:0,关闭
//     1,彩条 
//     2,色块

// Flash Lamp
//  sw:  0: off
//       1:  on
void OV5640_Flash_Lamp(uint8_t sw)
{
	OV5640_I2C_open();
	OV5640_WriteReg(0x3016,0X02);
	OV5640_WriteReg(0x301C,0X02); 
	if(sw)
		OV5640_WriteReg(0X3019,0X02); 
	else 
		OV5640_WriteReg(0X3019,0X00);
	OV5640_I2C_close();
} 

// set the output size
uint8_t OV5640_OutSize_Set(uint16_t offx,uint16_t offy,uint16_t width,uint16_t height)
{ 
	OV5640_I2C_open();
        OV5640_WriteReg(0X3212,0X03);  	

        OV5640_WriteReg(0x3808,width>>8);	
        OV5640_WriteReg(0x3809,width&0xff);
        OV5640_WriteReg(0x380a,height>>8);
        OV5640_WriteReg(0x380b,height&0xff);

        OV5640_WriteReg(0x3810,offx>>8);	
        OV5640_WriteReg(0x3811,offx&0xff);

        OV5640_WriteReg(0x3812,offy>>8);	
        OV5640_WriteReg(0x3813,offy&0xff);

        OV5640_WriteReg(0X3212,0X13);		
        OV5640_WriteReg(0X3212,0Xa3);
		OV5640_I2C_close();
		/*此处不加延时就有问题，具体问题没高清楚 */
		HAL_Delay(20);

        return 0; 
}

uint8_t FoucusInit = 0;

uint8_t OV5640_Focus_Init(void)
{ 
	uint16_t i; 
	uint16_t addr=0x8000;
	uint8_t state=0x8F;
	
	OV5640_I2C_open();
	OV5640_WriteReg(0x3000, 0x20);	//reset 	 
	for(i=0;i<sizeof(OV5640_AF_Config);i++) {
		OV5640_WriteReg(addr,OV5640_AF_Config[i]);
		addr++;
	}  
	OV5640_WriteReg(0x3022,0x00);
	OV5640_WriteReg(0x3023,0x00);
	OV5640_WriteReg(0x3024,0x00);
	OV5640_WriteReg(0x3025,0x00);
	OV5640_WriteReg(0x3026,0x00);
	OV5640_WriteReg(0x3027,0x00);
	OV5640_WriteReg(0x3028,0x00);
	OV5640_WriteReg(0x3029,0x7f);
	OV5640_WriteReg(0x3000,0x00); 
	OV5640_I2C_close();
	
	i=0;
	do {
		OV5640_I2C_open();
		state=OV5640_ReadReg(0x3029);
		OV5640_I2C_close();
		
		HAL_Delay(5);
		i++;
		if(i>1000)return 1;
	} while(state!=0x70); 
	return 0;    
}  

uint8_t OV5640_Auto_Focus(void)
{
	uint8_t temp=0;   
	uint16_t retry=0; 

	OV5640_I2C_open();
	OV5640_WriteReg(0x3023,0x01);
	OV5640_WriteReg(0x3022,0x08);
	OV5640_I2C_close();
	do {
		OV5640_I2C_open();
		temp=OV5640_ReadReg(0x3023); 
		OV5640_I2C_close();
		retry++;
		if(retry>1000)return 2;
		HAL_Delay(5);
	} while(temp!=0x00); 
		
	OV5640_I2C_open();
	OV5640_WriteReg(0x3023,0x01);
	OV5640_WriteReg(0x3022,0x04);
	OV5640_I2C_close();
	retry=0;
	do {
		OV5640_I2C_open();
		temp=OV5640_ReadReg(0x3023); 
		OV5640_I2C_close();
		retry++;
		if(retry>1000)return 2;
		HAL_Delay(5);
	}while(temp!=0x00);
	return 0;
} 
/*
	slow down OV5640 clocks ,adapt to the refresh rate of the LCD
	配置 SC PLL,降低时钟，适配LCD帧率
	type =1, 480*272的RGB屏幕
	type = 2, 800*480的屏幕
	type = 3, 1000*600RGB屏幕
	
	注意：
		具体如何匹配，还没搞清楚
		改变时钟，需要延时，具体怎么延时还没搞清楚，
		目前能用。
	*/
int OV5640_SET_SCPLL(uint8_t type)
{
	HAL_Delay(20);

	OV5640_I2C_open();
	/* 帧率要配置，如果跟LCD不匹配，LCD显示花屏 */
			
	if (type == 1) {
		/* 30 帧 480*272的屏幕*/
		OV5640_WriteReg(0x3035,0X21);
		OV5640_WriteReg(0x3036,0x98);
	} else if (type == 2) {
		/* 15 帧：800*480的屏幕 */
		OV5640_WriteReg(0x3035,0X41); 
		OV5640_WriteReg(0x3036,0x69);
	} else if (type == 3) {
		OV5640_WriteReg(0x3035,0X31);  
        OV5640_WriteReg(0x3036,0xE0); 
	} else {
		OV5640_WriteReg(0x3035,0X21);
		OV5640_WriteReg(0x3036,0x98);
	}
	OV5640_I2C_close();
	return 0;
}
/*
	DMA 有点复杂
	请参考：https://www.cnblogs.com/armfly/p/12174976.html

	*/
#if 0
#include "board_sysconf.h"

extern DMA_HandleTypeDef hdma_dcmi_pssi;
extern DCMI_HandleTypeDef hdcmi;

extern uint32_t RGB565_480x272[LCD_WIDTH*LCD_HEIGHT/2];

int OV5640_RGB(void)
{


	/* DCMI DMA DeInit */
	HAL_DMA_DeInit(&hdma_dcmi_pssi);

	/* DCMI DMA Init */
	/* DCMI Init */
	/* DCMI DMA Init */
	/* DCMI_PSSI Init */
	hdma_dcmi_pssi.Instance = DMA1_Stream0;
	hdma_dcmi_pssi.Init.Request = DMA_REQUEST_DCMI_PSSI;
	hdma_dcmi_pssi.Init.Direction = DMA_PERIPH_TO_MEMORY;
	hdma_dcmi_pssi.Init.PeriphInc = DMA_PINC_DISABLE;
	hdma_dcmi_pssi.Init.MemInc = DMA_MINC_ENABLE;

	/* PeriphDataAlignment 就是DCMI的DR，对齐方式（长度）是WORD
	MemDataAlignment则是目标地址，在这里是LCD显存
	对齐方式要和定义的一致，比如本例程定义的是4字节对齐，如下
	__align(4)
	uint32_t RGB565_480x272[LCD_WIDTH*LCD_HEIGHT/2];
	所以DMA可以用WORD。
	假如，DMA源地址和目的对齐不一样，还可以通过使用DMA的FIFO来进行对齐

	*/
	hdma_dcmi_pssi.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
	hdma_dcmi_pssi.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;

	hdma_dcmi_pssi.Init.Mode = DMA_CIRCULAR;
	hdma_dcmi_pssi.Init.Priority = DMA_PRIORITY_HIGH;
	/*使用直接传输方式，不使用FIFO*/
	hdma_dcmi_pssi.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	hdma_dcmi_pssi.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;

	hdma_dcmi_pssi.Init.MemBurst = DMA_MBURST_SINGLE;
	hdma_dcmi_pssi.Init.PeriphBurst = DMA_PBURST_SINGLE;
	if (HAL_DMA_Init(&hdma_dcmi_pssi) != HAL_OK)
	{
		Error_Handler();
	}

	/* Several peripheral DMA handle pointers point to the same DMA handle.
	Be aware that there is only one channel to perform all the requested DMAs. */
	__HAL_LINKDMA(&hdcmi,DMA_Handle,hdma_dcmi_pssi);
	/*别改，HAL库要用中断 */	
	__HAL_DCMI_ENABLE_IT(&hdcmi, DCMI_IT_FRAME); 
	
	// hdma_dcmi_pssi.XferHalfCpltCallback = dcmi_dma_Half_TC_callback;                     
	//hdma_dcmi_pssi.XferCpltCallback = dcmi_dma_Full_TC_callback;

	HAL_StatusTypeDef status;
	/* Start the Camera capture : 
	注意， Length长度是 源数据个数，在DCMI上来说，源是DCMI的DR寄存器，是WORD类型
	摄像头RGB模式数据实际是HALFWORD，像素是800*480，所以，传输word数要除2 */
	status = HAL_DCMI_Start_DMA(&hdcmi, DCMI_MODE_CONTINUOUS, (u32)RGB565_480x272, LCD_WIDTH*LCD_HEIGHT/2);
	uart_printf("status:%d\r\n", status);

}
#endif

