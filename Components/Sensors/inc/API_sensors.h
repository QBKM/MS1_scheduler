/** ************************************************************* *
 * @file        API_sensors.h
 * @brief       
 * 
 * @date        2021-11-29
 * @author      Quentin Bakrim (quentin.bakrim@hotmail.fr)
 * 
 * Mines Space
 * 
 * ************************************************************* **/

#ifndef RECOVERY_INC_API_SENSORS_H_
#define RECOVERY_INC_API_SENSORS_H_

/* ------------------------------------------------------------- --
   includes
-- ------------------------------------------------------------- */
#include "stdint.h"
#include "stdbool.h"
#include "mpu6050.h"
#include "bmp280.h"

/* ------------------------------------------------------------- --
   types
-- ------------------------------------------------------------- */
typedef struct
{
   bool        status;
   MPU6050_t   data;
}STRUCT_SENSORS_MPU6050_t;

typedef struct
{
   bool        status;
   BMP280_t    data;
}STRUCT_SENSORS_BMP280_t;


/* ------------------------------------------------------------- --
   function propotypes
-- ------------------------------------------------------------- */
void API_SENSORS_START(void);
bool API_SENSORS_GET_MPU6050(STRUCT_SENSORS_MPU6050_t* data);
bool API_SENSORS_GET_BMP280(STRUCT_SENSORS_BMP280_t* data);

/* ------------------------------------------------------------- --
   end of file
-- ------------------------------------------------------------- */

#endif /* RECOVERY_INC_API_SENSORS_H_ */
