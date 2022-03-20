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
#define MPU6050_DEFAULT_PERIOD_TASK     10u     /* [ms] */
#define BMP280_DEFAULT_PERIOD_TASK      10u

/* ------------------------------------------------------------- --
   types
-- ------------------------------------------------------------- */
typedef struct 
{
    float Q_angle;
    float Q_bias;
    float R_measure;
    float angle;
    float bias;
    float P[2][2];
} Kalman_t;

/* ------------------------------------------------------------- --
   handles
-- ------------------------------------------------------------- */
TaskHandle_t TaskHandle_mpu6050;
TaskHandle_t TaskHandle_bmp280;
QueueHandle_t QueueHandle_sensors_mntr;
QueueHandle_t QueueHandle_sensors_mpu6050;
QueueHandle_t QueueHandle_sensors_bmp280;

/* ------------------------------------------------------------- --
   variables
-- ------------------------------------------------------------- */
static STRUCT_SENSORS_MNTR_t mntr = {0};

/* ------------------------------------------------------------- --
   prototypes
-- ------------------------------------------------------------- */
static void handler_mpu6050(void* parameters);
static void handler_bmp280(void* parameters);

/* ============================================================= ==
   tasks functions
== ============================================================= */
/** ************************************************************* *
 * @brief       
 * 
 * @param       parameters 
 * ************************************************************* **/
static void handler_mpu6050(void* parameters)
{
    BaseType_t status;

    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();

    MPU6050_t mpu6050 = {0};

    TickType_t lastTime = xTaskGetTickCount();
    float dt;

    while(1)
    {
        /* read new data from the sensor and update the monitor */

        dt = ((float)(xTaskGetTickCount() - lastTime) / configTICK_RATE_HZ);
        lastTime = xTaskGetTickCount();
        status = MPU6050_Read_All_Kalman(dt);
        if(status == true) 
        {
            mntr.MPU6050 = status;
            xQueueSend(QueueHandle_sensors_mntr, &mntr, (TickType_t)0);
        }

        /* get the data read */
        taskENTER_CRITICAL();
        MPU6050_Get_Struct(&mpu6050);
        taskEXIT_CRITICAL();

        /* apply the kalman filter */
        taskENTER_CRITICAL();
        //Get_Kalman_X_Y(&mpu6050, &KalmanX, &KalmanY);
        taskEXIT_CRITICAL();
        
        /* wait until next task period */
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(MPU6050_DEFAULT_PERIOD_TASK));
    }
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       parameters 
 * ************************************************************* **/
static void handler_bmp280(void* parameters)
{
    BaseType_t status;

    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();

    BMP280_t bmp280 = {0};

    while(1)
    {
        /* read new data from the sensor and update the monitor */
        status = BMP280_Read_All();
        if(status == true) 
        {
            mntr.BMP280 = status;
            xQueueSend(QueueHandle_sensors_mntr, &mntr, (TickType_t)0);
        }

        /* get the data read */
        taskENTER_CRITICAL();
        BMP280_Get_All(&bmp280.data);
        taskEXIT_CRITICAL();

        /* wait until next task period */
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(BMP280_DEFAULT_PERIOD_TASK));
    }
}
/** ************************************************************* *
 * @brief       
 * 
 * ************************************************************* **/
void API_SENSORS_START(void)
{
    BaseType_t status;

    QueueHandle_sensors_mntr = xQueueCreate(1, sizeof(STRUCT_SENSORS_MNTR_t));

    /* init the mpu6050 */
    status = MPU6050_Init();
    mntr.MPU6050 = status;

    /* init the bmp280 */
    status = BMP280_Init();
    mntr.BMP280 = status;

    /* send the sensors init status to the mntr queue */
    xQueueSend(QueueHandle_sensors_mntr, &mntr, (TickType_t)0);

    /* create the task */
    status = xTaskCreate(handler_mpu6050, "task_mpu6050", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_SENSORS_MPU6050, &TaskHandle_mpu6050);
    configASSERT(status == pdPASS);
    status = xTaskCreate(handler_bmp280, "task_bmp280", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_SENSORS_BMP280, &TaskHandle_bmp280);
    configASSERT(status == pdPASS);
}

/** ************************************************************* *
 * @brief       get the recovery status
 * 
 * @param       monitoring 
 * @return      true    new status received
 * @return      false   nothing received
 * ************************************************************* **/
bool API_SENSORS_GET_MNTR(STRUCT_SENSORS_MNTR_t* monitoring)
{
    return (xQueueReceive(QueueHandle_sensors_mntr, monitoring, (TickType_t)0)) ? true : false;
}

bool API_SENSORS_GET_MPU6050(MPU6050_data_t* data)
{
    return (xQueueReceive(QueueHandle_sensors_mpu6050, data, (TickType_t)0)) ? true : false;
}

bool API_SENSORS_GET_BMP280(BMP280_data_t* data)
{
    return (xQueueReceive(QueueHandle_sensors_bmp280, data, (TickType_t)0)) ? true : false;
}


/* ------------------------------------------------------------- --
   end of file
-- ------------------------------------------------------------- */
