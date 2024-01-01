

#include "mcu.h"
#include "petite_config.h"

#if (DRV_CAMERA_MODULE == 1)

#include "board_sysconf.h"

#include "mcu_dcmi.h"
#include "camera_api.h"
#include "drv_ov9655.h"
#include "drv_ov2640.h"
#include "drv_lcd.h"
#include "log.h"

#include "petite.h"


Camera_TypeDef Camera;


s32 CameraGd = -2;
/**
 *@brief:	   dev_camera_config
 *@details:    配置摄像头
 *@param[in]   Format 图像格式
 *@param[out]  无
 *@retval:	   
 */
void camera_config(ImageFormat_TypeDef Format)
{	

}
/**
 *@brief:	   dev_camera_fresh
 *@details:    启动摄像头数据传输
 *@param[in]   
 *@param[out]  无
 *@retval:	   
 */
s32 camera_capture(void)
{

	return 0;
}
/**
 *@brief:	   dev_camera_stop
 *@details:    停止摄像头数据传输
 *@param[in]   
 *@param[out]  无
 *@retval:	   
 */
s32 dev_camera_stop(void)
{
	
	return 0;
}
/**
 *@brief:	   dev_camera_init
 *@details:    初始化摄像头
 *@param[in]   
 *@param[out]  无
 *@retval:	   
 */
s32 camera_init(void)
{
	int res = -1;

	wjq_log(LOG_DEBUG, "\r\ncamera_init\r\n");

	if (res == -1) {
		res = OV2640_Init();
		if (res == 0) {
			Camera = OV2640_CAMERA;
		} else wjq_log(LOG_WAR, "no OV2640!\r\n");
	}

	if (res == -1) {
		res = OV5640_Init();
		if (res == 0) {
			wjq_log(LOG_WAR, "OV5640 init!\r\n");
			Camera = OV5640_CAMERA;
		}
	}

	
	if (res == -1) {
		wjq_log(LOG_WAR, "Check the Camera HW and try again\r\n");
		wjq_log(LOG_WAR, "check i2c oled and camera...\r\n");
		return -1;  
	}
	
	CameraGd = -1;

	return 0;
}

s32 camera_prob()
{
	
}
/**
 *@brief:	   dev_camera_open
 *@details:    打开摄像头
 *@param[in]   
 *@param[out]  无
 *@retval:	   
 */
s32 camera_open(void)
{
	if(CameraGd!= -1)
		return -1;

	CameraGd = 0;
	return 0;	
}
/**
 *@brief:	   dev_camera_close
 *@details:    关闭摄像头
 *@param[in]   
 *@param[out]  无
 *@retval:	   
 */
s32 camera_close(void)
{
	if(CameraGd!= 0)
		return -1;

	CameraGd = -1;
	dev_camera_stop(); 
	return 0;
}

// Flash Lamp
//  sw:  0: off
//       1:  on
void camera_Flash_Lamp(uint8_t sw)
{
	if ( Camera == OV5640_CAMERA ){
		OV5640_Flash_Lamp(sw);
	} else {
	#if 0
		if (sw == 1)
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6, GPIO_PIN_SET);
		else
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6, GPIO_PIN_RESET);
	#endif
	}
}


/*--------------------------------------------------------------*/
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

