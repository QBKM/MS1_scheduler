/** ************************************************************* *
 * @file       mpu6050.h
 * @brief      
 * 
 * @date       2021-04-23
 * @author     Quentin Bakrim (quentin.bakrim@hotmail.fr)
 * 
 * Mines Space
 * 
 * ************************************************************* **/

#ifndef __MPU6050_H__
#define __MPU6050_H__


/* ------------------------------------------------------------- --
   Includes
-- ------------------------------------------------------------- */
#include "stdint.h"

/* ------------------------------------------------------------- --
   types
-- ------------------------------------------------------------- */
/* Accel full scale range settings */
typedef enum
{
	MPU6050_AFS_2G 	= 0,
	MPU6050_AFS_4G 	= 1,
	MPU6050_AFS_8G 	= 2,
	MPU6050_AFS_16G  = 3
}MPU6050_AccelFullScale;

/* gyro full scale range settings */
typedef enum
{
	MPU6050_GFS_250_DEG_S 	= 0,
	MPU6050_GFS_500_DEG_S 	= 1,
	MPU6050_GFS_1000_DEG_S 	= 2,
	MPU6050_GFS_2000_DEG_S 	= 3
}MPU6050_GyroFullScale;

/* sample rate settings */
typedef enum
{							    /* Sample Rate = Gyroscope Output Rate / (1 + SMPLRT_DIV) */
	MPU6050_SR_8KHZ = 1,	    /* Gyroscope Output Rate = 8kHz when the DLPF is disabled (DLPF_CFG= 0 or 7), and 1kHz when the DLPF is enabled */
	MPU6050_SR_4KHZ = 3,
	MPU6050_SR_2KHZ = 5,
	MPU6050_SR_1KHZ = 7,
}MPU6050_SampleRate;

/* configuration structure */
typedef struct
{
	MPU6050_AccelFullScale AFS;
	MPU6050_GyroFullScale GFS;
	MPU6050_SampleRate SR;
}MPU6050_config_t;

/* raw data structure */
typedef struct
{
    int16_t X;
    int16_t Y;
    int16_t Z;
}MPU6050_raw_accel_t;

/* raw data structure */
typedef struct
{
    int16_t X;
    int16_t Y;
    int16_t Z;
}MPU6050_raw_gyro_t;

/* raw data structure */
typedef struct
{
    float X;
    float Y;
    float Z;
}MPU6050_accel_t;

/* raw data structure */
typedef struct
{
    float X;
    float Y;
    float Z;
}MPU6050_gyro_t;

typedef int16_t MPU6050_raw_temp_t;
typedef int16_t MPU6050_temp_t;

/* raw data structure */
typedef struct
{
    MPU6050_raw_accel_t A;
    MPU6050_raw_gyro_t G;
    MPU6050_raw_temp_t Temp;
}MPU6050_raw_data_t;

/* raw data structure */
typedef struct
{
    MPU6050_accel_t A;
    MPU6050_gyro_t G;
    MPU6050_temp_t Temp;
}MPU6050_data_t;

typedef struct 
{
    float KalmanAngleX;
    float KalmanAngleY;
}MPU6050_kalman_data_t;

/* MPU6050 handle structure */
typedef struct 
{
    MPU6050_data_t data;
    MPU6050_raw_data_t raw;
    MPU6050_kalman_data_t kalman;
    MPU6050_config_t config;
}MPU6050_t;


/* ------------------------------------------------------------- --
   prototypes
-- ------------------------------------------------------------- */
uint8_t MPU6050_Init(void);

uint8_t MPU6050_Read_Accel(void);
uint8_t MPU6050_Read_Gyro(void);
uint8_t MPU6050_Read_Temp(void);
uint8_t MPU6050_Read_All(void);
uint8_t MPU6050_Read_All_Kalman(float dt);

void MPU6050_Get_Struct(MPU6050_t* data);

#endif /* __MPU6050_H__ */
/* ------------------------------------------------------------- --
   end of files
-- ------------------------------------------------------------- */
