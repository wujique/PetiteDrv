/* USER CODE BEGIN Header */
/*
 * FreeRTOS Kernel V10.3.1
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */
/* USER CODE END Header */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * These parameters and more are described within the 'configuration' section of the
 * FreeRTOS API documentation available on the FreeRTOS.org web site.
 *
 * See http://www.freertos.org/a00110.html
 *----------------------------------------------------------*/

/* USER CODE BEGIN Includes */
/* Section where include file can be added */
/* USER CODE END Includes */

/* Ensure definitions are only used by the compiler, and not by the assembler. */
#if defined(__ICCARM__) || defined(__CC_ARM) || defined(__GNUC__)
  #include <stdint.h>
  extern uint32_t SystemCoreClock;//这是在STM32hal库中定义的，表示系统时钟。
  void xPortSysTickHandler(void);
#endif
#ifndef CMSIS_device_header
#define CMSIS_device_header "stm32f4xx.h"
#endif /* CMSIS_device_header */

/*
	使能FPU和MPU，和FREERTOS的关系要搞清楚
	H7用不到configENABLE_FPU和configENABLE_MPU  
	是给 ARMv8M架构新版准备的
	https://www.freertos.org/2020/04/using-freertos-on-armv8-m-microcontrollers.html
	*/
#define configENABLE_FPU                         0
#define configENABLE_MPU                         0


/* RTOS内存分配 
	静态和动态都支持，这两种模式用不同的API */
#define configSUPPORT_STATIC_ALLOCATION          1
#define configSUPPORT_DYNAMIC_ALLOCATION         1
#define configTOTAL_HEAP_SIZE			( ( size_t ) ( 10 * 1024 ) )//堆，使用FreeRtos的内存分配方案才会用到

#define configUSE_IDLE_HOOK				0//hook就是钩子函数
#define configUSE_TICK_HOOK				0

#define configUSE_PREEMPTION                     1//抢占
#define configCPU_CLOCK_HZ				( SystemCoreClock )
#define configTICK_RATE_HZ				( ( TickType_t ) 1000 )//系统频率，1Khz，也就是1MS调度一次
#define configMAX_PRIORITIES			( 56 )//56个优先级
#define configMINIMAL_STACK_SIZE		( ( unsigned short ) 512 )/* 任务栈最小SIZE，涉及IDLE任务栈和定时任务*/
#define configMAX_TASK_NAME_LEN                  ( 16 )

/* 低功耗模式，在IDLE 任务中进入 tickless模式 
	实现下面两个宏函数，就会在sleep前后调用。
	#define configPRE_SLEEP_PROCESSING( x )
	#define configPOST_SLEEP_PROCESSING( x )

	*/
#define configUSE_TICKLESS_IDLE		1

#define configUSE_TRACE_FACILITY                1/* 使能可视化追踪功能*/
#define configUSE_STATS_FORMATTING_FUNCTIONS	1

#define configUSE_16_BIT_TICKS                   0// STM32是32bit
#define configUSE_MUTEXES                        1//互斥信号量， freertos的信号量，其实也是基于队列实现的
#define configUSE_RECURSIVE_MUTEXES              0/* 递归mutexes*/
#define configUSE_COUNTING_SEMAPHORES            1//计数信号量
#define configUSE_TASK_NOTIFICATIONS			1 //任务间通信
/* 队列注册数量，这个与内核调试有关，如果不用内核调试，无意义。
	*/
#define configQUEUE_REGISTRY_SIZE                8


/* 任务切换时间进行 SP指针是否越界， 
	void vApplicationStackOverflowHook( TaskHandle_t xTask,
	signed char *pcTaskName );
	*/
//#define configCHECK_FOR_STACK_OVERFLOW 			1
/*任务创建的时候将任务栈所有数据初始化为 0xa5，任务切换时进行任务栈检测的时候会检测末
	尾的 16 个字节是否都是 0xa5，通过这种方式来检测任务栈是否溢出了
	*/
#define configCHECK_FOR_STACK_OVERFLOW 			2


/* 统计CPU使用率，CPU_RunTime 是一个时钟滴答，最好比 RTOS 的tick精度高*/
#define configGENERATE_RUN_TIME_STATS     		1
extern volatile uint32_t LocalTime; 
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS() 	(LocalTime = 0ul) 
#define portGET_RUN_TIME_COUNTER_VALUE() 			LocalTime
/*--------------------------------*/
#define configUSE_PORT_OPTIMISED_TASK_SELECTION  0
/* USER CODE BEGIN MESSAGE_BUFFER_LENGTH_TYPE */
/* Defaults to size_t for backward compatibility, but can be changed
   if lengths will always be less than the number of bytes in a size_t. */
#define configMESSAGE_BUFFER_LENGTH_TYPE         size_t
/* USER CODE END MESSAGE_BUFFER_LENGTH_TYPE */

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES                    0
#define configMAX_CO_ROUTINE_PRIORITIES          ( 2 )

/* Software timer definitions.  
	软件定时器使用一个任务去管理，是时间轮吗？
	优先级要根据实际情况配置 */
#define configUSE_TIMERS                         1/* 软定时器任务 Tmr_SVC*/
#define configTIMER_TASK_PRIORITY                ( 3  )
#define configTIMER_QUEUE_LENGTH                 10
#define configTIMER_TASK_STACK_DEPTH             256

/* CMSIS-RTOS V2 flags */
#define configUSE_OS2_THREAD_SUSPEND_RESUME  1
#define configUSE_OS2_THREAD_ENUMERATE       1
#define configUSE_OS2_EVENTFLAGS_FROM_ISR    1/*  */
#define configUSE_OS2_THREAD_FLAGS           1
#define configUSE_OS2_TIMER                  1
#define configUSE_OS2_MUTEX                  1

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */
#define INCLUDE_vTaskPrioritySet             1
#define INCLUDE_uxTaskPriorityGet            1
#define INCLUDE_vTaskDelete                  1
#define INCLUDE_vTaskCleanUpResources        0
#define INCLUDE_vTaskSuspend                 1
#define INCLUDE_vTaskDelayUntil              1
#define INCLUDE_vTaskDelay                   1
#define INCLUDE_xTaskGetSchedulerState       1
#define INCLUDE_xTimerPendFunctionCall       1/* */
#define INCLUDE_xQueueGetMutexHolder         1
#define INCLUDE_uxTaskGetStackHighWaterMark  1
#define INCLUDE_xTaskGetCurrentTaskHandle    1
#define INCLUDE_eTaskGetState                1

/*
 * The CMSIS-RTOS V2 FreeRTOS wrapper is dependent on the heap implementation used
 * by the application thus the correct define need to be enabled below
 */
#define USE_FreeRTOS_HEAP_4

/* Cortex-M specific definitions. */
/* configPRIO_BITS 定义了芯片优先级用了内核的几个bit，M内核是8个bit
	STM32只用了高4bit
	*/
#ifdef __NVIC_PRIO_BITS
 /* __BVIC_PRIO_BITS will be specified when CMSIS is being used. */
 #define configPRIO_BITS         __NVIC_PRIO_BITS
#else
 #define configPRIO_BITS         4
#endif

/* The lowest interrupt priority that can be used in a call to a "set priority"
function. */
/* freertos使用中断分组4，所有BIT都是抢占优先级，没有次优先级 
	所以最低优先级就是15*/
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY   15

/* The highest interrupt priority that can be used by any interrupt service
routine that makes calls to interrupt safe FreeRTOS API functions.  DO NOT CALL
INTERRUPT SAFE FREERTOS API FUNCTIONS FROM ANY INTERRUPT THAT HAS A HIGHER
PRIORITY THAN THIS! (higher priorities are lower numeric values. 
	系统管理中断阈值
	优先级高于这个值（数值小于这个值）的中断不受RTOS管理(关中断)
	这些中断不能调用系统接口，比如 xxx_FromISR函数
	关闭了所有小于等于宏定义 configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY
    所定义的中断优先级，这样临界段代码就不会被中断干扰到
*/
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY	5

/* Interrupt priorities used by the kernel port layer itself.  These are generic
to all Cortex-M ports, and do not rely on any particular library functions. 
内核中断优先级
用来设置PendSV 和滴答定时器的中断优先级
*/
#define configKERNEL_INTERRUPT_PRIORITY 		( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )
/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. 
	FreeRtos关中断接口，只会将优先级低于本宏的中断关掉
*/
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 	( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )

/* Normal assert() semantics without relying on the provision of an assert.h
header file. */
/* USER CODE BEGIN 1 */
#define configASSERT( x ) if ((x) == 0) {taskDISABLE_INTERRUPTS(); for( ;; );}
/* USER CODE END 1 */

/* Definitions that map the FreeRTOS port interrupt handlers to their CMSIS
standard names. */
#define vPortSVCHandler SVC_Handler
#define xPortPendSVHandler PendSV_Handler
//#define xPortSysTickHandler SysTick_Handler
//#define pvPortMalloc wjq_malloc_m
//#define vPortFree	wjq_free_m

#define configPRE_SLEEP_PROCESSING( x ) board_pre_sleep(x)
#define configPOST_SLEEP_PROCESSING( x ) board_post_sleep(x)


/* IMPORTANT: After 10.3.1 update, Systick_Handler comes from NVIC (if SYS timebase = systick), otherwise from cmsis_os2.c */

#define USE_CUSTOM_SYSTICK_HANDLER_IMPLEMENTATION 1

/* USER CODE BEGIN Defines */
/* Section where parameter definitions can be added (for instance, to override default ones in FreeRTOS.h) */
/* USER CODE END Defines */

#endif /* FREERTOS_CONFIG_H */

