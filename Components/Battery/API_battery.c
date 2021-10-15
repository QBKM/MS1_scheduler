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

/* ------------------------------------------------------------- --
   defines
-- ------------------------------------------------------------- */
#define BATTERY_DEFAULT_PERIOD_TASK         1000u    /* [ms] */
#define BATTERY_DEFAULT_MAX_VOLTAGE         18u     /* [volt] */
#define BATTERY_DEFAULT_ADC_RANGE           1024u
#define BATTERY_DEFAULT_THRESHOLD_VOLTAGE   7.5f    /* [volt] */

#define BATTERY_DEFAULT_ADC_VBAT_SEQ        0
#define BATTERY_DEFAULT_ADC_IBAT_SEQ        1
#define BATTERY_DEFAULT_ADC_VBAT_MOTOR_1    2
#define BATTERY_DEFAULT_ADC_IBAT_MOTOR_1    3
#define BATTERY_DEFAULT_ADC_VBAT_MOTOR_2    4
#define BATTERY_DEFAULT_ADC_IBAT_MOTOR_2    5

/* ------------------------------------------------------------- --
   types
-- ------------------------------------------------------------- */

/* ------------------------------------------------------------- --
   handles
-- ------------------------------------------------------------- */
TaskHandle_t TaskHandle_battery;
QueueHandle_t QueueHandle_battery;

/* ------------------------------------------------------------- --
   variables
-- ------------------------------------------------------------- */

/* ------------------------------------------------------------- --
   prototypes
-- ------------------------------------------------------------- */
static void handler_battery(void* parameters);
static float convert_adc_volt(uint32_t raw_adc);
static float convert_adc_current(uint32_t raw_adc);

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
        DATA.BAT_SEQ.volt       = convert_adc_volt(adc_result[BATTERY_DEFAULT_ADC_VBAT_SEQ]);
        DATA.BAT_SEQ.current    = convert_adc_current(adc_result[BATTERY_DEFAULT_ADC_IBAT_SEQ]);
        DATA.BAT_MOTOR1.volt    = convert_adc_volt(adc_result[BATTERY_DEFAULT_ADC_VBAT_MOTOR_1]);
        DATA.BAT_MOTOR1.current = convert_adc_current(adc_result[BATTERY_DEFAULT_ADC_IBAT_MOTOR_1]);
        DATA.BAT_MOTOR2.volt    = convert_adc_volt(adc_result[BATTERY_DEFAULT_ADC_VBAT_MOTOR_2]);
        DATA.BAT_MOTOR2.current = convert_adc_current(adc_result[BATTERY_DEFAULT_ADC_IBAT_MOTOR_2]);

        /* update batteries status */
        (DATA.BAT_SEQ.volt    < BATTERY_DEFAULT_THRESHOLD_VOLTAGE) ? DATA.BAT_SEQ.status    = E_BATTERY_KO : E_BATTERY_OK;
        (DATA.BAT_MOTOR1.volt < BATTERY_DEFAULT_THRESHOLD_VOLTAGE) ? DATA.BAT_MOTOR1.status = E_BATTERY_KO : E_BATTERY_OK;
        (DATA.BAT_MOTOR2.volt < BATTERY_DEFAULT_THRESHOLD_VOLTAGE) ? DATA.BAT_MOTOR2.status = E_BATTERY_KO : E_BATTERY_OK;
        ((DATA.BAT_SEQ.status || DATA.BAT_MOTOR1.status || DATA.BAT_MOTOR2.status) == E_BATTERY_KO) ? DATA.STATUS = E_BATTERY_KO : E_BATTERY_OK;

        /* send the data to the queue */
        xQueueSend(QueueHandle_battery, &DATA, 0);

        /* wait until next task period */
        vTaskDelayUntil(xLastWakeTime, pdMS_TO_TICKS(BATTERY_DEFAULT_PERIOD_TASK));
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
    return (BATTERY_DEFAULT_MAX_VOLTAGE * ((float)raw_adc / BATTERY_DEFAULT_ADC_RANGE));
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       raw_adc 
 * @return      float 
 * ************************************************************* **/
static float convert_adc_current(uint32_t raw_adc)
{
	return 0;
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
    QueueHandle_battery = xQueueCreate(1, sizeof(STRUCT_BATTERY_t));

    /* create the task */
    status = xTaskCreate(handler_battery, "task_battery", configMINIMAL_STACK_SIZE, NULL, 3, &TaskHandle_battery);
    configASSERT(status == pdPASS);
}

/** ************************************************************* *
 * @brief       get the voltage and current from the batteries
 *              -> if no new data
 * 
 * @param       dataStruct 
 * ************************************************************* **/
void API_BATTERY_GET(STRUCT_BATTERY_t* dataStruct)
{
    xQueueReceive(QueueHandle_battery, &dataStruct, 0);
}

/* ------------------------------------------------------------- --
   end of file
-- ------------------------------------------------------------- */
