
#include "mcu.h"
#include "log.h"
#include "board_sysconf.h"


s32 board_main_init(void)
{
	wjq_log(LOG_DEBUG, "board: canary!\r\n");
	wjq_log(LOG_DEBUG, "    www.wujiue.com\r\n");
	
	sys_dev_register();
	
	#ifdef SYS_USE_KEYPAD
	dev_keypad_init();
	#endif

	#ifdef SYS_USE_TP
	dev_touchscreen_init();
	#endif

	#ifdef BOARD_DEV_HCOC
	dev_ptHCHO_init();
	#endif
	
	#ifdef BOARD_DEV_HTU21U
	dev_htu21d_init();
	#endif
	//dev_keypad_open();
	canary_103test_init();
	
	return 0;
}

#if 0
u8 ioindex = 0;
u16 timecnt=0;

KeyPadIO testiolist[]=
	{
		{NULL, NULL},
		{MCU_PORT_D, GPIO_Pin_12},//D12
		{MCU_PORT_D, GPIO_Pin_13},//D13
		{MCU_PORT_B, GPIO_Pin_7},//B7
		{MCU_PORT_B, GPIO_Pin_6},//B6
		{MCU_PORT_A, GPIO_Pin_3},//A3
		{MCU_PORT_A, GPIO_Pin_1},//A1
		{MCU_PORT_E, GPIO_Pin_6},//E6
		{MCU_PORT_A, GPIO_Pin_0},//A0
		{MCU_PORT_D, GPIO_Pin_3},//D3
		{MCU_PORT_A, GPIO_Pin_2},//A2
		{MCU_PORT_A, GPIO_Pin_9},//A9
		{MCU_PORT_A, GPIO_Pin_10},//A10
		{MCU_PORT_A, GPIO_Pin_4},//A4
		{MCU_PORT_E, GPIO_Pin_5},//E5
		{MCU_PORT_A, GPIO_Pin_7},//A7
		{MCU_PORT_A, GPIO_Pin_5},//A5
		{MCU_PORT_E, GPIO_Pin_4},//E4
		{MCU_PORT_A, GPIO_Pin_6},//A6
		{MCU_PORT_B, GPIO_Pin_14},//B14
		{MCU_PORT_C, GPIO_Pin_6},//C6
		{MCU_PORT_B, GPIO_Pin_13},//B13
		{MCU_PORT_B, GPIO_Pin_15},//B15
		{MCU_PORT_D, GPIO_Pin_6},//D6
		{MCU_PORT_B, GPIO_Pin_12},//B12
		{MCU_PORT_B, GPIO_Pin_8},//B8
		{MCU_PORT_B, GPIO_Pin_9},//B9
		{MCU_PORT_A, GPIO_Pin_8},//a8
		{MCU_PORT_C, GPIO_Pin_10},//C10
		{MCU_PORT_D, GPIO_Pin_2},//D2
		{MCU_PORT_C, GPIO_Pin_8},//C8
		{MCU_PORT_C, GPIO_Pin_12},//C12
		{MCU_PORT_C, GPIO_Pin_9},//C9
		{MCU_PORT_C, GPIO_Pin_11},//C11
		{MCU_PORT_C, GPIO_Pin_4},//C4
		{MCU_PORT_B, GPIO_Pin_0},//B0
		{MCU_PORT_C, GPIO_Pin_5},//C5
		{MCU_PORT_B, GPIO_Pin_1},//B1
		{MCU_PORT_E, GPIO_Pin_9},//E9
		{MCU_PORT_E, GPIO_Pin_8},//E8
		{MCU_PORT_E, GPIO_Pin_11},//E11
		{MCU_PORT_E, GPIO_Pin_10},//E10
		{MCU_PORT_E, GPIO_Pin_13},//E13
		{MCU_PORT_E, GPIO_Pin_12},//E12
		{MCU_PORT_E, GPIO_Pin_15},//E15
		{MCU_PORT_E, GPIO_Pin_14},//E14
		
	};

void canary_exio_test(void)
{
	u8 key;
	s32 res;
	
	res = dev_keypad_read(&key, 1);
	if(res ==  1 && key == 16 )
	{
		wjq_log(LOG_DEBUG, "***canary_exio_test next!***\r\n");
		
		ioindex++;
		if(ioindex >=46)
		{
			ioindex = 1;
		}
		mcu_io_config_out(testiolist[ioindex].port, testiolist[ioindex].pin);
		timecnt = 0;
	}
	
	if(ioindex !=0)
	{
		timecnt++;
		if(timecnt == 100)
		{
			mcu_io_output_setbit(testiolist[ioindex].port, testiolist[ioindex].pin);	
		}
		else if(timecnt == 200)
		{
			timecnt = 0;
			mcu_io_output_resetbit(testiolist[ioindex].port, testiolist[ioindex].pin);
		}
			
	}
}
#endif

void board_low_task(void)
{
	//wjq_log(LOG_DEBUG, "***board: canary low task!***\r\n");
	#ifdef SYS_USE_KEYPAD
	dev_keypad_scan();
	#endif	

	//canary_exio_test();
}




