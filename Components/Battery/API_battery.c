/** ************************************************************* *
 * @file        API_battery.c
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
#include "API_battery.h"

#include "task.h"

#include "gpio.h"
#include "adc.h"

#include "config.h"

/* ------------------------------------------------------------- --
   defines
-- ------------------------------------------------------------- */
#ifndef BATTERY_DEFAULT_MAX_VOLTAGE
#define BATTERY_DEFAULT_MAX_VOLTAGE       18
#endif

#ifndef BATTERY_DEFAULT_ADC_RANGE
#define BATTERY_DEFAULT_ADC_RANGE         1024
#endif

#ifndef BATTERY_DEFAULT_PERIOD
#define BATTERY_DEFAULT_PERIOD            1000
#endif

#ifndef BATTERY_DEFAULT_THRESHOLD_VOLTAGE
#define BATTERY_DEFAULT_THRESHOLD_VOLTAGE 7.5
#endif

/* ------------------------------------------------------------- --
   variable
-- ------------------------------------------------------------- */
API_BATTERY_t battery = {0};
TaskHandle_t battery_handle;

/* ------------------------------------------------------------- --
   prototypes
-- ------------------------------------------------------------- */
static void battery_handler(void* parameters);

/* ------------------------------------------------------------- --
   tasks
-- ------------------------------------------------------------- */
/** ************************************************************* *
 * @brief       
 * 
 * @param       parameters 
 * ************************************************************* **/
static void battery_handler(void* parameters)
{
    TickType_t last_wakeup_time;
    last_wakeup_time = xTaskGetTickCount();

    while(1)
    {
        /* voltage acquisition */
        HAL_ADC_Start(&hadc1);
        HAL_ADC_PollForConversion(&hadc1, 1);

        /* battery scheduler */
        battery.scheduler.volt = (BATTERY_DEFAULT_MAX_VOLTAGE * ((float)HAL_ADC_GetValue(&hadc1) / BATTERY_DEFAULT_ADC_RANGE));
        if(battery.scheduler.volt < BATTERY_DEFAULT_THRESHOLD_VOLTAGE)
        {
            battery.scheduler.isOk = false;
        }

        /* push data to the queue */
        xQueueSend(QueueHandle_battery, &battery, 0);
        
        xTaskDelayUntil(&last_wakeup_time, pdMS_TO_TICKS(BATTERY_DEFAULT_PERIOD));
    }
}

/* ------------------------------------------------------------- --
   functions
-- ------------------------------------------------------------- */
/** ************************************************************* *
 * @brief       init the battery and return 1 if OK
 * 
 * @return      uint8_t 
 * ************************************************************* **/
void API_BATTERY_START(void)
{
    BaseType_t status;

    battery.scheduler.volt  = 0.00;
    battery.motor.volt      = 0.00;

    battery.scheduler.isOk  = true;
    battery.motor.isOk      = true;

    HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);

    status = xTaskCreate(battery_handler, "task_battery", configMINIMAL_STACK_SIZE, NULL, 3, &battery_handle);

    /* check  */
    if(status != pdPASS)
    {
        xTaskNotify(xTaskGetHandle("task_application"), BATTERY_NOTIFY_INIT_ID, eSetBits);
    }

   QueueHandle_battery = xQueueCreate(1, API_BATTERY_t);
}


