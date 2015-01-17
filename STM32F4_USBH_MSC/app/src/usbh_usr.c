/**
  ******************************************************************************
  * @file    usbh_usr.c
  * @author  MCD Application Team
  * @version V2.1.0
  * @date    19-March-2012
  * @brief   This file includes the usb host library user callbacks
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include "usbh_usr.h"
#include "ff.h"
#include "usbh_msc_core.h"
#include "usbh_msc_scsi.h"
#include "usbh_msc_bot.h"


extern USB_OTG_CORE_HANDLE          USB_OTG_Core;

uint8_t USBH_USR_ApplicationState = USH_USR_FS_INIT;
uint8_t filenameString[15]  = {0};

FATFS fatfs;
FIL file;
uint8_t line_idx = 0;

/*  Points to the DEVICE_PROP structure of current device */
/*  The purpose of this register is to speed up the execution */

USBH_Usr_cb_TypeDef USR_cb =
{
  USBH_USR_Init,
  USBH_USR_DeInit,
  USBH_USR_DeviceAttached,
  USBH_USR_ResetDevice,
  USBH_USR_DeviceDisconnected,
  USBH_USR_OverCurrentDetected,
  USBH_USR_DeviceSpeedDetected,
  USBH_USR_Device_DescAvailable,
  USBH_USR_DeviceAddressAssigned,
  USBH_USR_Configuration_DescAvailable,
  USBH_USR_Manufacturer_String,
  USBH_USR_Product_String,
  USBH_USR_SerialNum_String,
  USBH_USR_EnumerationDone,
  USBH_USR_UserInput,
  USBH_USR_MSC_Application,
  USBH_USR_DeviceNotSupported,
  USBH_USR_UnrecoveredError
    
};


#define DEBUG

#ifdef DEBUG
#define print(str, args...) printf(""str"%s",##args,"")
#define println(str, args...) printf("USB_USR--> "str"%s",##args,"\r\n")
#else
#define print(str, args...) (void)0
#define println(str, args...) (void)0
#endif

/**
* @brief  USBH_USR_Init 
*         Displays the message on LCD for host lib initialization
* @param  None
* @retval None
*/
void USBH_USR_Init(void) {

  println("USB Host library started.");
}

/**
* @brief  USBH_USR_DeviceAttached 
*         Displays the message on LCD on device attached
* @param  None
* @retval None
*/
void USBH_USR_DeviceAttached(void) {
  println("Device attached");
}

/**
* @brief  USBH_USR_UnrecoveredError
* @param  None
* @retval None
*/
void USBH_USR_UnrecoveredError (void) {
  println("Unrecovered error");
}

/**
* @brief  USBH_DisconnectEvent
*         Device disconnect event
* @param  None
* @retval Staus
*/
void USBH_USR_DeviceDisconnected (void) {
  println("Device disconnected");
}
/**
* @brief  USBH_USR_ResetUSBDevice 
* @param  None
* @retval None
*/
void USBH_USR_ResetDevice(void) {
  println("Device reset");
}

/**
* @brief  USBH_USR_DeviceSpeedDetected 
*         Displays the message on LCD for device speed
* @param  Device speed
* @retval None
*/
void USBH_USR_DeviceSpeedDetected(uint8_t DeviceSpeed) {
  if(DeviceSpeed == HPRT0_PRTSPD_HIGH_SPEED) {
    println("High speed device");
  } else if(DeviceSpeed == HPRT0_PRTSPD_FULL_SPEED) {
    println("Full speed device");
  } else if(DeviceSpeed == HPRT0_PRTSPD_LOW_SPEED) {
    println("Low speed device");
  } else {
    println("Error in speed device");
  }
}

/**
* @brief  USBH_USR_Device_DescAvailable 
*         Displays the message on LCD for device descriptor
* @param  device descriptor
* @retval None
*/
void USBH_USR_Device_DescAvailable(void *DeviceDesc) {

  USBH_DevDesc_TypeDef *hs;
  hs = DeviceDesc;  
  
  println("VID : %04xh" , (unsigned int)((*hs).idVendor));
  println("PID : %04xh" , (unsigned int)((*hs).idProduct));
}

/**
* @brief  USBH_USR_DeviceAddressAssigned 
*         USB device is successfully assigned the Address 
* @param  None
* @retval None
*/
void USBH_USR_DeviceAddressAssigned(void) {
  println("Device addressed");
}


/**
* @brief  USBH_USR_Conf_Desc 
*         Displays the message on LCD for configuration descriptor
* @param  Configuration descriptor
* @retval None
*/
void USBH_USR_Configuration_DescAvailable(USBH_CfgDesc_TypeDef * cfgDesc,
                                          USBH_InterfaceDesc_TypeDef *itfDesc,
                                          USBH_EpDesc_TypeDef *epDesc) {
  USBH_InterfaceDesc_TypeDef *id;
  
  id = itfDesc;  
  
  if((*id).bInterfaceClass  == 0x08) {
    println("MSC device");
  } else if((*id).bInterfaceClass  == 0x03) {
    println("HID device");
  }    
}

/**
* @brief  USBH_USR_Manufacturer_String 
*         Displays the message on LCD for Manufacturer String 
* @param  Manufacturer String 
* @retval None
*/
void USBH_USR_Manufacturer_String(void *ManufacturerString) {
  println("Manufacturer : %s", (char *)ManufacturerString);
}

/**
* @brief  USBH_USR_Product_String 
*         Displays the message on LCD for Product String
* @param  Product String
* @retval None
*/
void USBH_USR_Product_String(void *ProductString) {
  println("Product : %s", (char *)ProductString);
}

/**
* @brief  USBH_USR_SerialNum_String 
*         Displays the message on LCD for SerialNum_String 
* @param  SerialNum_String 
* @retval None
*/
void USBH_USR_SerialNum_String(void *SerialNumString) {
  println( "Serial Number : %s", (char *)SerialNumString);
} 

/**
* @brief  EnumerationDone 
*         User response request is displayed to ask application jump to class
* @param  None
* @retval None
*/
void USBH_USR_EnumerationDone(void) {
  println("Enumeration done");
  
} 

/**
* @brief  USBH_USR_DeviceNotSupported
*         Device is not supported
* @param  None
* @retval None
*/
void USBH_USR_DeviceNotSupported(void) {
  println ("Device not supported");
}  


/**
* @brief  USBH_USR_UserInput
*         User Action for application state entry
* @param  None
* @retval USBH_USR_Status : User response for key button
*/
USBH_USR_Status USBH_USR_UserInput(void)
{
  USBH_USR_Status usbh_usr_status;
  
  usbh_usr_status = USBH_USR_NO_RESP;  
  
  /*Key B3 is in polling mode to detect user action */
//  if(STM_EVAL_PBGetState(Button_KEY) == RESET)
//  {
    
    usbh_usr_status = USBH_USR_RESP_OK;
    
//  }
  return usbh_usr_status;
}  

/**
* @brief  USBH_USR_OverCurrentDetected
*         Over Current Detected on VBUS
* @param  None
* @retval Staus
*/
void USBH_USR_OverCurrentDetected (void) {
  println("Overcurrent detected.");
}

static uint8_t Explore_Disk (char* path , uint8_t recu_level);
/**
* @brief  USBH_USR_MSC_Application 
*         Demo application for mass storage
* @param  None
* @retval Staus
*/
int USBH_USR_MSC_Application(void) {

  FRESULT res;
  char writeTextBuff[] = "Hello from STM32F4 USB HOST!!!";
  uint32_t bytesWritten;
  uint32_t bytesToWrite;
  
  switch(USBH_USR_ApplicationState) {
  case USH_USR_FS_INIT: 
    
    /* Initialises the File System*/
    if ( f_mount( 0, &fatfs ) != FR_OK )  {
      /* fs initialisation fails*/
      println("Cannot initialize File System.");
      return(-1);
    }
    println("File System initialized.");
    println("Disk capacity : %d KB",
        (unsigned int)(USBH_MSC_Param.MSCapacity / 1024 *
      USBH_MSC_Param.MSPageLength));
    
    if(USBH_MSC_Param.MSWriteProtect == DISK_WRITE_PROTECTED) {
      println("Write protected");
    }
    
    USBH_USR_ApplicationState = USH_USR_FS_READLIST;
    break;
    
  case USH_USR_FS_READLIST:
    
    println("Reading root dir");
    Explore_Disk("0:/", 1);
    line_idx = 0;   
    USBH_USR_ApplicationState = USH_USR_FS_WRITEFILE;
    
    break;
    
  case USH_USR_FS_WRITEFILE:
    
    USB_OTG_BSP_mDelay(100);
    
    /* Writes a text file, STM32.TXT in the disk*/
    println("Writing File to disk flash ...");
    if(USBH_MSC_Param.MSWriteProtect == DISK_WRITE_PROTECTED) {
      
      println ( "Disk flash is write protected");
      USBH_USR_ApplicationState = USH_USR_FS_DRAW;
      break;
    }
    
    /* Register work area for logical drives */
    f_mount(0, &fatfs);
    
    if(f_open(&file, "0:HELLO.TXT", FA_CREATE_ALWAYS | FA_WRITE) == FR_OK) {
      /* Write buffer to file */
      bytesToWrite = strlen(writeTextBuff);
      res= f_write (&file, (void*)writeTextBuff,
          bytesToWrite, &(bytesWritten));
      
      if((bytesWritten == 0) || (res != FR_OK)) {
        /*EOF or Error*/
        println("STM32.TXT CANNOT be writen.");
      }
      else if (bytesWritten != bytesToWrite) {
        println("'STM32.TXT' wrote incomplete");
      } else {
        println("'STM32.TXT' file created");
      }
      
      /*close file and filesystem*/
      f_close(&file);
      f_mount(0, NULL); 
    } else {
      println ("File write error");
    }

    USBH_USR_ApplicationState = USH_USR_FS_DRAW; 
  
    break;
    
  case USH_USR_FS_DRAW:

//    while(HCD_IsDeviceConnected(&USB_OTG_Core))
//    {
//      if ( f_mount( 0, &fatfs ) != FR_OK )
//      {
//        /* fat_fs initialisation fails*/
//        return(-1);
//      }
//      return Image_Browser("0:/");
//    }
    break;
  default: break;
  }
  return(0);
}

/**
* @brief  Explore_Disk 
*         Displays disk content
* @param  path: pointer to root path
* @retval None
*/
static uint8_t Explore_Disk (char* path , uint8_t recu_level) {

  FRESULT res;
  FILINFO fno;
  DIR dir;
  char *fn;
  char tmp[30];
  
  res = f_opendir(&dir, path);

  if (res == FR_OK) {

    println("\nPrinting root dir *************");
    while(HCD_IsDeviceConnected(&USB_OTG_Core)) {
      res = f_readdir(&dir, &fno);
      if (res != FR_OK || fno.fname[0] == 0)  {
        break;
      }

      if (fno.fname[0] == '.') {
        continue;
      }

      fn = fno.fname;
      strcpy(tmp, fn); 
      
      if (recu_level == 1) {

      } else if (recu_level == 2) {

      }

      if((fno.fattrib & AM_MASK) == AM_DIR) {
         strcat(tmp, " D");
      }
      println("%s", tmp);

//      if(((fno.fattrib & AM_MASK) == AM_DIR)&&(recu_level == 1))
//      {
//        Explore_Disk(fn, 2);
//      }
    }
    println("Finished printing root dir *************\n");
  }

  return res;
}

/**
* @brief  USBH_USR_DeInit
*         Deint User state and associated variables
* @param  None
* @retval None
*/
void USBH_USR_DeInit(void) {
  println("Deinit");
  USBH_USR_ApplicationState = USH_USR_FS_INIT;
}


/**
* @}
*/ 

/**
* @}
*/ 

/**
* @}
*/

/**
* @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

