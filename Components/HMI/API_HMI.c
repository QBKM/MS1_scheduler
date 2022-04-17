/** ************************************************************* *
 * @file        API_HMI.c
 * @brief       
 * 
 * @date        2021-10-16
 * @author      Quentin Bakrim (quentin.bakrim@hotmail.fr)
 * 
 * Mines Space
 * 
 * ************************************************************* **/

/* ------------------------------------------------------------- --
   include
-- ------------------------------------------------------------- */
#include "API_HMI.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "string.h"
#include "stdio.h"
#include "stdarg.h"
#include "usart.h"

#include "TinyFrame.h"
#include "utils.h"
#include "string.h"

TinyFrame *TinyFrame_TX;

#include "MS1_config.h"

/* ------------------------------------------------------------- --
   defines
-- ------------------------------------------------------------- */
#define HMI_DEFAULT_QUEUE_SIZE      32u 
#define HMI_DEFAULT_BUFFER_SIZE     16u
#define HMI_DEFAULT_UART_TIMEOUT    1u
#define HMI_DEFAULT_HEADER          "[%x]"


typedef struct 
{
    TYPE_HMI_ID_t ID;
    uint8_t buffer[16];
}STRUCT_HMI_FORM_t;

/* ------------------------------------------------------------- --
   handles
-- ------------------------------------------------------------- */
TaskHandle_t TaskHandle_hmi;
QueueHandle_t QueueHandle_hmi;

/* ------------------------------------------------------------- --
   variables
-- ------------------------------------------------------------- */

/* ------------------------------------------------------------- --
   prototypes
-- ------------------------------------------------------------- */
static void handler_hmi(void* parameters);

/* ============================================================= ==
   tasks functions
== ============================================================= */
/** ************************************************************* *
 * @brief       This task manage the hmi system with one 
 *              parameters. 
 *              - The data buffer
 *              The task need to receive buffer from queue to 
 *              operate.
 * 
 * @param       parameters 
 * ************************************************************* **/
static void handler_hmi(void* parameters)
{
	STRUCT_HMI_FORM_t form;
    TF_Msg msg;

    while(1)
    {
        /* wait until receiving something */
        xQueueReceive(QueueHandle_hmi, &form, portMAX_DELAY);

        TF_ClearMsg(&msg);
        
        msg.type = form.ID;
        msg.data = form.buffer;
        msg.len = sizeof(form.buffer);

        TF_Send(TinyFrame_TX, &msg);
    }
}

/* ============================================================= ==
   public functions
== ============================================================= */
/** ************************************************************* *
 * @brief       init and start the HMI task
 * 
 * ************************************************************* **/
void API_HMI_START(void)
{
    BaseType_t status;

    TinyFrame_TX = TF_Init(TF_MASTER); // 1 = master, 0 = slave

    /* create the queue */
    QueueHandle_hmi = xQueueCreate(HMI_DEFAULT_QUEUE_SIZE, sizeof(STRUCT_HMI_FORM_t));

    /* create the task */
    status = xTaskCreate(handler_hmi, "task_hmi", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_HMI, &TaskHandle_hmi);
    configASSERT(status == pdPASS);
}

/** ************************************************************* *
 * @brief       send data to the hmi uart with the ID as header.
 *              total buffer must be smaller than 32 bytes.
 * 
 * @param       dataID 
 * @param       fmt
 * @param       ... 
 * ************************************************************* **/
void API_HMI_SEND_DATA(TYPE_HMI_ID_t  dataID, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    STRUCT_HMI_FORM_t form;
    form.ID = dataID;
    memcpy(form.buffer, fmt, HMI_DEFAULT_BUFFER_SIZE);

    va_end(args);

    /* send to task */
    xQueueSend(QueueHandle_hmi, &form, 0);
}

/**
 * This function should be defined in the application code.
 * It implements the lowest layer - sending bytes to UART (or other)
 */
void TF_WriteImpl(TinyFrame *tf, const uint8_t *buff, uint32_t len)
{
    /* send data on UART */
    HAL_UART_Transmit(&huart4, (uint8_t*)buff, len, HMI_DEFAULT_UART_TIMEOUT);
}

/* ------------------------------------------------------------- --
   end of file
-- ------------------------------------------------------------- */

