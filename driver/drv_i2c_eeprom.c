/*

	24c系列EPROM驱动程序

*/
#include "main.h"
#include "log.h"
#include "bus_i2c.h"

/* M24器件的地址 使用7bit地址模式 */
#define MC24CXX_ADDR  0x50//需要根据读写地址设置bit0
/*  容量 */
#define MC24XX_SIZE 256


//#define EEPROM_I2C_BUS "VI2C1"
char *EEPROM_I2C_BUS;
DevI2c *DevVi2cEeprom;

/*
	
*/
int m24c_init(void)
{
	
}
/*
	读1个字节数据 
*/
int m24c_read_byte(char *Buf, uint32_t Addr)
{
	uint8_t sel;
	uint8_t tmp;
	s32 res;

	/* 求出低8位地址 */
	tmp = Addr&0xff;

	/* 
		判断最高位地址
		也即是说，如果地址大于256，就需要在器件地址的最低位置位为1
	*/
	if((Addr & 0x100) != 0)
		sel = MC24CXX_ADDR | 0x01;
	else
		sel = MC24CXX_ADDR;

	DevI2cNode *busnode;
	busnode = bus_i2c_open(EEPROM_I2C_BUS, 0xffffffff, 400);
	mcu_vi2c_transfer_reg(DevVi2cEeprom, sel, tmp, 1, MCU_I2C_MODE_R, Buf, 1);
	bus_i2c_close(busnode);
	#if 0
	//发送起始
    mcu_vi2c_start();
    /*发送地址+读标志
		地址使用7位地址模式，因此，发送的时候，左移1位，加上低些标志
		本函数是读，所以读写标识为0，与上0xfe，就能保证bit0一定是0
	*/
    sel = ((sel<<1)&0xfe);
	
    mcu_vi2c_writebyte(sel);
	res = mcu_vi2c_wait_ack();
	if (res == 1) {
		/* 错误，发送器件地址，没收到设备 ACK*/
		uart_printf("1 ");
		return 1;
	}
	/* 发送要读的地址 ，注意和设备地址的区别 */
	mcu_vi2c_writebyte(tmp);
	res = mcu_vi2c_wait_ack();
	if (res == 1) {
		uart_printf("2 ");
		return 1;
	}

	//发送起始
    mcu_vi2c_start();
	/*  发送地址+读标志 */
    sel = (sel|0x01);
    mcu_vi2c_writebyte(sel);
	res = mcu_vi2c_wait_ack();
	if (res == 1) {
		uart_printf("3 ");
		return 1;
	}

	/* 读数据 */
	tmp = mcu_vi2c_readbyte();
	mcu_vi2c_stop();

	*Buf = tmp;
	//printf("m24c read %d = %02x\r\n", Addr, tmp);
	#endif
	return 0;
}

int m24c_write_byte(char Data, uint32_t Addr)
{
	uint8_t sel;
	uint8_t tmp;
	s32 res;
    uint16_t cnt;
	char senddata = Data;
	/* 处理要读数据的地址
		最多支持512， 使用1byte做地址寻址，
		大于256的地址，需要在器件地址的最低位置1标识。
	*/
	tmp = Addr&0xff;
	if((Addr & 0x100) != 0)//bug
		sel = MC24CXX_ADDR | 0x01;
	else
		sel = MC24CXX_ADDR;

	DevI2cNode *busnode;
	busnode = bus_i2c_open(EEPROM_I2C_BUS, 0xffffffff, 400);
	mcu_vi2c_transfer_reg(DevVi2cEeprom, sel, tmp, 1, MCU_I2C_MODE_W, &senddata, 1);
	#if 0
	//发送起始
    mcu_vi2c_start();
    //发送地址+读标志
    sel = ((sel<<1)&0xfe);
    mcu_vi2c_writebyte(sel);
	res = mcu_vi2c_wait_ack();
	if (res == 1) {
		uart_printf("4 ");
		return 1;
	}

	mcu_vi2c_writebyte(tmp);
	res = mcu_vi2c_wait_ack();
	if (res == 1) {
		uart_printf("5 ");
		return 1;
	}

	mcu_vi2c_writebyte(Data);
	res = mcu_vi2c_wait_ack();
	if (res == 1) {
		uart_printf("6 ");
		return 1;
	}
	
	mcu_vi2c_stop();
	#endif
	/* check finish:< 5ms 
		这里是写等待处理， 写EEPROM需要比较长的时间。
		很多例程都是直接延时10ms，这并不是很好的方法。
		在芯片规格书中有介绍如何判断写完成。
		我们这里就是按照文档推荐的方法做：发送器件地址，如果器件返回ACK，说明写操作已经完成。
	*/
	cnt = 0;
	while(1) {
		osDelay(2);
		res = bus_i2c_transfer(busnode, sel, MCU_I2C_MODE_W, NULL, 0);
		if (res == 0) {
			break;
		}
		cnt++;
		if (cnt >= 1000){
			break;
		}
		#if 0	
		mcu_vi2c_start();
    	mcu_vi2c_writebyte(sel);
		res = mcu_vi2c_wait_ack();
		if (res == 0) {
			//printf("cnt:%d\r\n", cnt);
			mcu_vi2c_stop();
			break;
		}
		cnt++;
		if (cnt >= 1000) {
			mcu_vi2c_stop();
			return 1;	
		}
		#endif
	}
	//printf("m24c write ok!\r\n");
	bus_i2c_close(busnode);
	return 1;
}

/*
	
*/
void mc24c_test(char *I2cName, DevI2c *dev)
{
	uint16_t addr = 0;
	uint8_t wdata = 0;
	uint8_t rdata[MC24XX_SIZE];

	EEPROM_I2C_BUS = I2cName;
	DevVi2cEeprom = dev;

	uart_printf("mc24c_test read data:\r\n");
	while(1) {	
		if(addr >= MC24XX_SIZE)
			break;
		
		m24c_read_byte(&rdata[addr], addr);
		addr++;	
	}
	dump_hex(rdata, MC24XX_SIZE);

	uart_printf("\r\n mc24c_test write&check:\r\n");
	addr = 0;
	while(1) {	
		if(addr >= MC24XX_SIZE)
			break;
		wdata = 0x89;
		///@bug 写进去就要读出来，否则写不进去？
		m24c_write_byte(wdata, addr);
		m24c_read_byte(&rdata[addr], addr);
		if (wdata != rdata[addr]) {
			uart_printf("mc24c_test err:%d!\r\n", addr);	
		}
		wdata++;
		addr++;
		
	}

	dump_hex(rdata, MC24XX_SIZE);

	uart_printf("mc24c_test finish!\r\n");
	while(1) {
		osDelay(1000);	
	}
}




#include "partition.h"
#if 0
const StorageDev StorageI2cEeprom ={
	
	.getblksize = storage_spiflash_getblksize,
	.getsize = storage_spiflash_getsize,

	.getnode = dev_spiflash_getnode,
	.open = dev_spiflash_opennode,	
	.read = storage_spiflash_read,
	.write = storage_spiflash_write,
	.erase = storage_spiflash_erase,
	.close = dev_spiflash_close,
};
#endif


