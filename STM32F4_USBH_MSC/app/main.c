/**
 * @file    main.c
 * @brief   USB MSC Host example based on example provided by ST
 * @date    9 kwi 2014
 * @author  Michal Ksiezopolski
 *
 *
 * @verbatim
 * Copyright (c) 2014 Michal Ksiezopolski.
 * All rights reserved. This program and the
 * accompanying materials are made available
 * under the terms of the GNU Public License
 * v3.0 which accompanies this distribution,
 * and is available at
 * http://www.gnu.org/licenses/gpl.html
 * @endverbatim
 */

#include <stdio.h>
#include <string.h>
#include <timers.h>
#include <led.h>
#include <comm.h>
#include <keys.h>
#include "usbh_core.h"
#include "usbh_usr.h"
#include "usbh_msc_core.h"

#define SYSTICK_FREQ 1000 ///< Frequency of the SysTick set at 1kHz.
#define COMM_BAUD_RATE 115200UL ///< Baud rate for communication with PC

void softTimerCallback(void);

#define DEBUG

#ifdef DEBUG
#define print(str, args...) printf(""str"%s",##args,"")
#define println(str, args...) printf("MAIN--> "str"%s",##args,"\r\n")
#else
#define print(str, args...) (void)0
#define println(str, args...) (void)0
#endif

__ALIGN_BEGIN USB_OTG_CORE_HANDLE      USB_OTG_Core __ALIGN_END;
__ALIGN_BEGIN USBH_HOST                USB_Host __ALIGN_END;

/**
 * @brief Main
 * @return None
 */
int main(void) {

  COMM_Init(COMM_BAUD_RATE); // initialize communication with PC
  println("Starting program"); // Print a string to terminal

  TIMER_Init(SYSTICK_FREQ); // Initialize timer

  // Add a soft timer with callback running every 1000ms
  int8_t timerID = TIMER_AddSoftTimer(1000, softTimerCallback);
  TIMER_StartSoftTimer(timerID); // start the timer

  LED_Init(LED0); // Add an LED
  LED_Init(LED1); // Add an LED
  LED_Init(LED2); // Add an LED
  LED_Init(LED3); // Add an LED
  LED_Init(LED5); // Add nonexising LED for test
  LED_ChangeState(LED5, LED_ON);

  uint8_t buf[255]; // buffer for receiving commands from PC
  uint8_t len;      // length of command

  // test another way of measuring time delays
  uint32_t softTimer = TIMER_GetTime(); // get start time for delay

  // Init USB Host Library
  USBH_Init(&USB_OTG_Core, 
            USB_OTG_FS_CORE_ID,
            &USB_Host,
            &USBH_MSC_cb, 
            &USR_cb);
  
  while (1) {


    // Host Task handler
    USBH_Process(&USB_OTG_Core, &USB_Host);
    
    // test delay method
    if (TIMER_DelayTimer(1000, softTimer)) {
      LED_Toggle(LED3);
      softTimer = TIMER_GetTime(); // get start time for delay
    }

    // check for new frames from PC
    if (!COMM_GetFrame(buf, &len)) {
      println("Got frame of length %d: %s", (int)len, (char*)buf);

//      char* tok = strtok((char*)buf, " ");
//      if(!strcmp(tok, ":LED")) {
//        println("LED command received >%s<", tok);
//      }
//
//      tok = strtok(NULL, " ");
//      if(!strcmp(tok, "0")) {
//        println("Turn LED0 on/off >%s<", tok);
//      }
//      println(">%s<", buf);

      // control LED0 from terminal
      if (!strcmp((char*)buf, ":LED 0 ON")) {
        LED_ChangeState(LED0, LED_ON);
      }
      if (!strcmp((char*)buf, ":LED 0 OFF")) {
        LED_ChangeState(LED0, LED_OFF);
      }
      if (!strcmp((char*)buf, ":LED 1 ON")) {
        LED_ChangeState(LED1, LED_ON);
      }
      if (!strcmp((char*)buf, ":LED 1 OFF")) {
        LED_ChangeState(LED1, LED_OFF);
      }
      if (!strcmp((char*)buf, ":USB ROOT")) {
        USB_ApplicationState = USB_USR_APP_READROOT;
      }
      if (!strcmp((char*)buf, ":USB WRITE")) {
        USB_ApplicationState = USB_USR_APP_WRITEFILE;
      }
      if (!strcmp((char*)buf, ":USB READ")) {
        USB_ApplicationState = USB_USR_APP_READFILE;
      }


    }

    TIMER_SoftTimersUpdate(); // run timers
  }
}

/**
 * @brief Callback function called on every soft timer overflow
 */
void softTimerCallback(void) {

  static uint8_t counter;

  switch (counter % 3) {

  case 0:
    LED_ChangeState(LED1, LED_OFF);
    LED_ChangeState(LED2, LED_OFF);
    break;

  case 1:
    LED_ChangeState(LED1, LED_ON);
    LED_ChangeState(LED2, LED_OFF);
    break;

  case 2:
    LED_ChangeState(LED1, LED_OFF);
    LED_ChangeState(LED2, LED_ON);
    break;

  }

//  println("Test string sent from STM32F4!!!"); // Print test string
  counter++;
}
