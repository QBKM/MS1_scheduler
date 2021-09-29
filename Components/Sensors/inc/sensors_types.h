/** ************************************************************* *
 * @file        sensors_types.h
 * @brief       
 * 
 * @date        2021-09-29
 * @author      Quentin Bakrim (quentin.bakrim@hotmail.fr)
 * 
 * Mines Space
 * 
 * ************************************************************* **/

#ifndef SENSORS_INC_SENSORS_TYPES_H_
#define SENSORS_INC_SENSORS_TYPES_H_

/* ------------------------------------------------------------- --
   includes
-- ------------------------------------------------------------- */
#include "stdint.h"
#include "stdbool.h"

/* ------------------------------------------------------------- --
   types
-- ------------------------------------------------------------- */
/* ID used by the queue to read the right data, depending of the types */
typedef enum
{
    E_BARO_ID,
    E_IMU_ID,
    E_GPS_ID,
    E_AEROCONTACT_ID,
    E_THERMO_ID
}ENUM_SENSOR_ID_t;

/* data type for the barometer */
typedef struct
{
    ENUM_SENSOR_ID_t ID;
    float pressure;
    float temperature;
}STRUCT_BARO_t;

/* data type for the IMU */
typedef struct 
{
    ENUM_SENSOR_ID_t ID; 
    float Ax;
    float Ay;
    float Az;

    float Gx;
    float Gy;
    float Gz;

    float Degx;
    float Degy;

    float temperature;
}STRUCT_IMU_t;

/* data type for GPS */
typedef struct 
{
    ENUM_SENSOR_ID_t ID;
    uint32_t todo;
}STRUCT_GPS_t;

/* data type for thermometer */
typedef struct
{
    ENUM_SENSOR_ID_t ID;
    float temperature_1;
    float temperature_2;
}STRUCT_THERMO_t;

/* data type for aerocaontact */
typedef struct 
{
    ENUM_SENSOR_ID_t ID;
    bool     isTrigger;
    uint32_t time_trigger;
}STRUCT_AEROCONTACT_t;


/* ------------------------------------------------------------- --
   end of file
-- ------------------------------------------------------------- */
#endif /* SENSORS_INC_SENSORS_TYPES_H_ */
