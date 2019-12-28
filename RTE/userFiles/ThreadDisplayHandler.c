#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "ThreadDisplayHandler.h"
#include "stm32f769i_discovery_lcd.h"   // Keil.STM32F769I-Discovery::Board Support:Drivers:LCD
#include "lcd_log.h"
#include "string.h"



/*----------------------------------------------------------------------------
 *      Thread 1 'ThreadDisplayHandler': This thread handles LCD
 *---------------------------------------------------------------------------*/
 
osThreadId_t tid_ThreadDisplayHandler;                        // thread id
static osMessageQueueId_t MessageQueueDisplay_ID;             // queue id
static const osMessageQueueAttr_t DisplayQueueAtt = 
{
    .name = "DisplayQueue",
    .attr_bits = NULL,
    .cb_mem = NULL,
    .cb_size = 0,
    .mq_mem = NULL,
    .mq_size = 0
};

static const osThreadAttr_t DisplayThreatAtt = 
{
    .name = "DisplayThreadHandler",
    .attr_bits = osThreadDetached,
    .cb_mem = NULL,
    .cb_size = 0,
    .stack_mem = NULL,
    .stack_size = 0,
    .priority = osPriorityBelowNormal,
    .tz_module = NULL,
    .reserved = 0
};


void ThreadDisplayHandler (void *argument);                   // thread function
static void BSP_LCD_Config(void);

/**
  * @brief  Initializes the STM32F769I-DISOVERY's LCD and LEDs resources.
  * @param  None
  * @retval None
  */
int ThreadDisplayHandler_Init (void) 
{
    uint32_t msg_size = sizeof(DispMsgContainer_t);
    BSP_LCD_Config();
    MessageQueueDisplay_ID = osMessageQueueNew(DISPLAY_MSG_CNT, msg_size, &DisplayQueueAtt);
    tid_ThreadDisplayHandler = osThreadNew(ThreadDisplayHandler, NULL, &DisplayThreatAtt);
    if (tid_ThreadDisplayHandler == NULL || MessageQueueDisplay_ID == NULL) 
    {
        return(-1);
    }
    return(0);
}
 
void ThreadDisplayHandler (void *argument) 
{
    osStatus_t DisplayQueueStatus = osErrorTimeout;
    DispMsgContainer_t Message;
    memset(Message.msg, 0, DISPLAY_MSG_LENGTH);
    while (1) 
    {
        osMessageQueueGet(MessageQueueDisplay_ID, (void*)(&Message), NULL, osWaitForever);
        switch(Message.msg_type)
        {
            case DISP_MSG_TYPE_USER:
                LCD_UsrLog(Message.msg);
                break;
            
            case DISP_MSG_TYPE_DEBUG:
                LCD_DbgLog(Message.msg);
                break;
            
            default:
                LCD_ErrLog(Message.msg);
                break;
        }
        memset(Message.msg, 0, DISPLAY_MSG_LENGTH);
    }
}



/**
  * @brief  Initializes the STM32F769I-DISOVERY's LCD and LEDs resources.
  * @param  None
  * @retval None
  */
static void BSP_LCD_Config(void)
{
  /* Initialize the LCD */
  BSP_LCD_Init();
  
  /* Initialize the LCD Layers */
  BSP_LCD_LayerDefaultInit(1, LCD_FB_START_ADDRESS);
  
  /* Set LCD Foreground Layer  */
  BSP_LCD_SelectLayer(1);
  
  BSP_LCD_SetFont(&LCD_DEFAULT_FONT);
  
  /* Initialize LCD Log module */
  LCD_LOG_Init();
  
  /* Show Header and Footer texts */
  LCD_LOG_SetHeader((uint8_t *)"PTP IEEE 1588");
  LCD_LOG_SetFooter((uint8_t *)"STM32F769I-DISOVERY board");
  
  LCD_UsrLog ("  State: Initialization started\n");
}


osStatus_t ThreadDisplayHandler_PutMessage(DispMsgContainer_t* queue_msg)
{
    osStatus_t q_status = osErrorTimeout;
    osMessageQueuePut(MessageQueueDisplay_ID, (void*)queue_msg, 1, 100);
    return q_status;
}

