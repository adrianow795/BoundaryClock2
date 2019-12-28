#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "ThreadDisplayHandler.h"
#include "string.h"
#include "rl_net.h"                     // Keil.MDK-Pro::Network:CORE
#include <stdio.h>
 
/*----------------------------------------------------------------------------
 *      Thread 1 'ThreadUserEth': Sample thread
 *---------------------------------------------------------------------------*/
 
DispMsgContainer_t user_eth_buffer;
 
osThreadId_t tid_ThreadUserEth;                        // thread id
static const osThreadAttr_t DisplayThreatAtt = 
{
    .name = "DisplayThreadHandler",
    .attr_bits = osThreadDetached,
    .cb_mem = NULL,
    .cb_size = 0,
    .stack_mem = NULL,
    .stack_size = 0,
    .priority = osPriorityHigh,
    .tz_module = NULL,
    .reserved = 0
};



void ThreadUserEth (void *argument);                   // thread function


int Init_Thread (void) {
 
  tid_ThreadUserEth = osThreadNew(ThreadUserEth, NULL, NULL);
  if (tid_ThreadUserEth == NULL) 
  {
    return(-1);
  }
 
  return(0);
}
 
void ThreadUserEth (void *argument) 
{
    user_eth_buffer.msg_type = DISP_MSG_TYPE_USER;
    sprintf (user_eth_buffer.msg, " - State: UserEth Thread started.\n");
    ThreadDisplayHandler_PutMessage(&user_eth_buffer);
 
  while (1) {
    ; // Insert thread code here...
    osThreadYield();                            // suspend thread
  }
}


void netETH_Notify (uint32_t if_num, netETH_Event event, uint32_t val) 
{
    NET_ETH_LINK_INFO *info;
    memset(user_eth_buffer.msg, 0, DISPLAY_MSG_LENGTH);
    user_eth_buffer.msg_type = DISP_MSG_TYPE_USER;

    switch (event) 
    {
        case netETH_LinkDown:
            sprintf (user_eth_buffer.msg, " - ethNotify: Link is down\n");
            ThreadDisplayHandler_PutMessage(&user_eth_buffer);
            memset(user_eth_buffer.msg, 0, DISPLAY_MSG_LENGTH);
            break;
        case netETH_LinkUp:
            sprintf (user_eth_buffer.msg, " - ethNotify: Link is up\n");
            ThreadDisplayHandler_PutMessage(&user_eth_buffer);
            memset(user_eth_buffer.msg, 0, DISPLAY_MSG_LENGTH);
            info = (NET_ETH_LINK_INFO *)&val;
            switch (info->speed) 
            {
                case 0:
                    sprintf (user_eth_buffer.msg, " - ethNotify: 10 MBit\n");
                    ThreadDisplayHandler_PutMessage(&user_eth_buffer);
                    memset(user_eth_buffer.msg, 0, DISPLAY_MSG_LENGTH);
                    break;
                case 1:
                    sprintf (user_eth_buffer.msg, " - ethNotify: 100 MBit\n");
                    ThreadDisplayHandler_PutMessage(&user_eth_buffer);
                    memset(user_eth_buffer.msg, 0, DISPLAY_MSG_LENGTH);
                    break;
                case 2:
                    sprintf (user_eth_buffer.msg, " - ethNotify: 1 GBit\n");
                    ThreadDisplayHandler_PutMessage(&user_eth_buffer);
                    memset(user_eth_buffer.msg, 0, DISPLAY_MSG_LENGTH);
                    break;
            }
            switch (info->duplex) 
            {
                case 0:
                    sprintf (user_eth_buffer.msg, " - ethNotify: half dubplex\n");
                    ThreadDisplayHandler_PutMessage(&user_eth_buffer);
                    memset(user_eth_buffer.msg, 0, DISPLAY_MSG_LENGTH);
                    break;
                case 1:
                    sprintf (user_eth_buffer.msg, " - ethNotify: full dubplex\n");
                    ThreadDisplayHandler_PutMessage(&user_eth_buffer);
                    memset(user_eth_buffer.msg, 0, DISPLAY_MSG_LENGTH);
            }
            break;
        case netETH_Wakeup:
            sprintf (user_eth_buffer.msg, " - ethNotify: Wakeup frame received\n");
            ThreadDisplayHandler_PutMessage(&user_eth_buffer);
            memset(user_eth_buffer.msg, 0, DISPLAY_MSG_LENGTH);
            break;
        case netETH_TimerAlarm:
            sprintf (user_eth_buffer.msg, " - ethNotify: Timer alarm\n");
            ThreadDisplayHandler_PutMessage(&user_eth_buffer);
            memset(user_eth_buffer.msg, 0, DISPLAY_MSG_LENGTH);
            break;
    }
} 
