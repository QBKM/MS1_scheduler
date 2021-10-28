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
#define APPLICATION_DEFAULT_AEROCONTACT_ID      0x00000001u
#define APPLICATION_DEFAULT_WINDOW_IN_IT_ID     0x00000002u
#define APPLICATION_DEFAULT_WINDOW_OUT_IT_ID    0x00000004u
#define APPLICATION_DEFAULT_RECOV_TIMEOUT_IT_ID 0x00000008u

/* Buzzer settings */
#define BUZZER_ASCEND_PERIOD        100u    /* [ms] */
#define BUZZER_ASCEND_DUTYCYCLE     0.1f    /* ratio */
#define BUZZER_DESCEND_PERIOD       1000u   /* [ms] */
#define BUZZER_DESCEND_DUTYCYCLE    0.5f    /* ratio */

#define WINDOW_IN_TIME              6000u   /* [ms] */
#define WINDOW_OUT_TIME             2000u   /* [ms] */

#define RECOVERY_TIMEOUT            10000u  /* [ms] */

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
TimerHandle_t TimerHandle_recov_timeout;

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
static void callback_timer_window_out(TimerHandle_t xTimer);
static void callback_timer_recov_timeout(TimerHandle_t xTimer);

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

    static STRUCT_recovery_t MNTR_recov;

    while(1)
    {
        /* check task notify */
        notify_check();

        API_RECOVERY_GET_MNTR(&MNTR_recov);

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

    if(xTaskNotifyWait(0xFFFF,0, &notify_id, 0) == pdTRUE)
    {
        /* check aerocontact if the rocket has launched */
        if(notify_id & APPLICATION_DEFAULT_AEROCONTACT_ID)
        {
            static bool trigger_aero = false;
            if(trigger_aero == false)
            {
                trigger_aero = true;
                phase = E_PHASE_ASCEND;
                aerocontact.flag = true;
                aerocontact.triggerDate = xTaskGetTickCount();

                /* user indicators */
                API_BUZZER_SEND_PARAMETER(BUZZER_ASCEND_PERIOD, BUZZER_ASCEND_DUTYCYCLE);
                API_HMI_SEND_DATA(HMI_ID_APP_PHASE, "ascend");

                /* start the window in counter */
                xTimerStart(TimerHandle_window_in, 0);
            }
        }

        /* check timer window in */
        if(notify_id & APPLICATION_DEFAULT_WINDOW_IN_IT_ID)
        {
            static bool trigger_win = false;
            if(trigger_win == false)
            {
                trigger_win = true;

                API_HMI_SEND_DATA(HMI_ID_APP_WINDOW, "in");

                /* start the window out counter */
                xTimerStart(TimerHandle_window_out, 0);
            }
        }

        /* check timer window out */
        if(notify_id & APPLICATION_DEFAULT_WINDOW_OUT_IT_ID)
        {
            /* force the recovery if not deployed before */
            if(phase == E_PHASE_ASCEND)
            {
                phase = E_PHASE_DESCEND;

                /* data to hmi */
                API_HMI_SEND_DATA(HMI_ID_APP_WINDOW, "out");
                API_HMI_SEND_DATA(HMI_ID_APP_PHASE, "descend");

                /* update buzzer */
                API_BUZZER_SEND_PARAMETER(BUZZER_DESCEND_PERIOD, BUZZER_DESCEND_DUTYCYCLE);

                /* force the opening */
                API_RECOVERY_SEND_CMD(E_CMD_OPEN);

                /* start the window out counter */
                xTimerStart(TimerHandle_recov_timeout, 0);
            }
        }

        /* check timer recovery timeout */
        if(notify_id & APPLICATION_DEFAULT_RECOV_TIMEOUT_IT_ID)
        {
            static bool trigger_recov = false;
            if(trigger_recov == false)
            {
                trigger_recov = true;

                /* force the stop to protect the system */
                API_RECOVERY_SEND_CMD(E_CMD_STOP);

                /* data to hmi */
                API_HMI_SEND_DATA(HMI_ID_APP_RECOV_TO, "timeout");
            }
        }
    }
}


/** ************************************************************* *
 * @brief       
 * 
 * ************************************************************* **/
void API_APPLICATION_START(uint32_t priority)
{
    BaseType_t status;

    phase = E_PHASE_WAIT;

    aerocontact.flag = false;
    aerocontact.triggerDate = 0xFFFFFFFF;
    
    /* create the task */
    status = xTaskCreate(handler_application, "task_application", configMINIMAL_STACK_SIZE, NULL, priority, &TaskHandle_application);
    configASSERT(status == pdPASS);

    /* init the temporal window timers */
    TimerHandle_window_in  = xTimerCreate("timer_window_in", pdMS_TO_TICKS(WINDOW_IN_TIME), pdFALSE, (void*)0, callback_timer_window_in);
    TimerHandle_window_out = xTimerCreate("timer_window_out", pdMS_TO_TICKS(WINDOW_OUT_TIME), pdFALSE, (void*)0, callback_timer_window_out);
    TimerHandle_recov_timeout = xTimerCreate("timer_recovery_timeout", pdMS_TO_TICKS(RECOVERY_TIMEOUT), pdFALSE, (void*)0, callback_timer_recov_timeout);
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       xTimer 
 * ************************************************************* **/
static void callback_timer_window_in(TimerHandle_t xTimer)
{
    /* notify the application task with aerocontact ID flag */
    xTaskNotifyFromISR(TaskHandle_application, APPLICATION_DEFAULT_WINDOW_IN_IT_ID, eSetBits, pdFALSE);
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       xTimer 
 * ************************************************************* **/
static void callback_timer_window_out(TimerHandle_t xTimer)
{
    /* notify the application task with aerocontact ID flag */
    xTaskNotifyFromISR(TaskHandle_application, APPLICATION_DEFAULT_WINDOW_OUT_IT_ID, eSetBits, pdFALSE);
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       xTimer 
 * ************************************************************* **/
static void callback_timer_recov_timeout(TimerHandle_t xTimer)
{
    /* notify the application task with aerocontact ID flag */
    xTaskNotifyFromISR(TaskHandle_application, APPLICATION_DEFAULT_RECOV_TIMEOUT_IT_ID, eSetBits, pdFALSE);
}

/** ************************************************************* *
 * @brief       
 * 
 * ************************************************************* **/
void API_APPLICATION_CALLBACK_ISR(ENUM_APP_ISR_ID_t ID)
{
    if(ID == E_APP_ISR_AEROC)
    {
        if(aerocontact.flag == false)
        {
            /* notify the application task with aerocontact ID flag */
            xTaskNotifyFromISR(TaskHandle_application, APPLICATION_DEFAULT_AEROCONTACT_ID, eSetBits, pdFALSE);
        }
    }
}

