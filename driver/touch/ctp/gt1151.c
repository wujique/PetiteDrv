#include "mcu.h"
#include "petite.h"
#include "touch.h"

/* when the gt9147 resetting , if int pin is 0, i2c addr= 0x5d; 
						  				1,  0x14
			(7-bit addr) */
#define GT1151_DEVICE_ADDR 0x14//0x5d
/* hardware io define */
#define GT9147_RST_PORT GPIOD
#define GT9147_RST_PIN GPIO_PIN_14

#define GT9147_INT_PORT GPIOD
#define GT9147_INT_PIN GPIO_PIN_9

extern const DevI2c DevVi2c1;
DevI2c *DevVi2cGT1151 = (DevI2c *)&DevVi2c1;

/* reg addr */
#define GT1151_REG_CTRL		0X8040
#define GT1151_REG_CFGS 	0X8050  
#define GT1151_REG_CHECK 	0X813C  
#define GT1151_REG_PID 		0X8140  

#define GT1151_REG_GSTID 	0X814E   	
#define GT1151_REG_TP1 		0X8150  	
#define GT1151_REG_TP2 		0X8158		
#define GT1151_REG_TP3 		0X8160		
#define GT1151_REG_TP4 		0X8168		
#define GT1151_REG_TP5 		0X8170	 

/* config 
	the first byte is version.
	GT1151 配置的是800*480像素屏幕
*/
const u8 GT1151CfgTab[]=
{ 
	0x44,0x20,0x03,0xE0,0x01,0x05,0x35,0x04,0x00,0x08,
	0x09,0x0F,0x55,0x37,0x33,0x11,0x00,0x03,0x08,0x56,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x48,0x00,0x00,
	0x3C,0x08,0x0A,0x28,0x1E,0x50,0x00,0x00,0x82,0xB4,
	0xD2,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x85,0x25,0x11,0x41,0x43,0x31,
	0x0D,0x00,0xAD,0x22,0x24,0x7D,0x1D,0x1D,0x32,0xDF,
	0x4F,0x44,0x0F,0x80,0x2C,0x50,0x50,0x00,0x00,0x00,
	0x00,0xD3,0x00,0x00,0x00,0x00,0x0F,0x28,0x1E,0xFF,
	0xF0,0x37,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x50,0xB4,0xC0,0x94,0x53,0x2D,
	0x0A,0x02,0xBE,0x60,0xA2,0x71,0x8F,0x82,0x80,0x92,
	0x74,0xA3,0x6B,0x01,0x0F,0x14,0x03,0x1E,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x0C,0x0D,0x0E,0x0F,0x10,
	0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,
	0x1D,0x1F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x19,0x18,0x17,
	0x15,0x14,0x13,0x12,0x0C,0x08,0x06,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,
	0xC4,0x09,0x23,0x23,0x50,0x5D,0x54,0x4B,0x3C,0x0F,
	0x32,0xFF,0xE4,0x04,0x40,0x00,0x8A,0x05,0x40,0x00,
	0xAA,0x00,0x22,0x22,0x00,0x00,0xAA,0x05,0x01,

};	

/*
	init gt9147 ic io.

	rst :0 reset
	    1  work
*/
static int gt1151_io_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	HAL_GPIO_WritePin(GT9147_RST_PORT, GT9147_RST_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GT9147_INT_PORT, GT9147_INT_PIN, GPIO_PIN_RESET);

	GPIO_InitStructure.Pin = GT9147_RST_PIN;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
   	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
   	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
   	HAL_GPIO_Init(GT9147_RST_PORT, &GPIO_InitStructure);
	
   	GPIO_InitStructure.Pin = GT9147_INT_PIN;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
   	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
   	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
   	HAL_GPIO_Init(GT9147_INT_PORT, &GPIO_InitStructure);	

	HAL_Delay(10);
	
	HAL_GPIO_WritePin(GT9147_RST_PORT, GT9147_RST_PIN, GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(GT9147_RST_PORT, GT9147_RST_PIN, GPIO_PIN_SET);
	HAL_Delay(10);

	GPIO_InitStructure.Pin = GT9147_INT_PIN;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
   	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
   	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
   	HAL_GPIO_Init(GT9147_INT_PORT, &GPIO_InitStructure);
	HAL_Delay(100);
	return 0;
	
}


static int gt1151_update_cfg(void)
{
	u8 buf[2];
	u8 i=0;
	u8 tmp[4];
	u8 reg[4];
	u16 checksum=0;
	
	uart_printf("gt1151_update_cfg\r\n");
	
	buf[0]=0;
	buf[1]=1;
	
	for(i=0; i<sizeof(GT1151CfgTab)-3; i+=2)
		checksum +=((GT1151CfgTab[i]<<8) | GT1151CfgTab[i+1]);
	checksum =(~checksum)+1;
	
    buf[0]= checksum>>8;
	buf[1]= checksum;
	buf[2]= 0;	//是否写入到FLASH,   即是否掉电保存,0不保存，1 保存

	reg[0] = GT1151_REG_CFGS>>8;
	reg[1] = GT1151_REG_CFGS&0xff;
	mcu_vi2c_transfer_reg(DevVi2cGT1151, GT1151_DEVICE_ADDR, reg, 2, MCU_I2C_MODE_W, (u8*)GT1151CfgTab, sizeof(GT1151CfgTab)-3);
	
	reg[0] = GT1151_REG_CHECK>>8;
	reg[1] = GT1151_REG_CHECK&0xff;
	mcu_vi2c_transfer_reg(DevVi2cGT1151, GT1151_DEVICE_ADDR, reg, 2, MCU_I2C_MODE_W, buf, 3);
	
	return 0;
} 
/*---------------------------------------*/
/* gd */
static int Gt1151Gd = -1;
/*
	1 init hardware
	2 read ID
	3 check & update the config

*/
int gt1151_init(void)
{
	u8 ID[4];
	u8 tmp[4];
	u8 reg[2];
	DevI2cNode *node;

	gt1151_io_init();


	//HAL_Delay(1000);
		
	ID[0] = 0;
	ID[1] = 0;
	ID[2] = 0;
	ID[3] = 0;
	/* read gt9147 ID*/
	reg[0] = GT1151_REG_PID>>8;
	reg[1] = GT1151_REG_PID&0xff;
	node = bus_i2c_open("VI2C1", 0xffffffff);
	if (node ==NULL ) { 
		uart_printf("gt1151 open i2c err!\r\n");
		return -1;
	}
	mcu_vi2c_transfer_reg(DevVi2cGT1151, GT1151_DEVICE_ADDR, reg, 2, MCU_I2C_MODE_R, ID, 4);
	
	uart_printf("gt1151 read ID:%c%c%c%c\r\n", ID[0], ID[1], ID[2], ID[3]);	//"9147", ASC

	if(ID[0] == '1'
		&& ID[1] == '1'
		&& ID[2] == '5'
		&& ID[3] == '8') {
		uart_printf("check gt1151 ok!\r\n");
	} else {
		bus_i2c_close(node);
		uart_printf("err!\r\n");
		return -1;
	}
	
	/* soft reset */
	tmp[0] = 0X02;
	reg[0] = GT1151_REG_CTRL>>8;
	reg[1] = GT1151_REG_CTRL&0xff;
	mcu_vi2c_transfer_reg(DevVi2cGT1151, GT1151_DEVICE_ADDR, reg, 2, MCU_I2C_MODE_W, tmp, 1);
	HAL_Delay(10);//dont remove
	
	/* read the first byte of config */
	reg[0] = GT1151_REG_CFGS>>8;
	reg[1] = GT1151_REG_CFGS&0xff;
	mcu_vi2c_transfer_reg(DevVi2cGT1151, GT1151_DEVICE_ADDR, reg, 2, MCU_I2C_MODE_R, tmp, 1);
	uart_printf("config ver:%02x\r\n", tmp[0]);

	/* update config */
	if (tmp[0] < GT1151CfgTab[0]) {
		gt1151_update_cfg();	
	}
	
	HAL_Delay(10);//dont remove
	
	tmp[0] = 0X00;//配置不保存到Flash
	reg[0] = GT1151_REG_CTRL>>8;
	reg[1] = GT1151_REG_CTRL&0xff;
	mcu_vi2c_transfer_reg(DevVi2cGT1151, GT1151_DEVICE_ADDR, reg, 2, MCU_I2C_MODE_W, tmp, 1);

	uart_printf("gt1151 init finish\r\n");
	Gt1151Gd = 0;
	bus_i2c_close(node);
	
	return 0;
}

/*
	reg GT9147_REG_GSTID,
	
	bit 7: 1=data ready, write 0 to GT9147_REG_GSTID clear 
	bit 5：proximity valid
	bit 4: 1 = have key, 0= no key（realease）
	bit 3~0: Number of touch points
*/
static u8 Gt1151Sta;

const u16 GT1151TpxTab[5]={GT1151_REG_TP1,GT1151_REG_TP2,GT1151_REG_TP3,GT1151_REG_TP4,GT1151_REG_TP5};

/*
	gt9147 task: scan ctp.
	call this function in while or a rtos task.
	this fun never block the cpu.

	this demo just read the poin xy.
*/
int gt1151_task(void)
{
	u8 tmp[4];
	u8 reg[2];
	u8 point_num;
	u16 x,y;
	DevI2cNode *node;
	
	if (Gt1151Gd > 0) 
	{
	node = bus_i2c_open("VI2C1", 1);
	if (node ==NULL ) { 
		//uart_printf("gt9147 open i2c err!\r\n");
		return -1;
	}
		reg[0] = GT1151_REG_GSTID>>8;
		reg[1] = GT1151_REG_GSTID&0xff;
		mcu_vi2c_transfer_reg(DevVi2cGT1151, GT1151_DEVICE_ADDR, reg, 2, MCU_I2C_MODE_R, tmp, 1);
		Gt1151Sta = tmp[0];
		
		//uart_printf("%02x...", Gt1151Sta);
		
		point_num = Gt1151Sta&0x0f;
		
		if ((Gt1151Sta & 0x80) == 0x80
			&& (point_num < 6)) {
			/* clear reg GT9147_REG_GSTID*/
			//uart_printf("clr...");
			tmp[0] = 0;
			mcu_vi2c_transfer_reg(DevVi2cGT1151, GT1151_DEVICE_ADDR, reg, 2, MCU_I2C_MODE_W, tmp, 1);
		}

		if ((point_num >0) && (point_num < 6)) {
			//uart_printf("ctp:%dp\r\n", point_num);
			while(point_num > 0) {
				point_num--;
				/*read the tp */
				reg[0] = GT1151TpxTab[point_num]>>8;
				reg[1] = GT1151TpxTab[point_num]&0xff;
				mcu_vi2c_transfer_reg(DevVi2cGT1151, GT1151_DEVICE_ADDR, reg, 2, MCU_I2C_MODE_R, tmp, 4);
				x = tmp[1]<<8;
				x += tmp[0];

				y = tmp[3]<<8;
				y += tmp[2];

				uart_printf("tp:%d, x=%d, y=%d\r\n", point_num+1, x, y);
				/* this is a demo, you can fill the xy into a buf,
					the app read the buf get the point.  */
				ctp_fill_buf(x, y);
			}
		}
		bus_i2c_close(node);
		return 0;
	}	

	return -1;
}

int gt1151_open(void)
{
	if (Gt1151Gd == 0)
		Gt1151Gd = 1;
	return 0;
}

int gt1151_close(void)
{
	if (Gt1151Gd == 1)
		Gt1151Gd = 0;
	return 0;
}


const TouchDev CtpGt1151={
	.name ="ctp_gt1151",
	.type = 2, 

	.init = gt1151_init,
	.open = gt1151_open,
	.close = gt1151_close,
	.task = gt1151_task,
};


