/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file            : usb_host.c
  * @version         : v1.0_Cube
  * @brief           : This file implements the USB Host
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/

#include "usb_host.h"
#include "usbh_core.h"
#include "usbh_msc.h"

/* USER CODE BEGIN Includes */
#include "ff.h"
#include "fatfs.h"
FATFS USBH_FatFs;

#include "stm32469i_discovery.h"
#include "MessageEvent.h"

/* USER CODE END Includes */

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USB Host core handle declaration */
USBH_HandleTypeDef hUsbHostFS;
ApplicationTypeDef Appli_state = APPLICATION_IDLE;

/*
 * -- Insert your variables declaration here --
 */
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*
 * user callback declaration
 */
static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id);

/*
 * -- Insert your external function declaration here --
 */
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
  * Init USB host library, add supported class and start the library
  * @retval None
  */
void MX_USB_HOST_Init(void)
{
  /* USER CODE BEGIN USB_HOST_Init_PreTreatment */
  
  /* USER CODE END USB_HOST_Init_PreTreatment */

  /* Init host Library, add supported class and start the library. */
  if (USBH_Init(&hUsbHostFS, USBH_UserProcess, HOST_FS) != USBH_OK)
  {
    Error_Handler();
  }
  if (USBH_RegisterClass(&hUsbHostFS, USBH_MSC_CLASS) != USBH_OK)
  {
    Error_Handler();
  }
  if (USBH_Start(&hUsbHostFS) != USBH_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USB_HOST_Init_PostTreatment */

  /* USER CODE END USB_HOST_Init_PostTreatment */
}

/*
 * user callback definition
 */
static void USBH_UserProcess  (USBH_HandleTypeDef *phost, uint8_t id)
{
  /* USER CODE BEGIN CALL_BACK_1 */
  switch(id)
  {
  case HOST_USER_SELECT_CONFIGURATION:
  break;

  case HOST_USER_DISCONNECTION:
  BSP_LED_Off(LED_GREEN);
  if(f_mount(NULL, "", 0) != FR_OK)
  {
	//LCD_ErrLog("ERROR : Cannot DeInitialize FatFs! \n");
  }
  osMessagePut(AppliEvent,MESS_USB_DISCONNECT,0);
  break;

  case HOST_USER_CLASS_ACTIVE:
  BSP_LED_On(LED_GREEN);
//  FIL MyFile;
//  uint32_t wbytes; /* File write counts */
//  uint8_t wtext[] = "FUCKING MALLOC DEVIL"; /* File write buffer */
//  if(1)
//  {
//	  if(f_mount(&USBH_FatFs,(TCHAR const*)USBHPath, 0) == FR_OK)
//	  {
//		  f_open(&MyFile, "FUCK.TXT", FA_CREATE_ALWAYS | FA_WRITE);
//		  f_write(&MyFile, wtext, sizeof(wtext), (void *)&wbytes);
//		  f_close(&MyFile);
//	  }
//  }
  if(USBStorage_Init())
	  USBStorage_Parse();
  osMessagePut(AppliEvent,MESS_USBLISTFILE_READY,0);
  break;

  case HOST_USER_CONNECTION:
  break;

  default:
  break;
  }
  /* USER CODE END CALL_BACK_1 */
}

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
