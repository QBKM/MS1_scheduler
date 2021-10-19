/** ************************************************************* *
 * @file        API_application.c
 * @brief       
 * 
 * @date        2021-09-29
 * @author      Quentin Bakrim (quentin.bakrim@hotmail.fr)
 * 
 * Mines Space
 * 
 * ************************************************************* **/

/* ------------------------------------------------------------- --
   includes
-- ------------------------------------------------------------- */
#include "stdbool.h"
#include "stdint.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#include "API_application.h"
#include "API_recovery.h"
#include "API_buzzer.h"
#include "API_HMI.h"

#include "SEGGER_SYSVIEW.h"
//#include "config.h"

/* ------------------------------------------------------------- --
   defines
-- ------------------------------------------------------------- */
/* notify flag IDs */
#define APPLICATION_DEFAULT_AEROCONTACT_ID 0x08000000

/* Buzzer settings */
#define BUZZER_ASCEND_PERIOD        100u    /* [ms] */
#define BUZZER_ASCEND_DUTYCYCLE     0.1f    /* ratio */
#define BUZZER_DESCEND_PERIOD       10u    /* [ms] */
#define BUZZER_DESCEND_DUTYCYCLE    0.5f    /* ratio */

#define WINDOW_IN_TIME              6000u /* [ms] */
#define WINDOW_OUT_TIME             2000u /* [ms] */

/* ------------------------------------------------------------- --
   types
-- ------------------------------------------------------------- */
typedef struct
{
    volatile bool       flag;
    volatile TickType_t triggerDate;
}STRUCT_AEROCONTACT_t;

/* ------------------------------------------------------------- --
   handles
-- ------------------------------------------------------------- */
TaskHandle_t TaskHandle_application;
TimerHandle_t TimerHandle_window_in;
TimerHandle_t TimerHandle_window_out;

/* ------------------------------------------------------------- --
   variables
-- ------------------------------------------------------------- */
static STRUCT_AEROCONTACT_t aerocontact;
static ENUM_PHASE_t phase;

/* ------------------------------------------------------------- --
   prototypes
-- ------------------------------------------------------------- */
static void handler_application(void* parameters);
static void notify_check(void);
static void callback_timer_window_in(TimerHandle_t xTimer);

/* ------------------------------------------------------------- --
   functions
-- ------------------------------------------------------------- */
/** ************************************************************* *
 * @brief       
 * 
 * ************************************************************* **/
static void handler_application(void* parameters)
{
    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();

    while(1)
    {
        /* check task notify */
        notify_check();

        //xTaskNotify(TaskHandle_sensors, 0, eNoAction);

        //xqueuereceive ihm
            //if btn pressed, notify recovery
        //xqueuereceive battery
            //if battery ok/ko => data + ihm

        /* receive data from API_SENSORS */
        //xQueueReceive(QueueHandle_sensors, &pressure, pdMS_TO_TICKS(10)); 

        //xqueuesend recovery
            //if new cmd, send it to recovery (ihm btn or algo)
        //xqueuesend payload
            //if new cmd, send it to recovery (ihm btn or algo)

        //xqueuesend flash
        //xqueuesend radio
        //xqueuesend ihm
            //if status == ko => send msg + led
        xTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(1));
    }
}

/** ************************************************************* *
 * @brief       
 * 
 * ************************************************************* **/
static void notify_check(void)
{
    uint32_t notify_id;

    if(xTaskNotifyWait(0,0, &notify_id, 0) == pdTRUE)
    {
        /* check aerocontact */
        if(notify_id & APPLICATION_DEFAULT_AEROCONTACT_ID)
        {
            phase = E_PHASE_ASCEND;
            API_BUZZER_SEND_PARAMETER(BUZZER_ASCEND_PERIOD, BUZZER_ASCEND_DUTYCYCLE);
            API_HMI_SEND_DATA(HMI_ID_APP_AEROC, "start");
            //UNION_HMI_DATA_t test.u8 = 123;
            API_HMI_SEND_DATA(HMI_ID_APP_PHASE, "Phase : ascend %d\n", 13);

            //xTimerStart(TimerHandle_window_in, 0);
        }
    }
}

/** ************************************************************* *
 * @brief       
 * 
 * ************************************************************* **/
static void callback_timer_window_in(TimerHandle_t xTimer)
{
    phase = E_PHASE_DESCEND;
    API_BUZZER_SEND_PARAMETER(BUZZER_DESCEND_PERIOD, BUZZER_DESCEND_DUTYCYCLE);
}

/** ************************************************************* *
 * @brief       
 * 
 * ************************************************************* **/
void API_APPLICATION_START(void)
{
    BaseType_t status;

    phase = E_PHASE_WAIT;

    aerocontact.flag = false;
    aerocontact.triggerDate = 0xFFFFFFFF;
    
    status = xTaskCreate(handler_application, "task_application", configMINIMAL_STACK_SIZE, NULL, 3, &TaskHandle_application);
    configASSERT(status == pdPASS);

    //TimerHandle_window_in = xTimerCreate("timer_window_in", pdMS_TO_TICKS(WINDOW_IN_TIME), pdFALSE, (void*)0, callback_timer_window_in);
    //configASSERT(TimerHandle_window_in == NULL);
}

/** ************************************************************* *
 * @brief       
 * 
 * ************************************************************* **/
void API_APPLICATION_CALLBACK(void)
{
    if(aerocontact.flag == false)
    {
        aerocontact.flag = true;
        aerocontact.triggerDate = xTaskGetTickCountFromISR();

        /* notify the application task with aerocontact ID flag */
        xTaskNotifyFromISR(TaskHandle_application, APPLICATION_DEFAULT_AEROCONTACT_ID, eSetBits, pdFALSE);
    }
}

