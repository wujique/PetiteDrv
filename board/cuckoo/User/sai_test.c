


/* Includes ------------------------------------------------------------------*/
#include "main.h"

/** @addtogroup STM32H7xx_HAL_Examples
  * @{
  */

/** @addtogroup SAI_AudioPlay
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define AUDIO_FILE_ADDRESS   0x08080000
#define AUDIO_FILE_SIZE      (180*1024)
#define PLAY_HEADER          0x2C
#define PLAY_BUFF_SIZE       4096

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

static AUDIO_Drv_t                     *Audio_Drv = NULL;
void *Audio_CompObj = NULL;
WM8994_Init_t codec_init;
__IO int16_t        UpdatePointer = -1;

/* Buffer location should aligned to cache line size (32 bytes) */
ALIGN_32BYTES (uint16_t PlayBuff[PLAY_BUFF_SIZE]);



/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int sai_test(void)
{
  __IO uint32_t PlaybackPosition   = PLAY_BUFF_SIZE + PLAY_HEADER;
  WM8994_Init_t codec_init;


  /* Check if the buffer has been loaded in flash */
  if(*((uint64_t *)AUDIO_FILE_ADDRESS) != 0x017EFE2446464952 ) Error_Handler();

  /* Initialize playback */
  Playback_Init();

  /* Initialize the data buffer */
  for(int i=0; i < PLAY_BUFF_SIZE; i+=2)
  {
    PlayBuff[i]=*((__IO uint16_t *)(AUDIO_FILE_ADDRESS + PLAY_HEADER + i));
  }
 
  codec_init.Resolution   = 0;
  
  /* Fill codec_init structure */
  codec_init.Frequency    = 16000;
  codec_init.InputDevice  = WM8994_IN_NONE;
  codec_init.OutputDevice = AUDIO_OUT_DEVICE_HEADPHONE;
  
  /* Convert volume before sending to the codec */
  codec_init.Volume       = VOLUME_OUT_CONVERT(60);
  
  /* Initialize the codec internal registers */
  if(Audio_Drv->Init(Audio_CompObj, &codec_init) != 0)
  {
    Error_Handler();
  }

  /* Start the playback */
  if(Audio_Drv->Play(Audio_CompObj) < 0)
  {
    Error_Handler();
  }

  

  /* Start loopback */
  while(1)
  {
    
    /* Wait a callback event */
    while(UpdatePointer==-1);
    
    int position = UpdatePointer;
    UpdatePointer = -1;
    
    /* Update the first or the second part of the buffer */
    for(int i = 0; i < PLAY_BUFF_SIZE/2; i++)
    {
      PlayBuff[i+position] = *(uint16_t *)(AUDIO_FILE_ADDRESS + PlaybackPosition);
      PlaybackPosition+=2;
    }
    
    /* Clean Data Cache to update the content of the SRAM */
    SCB_CleanDCache_by_Addr((uint32_t*)&PlayBuff[position], PLAY_BUFF_SIZE);
    
    /* check the end of the file */
    if((PlaybackPosition+PLAY_BUFF_SIZE/2) > AUDIO_FILE_SIZE)
    {
      PlaybackPosition = PLAY_HEADER;
    }
    
    if(UpdatePointer != -1)
    {
      /* Buffer update time is too long compare to the data transfer time */
      Error_Handler();
    }
  }
}










/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

