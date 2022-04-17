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
#include "math.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "gpio.h"

#include "MS1_config.h"

#include "API_application.h"
#include "API_recovery.h"
#include "API_payload.h"
#include "API_buzzer.h"
#include "API_HMI.h"
#include "API_battery.h"
#include "API_sensors.h"

/* ------------------------------------------------------------- --
   defines
-- ------------------------------------------------------------- */
/* buzzer settings */
#define BUZZER_WAIT_PERIOD          1000u   /* [ms] */
#define BUZZER_WAIT_DUTYCYCLE       0.5f    /* ratio */
#define BUZZER_ASCEND_PERIOD        100u    /* [ms] */
#define BUZZER_ASCEND_DUTYCYCLE     0.1f    /* ratio */
#define BUZZER_DESCEND_PERIOD       1000u   /* [ms] */
#define BUZZER_DESCEND_DUTYCYCLE    0.5f    /* ratio */

/* windows settings */
#define WINDOW_IN_TIME              6000u   /* [ms] */
#define WINDOW_OUT_TIME             10000u  /* [ms] */

/* include functions */
#define APPLICATION_INC_FLAG_AEROC      1
#define APPLICATION_INC_FLAG_WININ      1
#define APPLICATION_INC_FLAG_WINOUT     1

#define APPLICATION_INC_DATA_MPU6050    1
#define APPLICATION_INC_DATA_BMP280     1

#define APPLICATION_INC_MNTR_RECOV      0
#define APPLICATION_INC_MNTR_PAYLOAD    0
#define APPLICATION_INC_MNTR_BATTERY    0

#define APPLICATION_INC_LOG_DATALOG     0
#define APPLICATION_INC_LOG_RADIO       0

#define APPLICATION_INC_USER_BTN        1

/* ------------------------------------------------------------- --
   handles
-- ------------------------------------------------------------- */
TaskHandle_t TaskHandle_application;

TimerHandle_t TimerHandle_window_in;
TimerHandle_t TimerHandle_window_out;

/* ------------------------------------------------------------- --
   variables
-- ------------------------------------------------------------- */
volatile bool flagAeroc;
volatile bool flagWinIn;
volatile bool flagWinOut;
volatile bool flagDeploy;

volatile ENUM_APP_ISR_ID_t userBtn;

static STRUCT_SENSORS_MPU6050_t mpu6050;
static STRUCT_SENSORS_BMP280_t bmp280;

static STRUCT_RECOV_MNTR_t mntr_recov;
static STRUCT_PAYLOAD_MNTR_t mntr_payload;
static STRUCT_BATTERY_MNTR_t mntr_battery;

/* ------------------------------------------------------------- --
   prototypes
-- ------------------------------------------------------------- */
/* tasks handlers */
static void handler_application(void* parameters);

/* monitoring */
static void process_deploy(void);
static void process_mntr_recov(STRUCT_RECOV_MNTR_t MNTR_RECOV);
static void process_mntr_payload(STRUCT_PAYLOAD_MNTR_t MNTR_PAYLOAD);
static void process_mntr_battery(STRUCT_BATTERY_MNTR_t MNTR_battery);

static void process_user_btn(ENUM_APP_ISR_ID_t ID);

/* callbacks */
static void callback_timer_window_in(TimerHandle_t xTimer);
static void callback_timer_window_out(TimerHandle_t xTimer);

/* ------------------------------------------------------------- --
   functions
-- ------------------------------------------------------------- */
static void handler_application(void* parameters)
{
    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();

    API_BUZZER_SEND_PARAMETER(BUZZER_WAIT_PERIOD, BUZZER_WAIT_DUTYCYCLE);
    API_HMI_SEND_DATA(HMI_ID_APP_PHASE, "WAIT");
    API_HMI_SEND_DATA(HMI_ID_APP_AEROC, "WAIT");

    /* delay until start */
    vTaskDelay(pdMS_TO_TICKS(1000));

    while(1)
    {

//////////////////////////////////////////////////////////////////////////////////////////////////////////
#if APPLICATION_INC_FLAG_AEROC
        /* This section is used to scan the flagAero variable.
           The variable is set to true by the IT callback when the aerocontact is triggered.
           The purpose is to initialize set the start by starting the window timers */
        if(flagAeroc == true)
        {
            /* user indicators */
            API_BUZZER_SEND_PARAMETER(BUZZER_ASCEND_PERIOD, BUZZER_ASCEND_DUTYCYCLE);
            API_HMI_SEND_DATA(HMI_ID_APP_AEROC, "GO");

            /* start the window timers */
            xTimerStart(TimerHandle_window_in, 0);
            xTimerStart(TimerHandle_window_out, 0);

            /* reset flag */
            flagAeroc = false;
        }
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////
#if APPLICATION_INC_DATA_MPU6050
        /* This section is used to get the values from the 
           inertial sensor (mpu6050). 
           The data gathered are the acceleration, angular speed, temperature and the degrees */
        if(API_SENSORS_GET_MPU6050(&mpu6050) == true)
        {
            API_HMI_SEND_DATA(HMI_ID_SENS_IMU_X_KALMAN, "%d", mpu6050.data.KalmanAngleX);
            API_HMI_SEND_DATA(HMI_ID_SENS_IMU_Y_KALMAN, "%d", mpu6050.data.KalmanAngleY);
        }
        else
        {
            API_HMI_SEND_DATA(HMI_ID_SENS_IMU_X_KALMAN, "ERROR");
            API_HMI_SEND_DATA(HMI_ID_SENS_IMU_Y_KALMAN, "ERROR");
        }
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////
#if APPLICATION_INC_DATA_BMP280
        /* This section is used to get the values from the 
           barometer sensor (bmp280)
           The data gathered are the pressure and temperature */
        if(API_SENSORS_GET_BMP280(&bmp280) == true)
        {
            API_HMI_SEND_DATA(HMI_ID_SENS_BARO_PRESS, "%d", bmp280.data.pressure);
        }
        else
        {
            API_HMI_SEND_DATA(HMI_ID_SENS_BARO_ERROR, "ERROR");
        }
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////
#if APPLICATION_INC_FLAG_WINOUT
        /* This section is use to deploy the parachute if the sensors haven't detected the apogee.
           It may enter in this condition if the variable flagWinOut == true.
           flagWinOut is set to true by the software timer started by the aerocontact */
        if(flagWinOut == true)
        {
            if(flagDeploy == false)
            {
                process_deploy();
            }

            /* reset flag */
            flagWinIn = false;
            flagWinOut = false;
        }
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////
#if APPLICATION_INC_FLAG_WININ
        /* This section is use to scan when deploy the parachute and deploy it.
           To do that, the angles are used to determine if the rocket has 
           reach an angle. At the apogee, the rocket must have a specific angle
           that can be determined in the simulations. */
        if(flagWinIn == true)
        {
            if((fabs(mpu6050.data.KalmanAngleY) >= 70)
            || (fabs(mpu6050.data.KalmanAngleX) >= 70))
            {
                process_deploy();
            }
        }
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////
        // if(API_GNSS_GET_GPS(&gps) == true)
        // {

        // }

//////////////////////////////////////////////////////////////////////////////////////////////////////////
#if APPLICATION_INC_MNTR_RECOV
        if(API_RECOVERY_GET_MNTR(&recov) == true)
        {
            process_mntr_recov(recov);
        }
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////
#if APPLICATION_INC_MNTR_PAYLOAD
        if(API_PAYLOAD_GET_MNTR(&payload) == true)
        {
            process_mntr_payload(payload);
        }
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////
#if APPLICATION_INC_MNTR_BATTERY
        if(API_BATTERY_GET_MNTR(&battery) == true)
        {
            process_mntr_battery(battery);
        }
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////
#if APPLICATION_INC_LOG_DATALOG
        //send on SD
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////
#if APPLICATION_INC_LOG_RADIO
        //send on radio
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////
#if APPLICATION_INC_USER_BTN
        /* This section is use to scan when the user button is pressed.
           It can be use to open or close the payload or recovery system manually */
        if(userBtn != E_APP_ISR_NONE)
        {
            process_user_btn(userBtn);
            userBtn == E_APP_ISR_NONE;
        }
#endif
//////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    /* wait until next period */
    vTaskDelayUntil(&xLastWakeTime, TASK_PERIOD_APPLICATION);
    }
}

/** ************************************************************* *
 * @brief       
 * 
 * ************************************************************* **/
static void process_deploy(void)
{
    flagDeploy = true;
    API_RECOVERY_SEND_CMD(E_CMD_RECOV_OPEN);
    API_BUZZER_SEND_PARAMETER(BUZZER_DESCEND_PERIOD, BUZZER_DESCEND_DUTYCYCLE);
    API_HMI_SEND_DATA(HMI_ID_APP_PHASE, "DESCEND");
}

/** ************************************************************* *
 * @brief       
 * 
 * ************************************************************* **/
static void process_user_btn(ENUM_APP_ISR_ID_t ID)
{
    switch (ID)
    {
        /* force to open the recovery */
        case E_APP_ISR_RECOV_OPEN:
            if(HAL_GPIO_ReadPin(RECOVERY_OPEN_GPIO_Port, RECOVERY_OPEN_Pin) == GPIO_PIN_SET)
            {
                API_RECOVERY_SEND_CMD(E_CMD_RECOV_OPEN);
            }
            else
            {
                API_RECOVERY_SEND_CMD(E_CMD_RECOV_STOP);
            }
        break;

        /* force to close the recovery */
        case E_APP_ISR_RECOV_CLOSE:
            if(HAL_GPIO_ReadPin(RECOVERY_CLOSE_GPIO_Port, RECOVERY_CLOSE_Pin) == GPIO_PIN_SET)
            {
                API_RECOVERY_SEND_CMD(E_CMD_RECOV_CLOSE);
            }
            else
            {
                API_RECOVERY_SEND_CMD(E_CMD_RECOV_STOP);
            }
        break;

        /* force to open the payload */
        // case E_APP_ISR_PAYLOAD_OPEN:
        //     if(HAL_GPIO_ReadPin(PAYLOAD_OPEN_GPIO_Port, PAYLOAD_OPEN_Pin) == GPIO_PIN_SET)
        //     {
        //         API_PAYLOAD_SEND_CMD(E_CMD_PL_OPEN);
        //     }
        //     else
        //     {
        //         API_PAYLOAD_SEND_CMD(E_CMD_PL_STOP);
        //     }
        // break;

        // /* force to close the payload */
        // case E_APP_ISR_PAYLOAD_CLOSE:
        //     if(HAL_GPIO_ReadPin(PAYLOAD_CLOSE_GPIO_Port, PAYLOAD_CLOSE_Pin) == GPIO_PIN_SET)
        //     {
        //         API_PAYLOAD_SEND_CMD(E_CMD_PL_CLOSE);
        //     }
        //     else
        //     {
        //         API_PAYLOAD_SEND_CMD(E_CMD_PL_STOP);
        //     }
        // break;
        default : break;
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
        case E_CMD_RECOV_NONE:  API_HMI_SEND_DATA(HMI_ID_RECOV_LAST_CMD, "NONE");  break;
        case E_CMD_RECOV_STOP:  API_HMI_SEND_DATA(HMI_ID_RECOV_LAST_CMD, "STOP");  break;
        case E_CMD_RECOV_OPEN:  API_HMI_SEND_DATA(HMI_ID_RECOV_LAST_CMD, "OPEN");  break;
        case E_CMD_RECOV_CLOSE: API_HMI_SEND_DATA(HMI_ID_RECOV_LAST_CMD, "CLOSE"); break;
        default: break;
    }

    /* send to hmi the status of the recovery */
    switch(MNTR_RECOV.status)
    {
        case E_STATUS_RECOV_NONE:    API_HMI_SEND_DATA(HMI_ID_RECOV_STATUS, "NONE");    break;
        case E_STATUS_RECOV_STOP:    API_HMI_SEND_DATA(HMI_ID_RECOV_STATUS, "STOP");    break;
        case E_STATUS_RECOV_RUNNING: API_HMI_SEND_DATA(HMI_ID_RECOV_STATUS, "RUNNING"); break;
        case E_STATUS_RECOV_OPEN:    API_HMI_SEND_DATA(HMI_ID_RECOV_STATUS, "OPEN");    break;
        case E_STATUS_RECOV_CLOSE:   API_HMI_SEND_DATA(HMI_ID_RECOV_STATUS, "CLOSE");   break;
        default: break;
    }
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       MNTR_PAYLOAD 
 * ************************************************************* **/
static void process_mntr_payload(STRUCT_PAYLOAD_MNTR_t MNTR_PAYLOAD)
{
    /* send to hmi the last cmd received by the recovery */
    switch(MNTR_PAYLOAD.last_cmd)
    {
        case E_CMD_PL_NONE:  API_HMI_SEND_DATA(HMI_ID_RECOV_LAST_CMD, "NONE");   break;
        case E_CMD_PL_STOP:  API_HMI_SEND_DATA(HMI_ID_RECOV_LAST_CMD, "STOP");   break;
        case E_CMD_PL_OPEN:  API_HMI_SEND_DATA(HMI_ID_RECOV_LAST_CMD, "OPEN");   break;
        case E_CMD_PL_CLOSE: API_HMI_SEND_DATA(HMI_ID_RECOV_LAST_CMD, "CLOSE");  break;
        default: break;
    }

    /* send to hmi the status of the recovery */
    switch(MNTR_PAYLOAD.status)
    {
        case E_STATUS_PL_NONE:    API_HMI_SEND_DATA(HMI_ID_RECOV_STATUS, "NONE");    break;
        case E_STATUS_PL_STOP:    API_HMI_SEND_DATA(HMI_ID_RECOV_STATUS, "STOP");    break;
        case E_STATUS_PL_RUNNING: API_HMI_SEND_DATA(HMI_ID_RECOV_STATUS, "RUNNING"); break;
        case E_STATUS_PL_OPEN:    API_HMI_SEND_DATA(HMI_ID_RECOV_STATUS, "OPEN");    break;
        case E_STATUS_PL_CLOSE:   API_HMI_SEND_DATA(HMI_ID_RECOV_STATUS, "CLOSE");   break;
        default: break;
    }
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       MNTR_battery 
 * ************************************************************* **/
static void process_mntr_battery(STRUCT_BATTERY_MNTR_t MNTR_battery)
{
    /* check status SEQ */
    if(MNTR_battery.BAT_SEQ.status == E_BATTERY_KO)
    {
        API_HMI_SEND_DATA(HMI_ID_MNTR_BAT_SEQ, "DEFAULT");
    }
    else
    {
        API_HMI_SEND_DATA(HMI_ID_MNTR_BAT_SEQ, "OK");
    }

    /* check status MOTOR1 */
    if(MNTR_battery.BAT_MOTOR1.status == E_BATTERY_KO)
    {
        API_HMI_SEND_DATA(HMI_ID_MNTR_BAT_MOTOR1, "DEFAULT");
    }
    else
    {
        API_HMI_SEND_DATA(HMI_ID_MNTR_BAT_MOTOR1, "OK");
    }

    /* check status MOTOR2 */
    if(MNTR_battery.BAT_MOTOR2.status == E_BATTERY_KO)
    {
        API_HMI_SEND_DATA(HMI_ID_MNTR_BAT_MOTOR2, "DEFAULT");
    }
    else
    {
        API_HMI_SEND_DATA(HMI_ID_MNTR_BAT_MOTOR2, "OK");
    }
}

/** ************************************************************* *
 * @brief       
 * 
 * ************************************************************* **/
void API_APPLICATION_START(void)
{
    BaseType_t status;

    flagAeroc = false;
    flagWinIn = false;
    flagWinOut = false;
    flagDeploy = false;
    
    /* create the tasks */
    status = xTaskCreate(handler_application, "task_application", 2*configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_APPLICATION, &TaskHandle_application);
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
    flagWinIn = true;
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       xTimer 
 * ************************************************************* **/
static void callback_timer_window_out(TimerHandle_t xTimer)
{
    flagWinOut = true;
}

/** ************************************************************* *
 * @brief       
 * 
 * ************************************************************* **/
void API_APPLICATION_CALLBACK_ISR(ENUM_APP_ISR_ID_t ID)
{
    switch(ID)
    {
        case E_APP_ISR_AEROC :          flagAeroc = true; break;

        case E_APP_ISR_RECOV_OPEN :     userBtn = ID; break;
        case E_APP_ISR_RECOV_CLOSE :    userBtn = ID; break;
        case E_APP_ISR_PAYLOAD_OPEN :   userBtn = ID; break;
        case E_APP_ISR_PAYLOAD_CLOSE :  userBtn = ID; break;
        default :  break;
    }
}

