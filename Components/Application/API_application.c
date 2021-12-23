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
#include "gpio.h"

#include "MS1_config.h"

#include "API_application.h"
#include "API_recovery.h"
#include "API_buzzer.h"
#include "API_HMI.h"
#include "API_battery.h"

#include "SEGGER_SYSVIEW.h"

/* ------------------------------------------------------------- --
   defines
-- ------------------------------------------------------------- */
/* notify flag IDs */
#define APPLICATION_DEFAULT_AEROCONTACT_IT_ID   0x00000001u
#define APPLICATION_DEFAULT_WINDOW_IN_IT_ID     0x00000002u
#define APPLICATION_DEFAULT_WINDOW_OUT_IT_ID    0x00000004u
#define APPLICATION_DEFAULT_RECOV_TIMEOUT_IT_ID 0x00000008u
#define APPLICATION_DEFAULT_BTN_OPEN_IT_ID      0x00000010u
#define APPLICATION_DEFAULT_BTN_CLOSE_IT_ID     0x00000020u

#define APPLICATION_DEFAULT_PERIOD_MONITORING   100u    /* [ms] */
#define APPLICATION_DEFAULT_PERIOD_RECOVERY     10u     /* [ms] */

/* buzzer settings */
#define BUZZER_ASCEND_PERIOD        100u    /* [ms] */
#define BUZZER_ASCEND_DUTYCYCLE     0.1f    /* ratio */
#define BUZZER_DESCEND_PERIOD       1000u   /* [ms] */
#define BUZZER_DESCEND_DUTYCYCLE    0.5f    /* ratio */

/* windows settings */
#define WINDOW_IN_TIME              6000u   /* [ms] */
#define WINDOW_OUT_TIME             2000u   /* [ms] */

/* recovery settings */
#define RECOVERY_READ_DELAY         10u     /* [ms] */

/* include functions */
#define APPLICATION_INC_MNTR_RECOV      1
#define APPLICATION_INC_MNTR_BATTERY    1

/* ------------------------------------------------------------- --
   handles
-- ------------------------------------------------------------- */
TaskHandle_t TaskHandle_app_aerocontact;
TaskHandle_t TaskHandle_app_monitoring;
TaskHandle_t TaskHandle_app_windows;
TaskHandle_t TaskHandle_app_recovery;
TaskHandle_t TaskHandle_app_user_buttons;

TimerHandle_t TimerHandle_window_in;
TimerHandle_t TimerHandle_window_out;

/* ------------------------------------------------------------- --
   variables
-- ------------------------------------------------------------- */
static ENUM_PHASE_t phase;

/* ------------------------------------------------------------- --
   prototypes
-- ------------------------------------------------------------- */
/* tasks handlers */
static void handler_app_monitoring(void* parameters);
static void handler_app_aerocontact(void* parameters);
static void handler_app_windows(void* parameters);
static void handler_app_recovery(void* parameters);
static void handler_app_user_buttons(void* parameters);

/* monitoring */
static void process_mntr_recov(STRUCT_RECOV_MNTR_t MNTR_RECOV);
static void process_mntr_battery(STRUCT_BATTERY_MNTR_t MNTR_battery);

/* callbacks */
static void callback_timer_window_in(TimerHandle_t xTimer);
static void callback_timer_window_out(TimerHandle_t xTimer);

/* ------------------------------------------------------------- --
   functions
-- ------------------------------------------------------------- */
/** ************************************************************* *
 * @brief       
 * 
 * @param       parameters 
 * ************************************************************* **/
static void handler_app_aerocontact(void* parameters)
{
    uint32_t notify_id;

    while(1)
    {
        /* check task notify */
        if(xTaskNotifyWait(0xFFFF,0, &notify_id, portMAX_DELAY) == pdTRUE) 
        {
            /* check aerocontact if the rocket has launched */
            if(notify_id & APPLICATION_DEFAULT_AEROCONTACT_IT_ID)
            {
                /* user indicators */
                API_BUZZER_SEND_PARAMETER(BUZZER_ASCEND_PERIOD, BUZZER_ASCEND_DUTYCYCLE);
                API_HMI_SEND_DATA(HMI_ID_APP_PHASE, "a");

                /* start the window in counter */
                xTimerStart(TimerHandle_window_in, 0);

                /* update phase */
                phase = E_PHASE_ASCEND;

                /* suspend itself */
                vTaskSuspend(NULL);
            }
        }
    }
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       parameters 
 * ************************************************************* **/
static void handler_app_monitoring(void* parameters)
{
    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();

    STRUCT_RECOV_MNTR_t MNTR_recov;
    STRUCT_BATTERY_MNTR_t MNTR_battery;

    while(1)
    {
        /* check task monitoring */
        #if(APPLICATION_INC_MNTR_RECOV == 1)
        if(API_RECOVERY_GET_MNTR(&MNTR_recov)) process_mntr_recov(MNTR_recov);
        #endif

        #if(APPLICATION_INC_MNTR_BATTERY == 1)
        if(API_BATTERY_GET_MNTR(&MNTR_battery)) process_mntr_battery(MNTR_battery);
        #endif

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(APPLICATION_DEFAULT_PERIOD_MONITORING));
    }
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       parameters 
 * ************************************************************* **/
static void handler_app_windows(void* parameters)
{
    uint32_t notify_id;

    while(1)
    {
        /* check task notify */
        if(xTaskNotifyWait(0xFFFF,0, &notify_id, portMAX_DELAY) == pdTRUE) 
        {
            /* check timer window in */
            if(notify_id & APPLICATION_DEFAULT_WINDOW_IN_IT_ID)
            {
                /* data to hmi */
                API_HMI_SEND_DATA(HMI_ID_APP_WINDOW, "i");

                /* start the window out counter */
                xTimerStart(TimerHandle_window_out, 0);
                
                /* start the recovery app task */
                vTaskResume(TaskHandle_app_recovery);
            }

            /* check timer window out */
            if(notify_id & APPLICATION_DEFAULT_WINDOW_OUT_IT_ID)
            {
                /* stop the recovery app task */
                vTaskSuspend(TaskHandle_app_recovery);

                /* data to hmi */
                API_HMI_SEND_DATA(HMI_ID_APP_WINDOW, "o");
                API_HMI_SEND_DATA(HMI_ID_APP_RECOV_APOGEE, "k");
                API_HMI_SEND_DATA(HMI_ID_APP_PHASE, "d");

                /* update buzzer */
                API_BUZZER_SEND_PARAMETER(BUZZER_DESCEND_PERIOD, BUZZER_DESCEND_DUTYCYCLE);

                /* force the opening */
                API_RECOVERY_SEND_CMD(E_CMD_OPEN);

                /* update the phase */
                phase = E_PHASE_DESCEND;
            }
        }
    }
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       parameters 
 * ************************************************************* **/
static void handler_app_recovery(void* parameters)
{
    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();

    /* supend itself and wait to be called */
    vTaskSuspend(NULL);

    while(1)
    {
        //get angle
        if (0 /*angle == ok*/)
        {
            if(phase == E_PHASE_ASCEND)
            {
                /* data to hmi */
                API_HMI_SEND_DATA(HMI_ID_APP_RECOV_APOGEE, "o");    /* apogee ok */
                API_HMI_SEND_DATA(HMI_ID_APP_PHASE, "d");           /* phase descend */

                /* update buzzer */
                API_BUZZER_SEND_PARAMETER(BUZZER_DESCEND_PERIOD, BUZZER_DESCEND_DUTYCYCLE);

                /* force the opening */
                API_RECOVERY_SEND_CMD(E_CMD_OPEN);

                /* update phase */
                phase = E_PHASE_DESCEND;

                /* suspend itself */
                vTaskSuspend(NULL);
            }
        }

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(APPLICATION_DEFAULT_PERIOD_RECOVERY));
    }
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       parameters 
 * ************************************************************* **/
static void handler_app_user_buttons(void* parameters)
{
    uint32_t notify_id;

    while(1)
    {
        /* check task notify */
        if(xTaskNotifyWait(0xFFFF, 0, &notify_id, portMAX_DELAY) == pdTRUE) 
        {
            /* force to open the recovery */
            if(notify_id & APPLICATION_DEFAULT_BTN_OPEN_IT_ID)
            {
                if(HAL_GPIO_ReadPin(RECOVERY_OPEN_GPIO_Port, RECOVERY_OPEN_Pin) == GPIO_PIN_SET)
                {
                    API_RECOVERY_SEND_CMD(E_CMD_OPEN);
                }
                else
                {
                    API_RECOVERY_SEND_CMD(E_CMD_STOP);
                }
            }

            /* force to close the recovery */
            if(notify_id & APPLICATION_DEFAULT_BTN_CLOSE_IT_ID)
            {
                if(HAL_GPIO_ReadPin(RECOVERY_CLOSE_GPIO_Port, RECOVERY_CLOSE_Pin) == GPIO_PIN_SET)
                {
                    API_RECOVERY_SEND_CMD(E_CMD_CLOSE);
                }
                else
                {
                    API_RECOVERY_SEND_CMD(E_CMD_STOP);
                }
            }
        }
    }
}

/** ************************************************************* *
 * @brief       process the recovery monitoring to send over HMI
 * 
 * @param       MNTR_RECOV 
 * ************************************************************* **/
static void process_mntr_recov(STRUCT_RECOV_MNTR_t MNTR_RECOV)
{
    /* send to hmi the last cmd received by the recovery */
    switch(MNTR_RECOV.last_cmd)
    {
        case E_CMD_NONE:
            API_HMI_SEND_DATA(HMI_ID_RECOV_LAST_CMD, "n");
            break;

        case E_CMD_STOP:
            API_HMI_SEND_DATA(HMI_ID_RECOV_LAST_CMD, "s");
            break;

        case E_CMD_OPEN:
            API_HMI_SEND_DATA(HMI_ID_RECOV_LAST_CMD, "o");
            break;

        case E_CMD_CLOSE:
            API_HMI_SEND_DATA(HMI_ID_RECOV_LAST_CMD, "c");
            break;
        
        default:
            break;
    }

    /* send to hmi the status of the recovery */
    switch(MNTR_RECOV.status)
    {
        case E_STATUS_NONE:
            API_HMI_SEND_DATA(HMI_ID_RECOV_STATUS, "n");
            break;

        case E_STATUS_STOP:
            API_HMI_SEND_DATA(HMI_ID_RECOV_STATUS, "s");
            break;

        case E_STATUS_RUNNING:
            API_HMI_SEND_DATA(HMI_ID_RECOV_STATUS, "r");
            break;

        case E_STATUS_OPEN:
            API_HMI_SEND_DATA(HMI_ID_RECOV_STATUS, "o");
            break;

        case E_STATUS_CLOSE:
            API_HMI_SEND_DATA(HMI_ID_RECOV_STATUS, "c");
            break;
        
        default:
            break;
    }
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       MNTR_battery 
 * ************************************************************* **/
static void process_mntr_battery(STRUCT_BATTERY_MNTR_t MNTR_battery)
{
    /* check if the global status is OK and send to the hmi */
    if(MNTR_battery.STATUS == E_BATTERY_OK)
    {
        API_HMI_SEND_DATA(HMI_ID_MONIT_BAT_SEQ, "o");
        API_HMI_SEND_DATA(HMI_ID_MONIT_BAT_MOTOR1, "o");
        API_HMI_SEND_DATA(HMI_ID_MONIT_BAT_MOTOR2, "o");
    }

    /* if the status is KO, thats mean at least one battery is KO */
    else
    {
        /* check status SEQ */
        if(MNTR_battery.BAT_SEQ.status == E_BATTERY_KO)
        {
            API_HMI_SEND_DATA(HMI_ID_MONIT_BAT_SEQ, "k");
        }
        else
        {
            API_HMI_SEND_DATA(HMI_ID_MONIT_BAT_SEQ, "o");
        }

        /* check status MOTOR1 */
        if(MNTR_battery.BAT_MOTOR1.status == E_BATTERY_KO)
        {
            API_HMI_SEND_DATA(HMI_ID_MONIT_BAT_MOTOR1, "k");
        }
        else
        {
            API_HMI_SEND_DATA(HMI_ID_MONIT_BAT_MOTOR1, "o");
        }

        /* check status MOTOR2 */
        if(MNTR_battery.BAT_MOTOR2.status == E_BATTERY_KO)
        {
            API_HMI_SEND_DATA(HMI_ID_MONIT_BAT_MOTOR2, "k");
        }
        else
        {
            API_HMI_SEND_DATA(HMI_ID_MONIT_BAT_MOTOR2, "o");
        }
    }
}

/** ************************************************************* *
 * @brief       
 * 
 * ************************************************************* **/
void API_APPLICATION_START(void)
{
    BaseType_t status;

    phase = E_PHASE_WAIT;
    
    /* create the tasks */
    status = xTaskCreate(handler_app_aerocontact, "task_app_aerocontact", 2*configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_APP_AEROCONTACT, &TaskHandle_app_aerocontact);
    configASSERT(status == pdPASS);
    status = xTaskCreate(handler_app_monitoring, "task_app_monitoring", 2*configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_APP_MONITORING, &TaskHandle_app_monitoring);
    configASSERT(status == pdPASS);
    status = xTaskCreate(handler_app_windows, "task_app_windows", 2*configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_APP_WINDOWS, &TaskHandle_app_windows);
    configASSERT(status == pdPASS);
    status = xTaskCreate(handler_app_recovery, "task_app_recovery", 2*configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_APP_RECOVERY, &TaskHandle_app_recovery);
    configASSERT(status == pdPASS);
    status = xTaskCreate(handler_app_user_buttons, "task_app_user_buttons", 2*configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_APP_USER_BUTTONS, &TaskHandle_app_user_buttons);
    configASSERT(status == pdPASS);

    /* init the temporal window timers */
    TimerHandle_window_in  = xTimerCreate("timer_window_in", pdMS_TO_TICKS(WINDOW_IN_TIME), pdFALSE, (void*)0, callback_timer_window_in);
    TimerHandle_window_out = xTimerCreate("timer_window_out", pdMS_TO_TICKS(WINDOW_OUT_TIME), pdFALSE, (void*)0, callback_timer_window_out);
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       xTimer 
 * ************************************************************* **/
static void callback_timer_window_in(TimerHandle_t xTimer)
{
    /* notify the application task with aerocontact ID flag */
    xTaskNotifyFromISR(TaskHandle_app_windows, APPLICATION_DEFAULT_WINDOW_IN_IT_ID, eSetBits, pdFALSE);
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       xTimer 
 * ************************************************************* **/
static void callback_timer_window_out(TimerHandle_t xTimer)
{
    /* notify the application task with aerocontact ID flag */
    xTaskNotifyFromISR(TaskHandle_app_windows, APPLICATION_DEFAULT_WINDOW_OUT_IT_ID, eSetBits, pdFALSE);
}

/** ************************************************************* *
 * @brief       
 * 
 * ************************************************************* **/
void API_APPLICATION_CALLBACK_ISR(ENUM_APP_ISR_ID_t ID)
{
    switch(ID)
    {
        case E_APP_ISR_AEROC :
            xTaskNotifyFromISR(TaskHandle_app_aerocontact, APPLICATION_DEFAULT_AEROCONTACT_IT_ID, eSetBits, pdFALSE);
            break;

        case E_APP_ISR_RECOV_OPEN : 
            xTaskNotifyFromISR(TaskHandle_app_user_buttons, APPLICATION_DEFAULT_BTN_OPEN_IT_ID, eSetBits, pdFALSE);
            break;

        case E_APP_ISR_RECOV_CLOSE : 
            xTaskNotifyFromISR(TaskHandle_app_user_buttons, APPLICATION_DEFAULT_BTN_CLOSE_IT_ID, eSetBits, pdFALSE);
            break;

        default :  
            break;
    }
}

