#include "mcu.h"
#include "board_sysconf.h"

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/


extern DMA_HandleTypeDef hdma_dcmi_pssi;
extern DCMI_HandleTypeDef hdcmi;

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
	Mode : 0 连续； 1 一次
	pData：数据地址
	Length：传输长度，注意，此处传入的是字节数
		如果长度是1，要将MemInc改为DMA_MINC_DISABLE，例如用于8080接口的LCD
	*/
int mcu_dcmi_captruce(uint32_t Mode, uint32_t pData, uint32_t Length)
{
	uint32_t DCMI_Mode;
	
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
	
	if (Length == 1) hdma_dcmi_pssi.Init.MemInc = DMA_MINC_DISABLE;
	else hdma_dcmi_pssi.Init.MemInc = DMA_MINC_ENABLE;

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
	
	if (Mode == 0) DCMI_Mode = DCMI_MODE_CONTINUOUS;
	else DCMI_Mode = DCMI_MODE_SNAPSHOT;
	
	HAL_StatusTypeDef status;
	status = HAL_DCMI_Start_DMA(&hdcmi, DCMI_Mode, pData, Length/4);
	uart_printf("status:%d\r\n", status);
	
	return 0;
}
/*---------------------- endif --------------------------------*/

