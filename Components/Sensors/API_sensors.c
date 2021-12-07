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

#define RAD_TO_DEG 					    57.295779513082320876798154814105f

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

static void Get_Kalman_Angle(Kalman_t *Kalman, float newAngle, float newRate, float dt);
static void Get_Kalman_X_Y(MPU6050_t* MPU6050, Kalman_t* KalmanX, Kalman_t* KalmanY);
static void Get_Roll(MPU6050_t MPU6050, float* Roll);
static void Get_Pitch(MPU6050_t MPU6050, float* Pitch);


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

    Kalman_t KalmanX = 
    {
        .Q_angle = 0.001f,
        .Q_bias = 0.003f,
        .R_measure = 0.03f
    };

    Kalman_t KalmanY = 
    {
        .Q_angle = 0.001f,
        .Q_bias = 0.003f,
        .R_measure = 0.03f,
    };

    while(1)
    {
        /* read new data from the sensor and update the monitor */
        status = MPU6050_Read_All();
        if(status == true) 
        {
            mntr.MPU6050 = status;
            xQueueSend(QueueHandle_sensors_mntr, &mntr, (TickType_t)0);
        }

        /* get the data read */
        taskENTER_CRITICAL();
        MPU6050_Get_All(&mpu6050.data);
        taskEXIT_CRITICAL();

        /* apply the kalman filter */
        taskENTER_CRITICAL();
        Get_Kalman_X_Y(&mpu6050, &KalmanX, &KalmanY);
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


/** ************************************************************* *
 * @brief       
 * 
 * @param       Kalman 
 * @param       newAngle 
 * @param       newRate 
 * @param       dt 
 * @return      float 
 * ************************************************************* **/
static void Get_Kalman_Angle(Kalman_t *Kalman, float newAngle, float newRate, float dt) 
{
    float rate = newRate - Kalman->bias;
    Kalman->angle += dt * rate;

    Kalman->P[0][0] += dt * (dt * Kalman->P[1][1] - Kalman->P[0][1] - Kalman->P[1][0] + Kalman->Q_angle);
    Kalman->P[0][1] -= dt * Kalman->P[1][1];
    Kalman->P[1][0] -= dt * Kalman->P[1][1];
    Kalman->P[1][1] += Kalman->Q_bias * dt;

    float S = Kalman->P[0][0] + Kalman->R_measure;
    float K[2];
    K[0] = Kalman->P[0][0] / S;
    K[1] = Kalman->P[1][0] / S;

    float y = newAngle - Kalman->angle;
    Kalman->angle += K[0] * y;
    Kalman->bias += K[1] * y;

    float P00_temp = Kalman->P[0][0];
    float P01_temp = Kalman->P[0][1];

    Kalman->P[0][0] -= K[0] * P00_temp;
    Kalman->P[0][1] -= K[0] * P01_temp;
    Kalman->P[1][0] -= K[1] * P00_temp;
    Kalman->P[1][1] -= K[1] * P01_temp;
};

/** ************************************************************* *
 * @brief       
 * 
 * @param       MPU6050 
 * @param       Roll 
 * ************************************************************* **/
static void Get_Roll(MPU6050_t MPU6050, float* Roll)
{
    float roll_sqrt;
    
    roll_sqrt = sqrt(MPU6050.raw.accel.X * MPU6050.raw.accel.X + MPU6050.raw.accel.Z * MPU6050.raw.accel.Z);

    if (roll_sqrt != 0.0) 
    {
        *Roll = atan(MPU6050.raw.accel.Y / roll_sqrt) * RAD_TO_DEG;
    } 
	else 
	{
        *Roll = 0.0;
    }
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       MPU6050 
 * @param       Pitch 
 * ************************************************************* **/
static void Get_Pitch(MPU6050_t MPU6050, float* Pitch)
{    
    *Pitch = atan2(-MPU6050.raw.accel.X, MPU6050.raw.accel.Z) * RAD_TO_DEG;
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       MPU6050 
 * @param       KalmanX 
 * @param       KalmanY 
 * ************************************************************* **/
static void Get_Kalman_X_Y(MPU6050_t* MPU6050, Kalman_t* KalmanX, Kalman_t* KalmanY)
{
    static uint32_t lastTime = 0;
    float Roll;
    float Pitch;

    float dt = (float) (xTaskGetTickCount() - lastTime) / configTICK_RATE_HZ;
    lastTime = xTaskGetTickCount();

    Get_Roll(*MPU6050, &Roll);
    Get_Pitch(*MPU6050, &Pitch);

    Get_Kalman_Angle(KalmanX, Roll, MPU6050->data.gyro.Y, dt);

    if((Pitch < -90 && KalmanY->angle >  90)
	|| (Pitch >  90 && KalmanY->angle < -90))
	{
        KalmanY->angle = Pitch;
    } 
	else 
	{
		Get_Kalman_Angle(KalmanY, Pitch, MPU6050->data.gyro.Y, dt);
    }

    if (fabs(KalmanY->angle) > 90) MPU6050->data.gyro.X = -MPU6050->data.gyro.X;
}

/* ------------------------------------------------------------- --
   end of file
-- ------------------------------------------------------------- */
