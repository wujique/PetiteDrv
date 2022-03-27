
#include "stm32h7xx_hal.h"

#include "board_sysconf.h"

#if 1

extern LTDC_HandleTypeDef hltdc;
#define 	BytesPerPixel		2

/*
	这些代码是
	STM32Cube_FW_H7_V1.5.0\Projects\STM32H750B-DK\Examples\LTDC\LTDC_Display_1Layer\MDK-ARM
*/
/**
  * @brief LCD Configuration.
  * @note  This function Configure tha LTDC peripheral :
  *        1) Configure the Pixel Clock for the LCD
  *        2) Configure the LTDC Timing and Polarity
  *        3) Configure the LTDC Layer 1 :
  *           - indirect color (L8) as pixel format  
  *           - The frame buffer is located at FLASH memory
  *           - The Layer size configuration : 480x272                      
  * @retval
  *  None
 */
/** 
  * @brief	ATK4342 Size  
  */

/*	480*272 = 130560个点，RGB565每个点2Byte，一共261120Byte
	填入数据的格式 
	uint32_t color 
	*/
__align(4)
uint32_t RGB565_480x272[LCD_WIDTH*LCD_HEIGHT/2];




void init_rgb565_buf(uint32_t color)
{
	uint32_t i;

	for (i=0; i< sizeof(RGB565_480x272)/sizeof(uint32_t); i++) {
		RGB565_480x272[i] = color;	
	}
}

extern const uint32_t RGB565_480x272_PIC[65280];
extern const unsigned char girl_800480[768000];
/*
	从外部Flash读数据，顺序读，连续读，速度会快很多。
	比如读一次800*480的图片，
	如果不顺序读，要133ms
	顺序,一轮读2个，56ms
	顺序,一轮读4个，32ms
	连续,一轮读8个，只要23ms
	*/
#if 0
void fill_rgb_buf_800_480_girl(void)
{
	char *p = (char *)RGB565_480x272;
	char *src;
	uint32_t i;

	src = (char *)girl_800480;
	for (i=0; i< 800*480*2; ) {
		p[i+1] = *src++;
		p[i] = *src++;
		p[i+3] = *src++;
		p[i+2] = *src++;
		i += 4;

		p[i+1] = *src++;
		p[i] = *src++;
		p[i+3] = *src++;
		p[i+2] = *src++;
		i += 4;
	}
}
#endif
void fill_rgb565buf(uint32_t *dot, uint32_t sx, uint32_t sy, uint32_t w, uint32_t h)
{
	uint32_t i, j;

	w = w/2;
	h = h;
	sx = sx/2;
	sy = sy;
	
	for (j=0;j<h; j++)
		for (i=0; i< w; i++) {
			RGB565_480x272[(j+sy)*LCD_WIDTH/2 + i + sx] = *(dot + j*w +i);	
		}	
}
#endif

/*
	为了兼容不同的LCD，本函数将STCUBE生成的两个函数
	MX_LTDC_Init 和 HAL_LTDC_MspInit 糅合在一起。
	*/
void LCD_Config(void)
{

  /* USER CODE BEGIN LTDC_Init 0 */
	
	/* 把在HAL_LTDC_MspInit中的时钟配置放到这里来 */
	/*
		 系统选择HSE作为输入时钟，则是25M,
		 HSE/PLL3M x PLL3N /PLL3R
		*/
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;
 #ifdef ATK4342
 	//fill_rgb565buf((uint32_t *)RGB565_480x272_PIC, 0,0,LCD_WIDTH,LCD_HEIGHT);
 	init_rgb565_buf(0x0f0f0f0f);
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
  PeriphClkInitStruct.PLL3.PLL3M = 5;
  PeriphClkInitStruct.PLL3.PLL3N = 160;
  PeriphClkInitStruct.PLL3.PLL3FRACN = 0;
  PeriphClkInitStruct.PLL3.PLL3P = 2;
  PeriphClkInitStruct.PLL3.PLL3Q = 2;
  PeriphClkInitStruct.PLL3.PLL3R = 83;
#endif

#ifdef WKS43WQ
	//fill_rgb_buf_800_480_girl();
	init_rgb565_buf(0x0f0f0f0f);
	/*	驱动IC是HX8264, 时钟不能超过50M 		，在这设置为33M */
  	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
    PeriphClkInitStruct.PLL3.PLL3M = 25;
    PeriphClkInitStruct.PLL3.PLL3N = 330;
	
    PeriphClkInitStruct.PLL3.PLL3P = 2;
    PeriphClkInitStruct.PLL3.PLL3Q = 2;
	
    PeriphClkInitStruct.PLL3.PLL3R = 10;
    PeriphClkInitStruct.PLL3.PLL3RGE = RCC_PLL3VCIRANGE_0;
    PeriphClkInitStruct.PLL3.PLL3VCOSEL = RCC_PLL3VCOMEDIUM;;
    PeriphClkInitStruct.PLL3.PLL3FRACN = 0;
#endif

    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }
  /* USER CODE END LTDC_Init 0 */

  LTDC_LayerCfgTypeDef pLayerCfg = {0};
  LTDC_LayerCfgTypeDef pLayerCfg1 = {0};

  /* USER CODE BEGIN LTDC_Init 1 */

  /* USER CODE END LTDC_Init 1 */
  hltdc.Instance = LTDC;
  hltdc.Init.HSPolarity = LTDC_HSPOLARITY_AL;
  hltdc.Init.VSPolarity = LTDC_VSPOLARITY_AL;
  hltdc.Init.DEPolarity = LTDC_DEPOLARITY_AL;
  hltdc.Init.PCPolarity = LTDC_PCPOLARITY_IPC;

  /* Timing configuration */
  hltdc.Init.HorizontalSync = (LCD_HSYNC - 1);
  hltdc.Init.VerticalSync = (LCD_VSYNC - 1);
  hltdc.Init.AccumulatedHBP = (LCD_HSYNC + LCD_HBP - 1);
  hltdc.Init.AccumulatedVBP = (LCD_VSYNC + LCD_VBP - 1);  
  hltdc.Init.AccumulatedActiveW = (LCD_WIDTH + LCD_HSYNC + LCD_HBP - 1);
  hltdc.Init.AccumulatedActiveH = (LCD_HEIGHT + LCD_VSYNC + LCD_VBP - 1);
  hltdc.Init.TotalWidth = (LCD_WIDTH + LCD_HSYNC + LCD_HBP + LCD_HFP - 1); 
  hltdc.Init.TotalHeigh = (LCD_HEIGHT + LCD_VSYNC + LCD_VBP + LCD_VFP - 1);

  /* Configure R,G,B component values for LCD background color */
  hltdc.Init.Backcolor.Blue = 0;
  hltdc.Init.Backcolor.Green = 0;
  hltdc.Init.Backcolor.Red = 0;
  
  if (HAL_LTDC_Init(&hltdc) != HAL_OK)
  {
    Error_Handler();
  }
  /* 配置显存层，一共两层，可以只用一层*/
  /* Windowing configuration */ 
  /* Pixel Format configuration*/ 
  pLayerCfg.WindowX0 = 0;
  pLayerCfg.WindowX1 = LCD_WIDTH;
  pLayerCfg.WindowY0 = 0;
  pLayerCfg.WindowY1 = LCD_HEIGHT;
  pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;
  pLayerCfg.Alpha = 255;
  pLayerCfg.Alpha0 = 0;
  pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
  pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;
    /* Start Address configuration : frame buffer*/
  pLayerCfg.FBStartAdress = (uint32_t)&RGB565_480x272[0];
	  /* Configure the number of lines and number of pixels per line */
  pLayerCfg.ImageWidth = LCD_WIDTH;
  pLayerCfg.ImageHeight = LCD_HEIGHT;
  pLayerCfg.Backcolor.Blue = 0;
  pLayerCfg.Backcolor.Green = 0;
  pLayerCfg.Backcolor.Red = 0;
  if (HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg, 1) != HAL_OK)
  {
    Error_Handler();
  }
  #if 0
  pLayerCfg1.WindowX0 = 0;
  pLayerCfg1.WindowX1 = 0;
  pLayerCfg1.WindowY0 = 0;
  pLayerCfg1.WindowY1 = 0;
  pLayerCfg1.PixelFormat = LTDC_PIXEL_FORMAT_ARGB8888;
  pLayerCfg1.Alpha = 0;
  pLayerCfg1.Alpha0 = 0;
  pLayerCfg1.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
  pLayerCfg1.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;
  pLayerCfg1.FBStartAdress = 0;
  pLayerCfg1.ImageWidth = 0;
  pLayerCfg1.ImageHeight = 0;
  pLayerCfg1.Backcolor.Blue = 0;
  pLayerCfg1.Backcolor.Green = 0;
  pLayerCfg1.Backcolor.Red = 0;
  if (HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  #endif
  /* USER CODE BEGIN LTDC_Init 2 */
	uart_printf("ltdc init finish!\r\n");
  
  /* USER CODE END LTDC_Init 2 */

}


/**
  * @brief LTDC MSP Initialization 
  *        This function configures the hardware resources used in this example: 
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration  
  * @param hltdc: LTDC handle pointer
  * @retval None
  */
void delay(int us)
{
	volatile int i;
	
 	for(i =0; i< (100*us); i++);
}

void lcd_reset(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_GPIOD_CLK_ENABLE();
	
	/*Configure GPIO pin Output Level */
  	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
  	/*Configure GPIO pins : PC4*/
  	GPIO_InitStruct.Pin = GPIO_PIN_14;
 	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
	delay(100);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
	delay(100);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
	
	/*Configure GPIO pin Output Level  背光*/
  	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
  	/*Configure GPIO pins : PC4*/
  	GPIO_InitStruct.Pin = GPIO_PIN_15;
 	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}

/* dma2d */
extern DMA2D_HandleTypeDef hdma2d;

// dma2D采用HAL初始化
void dma2d_set_int_callback(void (* callback)(DMA2D_HandleTypeDef *hdma2d))
{
 
	hdma2d.Instance = DMA2D;
	hdma2d.XferCpltCallback = callback;
	if (HAL_DMA2D_Init(&hdma2d) != HAL_OK)
	{

	}
}


void DMA2D_cpy_framebuff(uint16_t x, uint16_t y, uint16_t width, uint16_t height,uint32_t *color)
{
	DMA2D->CR&=~(DMA2D_CR_START);
	
	DMA2D->CR	   =	DMA2D_M2M;					
	DMA2D->FGPFCCR =	LTDC_PIXEL_FORMAT_RGB565;
	DMA2D->OPFCCR  = 	LTDC_PIXEL_FORMAT_RGB565;
    DMA2D->FGOR    =  0;	
	/* 行偏移 */
	DMA2D->OOR		=	LCD_WIDTH - width;	
	/*源数据*/
	DMA2D->FGMAR   =  (uint32_t)color;		
	/* 目标地址 */
	DMA2D->OMAR		=	(uint32_t)RGB565_480x272 + BytesPerPixel*(LCD_WIDTH * y + x);
	/* 设置填充区域的宽和高，单位是像素 */
	DMA2D->NLR		=	(width<<16)|(height);

	//DMA2D->CR |= DMA2D_IT_TC|DMA2D_IT_TE|DMA2D_IT_CE;
	
	DMA2D->CR	  |=	DMA2D_CR_START;				
	/* 等待传输完成 */	
	while (DMA2D->CR & DMA2D_CR_START) ;

}


void DMA2D_cpy_wait(void)
{
	/* 等待传输完成 */	
	while (DMA2D->CR & DMA2D_CR_START) ;
}

