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

/* ------------------------------------------------------------- --
   defines
-- ------------------------------------------------------------- */
#define HMI_DEFAULT_QUEUE_SIZE      32u 
#define HMI_DEFAULT_BUFFER_SIZE     16u
#define HMI_DEFAULT_UART_TIMEOUT    1u
#define HMI_DEFAULT_HEADER          "[%d]"

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
	char buffer[HMI_DEFAULT_BUFFER_SIZE];

    while(1)
    {
        /* wait until receiving something */
        xQueueReceive(QueueHandle_hmi, buffer, portMAX_DELAY);

        /* send data on UART */
        HAL_UART_Transmit(&huart4, (uint8_t*)buffer, strlen(buffer), HMI_DEFAULT_UART_TIMEOUT);

        /* wait the next tick to send another message */
        //vTaskDelay(1);
    }
}

/* ============================================================= ==
   public functions
== ============================================================= */
/** ************************************************************* *
 * @brief       init and start the HMI task
 * 
 * ************************************************************* **/
void API_HMI_START(uint32_t priority)
{
    BaseType_t status;

    /* create the queue */
    QueueHandle_hmi = xQueueCreate(HMI_DEFAULT_QUEUE_SIZE, HMI_DEFAULT_BUFFER_SIZE);
    
    /* create the task */
    status = xTaskCreate(handler_hmi, "task_hmi", configMINIMAL_STACK_SIZE, NULL, priority, &TaskHandle_hmi);
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
    char buffer[32];
    va_list args;
    va_start(args, fmt);

    /* add the header to the buffer */
    sprintf(buffer, HMI_DEFAULT_HEADER, dataID);
    
    /* add payload to the buffer */
    vsprintf(buffer + strlen(buffer), fmt, args);
    va_end(args);

    /* send to task */
    xQueueSend(QueueHandle_hmi, buffer, 0);
}

/* ------------------------------------------------------------- --
   end of file
-- ------------------------------------------------------------- */

