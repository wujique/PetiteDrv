#include "mcu.h"
#include "petite.h"
#include "touch.h"

/* when the gt9147 resetting , if int pin is 0, i2c addr= 0x5d; 
						  				1,  0x14
			(7-bit addr) */
#define GT9147_DEVICE_ADDR 0x14//0x5d
#define GT9147_I2C_CLK	100

/* hardware io define */
#define GT9147_RST_PORT GPIOD
#define GT9147_RST_PIN GPIO_PIN_14

#define GT9147_INT_PORT GPIOD
#define GT9147_INT_PIN GPIO_PIN_9

extern const DevI2c DevVi2c1;
DevI2c *DevVi2cGT9147 = (DevI2c *)&DevVi2c1;

/* reg addr */
#define GT9147_REG_CTRL		0X8040
#define GT9147_REG_CFGS 	0X8047   
#define GT9147_REG_CHECK 	0X80FF  
#define GT9147_REG_PID 		0X8140  

#define GT9147_REG_GSTID 	0X814E   	
#define GT9147_REG_TP1 		0X8150  	
#define GT9147_REG_TP2 		0X8158		
#define GT9147_REG_TP3 		0X8160		
#define GT9147_REG_TP4 		0X8168		
#define GT9147_REG_TP5 		0X8170	 

/* config 
	the first byte is version.
	9147 配置的是480*272像素屏幕
*/
#if 0
const u8 GT9147CfgTab[]=
{ 
	0x43,//版本
	0xe0,0x01,//X轴最大 480
	0x10,0x01,//Y轴最大 272
	0x05,0x0f,0x00,0x01,0x08,
    0x28,0x05,0x50,0x32,0x03,0x05,0x00,0x00,0xff,0xff,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x89,0x28,0x0a,
    0x17,0x15,0x31,0x0d,0x00,0x00,0x02,0x9b,0x03,0x25,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x32,0x00,0x00,
    0x00,0x0f,0x94,0x94,0xc5,0x02,0x07,0x00,0x00,0x04,
    0x8d,0x13,0x00,0x5c,0x1e,0x00,0x3c,0x30,0x00,0x29,
    0x4c,0x00,0x1e,0x78,0x00,0x1e,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x08,0x0a,0x0c,0x0e,0x10,0x12,0x14,0x16,
    0x18,0x1a,0x00,0x00,0x00,0x00,0x1f,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0x00,0x02,0x04,0x05,0x06,0x08,0x0a,0x0c,
    0x0e,0x1d,0x1e,0x1f,0x20,0x22,0x24,0x28,0x29,0xff,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,
    
};  
#endif
const u8 GT9147CfgTab[]=
{ 
	0x44,//版本
	0x20,0x03,//X轴最大 800
	0xe0,0x01,//Y轴最大 480
	0x05,0x0f,0x00,0x01,0x08,
    0x28,0x05,0x50,0x32,0x03,0x05,0x00,0x00,0xff,0xff,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x89,0x28,0x0a,
    0x17,0x15,0x31,0x0d,0x00,0x00,0x02,0x9b,0x03,0x25,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x32,0x00,0x00,
    0x00,0x0f,0x94,0x94,0xc5,0x02,0x07,0x00,0x00,0x04,
    0x8d,0x13,0x00,0x5c,0x1e,0x00,0x3c,0x30,0x00,0x29,
    0x4c,0x00,0x1e,0x78,0x00,0x1e,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x08,0x0a,0x0c,0x0e,0x10,0x12,0x14,0x16,
    0x18,0x1a,0x00,0x00,0x00,0x00,0x1f,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0x00,0x02,0x04,0x05,0x06,0x08,0x0a,0x0c,
    0x0e,0x1d,0x1e,0x1f,0x20,0x22,0x24,0x28,0x29,0xff,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,
    
};	
/*
	init gt9147 ic io.

	rst :0 reset
	    1  work
*/
static int gt9147_io_init(void)
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


static int gt9147_update_cfg(void)
{
	u8 buf[2];
	u8 i=0;
	u8 tmp[4];
	u8 reg[4];
	
	uart_printf("gt9147_update_cfg\r\n");
	
	buf[0]=0;
	buf[1]=1;
	
	for(i=0; i<sizeof(GT9147CfgTab); i++)
		buf[0]+=GT9147CfgTab[i];
		
    buf[0]=(~buf[0])+1;

	reg[0] = GT9147_REG_CFGS>>8;
	reg[1] = GT9147_REG_CFGS&0xff;
	mcu_vi2c_transfer_reg(DevVi2cGT9147,GT9147_DEVICE_ADDR, reg, 2, MCU_I2C_MODE_W, (u8*)GT9147CfgTab, sizeof(GT9147CfgTab));

	reg[0] = GT9147_REG_CHECK>>8;
	reg[1] = GT9147_REG_CHECK&0xff;
	mcu_vi2c_transfer_reg(DevVi2cGT9147,GT9147_DEVICE_ADDR, reg, 2, MCU_I2C_MODE_W, buf, 2);

	return 0;
} 
/*---------------------------------------*/
/* gd */
static int Gt9147Gd = -1;
/*
	1 init hardware
	2 read ID
	3 check & update the config

*/
int gt9147_init(void)
{
	u8 ID[4];
	u8 tmp[4];
	u8 reg[2];
	DevI2cNode *node;

	gt9147_io_init();

	//HAL_Delay(1000);
		
	ID[0] = 0;
	ID[1] = 0;
	ID[2] = 0;
	ID[3] = 0;
	/* read gt9147 ID*/
	reg[0] = GT9147_REG_PID>>8;
	reg[1] = GT9147_REG_PID&0xff;
	
	node = bus_i2c_open("VI2C1", 0xffffffff);
	if (node ==NULL ) { 
		uart_printf("gt9147 open i2c err!\r\n");
		return -1;
	}
	mcu_vi2c_transfer_reg(DevVi2cGT9147,GT9147_DEVICE_ADDR, reg, 2, MCU_I2C_MODE_R, ID, 4);
	
	uart_printf("gt9147 read ID:%c%c%c%c\r\n", ID[0], ID[1], ID[2], ID[3]);	//"9147", ASC

	if(ID[0] == '9'
		&& ID[1] == '1'
		&& ID[2] == '4'
		&& ID[3] == '7') {

		uart_printf("check gt9147 ok!\r\n");

	}else {
		bus_i2c_close(node);
		uart_printf("err!\r\n");
		return -1;
	}
	
	/* soft reset */
	tmp[0] = 0X02;
	reg[0] = GT9147_REG_CTRL>>8;
	reg[1] = GT9147_REG_CTRL&0xff;
	mcu_vi2c_transfer_reg(DevVi2cGT9147,GT9147_DEVICE_ADDR, reg, 2, MCU_I2C_MODE_W, tmp, 1);
	HAL_Delay(10);//dont remove
	
	/* read the first byte of config */
	reg[0] = GT9147_REG_CFGS>>8;
	reg[1] = GT9147_REG_CFGS&0xff;
	mcu_vi2c_transfer_reg(DevVi2cGT9147,GT9147_DEVICE_ADDR, reg, 2, MCU_I2C_MODE_R, tmp, 1);
	uart_printf("config ver:%02x\r\n", tmp[0]);

	/* update config */
	if (tmp[0] != GT9147CfgTab[0]) {
		gt9147_update_cfg();	
	}
	
	HAL_Delay(10);//dont remove
	
	tmp[0] = 0X00;
	reg[0] = GT9147_REG_CTRL>>8;
	reg[1] = GT9147_REG_CTRL&0xff;
	mcu_vi2c_transfer_reg(DevVi2cGT9147,GT9147_DEVICE_ADDR, reg, 2, MCU_I2C_MODE_W, tmp, 1);

	uart_printf("gt9147 init finish\r\n");
	Gt9147Gd = 0;

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
static u8 Gt9147Sta;

const u16 GT9147TpxTab[5]={GT9147_REG_TP1,GT9147_REG_TP2,GT9147_REG_TP3,GT9147_REG_TP4,GT9147_REG_TP5};

/*
	gt9147 task: scan ctp.
	call this function in while or a rtos task.
	this fun never block the cpu.

	this demo just read the poin xy.
*/
int gt9147_task(void)
{
	u8 tmp[4];
	u8 reg[2];
	u8 point_num;
	u16 x,y;
	DevI2cNode *node;
	
	if (Gt9147Gd <= 0) return 0; 

	/*  应该等待多少时间超时？等太久，会卡住board task
		不等，会不会经常获取不到I2C,造成触摸屏失效？*/
	node = bus_i2c_open("VI2C1", 2);
	if (node ==NULL ) { 
		//uart_printf("gt9147 open i2c err!\r\n");
		return -1;
	}
		reg[0] = GT9147_REG_GSTID>>8;
		reg[1] = GT9147_REG_GSTID&0xff;
		mcu_vi2c_transfer_reg(DevVi2cGT9147,GT9147_DEVICE_ADDR, reg, 2, MCU_I2C_MODE_R, tmp, 1);
		Gt9147Sta = tmp[0];
		
		//uart_printf("%02x...", Gt9147Sta);
		
		point_num = Gt9147Sta&0x0f;
		
		if ((Gt9147Sta & 0x80) == 0x80
			&& (point_num < 6)) {
			/* clear reg GT9147_REG_GSTID*/
			//uart_printf("clr...");
			tmp[0] = 0;
			mcu_vi2c_transfer_reg(DevVi2cGT9147,GT9147_DEVICE_ADDR, reg, 2, MCU_I2C_MODE_W, tmp, 1);
		}

		if ((point_num >0) && (point_num < 6)) {
			//uart_printf("ctp:%dp\r\n", point_num);
			while(point_num > 0) {
				point_num--;
				/*read the tp */
				reg[0] = GT9147TpxTab[point_num]>>8;
				reg[1] = GT9147TpxTab[point_num]&0xff;
				mcu_vi2c_transfer_reg(DevVi2cGT9147,GT9147_DEVICE_ADDR, reg, 2, MCU_I2C_MODE_R, tmp, 4);
				x = tmp[1]<<8;
				x += tmp[0];

				y = tmp[3]<<8;
				y += tmp[2];

				uart_printf("tp:%d, x=%d, y=%d\r\n", point_num+1, x, y);
				/* this is a demo, you can fill the xy into a buf,
					the app read the buf get the point.
				*/
				ctp_fill_buf(x, y);
			}
		}

		bus_i2c_close(node);
		return 0;
}

int gt9147_open(void)
{
	if (Gt9147Gd == 0)
		Gt9147Gd = 1;
	return 0;
}

int gt9147_close(void)
{
	if (Gt9147Gd == 1)
		Gt9147Gd = 0;
	
	return 0;
}


const TouchDev CtpGt9147={
	.name ="ctp_gt9147",
	.type = 2, 

	.init = gt9147_init,
	.open = gt9147_open,
	.close = gt9147_close,
	.task = gt9147_task,
};

