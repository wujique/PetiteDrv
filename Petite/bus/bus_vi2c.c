/**
 * @file            mcu_i2c.c
 * @brief           IO模拟I2C
 * @author          test
 * @date            2019年03月26日 星期一
 * @version         初稿
 * @par             版权所有 (C), 2013-2023
 * @par History:
 * 1.日    期:     
 *   作    者:      屋脊雀工作室
 *   修改内容:      创建文件
    	1 源码归屋脊雀工作室所有。
		2 可以用于的其他商业用途（配套开发板销售除外），不须授权。
		3 屋脊雀工作室不对代码功能做任何保证，请使用者自行测试，后果自负。
		4 可随意修改源码并分发，但不可直接销售本代码获利，并且请保留WUJIQUE版权说明。
		5 如发现BUG或有优化，欢迎发布更新。请联系：code@wujique.com
		6 使用本源码则相当于认同本版权说明。
		7 如侵犯你的权利，请联系：code@wujique.com
		8 一切解释权归屋脊雀工作室所有。
*/

#include "mcu.h"
#include "log.h"
#include "mcu_io.h"
#include "bus/bus_i2c.h"

#define MCU_I2C_TIMEOUT 250

extern uint32_t McuDelayUs;

/**
 *@brief:      mcu_i2c_delay
 *@details:    I2C信号延时函数
 *@param[in]   clk, 时钟频率 khz  
 *@param[out]  无
 *@retval:     static
 */
static void bus_vi2c_delay(uint16_t clk)
{
    volatile u32 i = 20;
	/* 不同的芯片时钟有不同算法，这里只是大概 */
	i = 1000/clk;//t的单位是us
	i = i*5;//1us要的循环数
	i = i/2;//半周期

    for(;i>0;i--);
}

/**
 *@brief:      mcu_vi2c_sda_input
 *@details:    将I2C sda IO设置为输入
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
void bus_vi2c_sda_input(DevI2c *dev)
{
	mcu_vi2c_io_config_in(dev->sdaport, dev->sdapin);
}
/**
 *@brief:      mcu_i2c_sda_output
 *@details:       将I2C sda IO设置为输出
 *@param[in]  void  
 *@param[out]  无
 *@retval:     
 */
void bus_vi2c_sda_output(DevI2c *dev)
{
	mcu_vi2c_io_config_out(dev->sdaport, dev->sdapin);
}
/**
 *@brief:      mcu_i2c_readsda
 *@details:    读SDA数据
 *@param[in]   void  
 *@param[out]  无
 *@retval:     static
 */
static s32 bus_vi2c_readsda(DevI2c *dev)
{

    if (MCU_IO_STA_1 == mcu_io_input_readbit(dev->sdaport, dev->sdapin))
        return MCU_IO_STA_1;
    else
        return MCU_IO_STA_0;
}
/**
 *@brief:      mcu_vi2c_sda
 *@details:       SDA输出高低电平
 *@param[in]  u8 sta  
 *@param[out]  无
 *@retval:     static
 */
static void bus_vi2c_sda(DevI2c *dev, u8 sta)
{

    if (sta == MCU_IO_STA_1) {
		mcu_io_output_setbit(dev->sdaport, dev->sdapin);
    } else if (sta == MCU_IO_STA_0) {
    	mcu_io_output_resetbit(dev->sdaport, dev->sdapin);
    }

}

/**
 *@brief:      mcu_i2c_scl
 *@details:    时钟SCL输出高低电平
 *@param[in]   u8 sta  
 *@param[out]  无
 *@retval:     static
 */
static void bus_vi2c_scl(DevI2c *dev, u8 sta)
{

	if (sta == MCU_IO_STA_1) {
		mcu_io_output_setbit(dev->sclport, dev->sclpin);
    } else if (sta == MCU_IO_STA_0) {
    	mcu_io_output_resetbit(dev->sclport, dev->sclpin);
    }
}
/**
 *@brief:      mcu_i2c_start
 *@details:    发送start时序
 *@param[in]   void  
 *@param[out]  无
 *@retval:     static
 */
static void bus_vi2c_start(DevI2c *dev, uint16_t clk)
{
    bus_vi2c_sda_output(dev);
    
    bus_vi2c_sda(dev, MCU_IO_STA_1);  
    bus_vi2c_scl(dev, MCU_IO_STA_1);

    bus_vi2c_delay(clk);
    bus_vi2c_sda(dev, MCU_IO_STA_0);

    bus_vi2c_delay(clk);
    bus_vi2c_scl(dev, MCU_IO_STA_0);
}
/**
 *@brief:      mcu_i2c_stop
 *@details:    发送I2C STOP时序
 *@param[in]   void  
 *@param[out]  无
 *@retval:     static
 */
static void bus_vi2c_stop(DevI2c *dev, uint16_t clk)
{
    bus_vi2c_sda_output(dev);

    bus_vi2c_scl(dev, MCU_IO_STA_0);
    bus_vi2c_sda(dev, MCU_IO_STA_0);   
    bus_vi2c_delay(clk);
    
    bus_vi2c_scl(dev, MCU_IO_STA_1);
    bus_vi2c_delay(clk);
    
    bus_vi2c_sda(dev, MCU_IO_STA_1);
    bus_vi2c_delay(clk);
}

/**
 *@brief:      mcu_i2c_wait_ack
 *@details:       等待ACK信号
 *@param[in]  void  
 *@param[out]  无
 *@retval:     static
 */
static s32 bus_vi2c_wait_ack(DevI2c *dev, uint16_t clk)
{
    u8 time_out = 0;
    
    //sda转输入
    bus_vi2c_sda_input(dev);
    bus_vi2c_sda(dev, MCU_IO_STA_1);
    bus_vi2c_delay(clk);
    
    bus_vi2c_scl(dev, MCU_IO_STA_1);
    bus_vi2c_delay(clk);
    
    while(1) {
        time_out++;
        if (time_out > MCU_I2C_TIMEOUT) {
            bus_vi2c_stop(dev,clk);
            //wjq_log(LOG_ERR, "i2c:wait ack time out!\r\n");
            return 1;
        }

        if (MCU_IO_STA_0 == bus_vi2c_readsda(dev)) {   
            break;
        }
    }
    
    bus_vi2c_scl(dev, MCU_IO_STA_0);
    
    return 0;
}
/**
 *@brief:      mcu_i2c_ack
 *@details:       发送ACK信号
 *@param[in]  void  
 *@param[out]  无
 *@retval:     static
 */
static void bus_vi2c_ack(DevI2c *dev, uint16_t clk)
{
    bus_vi2c_scl(dev, MCU_IO_STA_0);
    bus_vi2c_sda_output(dev);
    
    bus_vi2c_sda(dev, MCU_IO_STA_0);
    bus_vi2c_delay(clk);
    
    bus_vi2c_scl(dev, MCU_IO_STA_1);
    bus_vi2c_delay(clk);
    
    bus_vi2c_scl(dev, MCU_IO_STA_0);
}
/**
 *@brief:      mcu_i2c_writebyte
 *@details:       I2C总线写一个字节数据
 *@param[in]  u8 data  
 *@param[out]  无
 *@retval:     static
 */
static s32 bus_vi2c_writebyte(DevI2c *dev, u8 data, uint16_t clk)
{
    u8 i = 0;

    bus_vi2c_sda_output(dev);

    bus_vi2c_scl(dev, MCU_IO_STA_0);
    
    while(i<8) {
    
        if ((0x80 & data) == 0x80) {
            bus_vi2c_sda(dev, MCU_IO_STA_1);   
        } else {
            bus_vi2c_sda(dev, MCU_IO_STA_0);
        }
        
        bus_vi2c_delay(clk);

        bus_vi2c_scl(dev, MCU_IO_STA_1);
        bus_vi2c_delay(clk);
        
        bus_vi2c_scl(dev, MCU_IO_STA_0);
        bus_vi2c_delay(clk);
        
        data = data <<1;
        i++;
    }
		return 0;
}
/**
 *@brief:      mcu_i2c_readbyte
 *@details:       I2C总线读一个字节数据
 *@param[in]  void  
 *@param[out]  无
 *@retval:     static
 */
static u8 bus_vi2c_readbyte(DevI2c *dev, uint16_t clk)
{
    u8 i = 0;
    u8 data = 0;

    bus_vi2c_sda_input(dev);
    
    while (i<8) {
        bus_vi2c_scl(dev, MCU_IO_STA_0);
        bus_vi2c_delay(clk);
        
        bus_vi2c_scl(dev, MCU_IO_STA_1);

        data = (data <<1);

        if (1 == bus_vi2c_readsda(dev)) {
            data =data|0x01;    
        } else {
            data = data & 0xfe;
        }

        bus_vi2c_delay(clk);
        
        i++;
    }

    return data;
}


s32 bus_vi2c_init(DevI2c * dev)
{
	/*初始化IO口状态*/
	mcu_io_config_out(dev->sclport, dev->sclpin);
	
	mcu_vi2c_io_config_out(dev->sdaport, dev->sdapin);

	mcu_io_output_setbit(dev->sdaport, dev->sdapin);
	mcu_io_output_setbit(dev->sclport, dev->sclpin); 
	return 0;
}

s32 bus_vi2c_open(void)
{

}

s32 bus_vi2c_close(void)
{

}

/**
 *@brief:      mcu_i2c_transfer
 *@details:    中间无重新开始位的传输流程
 *@param[in]   DevI2cNode * node  I2C节点
 			   u8 addr   7位地址
               u8 rw    0 写，1 读    
               u8* data  
               s32 datalen 发送数据长度
 *@param[out]  无
 *@retval:     
 */
s32 bus_vi2c_transfer(DevI2cNode *node, u8 addr, u8 rw, u8* data, s32 datalen)
{
    s32 i;
    u8 ch;
	DevI2c *dev;
	uint16_t clk;
	s32 res;
	PDevNode *pnode;
	
	if (node == NULL) return -1;

	pnode = (PDevNode *)node;

	dev = (DevI2c *)pnode->pdev;
	clk = node->clkkhz;
	//I2C_DEBUG(LOG_DEBUG, "i2c trf %s\r\n", dev->name);
    //发送起始
    bus_vi2c_start(dev, clk);
    //发送地址+读写标志
    //处理ADDR
    if (rw == MCU_I2C_MODE_W) {
        addr = ((addr<<1)&0xfe);
        //uart_printf("write\r\n");
    } else {
        addr = ((addr<<1)|0x01);
        //uart_printf("read\r\n");
    }
    
    bus_vi2c_writebyte(dev, addr, clk);
	
    res = bus_vi2c_wait_ack(dev, clk);
	if (res == 1) return 1;
	
    i = 0;

    //数据传输
    if (rw == MCU_I2C_MODE_W) {
	    while(i < datalen) {
            ch = *(data+i);
            bus_vi2c_writebyte(dev, ch, clk);
            res = bus_vi2c_wait_ack(dev, clk);
			if (res == 1)
				return 1;
			
			i++;
	    }
    } else if (rw == MCU_I2C_MODE_R) {
       	while(i < datalen) {
            ch = bus_vi2c_readbyte(dev, clk);  
            bus_vi2c_ack(dev, clk);
            *(data+i) = ch;
			i++;
	    }
    }

    //发送结束
    bus_vi2c_stop(dev, clk);
    return 0;
}

/*
	带寄存器地址的操作方法
*/
s32 mcu_vi2c_transfer_reg(DevI2c *dev, u8 addr, u8* reg, u8 reglen, u8 rw, u8* data, s32 datalen)
{
    s32 i;
    u8 ch;
	s32 res;
	u8 addr_tmp;
	uint16_t khz = 200;
    //发送起始
    bus_vi2c_start(dev, khz);
   
	addr_tmp = ((addr<<1)&0xfe);
	
    bus_vi2c_writebyte(dev,addr_tmp, khz);
	
    res = bus_vi2c_wait_ack(dev, khz);
	if(res == 1)
		return 1;
	
    i = 0;
	while(i < reglen) {
		ch = *(reg+i);
        bus_vi2c_writebyte(dev,ch, khz);
        res = bus_vi2c_wait_ack(dev, khz);
		if(res == 1)
			return 1;

		i++;
    }
	
    //数据传输
    i=0;
    if(rw == MCU_I2C_MODE_W)//写
    {
	    while(i < datalen)	{
            ch = *(data+i);
            bus_vi2c_writebyte(dev,ch, khz);
            res = bus_vi2c_wait_ack(dev, khz);
			if(res == 1)
				return 1;
			
			i++;
	    }
    } else if(rw == MCU_I2C_MODE_R) {
		//读
		bus_vi2c_start(dev, khz);
		addr_tmp = ((addr<<1)|0x01);
    	bus_vi2c_writebyte(dev,addr_tmp, khz);
    	res = bus_vi2c_wait_ack(dev, khz);
		if(res == 1)
			return 1;

       	while(i < datalen) {
            ch = bus_vi2c_readbyte(dev, khz);  
            bus_vi2c_ack(dev, khz);
            *(data+i) = ch;
			i++;
	    }
    }

    //发送结束
    bus_vi2c_stop(dev, khz);
    return 0;
}


/*
	以下为SCCB读写逻辑（OV2640地址默认0x60）

写寄存器 
	Start+0x60+寄存器ID+数据+Stop
读寄存器 
	Start+0x60+寄存器ID+Stop  Start+0x61 + 所读数据+Stop
错误方式读寄存器：
	Start+0x61+寄存器ID+Stop  Start+0x61 + 所读数据+Stop
*/
#if 0
uint8_t mcu_sccb_writereg(DevI2c *dev, uint8_t DeviceAddr, uint16_t Addr, uint8_t Data)
{

	bus_vi2c_start(dev);
	bus_vi2c_writebyte(dev,DeviceAddr);
	bus_vi2c_wait_ack(dev);
	bus_vi2c_writebyte(dev,Addr);
	bus_vi2c_wait_ack(dev);
	bus_vi2c_writebyte(dev,Data);
	bus_vi2c_wait_ack(dev);
	bus_vi2c_stop(dev);

	return 0;
}

uint8_t mcu_sccb_readreg(DevI2c *dev, uint8_t DeviceAddr, uint16_t Addr)
{
	uint8_t ch;
	
	bus_vi2c_start(dev);
	bus_vi2c_writebyte(dev,DeviceAddr&0xfe);
	bus_vi2c_wait_ack(dev);
	bus_vi2c_writebyte(dev,Addr);
	bus_vi2c_wait_ack(dev);
	bus_vi2c_stop(dev);
	
	//bus_vi2c_delay(10*1000);
	
	bus_vi2c_start(dev);
	bus_vi2c_writebyte(dev,DeviceAddr);
	bus_vi2c_wait_ack(dev);
	ch = bus_vi2c_readbyte(dev);
	bus_vi2c_ack(dev);
	bus_vi2c_stop(dev);

	return ch;
}
#endif


