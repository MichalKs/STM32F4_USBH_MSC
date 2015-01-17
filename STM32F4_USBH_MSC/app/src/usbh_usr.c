
#include <string.h>
#include <stdio.h>
#include "usbh_usr.h"
#include "ff.h"
#include "usbh_msc_core.h"
#include "usbh_msc_scsi.h"
#include "usbh_msc_bot.h"

extern USB_OTG_CORE_HANDLE          USB_OTG_Core;

uint8_t USB_ApplicationState = USB_USR_APP_INIT;
uint8_t USB_Filename[15]  = "STM32F4.TXT";

static FATFS fatfs;
static FIL file;

void USBH_USR_Init(void);
void USBH_USR_DeInit(void);
void USBH_USR_DeviceAttached(void);
void USBH_USR_ResetDevice(void);
void USBH_USR_DeviceDisconnected (void);
void USBH_USR_OverCurrentDetected (void);
void USBH_USR_DeviceSpeedDetected(uint8_t DeviceSpeed);
void USBH_USR_Device_DescAvailable(void *);
void USBH_USR_DeviceAddressAssigned(void);
void USBH_USR_Configuration_DescAvailable(USBH_CfgDesc_TypeDef * cfgDesc,
                                          USBH_InterfaceDesc_TypeDef *itfDesc,
                                          USBH_EpDesc_TypeDef *epDesc);
void USBH_USR_Manufacturer_String(void *);
void USBH_USR_Product_String(void *);
void USBH_USR_SerialNum_String(void *);
void USBH_USR_EnumerationDone(void);
USBH_USR_Status USBH_USR_UserInput(void);
void USBH_USR_DeInit(void);
void USBH_USR_DeviceNotSupported(void);
void USBH_USR_UnrecoveredError(void);
int USBH_USR_MSC_Application(void);


USBH_Usr_cb_TypeDef USR_cb = {
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

  switch(USB_ApplicationState) {
  case USB_USR_APP_INIT:

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
    USB_ApplicationState = USB_USR_APP_IDLE;
    break;

  case USB_USR_APP_READROOT:

    println("Reading root dir");
    Explore_Disk("0:/", 1);
    USB_ApplicationState = USB_USR_APP_IDLE;
    break;

  case USB_USR_APP_WRITEFILE:

    USB_OTG_BSP_mDelay(100);

    /* Writes a text file, STM32.TXT in the disk*/
    println("Writing File to disk flash ...");
    if(USBH_MSC_Param.MSWriteProtect == DISK_WRITE_PROTECTED) {

      println ( "Disk flash is write protected");
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

      /*close file*/
      f_close(&file);

    } else {
      println ("File write error");
    }
    USB_ApplicationState = USB_USR_APP_IDLE;
    break;

  case USB_USR_APP_IDLE:

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
  } else {
    println("Read root error");
  }



  return res;
}

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
USBH_USR_Status USBH_USR_UserInput(void) {
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

/**
* @brief  USBH_USR_DeInit
*         Deint User state and associated variables
* @param  None
* @retval None
*/
void USBH_USR_DeInit(void) {
  println("Deinit");
  f_mount(0, NULL);
  USB_ApplicationState = USB_USR_APP_INIT;
}
