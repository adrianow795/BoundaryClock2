/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    HTTP_Server.c
 * Purpose: HTTP Server example
 *----------------------------------------------------------------------------*/

#include <stdio.h>

#include "main.h"

#include "rl_net.h"                     // Keil.MDK-Pro::Network:CORE

#include "stm32f7xx_hal.h"              // Keil::Device:STM32Cube HAL:Common
#include "Board_LED.h"                  // ::Board Support:LED
#include "Board_Buttons.h"              // ::Board Support:Buttons

#include "ThreadDisplayHandler.h"
#include "string.h"

DispMsgContainer_t http_text_buffer;
// Main stack size must be multiple of 8 Bytes
#define APP_MAIN_STK_SZ (1024U)
uint64_t app_main_stk[APP_MAIN_STK_SZ / 8];
const osThreadAttr_t app_main_attr = {
  .stack_mem  = &app_main_stk[0],
  .stack_size = sizeof(app_main_stk)
};




extern uint8_t  get_button     (void);
extern void     netDHCP_Notify (uint32_t if_num, uint8_t option, const uint8_t *val, uint32_t len);

extern bool LEDrun;
//extern char lcd_text[2][20+1];

extern osThreadId_t TID_Display;
extern osThreadId_t TID_Led;

bool LEDrun;
/*
char lcd_text[2][20+1] = { "LCD line 1",
                           "LCD line 2" };
*/

/* Thread IDs */
osThreadId_t TID_Display;
osThreadId_t TID_Led;

/* Thread declarations */
static void BlinkLed (void *arg);
static void Display  (void *arg);

__NO_RETURN void app_main (void *arg);



/* Read digital inputs */
uint8_t get_button (void) 
{
  return ((uint8_t)Buttons_GetState ());
}

/* IP address change notification */
void netDHCP_Notify (uint32_t if_num, uint8_t option, const uint8_t *val, uint32_t len) 
{

  (void)if_num;
  (void)val;
  (void)len;

  if (option == NET_DHCP_OPTION_IP_ADDRESS) {
    /* IP address change, trigger LCD update */
    osThreadFlagsSet (TID_Display, 0x01);
  }
}

/*
        DONT USE -s OPTION IN STRING FORMATING

*/


/*----------------------------------------------------------------------------
  Thread 'Display': LCD display handler
 *---------------------------------------------------------------------------*/
static __NO_RETURN void Display (void *arg) 
{
    static uint8_t ip_addr[NET_ADDR_IP6_LEN];
    static char    ip_ascii[40];
    static char    buf[25];
    uint32_t x = 5;

    (void)arg;
    
    http_text_buffer.msg_type = DISP_MSG_TYPE_USER;
    sprintf (http_text_buffer.msg, " - State: Display Thread started.\n");
    ThreadDisplayHandler_PutMessage(&http_text_buffer);
    memset(http_text_buffer.msg, 0, DISPLAY_MSG_LENGTH);

    
    
    /* Print Link-local IPv6 address */
    /*netIF_GetOption (NET_IF_CLASS_ETH,
                   netIF_OptionIP6_LinkLocalAddress, ip_addr, sizeof(ip_addr));

    netIP_ntoa(NET_ADDR_IP6, ip_addr, ip_ascii, sizeof(ip_ascii));
    
    //sprintf (http_text_buffer.msg, ip_ascii);
    memcpy(http_text_buffer.msg, ip_ascii, 40);
    ThreadDisplayHandler_PutMessage(&http_text_buffer);
    memset(http_text_buffer.msg, 0, DISPLAY_MSG_LENGTH);
    
    http_text_buffer.msg_type = DISP_MSG_TYPE_USER;
    sprintf (http_text_buffer.msg, "IP6:%s", ip_ascii);
    ThreadDisplayHandler_PutMessage(&http_text_buffer);
*/
    sprintf (http_text_buffer.msg, " - DHCP: Waiting for ip...\n");
    ThreadDisplayHandler_PutMessage(&http_text_buffer);
    memset(http_text_buffer.msg, 0, DISPLAY_MSG_LENGTH);
    while(1) 
    {
        /* Wait for signal from DHCP */
        osThreadFlagsWait (0x01U, osFlagsWaitAny, osWaitForever);

        /* Retrieve and print IPv4 address */
        netIF_GetOption (NET_IF_CLASS_ETH,
                         netIF_OptionIP4_Address, ip_addr, sizeof(ip_addr));


        sprintf (http_text_buffer.msg," - IP4: %d.%d.%d.%d \n", ip_addr[0], ip_addr[1], ip_addr[2], ip_addr[3] );
        ThreadDisplayHandler_PutMessage(&http_text_buffer);

    }
}

/*----------------------------------------------------------------------------
  Thread 'BlinkLed': Blink the LEDs on an eval board
 *---------------------------------------------------------------------------*/
static __NO_RETURN void BlinkLed (void *arg) {
  const uint8_t led_val[] = { 0x03,0x01,0x02 };
    
    http_text_buffer.msg_type = DISP_MSG_TYPE_USER;
    sprintf (http_text_buffer.msg, " - State: BlinkLed Thread started.\n");
    ThreadDisplayHandler_PutMessage(&http_text_buffer);
    memset(http_text_buffer.msg, 0, DISPLAY_MSG_LENGTH);
  
  uint32_t cnt = 0U;

  (void)arg;

  LEDrun = true;
  while(1) {
    /* Every 100 ms */
    if (LEDrun == true) {
      LED_SetOut (led_val[cnt]);
      if (++cnt >= sizeof(led_val)) {
        cnt = 0U;
      }
    }
    osDelay (100);
  }
}

/*----------------------------------------------------------------------------
  Main Thread 'main': Run Network
 *---------------------------------------------------------------------------*/
__NO_RETURN void app_main (void *arg) {
  (void)arg;

  LED_Initialize();
  Buttons_Initialize();

  netInitialize ();

  TID_Led     = osThreadNew (BlinkLed, NULL, NULL);
  TID_Display = osThreadNew (Display,  NULL, NULL);

  osThreadExit();
}
