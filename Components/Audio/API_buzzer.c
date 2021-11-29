/** ************************************************************* *
 * @file        API_buzzer.c
 * @brief       
 * 
 * @date        2021-08-16
 * @author      Quentin Bakrim (quentin.bakrim@hotmail.fr)
 * 
 * Mines Space
 * 
 * ************************************************************* **/


/* ------------------------------------------------------------- --
   include
-- ------------------------------------------------------------- */
#include "API_buzzer.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "gpio.h"

#include "MS1_config.h"

/* ------------------------------------------------------------- --
   defines
-- ------------------------------------------------------------- */
#define BUZZER_DEFAULT_PERIOD       1000u
#define BUZZER_DEFAULT_DUTYCYCLE    0.015f

/* ------------------------------------------------------------- --
   types
-- ------------------------------------------------------------- */
/* Buzzer config structure */
typedef struct
{
    uint16_t    period;
    float       dutycycle;
}STRUCT_BUZZER_t;

/* ------------------------------------------------------------- --
   handles
-- ------------------------------------------------------------- */
TaskHandle_t TaskHandle_buzzer;
QueueHandle_t QueueHandle_buzzer;

/* ------------------------------------------------------------- --
   variables
-- ------------------------------------------------------------- */
static STRUCT_BUZZER_t buzzer = {0};

/* ------------------------------------------------------------- --
   prototypes
-- ------------------------------------------------------------- */
static void handler_buzzer(void* parameters);

/* ============================================================= ==
   tasks functions
== ============================================================= */
/** ************************************************************* *
 * @brief       This task manage the buzzer system with two 
 *              parameters. 
 *              - The period
 *              - The dutycycle
 *              The task need to receive command from queue to 
 *              operate.
 *              Please check at the STRUCT_BUZZER_t structure
 *              to send new parameters
 * 
 * @param       parameters 
 * ************************************************************* **/
static void handler_buzzer(void* parameters)
{
    while(1)
    {
        /* (alpha) part of dutycycle */
        HAL_GPIO_TogglePin(BUZZER_GPIO_Port, BUZZER_Pin);
        xQueueReceive(QueueHandle_buzzer, &buzzer, pdMS_TO_TICKS(buzzer.period * buzzer.dutycycle));

        /* (1 - alpha) part of dutycycle */
        HAL_GPIO_TogglePin(BUZZER_GPIO_Port, BUZZER_Pin);
        xQueueReceive(QueueHandle_buzzer, &buzzer, pdMS_TO_TICKS(buzzer.period - buzzer.period * buzzer.dutycycle));
    }
}

/* ============================================================= ==
   public functions
== ============================================================= */
/** ************************************************************* *
 * @brief       init and start the buzzer task
 * 
 * ************************************************************* **/
void API_BUZZER_START(void)
{
    BaseType_t status;

    /* init the main structure */
    buzzer.period    = BUZZER_DEFAULT_PERIOD;
    buzzer.dutycycle = BUZZER_DEFAULT_DUTYCYCLE;

    /* create the queue */
    QueueHandle_buzzer = xQueueCreate (1, sizeof(STRUCT_BUZZER_t));
    
    /* create the task */
    status = xTaskCreate(handler_buzzer, "task_buzzer", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_AUDIO_BUZZER, &TaskHandle_buzzer);
    configASSERT(status == pdPASS);
}

/** ************************************************************* *
 * @brief       send new parameters to the buzzer task
 * 
 * @param       period 
 * @param       dutycycle 
 * ************************************************************* **/
void API_BUZZER_SEND_PARAMETER(uint16_t period, float dutycycle)
{
    STRUCT_BUZZER_t param = {.dutycycle = dutycycle, .period = period};
    xQueueSend(QueueHandle_buzzer, &param, 0);
}

/* ------------------------------------------------------------- --
   end of file
-- ------------------------------------------------------------- */
