#include "mcu.h"
#include "mcu_dcmi.h"
#include "board_sysconf.h"

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

#define DCMI_DR_ADDRESS       0x50050028

//u32 dcmi_dma_buf[320*240];
extern DCMI_HandleTypeDef hdcmi;
extern uint32_t RGB565_480x272[LCD_WIDTH*LCD_HEIGHT/2];

/*
	
	*/
#if 0	
void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi)
{
	while(HAL_DMA_Abort(hdcmi->DMA_Handle)!=HAL_OK);
    uart_printf("get pic!\r\n");
}
#endif
/*
	启动DCMI DMA传输，将图像数据传输到指定地址
	*/


void camera_test(void)
{
	HAL_DCMI_Start_DMA(&hdcmi, DCMI_MODE_CONTINUOUS, (u32)RGB565_480x272, LCD_WIDTH*LCD_HEIGHT);
}


