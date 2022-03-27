/**
  ******************************************************************************
  * @file    ff_gen_drv.c
  * @author  MCD Application Team
  * @brief   FatFs generic low level driver.
  *****************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics. All rights reserved.
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                       opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
**/
/* Includes ------------------------------------------------------------------*/
#include "ff_gen_drv.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
Disk_drvTypeDef disk = {{0},{0},{0},0};

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Links a compatible diskio driver/lun id and increments the number of active
  *         linked drivers.
  * @note   The number of linked drivers (volumes) is up to 10 due to FatFs limits.
  * @param  drv: pointer to the disk IO Driver structure
  * @param  path: pointer to the logical drive path
  * @param  lun : only used for USB Key Disk to add multi-lun management
            else the parameter must be equal to 0
  * @retval Returns 0 in case of success, otherwise 1.
  */
uint8_t FATFS_LinkDriverEx(const Diskio_drvTypeDef *drv, char *path, uint8_t lun)
{
  uint8_t ret = 1;
  uint8_t DiskNum = 0;

	DiskNum = path[0] - '0';
  if(DiskNum < FF_VOLUMES) {
    disk.is_initialized[DiskNum] = 0;
    disk.drv[DiskNum] = drv;
    disk.lun[DiskNum] = lun;
    ret = 0;
  }

  return ret;
}

/**
  * @brief  Links a compatible diskio driver and increments the number of active
  *         linked drivers.
  * @note   The number of linked drivers (volumes) is up to 10 due to FatFs limits
  * @param  drv: pointer to the disk IO Driver structure
  * @param  path: pointer to the logical drive path
  * @retval Returns 0 in case of success, otherwise 1.
  */
uint8_t FATFS_LinkDriver(const Diskio_drvTypeDef *drv, char *path)
{
  return FATFS_LinkDriverEx(drv, path, 0);
}

/**
  * @brief  Unlinks a diskio driver and decrements the number of active linked
  *         drivers.
  * @param  path: pointer to the logical drive path
  * @param  lun : not used
  * @retval Returns 0 in case of success, otherwise 1.
  */
uint8_t FATFS_UnLinkDriverEx(char *path, uint8_t lun)
{
  uint8_t DiskNum = 0;
  uint8_t ret = 1;

  if(disk.nbr >= 1)
  {
    DiskNum = path[0] - '0';
    if(disk.drv[DiskNum] != 0)
    {
      disk.drv[DiskNum] = 0;
      disk.lun[DiskNum] = 0;
      disk.nbr--;
      ret = 0;
    }
  }

  return ret;
}

/**
  * @brief  Unlinks a diskio driver and decrements the number of active linked
  *         drivers.
  * @param  path: pointer to the logical drive path
  * @retval Returns 0 in case of success, otherwise 1.
  */
uint8_t FATFS_UnLinkDriver(char *path)
{
  return FATFS_UnLinkDriverEx(path, 0);
}

/**
  * @brief  Gets number of linked drivers to the FatFs module.
  * @param  None
  * @retval Number of attached drivers.
  */
#if 0
uint8_t FATFS_GetAttachedDriversNbr(void)
{
  return disk.nbr;
}
#endif
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

