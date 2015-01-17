

#ifndef __USH_USR_H__
#define __USH_USR_H__

#include "usbh_core.h"

extern  USBH_Usr_cb_TypeDef USR_cb;

#define USB_USR_APP_INIT       0
#define USB_USR_APP_READROOT   1
#define USB_USR_APP_WRITEFILE  2
#define USB_USR_APP_IDLE       3

extern  uint8_t USB_ApplicationState;
extern uint8_t USB_Filename[];


#endif /*__USH_USR_H__*/
