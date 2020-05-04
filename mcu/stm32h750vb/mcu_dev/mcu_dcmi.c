#include "mcu.h"
#include "mcu_dcmi.h"

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

#define DCMI_DR_ADDRESS       0x50050028

#define DCMI_RESET_PORT GPIOC
#define DCMI_RESET_PIN GPIO_PIN_0

#define DCMI_PWDN_PORT GPIOB
#define DCMI_PWDN_PIN GPIO_PIN_3


void DCMI_PWDN_RESET_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	HAL_GPIO_WritePin(DCMI_PWDN_PORT, DCMI_PWDN_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(DCMI_RESET_PORT, DCMI_RESET_PIN, GPIO_PIN_RESET);

   	GPIO_InitStructure.Pin = DCMI_RESET_PIN;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
   	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
   	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
   	HAL_GPIO_Init(DCMI_RESET_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = DCMI_PWDN_PIN;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
   	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
   	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
   	HAL_GPIO_Init(DCMI_PWDN_PORT, &GPIO_InitStructure);

	HAL_Delay(10);
	
	/*PWDN*/
	HAL_GPIO_WritePin(DCMI_PWDN_PORT, DCMI_PWDN_PIN, GPIO_PIN_RESET);
	HAL_Delay(10);
	/* reset */
	//HAL_GPIO_WritePin(DCMI_RESET_PORT, DCMI_RESET_PIN, GPIO_PIN_RESET);
	//HAL_Delay(100);
	HAL_GPIO_WritePin(DCMI_RESET_PORT, DCMI_RESET_PIN, GPIO_PIN_SET);
	HAL_Delay(10);

}


