

/* Includes ------------------------------------------------------------------*/
#include "mcu.h"
#include "petite_config.h"

#include "petite.h"
#include "petite_def.h"

#include "cmsis_os.h"

#include "board_sysconf.h"

#include "drv_lcd.h"

/** @addtogroup Template_Project
  * @{
  */ 
/*
	创建petite线程，然后调用board init初始化板级设备
	*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

extern s32 board_init(void);
/**
 *@brief:      start_task
 *@details:    
 			   
 *@param[in]   void *pvParameters  
 *@param[out]  
 *@retval:     
 */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "Pet",
  .stack_size = START_TASK_STK_SIZE,
  .priority = (osPriority_t) START_TASK_PRIO,//osPriorityNormal,
};

extern osThreadId_t TestTaskHandle;


void petite_task(void *pvParameters)
{
	char TaskListBuf[256];
	uint16_t tcnt=0;
	UBaseType_t stackHWM;
	
	wjq_log(LOG_INFO,"[    _app] petite_task\r\n");

	/* 初始化petite模块状态*/
	vfs_init();
	font_init();
	
	board_init();

	/* 测试内存溢出 rtos检测 ，溢出后HOOK函数会被调用
		vApplicationStackOverflowHook */
	memset(TaskListBuf, 0, sizeof(TaskListBuf));
	
	for(;;) {
		
		osDelay(5);
		board_low_task(5);
		tcnt++;
		#if 1
		if (tcnt >= 1000) {
			tcnt = 0;
			
			//vTaskList(TaskListBuf);
			//uart_printf("  name         state priority   stack   NUM\r\n");
			//uart_printf("%s", TaskListBuf);
			
			vTaskGetRunTimeStats(TaskListBuf);
			//uart_printf("  name count per\r\n");
			uart_printf("%s", TaskListBuf);

			/* 栈的水位线，指栈剩余多少个 U32 */
			//stackHWM = uxTaskGetStackHighWaterMark(defaultTaskHandle);
			//uart_printf("PetiteTask hwm:%d\r\n", stackHWM);
			//stackHWM = uxTaskGetStackHighWaterMark(TestTaskHandle);
			//uart_printf("TestTask hwm:%d\r\n", stackHWM);
		}
		#endif
	}
}

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int petite_app(void)
{	
	/*           http://www.network-science.de/ascii/   */
	wjq_log(LOG_INFO,"\r\n*********************************************************\r\n");
	wjq_log(LOG_INFO,"*   ___     _   _ _          ___                        \r\n");       
	wjq_log(LOG_INFO,"*  / _ \\___| |_(_) |_ ___   /   \\_ ____   __             \r\n");
	wjq_log(LOG_INFO,"* / /_)/ _ \\ __| | __/ _ \\ / /\\ / '__\\ \\ / /             \r\n");
	wjq_log(LOG_INFO,"*/ ___/  __/ |_| | ||  __// /_//| |   \\ V /              \r\n");
	wjq_log(LOG_INFO,"*\\/    \\___|\\__|_|\\__\\___/___,' |_|    \\_/www.wujique.com\r\n");
	wjq_log(LOG_INFO,"*********************************************************\r\n\r\n");

  /* Infinite loop 
	尽快启动RTOS， 进入start_task再做其他设备初始化 */
	wjq_log(LOG_INFO,"ccreate start task!\r\n");
  	/* Init scheduler */
  	osKernelInitialize();
	defaultTaskHandle = osThreadNew(petite_task, NULL, &defaultTask_attributes);
					
	if(NULL != defaultTaskHandle){
		wjq_log(LOG_INFO,"[    _app] freertos Scheduler\r\n");
		/* Start scheduler */
  		osKernelStart();
	}else{
		wjq_log(LOG_WAR,"[    _app] xTaskCreate fail\r\n");
	}
	
	wjq_log(LOG_ERR,"while(1) err!\r\n");
	while(1);
  
}



/**
  * @}
  */
/*-------------------------------------
	一些OS HOOK函数定义，暂时放这里
-----------------------------------*/
void vApplicationStackOverflowHook (TaskHandle_t xTask, signed char *pcTaskName)
{
	uart_printf("stack overflow %s\r\n", pcTaskName);
	
}

#if 0

__asm int readr0(void){ 
     bx lr
}
 
__asm int readr1(void){
     MOV     R0, R1//所嵌入的汇编代码  
     bx lr
}
 
__asm int readr2(void){
     MOV     R0, R2//所嵌入的汇编代码  
     bx lr
}
 
__asm int readlr(void){
     MOV     R0, lr//所嵌入的汇编代码  
     bx lr
}
 
__asm int readsp(void){
     MOV     R0, sp//所嵌入的汇编代码   
     bx lr
}
 
__asm int readpc(void){
     MOV     R0, pc//所嵌入的汇编代码  
     bx lr
}

int (*list_cpu_resister[])(void)  =
{
	readr0,
	readr1,
	readr2,
	readlr,
	readsp,
	readpc
};
 
char *cpuname[] = {"r0", "r1", "r2","lr","sp","pc"};

int catsp(void)
{
	int i = 0;
 
	for(;i<sizeof(list_cpu_resister)/sizeof(list_cpu_resister[0]);i++)
		uart_printf("%s = 0x%08x\r\n" ,cpuname[i], list_cpu_resister[i]());
 
   return 0;
}


HardFault_Handler\
                PROC
                IMPORT  HardFault_Handler_c               
                MOVS r0, #4                
				MOV r1, LR
				TST r0, r1
				BEQ stacking_used_MSP      
				MRS R0, PSP              
				B get_LR_and_branch         
stacking_used_MSP
				MRS R0, MSP               
get_LR_and_branch
				MOV R1, LR

				BL HardFault_Handler_c
                ENDP

/* 发生hardfault ，先确定栈地址，然后将进入本中断时压栈的寄存器打印出来，
	找到lr寄存器，就是产生hardfault的地址，但是要注意，产生hardfault的地方
	不一定是问题原因 
	修改了启动代码，在汇编HardFault_Handler中进行处理，然后执行HardFault_Handler_c
	*/ 

void HardFault_Handler_c(unsigned int * hardfault_args, unsigned lr_value)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */
	unsigned int stacked_r0; //压栈的 r0
	unsigned int stacked_r1; //压栈的 r1
	unsigned int stacked_r2; //压栈的 r2
	unsigned int stacked_r3; //压栈的 r3
	unsigned int stacked_r12; //压栈的 r12
	unsigned int stacked_lr; //压栈的 lr
	unsigned int stacked_pc; //压栈的 pc
	unsigned int stacked_psr; //压栈的 psr

	stacked_r0 = ((unsigned int) hardfault_args[0]);
	stacked_r1 = ((unsigned int) hardfault_args[1]);
	stacked_r2 = ((unsigned int) hardfault_args[2]);
	stacked_r3 = ((unsigned int) hardfault_args[3]);
	stacked_r12 = ((unsigned int)hardfault_args[4]);
	stacked_lr = ((unsigned int) hardfault_args[5]);
	stacked_pc = ((unsigned int) hardfault_args[6]);
	stacked_psr = ((unsigned int) hardfault_args[7]);

	uart_printf("[Hard fault handler]\r\n");
	uart_printf("R0 = %x\r\n", stacked_r0);
	uart_printf("R1 = %x\r\n", stacked_r1);
	uart_printf("R2 = %x\r\n", stacked_r2);
	uart_printf("R3 = %x\r\n", stacked_r3);
	uart_printf("R12 = %x\r\n", stacked_r12);
	uart_printf("Stacked LR = %x\r\n", stacked_lr);
	uart_printf("Stacked PC = %x\r\n", stacked_pc);
	uart_printf("Stacked PSR = %x\r\n", stacked_psr);
	uart_printf("SCB_SHCSR=%x\r\n",SCB->SHCSR);
	uart_printf("Current LR = %x\r\n", lr_value);

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}
#endif

/*
	系统设备注册
	通过缩进区分层级和依赖关系。
	后续可以考虑实现可见字符表示的设备树
*/
s32 petite_dev_register(void *DevTable[])
{
	uint8_t i=0;
	PetiteNode *pnod;

	while(1){
		if (DevTable[i] == NULL) break;
		pnod =	(PetiteNode *)DevTable[i];
		wjq_log(LOG_DEBUG, "[register]:%s\r\n", pnod->name);
		switch(pnod->type)
		{
			case BUS_I2C_V:
				bus_i2c_register((const DevI2c *)DevTable[i]);
				break;

			case BUS_SPI_V:
			case BUS_SPI_H:
				bus_spi_register((const DevSpi *)DevTable[i]);
				break;
			
			case BUS_SPI_CH:
				bus_spich_register((const DevSpiCh *)DevTable[i]);
				break;

			case DEV_LCD:
				lcd_dev_register((const DevLcd *)DevTable[i]);
				break;
			case DEV_SPIFLASH:
				dev_spiflash_register((const DevLcd *)DevTable[i]);
				break;
			
			default:
				wjq_log(LOG_WAR, "\r\n\r\n[register] %s not register!\r\n\r\n", pnod->name);
				break;
		}
		i++;
	}
	
	wjq_log(LOG_DEBUG, "\r\n[register] petite_dev_register: %d device!\r\n\r\n", i);	
	
	return 0;
}




/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
