/**
 * @file            
 * @brief           
 * @author          
 * @date            2019年03月12日
 * @version         初稿
 * @par             版权所有 (C), 2013-2023
 * @par History:
 * 1.日    期:      
 *   作    者:         屋脊雀工作室
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
#include "board_sysconf.h"
#include "dev_ptHCHO.h"


#ifdef BOARD_DEV_HCOC

s32 PtHCHOGd = -2;

s32 dev_ptHCHO_init(void)
{

	PtHCHOGd = -1;
	return 0;
}

s32 dev_ptHCHO_open(void)
{
	s32 ret;
	
	if(PtHCHOGd != -1)
		return PtHCHOGd;

	ret = mcu_uart_open(DEV_PTHCHO_UART);
	if(ret != 0)
		return -1;
	
	mcu_uart_set_baud(DEV_PTHCHO_UART, 9600);
	PtHCHOGd = 0;
	
	return 0;
}


s32 dev_ptHCHO_close(void)
{
	if(PtHCHOGd != 0)
		return PtHCHOGd;

	PtHCHOGd = -1;
	
	return 0;
}


s32 dev_ptHCHO_read(u8 *buf, s32 len)
{
	s32 res;
	
	if(PtHCHOGd != 0)
		return PtHCHOGd;
	
	res = mcu_uart_read(DEV_PTHCHO_UART, buf, len);	
	
	return res;
}

s32 dev_ptHCHO_write(u8 *buf, s32 len)
{
	s32 res;
	
	if(PtHCHOGd != 0)
		return PtHCHOGd;
	
	res = mcu_uart_write(DEV_PTHCHO_UART, buf, len);

	return res;
}


s32 dev_ptHCHO_ioctrl(void)
{
	if(PtHCHOGd != 0)
		return PtHCHOGd;
	
	return 0;
}

/*

*/
const u8 HCHOAskCmd[7] = {0x42, 0x4D, 0x01, 0x00, 0x00, 0x00, 0x90};
//气体名称
const char numNameInx[][5] = {
  "无", "CO", "H2S", "CH4", "CL2", "HCL", "F2", "HF", "NH3", "HCN", "PH3", "NO", "NO2", "O3", "O2", "SO2", "CLO2",
  "COCL", "PH3", "SiH4", "HCHO", "CO2", "VOC", "ETO", "C2H4", "C2H2", "SF6", "AsH3", "H2", "TOX1", "TOX2"//,
  //"气体流量L/M", "电池电量/%"
};
//数据内容单位
const char numUnitInx[][5] = {"", "ppm", "VOL", "LEL", "Ppb", "MgM3"};
//数据当量
const int numKeysInx[] = {1, 1, 10, 100, 1000};

s32 dev_ptHCHO_test(void)
{
	u8 buf[32];
	u16 len;
	u32 timeout = 0;
	s32 ret;
	u16 value;
	
	//dev_ptHCHO_open();
	//while(1)
	{
		Delay(500);
		ret = dev_ptHCHO_write((u8*)HCHOAskCmd, 7);
		wjq_log(LOG_FUN, "write:%d\r\n", ret);
		
		timeout = 0;
		len = 0;
		while(1)
		{
			Delay(50);
			memset(buf, 0, sizeof(buf));
			len = dev_ptHCHO_read(buf, sizeof(buf));
			if(len != 0)
			{
				PrintFormat(buf, len);
				if((buf[0] == 0x42) &&(buf[1] == 0x4d)&&(buf[2] == 0x08))
				{
					value = (buf[6]<<8) +	buf[7];
					wjq_log(LOG_FUN, ">-%s:%d", numNameInx[buf[3]],value/numKeysInx[buf[5]]);
					wjq_log(LOG_FUN, ".");
					wjq_log(LOG_FUN, "%d%s---\r\n",value%(numKeysInx[buf[5]]), numUnitInx[buf[4]]);
				}
			}
			
			timeout++;
			if(timeout >= 100)
			{
				wjq_log(LOG_FUN, "timeout---\r\n");
				break;
			}
		}
	}
	return 0;
}
#endif

