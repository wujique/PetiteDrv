/**
  ******************************************************************************
  * @file    usb_bot.h
  * @author  MCD Application Team
  * @version V4.1.0
  * @date    26-May-2017
  * @brief   BOT State Machine management
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_BOT_H
#define __USB_BOT_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Bulk-only Command Block Wrapper */

typedef struct _Bulk_Only_CBW
{
  uint32_t dSignature;
  uint32_t dTag;
  uint32_t dDataLength;
  uint8_t  bmFlags;
  uint8_t  bLUN;
  uint8_t  bCBLength;
  uint8_t  CB[16];
}
Bulk_Only_CBW;

/* Bulk-only Command Status Wrapper */
typedef struct _Bulk_Only_CSW
{
  uint32_t dSignature;
  uint32_t dTag;
  uint32_t dDataResidue;
  uint8_t  bStatus;
}
Bulk_Only_CSW;
/* Exported constants --------------------------------------------------------*/

/*****************************************************************************/
/*********************** Bulk-Only Transfer State machine ********************/
/*****************************************************************************/
#define BOT_IDLE                      0       /* Idle state */
#define BOT_DATA_OUT                  1       /* Data Out state */
#define BOT_DATA_IN                   2       /* Data In state */
#define BOT_DATA_IN_LAST              3       /* Last Data In Last */
#define BOT_CSW_Send                  4       /* Command Status Wrapper */
#define BOT_ERROR                     5       /* error state */

#define BOT_CBW_SIGNATURE             0x43425355
#define BOT_CSW_SIGNATURE             0x53425355
#define BOT_CBW_PACKET_LENGTH         31

#define CSW_DATA_LENGTH               0x000D

/* CSW Status Definitions */
#define CSW_CMD_PASSED                0x00
#define CSW_CMD_FAILED                0x01
#define CSW_PHASE_ERROR               0x02

#define SEND_CSW_DISABLE              0
#define SEND_CSW_ENABLE               1

#define DIR_IN                        0
#define DIR_OUT                       1
#define BOTH_DIR                      2

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void Mass_Storage_In (void);
void Mass_Storage_Out (void);
void CBW_Decode(void);
void Transfer_Data_Request(uint8_t* Data_Pointer, uint16_t Data_Len);
void Set_CSW (uint8_t CSW_Status, uint8_t Send_Permission);
void Bot_Abort(uint8_t Direction);

#endif /* __USB_BOT_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

