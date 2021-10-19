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
#define HMI_DEFAULT_QUEUE_SIZE      10u 
#define HMI_DEFAULT_BUFFER_SIZE     32u
#define HMI_DEFAULT_UART_TIMEOUT    1u
#define HMI_DEFAULT_HEADER          "[%d]"

/* ------------------------------------------------------------- --
   types
-- ------------------------------------------------------------- */
typedef struct
{
    TYPE_HMI_ID_t ID;           /* store the ID */
    TYPE_HMI_LENGTH_t lenght;   /* length of string */
    ENUM_HMI_DATA_TYPE_t type;  /* type of data */
    UNION_HMI_DATA_t data;       /* store the data */
}STRUCT_DATA_HANDLER_t;

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
 * @brief       
 * 
 * @param       parameters 
 * ************************************************************* **/
static void handler_hmi(void* parameters)
{
	char buffer[32];

    while(1)
    {
        memset(buffer, 0, sizeof(buffer));

        /* wait until receiving something */
        xQueueReceive(QueueHandle_hmi, buffer, portMAX_DELAY);

        /* send data on UART */
        HAL_UART_Transmit(&huart4, (uint8_t*)buffer, strlen(buffer), HMI_DEFAULT_UART_TIMEOUT);
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

    /* create the queue */
    QueueHandle_hmi = xQueueCreate (HMI_DEFAULT_QUEUE_SIZE, HMI_DEFAULT_BUFFER_SIZE);
    
    /* create the task */
    status = xTaskCreate(handler_hmi, "task_hmi", configMINIMAL_STACK_SIZE, NULL, 3, &TaskHandle_hmi);
    configASSERT(status == pdPASS);
}

/** ************************************************************* *
 * @brief       
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
    sprintf(buffer, HMI_DEFAULT_HEADER, dataID);
    vsprintf(buffer + strlen(buffer), fmt, args);
    va_end(args);

    /* send to task */
    xQueueSend(QueueHandle_hmi, buffer, 0);
}

/* ------------------------------------------------------------- --
   end of file
-- ------------------------------------------------------------- */
