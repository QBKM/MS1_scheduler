/** ************************************************************* *
 * @file        API_sensors.c
 * @brief       
 * 
 * @date        2021-11-29
 * @author      Quentin Bakrim (quentin.bakrim@hotmail.fr)
 * 
 * Mines Space
 * 
 * ************************************************************* **/

/* ------------------------------------------------------------- --
   include
-- ------------------------------------------------------------- */
#include "API_sensors.h"
#include "freeRtos.h"
#include "task.h"
#include "queue.h"

#include "math.h"

#include "MS1_config.h"

/* ------------------------------------------------------------- --
   defines
-- ------------------------------------------------------------- */
#define SENSORS_PERIOD_TASK     10u     /* [ms] */

/* ------------------------------------------------------------- --
   types
-- ------------------------------------------------------------- */

/* ------------------------------------------------------------- --
   handles
-- ------------------------------------------------------------- */
TaskHandle_t TaskHandle_sensors;
QueueHandle_t QueueHandle_sensors_mpu6050;
QueueHandle_t QueueHandle_sensors_bmp280;

/* ------------------------------------------------------------- --
   variables
-- ------------------------------------------------------------- */
static STRUCT_SENSORS_MPU6050_t mpu6050 = {0};
static STRUCT_SENSORS_BMP280_t  bmp280 = {0};

/* ------------------------------------------------------------- --
   prototypes
-- ------------------------------------------------------------- */
static void handler_sensors(void* parameters);

/* ============================================================= ==
   tasks functions
== ============================================================= */
/** ************************************************************* *
 * @brief       
 * 
 * @param       parameters 
 * ************************************************************* **/
static void handler_sensors(void* parameters)
{
    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();

    while(1)
    {
        /* get and send mpu6050 data */
        mpu6050.status = MPU6050_Read_All_Kalman();
        if(mpu6050.status == 0)
        {
        	mpu6050.data = MPU6050_Get_Struct();
        	xQueueSend(QueueHandle_sensors_mpu6050, &mpu6050, (TickType_t)0);
        }


        /* get and send bmp280 data */
        bmp280.status = BMP280_Read_All();
        if(bmp280.status == 0)
        {
        	bmp280.data = BMP280_Get_Struct();
        	xQueueSend(QueueHandle_sensors_bmp280, &bmp280, (TickType_t)0);
        }
        
        /* wait until next task period */
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(SENSORS_PERIOD_TASK));
    }
}

/** ************************************************************* *
 * @brief       
 * 
 * ************************************************************* **/
void API_SENSORS_START(void)
{
    BaseType_t status;

    QueueHandle_sensors_mpu6050 = xQueueCreate(1, sizeof(STRUCT_SENSORS_MPU6050_t));
    QueueHandle_sensors_bmp280  = xQueueCreate(1, sizeof(STRUCT_SENSORS_BMP280_t));

    /* init the mpu6050 */
    mpu6050.status = MPU6050_Init();

    /* init the bmp280 */
    bmp280.status = BMP280_Init();

    /* create the task */
    status = xTaskCreate(handler_sensors, "task_sensors", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_SENSORS, &TaskHandle_sensors);
    configASSERT(status == pdPASS);
}


/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * @return      true 
 * @return      false 
 * ************************************************************* **/
bool API_SENSORS_GET_MPU6050(STRUCT_SENSORS_MPU6050_t* data)
{
    return (xQueueReceive(QueueHandle_sensors_mpu6050, data, (TickType_t)0)) ? true : false;
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       data 
 * @return      true 
 * @return      false 
 * ************************************************************* **/
bool API_SENSORS_GET_BMP280(STRUCT_SENSORS_BMP280_t* data)
{
    return (xQueueReceive(QueueHandle_sensors_bmp280, data, (TickType_t)0)) ? true : false;
}


/* ------------------------------------------------------------- --
   end of file
-- ------------------------------------------------------------- */
