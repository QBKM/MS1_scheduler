/** ************************************************************* *
 * @file        API_battery.c
 * @brief       
 * 
 * @date        2021-10-14
 * @author      Quentin Bakrim (quentin.bakrim@hotmail.fr)
 * 
 * Mines Space
 * 
 * ************************************************************* **/


/* ------------------------------------------------------------- --
   include
-- ------------------------------------------------------------- */
#include "API_battery.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "adc.h"
#include "dma.h"
#include "stdint.h"

#include "MS1_config.h"

/* ------------------------------------------------------------- --
   defines
-- ------------------------------------------------------------- */
#define BATTERY_MAX_VOLTAGE         18u     /* [volt] */
#define BATTERY_THRESHOLD_VOLTAGE   7.5f    /* [volt] */
#define BATTERY_ADC_RANGE           1024u

#define BATTERY_ADC_VBAT_SEQ        0
#define BATTERY_ADC_IBAT_SEQ        1
#define BATTERY_ADC_VBAT_MOTOR_1    2
#define BATTERY_ADC_IBAT_MOTOR_1    3
#define BATTERY_ADC_VBAT_MOTOR_2    4
#define BATTERY_ADC_IBAT_MOTOR_2    5

/* ------------------------------------------------------------- --
   types
-- ------------------------------------------------------------- */

/* ------------------------------------------------------------- --
   handles
-- ------------------------------------------------------------- */
TaskHandle_t TaskHandle_battery;
QueueHandle_t QueueHandle_battery_mntr;

/* ------------------------------------------------------------- --
   variables
-- ------------------------------------------------------------- */

/* ------------------------------------------------------------- --
   prototypes
-- ------------------------------------------------------------- */
static void handler_battery(void* parameters);
static float convert_adc_volt(uint32_t raw_adc);
static float convert_adc_current(uint32_t raw_adc);
static ENUM_BATTERY_STATUS_t update_status(TYPE_BATTERY_VOLTAGE_t volt);

/* ============================================================= ==
   tasks functions
== ============================================================= */
/** ************************************************************* *
 * @brief       
 * 
 * @param       parameters 
 * ************************************************************* **/
static void handler_battery(void* parameters)
{
    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();

    STRUCT_BATTERY_t DATA = {0};

    uint32_t adc_result[6]; 
    HAL_ADC_Start_DMA(&hadc3, adc_result, 6); // start adc in DMA mode

    while(1)
    {
        /* update batteries values from ADC */
        DATA.BAT_SEQ.volt       = convert_adc_volt(adc_result[BATTERY_ADC_VBAT_SEQ]);
        DATA.BAT_SEQ.current    = convert_adc_current(adc_result[BATTERY_ADC_IBAT_SEQ]);
        DATA.BAT_MOTOR1.volt    = convert_adc_volt(adc_result[BATTERY_ADC_VBAT_MOTOR_1]);
        DATA.BAT_MOTOR1.current = convert_adc_current(adc_result[BATTERY_ADC_IBAT_MOTOR_1]);
        DATA.BAT_MOTOR2.volt    = convert_adc_volt(adc_result[BATTERY_ADC_VBAT_MOTOR_2]);
        DATA.BAT_MOTOR2.current = convert_adc_current(adc_result[BATTERY_ADC_IBAT_MOTOR_2]);

        /* update batteries status */
        DATA.BAT_SEQ.status    = update_status(DATA.BAT_SEQ.volt);
        DATA.BAT_MOTOR1.status = update_status(DATA.BAT_MOTOR1.volt);
        DATA.BAT_MOTOR2.status = update_status(DATA.BAT_MOTOR2.volt);

        xQueueSend(QueueHandle_battery_mntr, &DATA, 0);

        /* wait until next task period */
        vTaskDelayUntil(&xLastWakeTime, TASK_PERIOD_BATTERY);
    }
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       raw_adc 
 * @return      float 
 * ************************************************************* **/
static float convert_adc_volt(uint32_t raw_adc)
{
    return (BATTERY_MAX_VOLTAGE * ((float)raw_adc / BATTERY_ADC_RANGE));
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       raw_adc 
 * @return      float 
 * ************************************************************* **/
static float convert_adc_current(uint32_t raw_adc)
{
    // TODO
	return 0;
}

/** ************************************************************* *
 * @brief       update the status of a battery
 * 
 * @param       volt 
 * @return      ENUM_BATTERY_STATUS_t 
 * ************************************************************* **/
static ENUM_BATTERY_STATUS_t update_status(TYPE_BATTERY_VOLTAGE_t volt)
{
	ENUM_BATTERY_STATUS_t result;

	if(volt < BATTERY_THRESHOLD_VOLTAGE)
    {
    	result = E_BATTERY_KO;
    }
    else
    {
    	result = E_BATTERY_OK;
    }

    return result;
}

/* ============================================================= ==
   public functions
== ============================================================= */
/** ************************************************************* *
 * @brief       init and start the battery task
 * 
 * ************************************************************* **/
void API_BATTERY_START(void)
{
    BaseType_t status;

    /* create the queue */
    QueueHandle_battery_mntr = xQueueCreate(1, sizeof(STRUCT_BATTERY_t));

    /* create the task */
    status = xTaskCreate(handler_battery, "task_battery", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_BATTERY, &TaskHandle_battery);
    configASSERT(status == pdPASS);
}

/** ************************************************************* *
 * @brief       get the voltage and current from the batteries
 *              -> if no new data
 * 
 * @param       dataStruct 
 * ************************************************************* **/
bool API_BATTERY_GET_MNTR(STRUCT_BATTERY_MNTR_t* monitoring)
{
    return (xQueueReceive(QueueHandle_battery_mntr, monitoring, (TickType_t)0)) ? true : false;
}

/* ------------------------------------------------------------- --
   end of file
-- ------------------------------------------------------------- */
