#ifndef THREAD_DISPLAY_HANDLER_H_
#define THREAD_DISPLAY_HANDLER_H_

#include "cmsis_os2.h"                  // ::CMSIS:RTOS2

#define DISPLAY_MSG_CNT          ((uint32_t) 10)
#define DISPLAY_MSG_LENGTH       ((uint32_t) 50)

typedef enum  { DISP_MSG_TYPE_ERROR, DISP_MSG_TYPE_USER, DISP_MSG_TYPE_DEBUG} DispMsgType_t;

typedef struct 
{
   char msg [DISPLAY_MSG_LENGTH];
   DispMsgType_t msg_type;
} DispMsgContainer_t;

int ThreadDisplayHandler_Init (void);
osStatus_t ThreadDisplayHandler_PutMessage(DispMsgContainer_t* queue_msg);


#endif

