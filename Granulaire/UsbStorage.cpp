/*
 * Usb_Storage.cpp
 *
 *  Created on: Oct 11, 2020
 *      Author: Gilles
 */

#define __USBSTORAGE__
#include "USBStorage.h"

/**************************************************/
/*                                                */
/*                                                */
/*                                                */
/*                                                */
/**************************************************/
uint8_t USBStorage_Init(void)
{
  NumObs = 0;

  /* Link the USB Key disk I/O driver */
  if(f_mount(&USBH_FatFs,(TCHAR const*)USBHPath, 0) == FR_OK)
  {
	/* FatFs Initialization Error */
	//printf("Cannot Initialize FatFs! \n");
	return 1;
  }
  else
  {
	//printf("INFO : FatFs Initialized! \n");
	return 0;
  }
}
/**************************************************/
/*                                                */
/*                                                */
/*                                                */
/*                                                */
/**************************************************/
FRESULT USBStorage_Parse(void)
{
  FRESULT res = FR_OK;
  FILINFO fno;
  DIR dir;
  char *fn;


  res = f_opendir(&dir, USBHPath);
  FileList.ptr = 0;

  if(res == FR_OK)
  {
    while(USBH_MSC_IsReady(&hUsbHostFS))
    {
      res = f_readdir(&dir, &fno);
      if(res != FR_OK || fno.fname[0] == 0)
      {
        break;
      }
      if(fno.fname[0] == '.')
      {
        continue;
      }

      fn = fno.fname;

      if(FileList.ptr < FILEMGR_LIST_DEPDTH)
      {
        if((fno.fattrib & AM_DIR) == 0)
        {
          if((strstr(fn, "wav")) || (strstr(fn, "WAV")))
          {
            strncpy((char *)FileList.file[FileList.ptr].name, (char *)fn, FILEMGR_FILE_NAME_SIZE);
            FileList.file[FileList.ptr].type = FILETYPE_FILE;
            FileList.ptr++;
          }
        }
      }
    }
  }
  NumObs = FileList.ptr;
  f_closedir(&dir);
  return res;
}

