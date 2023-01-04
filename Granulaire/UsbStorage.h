/*
 * Usb_Storage.h
 *
 *  Created on: Oct 11, 2020
 *      Author: Gilles
 */

#ifdef __cplusplus
    extern "C" {
    #endif

#include "fatfs.h"

#ifdef __USBSTORAGE__
#define USB_EXTRN
#else
#define USB_EXTRN	extern
#endif


#define  USBDISK_DISCONNECTION_EVENT 	1
#define  USBDISK_CONNECTION_EVENT		2
#define  MSDDISK_DISCONNECTION_EVENT	3
#define  MSDDISK_CONNECTION_EVENT		4


#define	USB_DISK_UNIT	0
#define	MSD_DISK_UNIT	1
#define	NUM_DISK_UNIT	2

#define FILEMGR_LIST_DEPDTH                        24
#define FILEMGR_FILE_NAME_SIZE                     40
#define FILEMGR_FULL_PATH_SIZE                     256
#define FILEMGR_MAX_LEVEL                          4
#define FILETYPE_DIR                               0
#define FILETYPE_FILE                              1


typedef struct _FILELIST_LineTypeDef {
  uint8_t type;
  uint8_t name[FILEMGR_FILE_NAME_SIZE];
}FILELIST_LineTypeDef;

typedef struct _FILELIST_FileTypeDef {
  FILELIST_LineTypeDef  file[FILEMGR_LIST_DEPDTH] ;
  uint16_t              ptr;
}FILELIST_FileTypeDef;

USB_EXTRN FILELIST_FileTypeDef FileList;
USB_EXTRN FILELIST_FileTypeDef SoundList;
USB_EXTRN uint16_t NumObs;
USB_EXTRN uint16_t NumSound;


extern char USBHPath[4];
extern FATFS USBH_FatFs;
extern USBH_HandleTypeDef hUsbHostFS;

#ifdef __USBSTORAGE__
USB_EXTRN char USBDISK_Drive[4];    /* USB Host logical drive number */
#else
USB_EXTRN char USBDISK_Drive[4]; 			 /* USB Host logical drive number */
#endif
USB_EXTRN char MSDDISK_Drive[4];       		/* USB Host logical drive number */
USB_EXTRN FATFS MSDDISK_FatFs;
USB_EXTRN FATFS USBDISK_FatFs;
USB_EXTRN USBH_HandleTypeDef hUSBHost;

USB_EXTRN uint8_t Test_Init(void);
USB_EXTRN uint8_t USBStorage_Init(void);
USB_EXTRN FRESULT USBStorage_Parse(void);

#ifdef __cplusplus
}
#endif

