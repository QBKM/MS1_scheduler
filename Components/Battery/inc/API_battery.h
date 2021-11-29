/** ************************************************************* *
 * @file        API_battery.h
 * @brief       
 * 
 * @date        2021-10-14
 * @author      Quentin Bakrim (quentin.bakrim@hotmail.fr)
 * 
 * Mines Space
 * 
 * ************************************************************* **/

#ifndef BATTERY_INC_API_BATTERY_H_
#define BATTERY_INC_API_BATTERY_H_

/* ------------------------------------------------------------- --
   includes
-- ------------------------------------------------------------- */
#include "stdint.h"
#include "stdbool.h"

/* ------------------------------------------------------------- --
   types
-- ------------------------------------------------------------- */
typedef float TYPE_BATTERY_VOLTAGE_t;
typedef float TYPE_BATTERY_CURRENT_t;

/* battery status */
typedef enum
{
    E_BATTERY_NONE,     /* not set */
    E_BATTERY_OK,       /* the battery is high */
    E_BATTERY_KO        /* the battery is low */
}ENUM_BATTERY_STATUS_t;

/* data struct for a battery 
 * this struct can resume a battery 
 * with its status and his voltage/current */
typedef struct
{
    ENUM_BATTERY_STATUS_t   status;
    TYPE_BATTERY_VOLTAGE_t  volt;
    TYPE_BATTERY_CURRENT_t  current;
}STRUCT_BATTERY_DATA_t;

/* main structure */
typedef struct
{
    ENUM_BATTERY_STATUS_t STATUS;
    STRUCT_BATTERY_DATA_t BAT_SEQ;
    STRUCT_BATTERY_DATA_t BAT_MOTOR1;
    STRUCT_BATTERY_DATA_t BAT_MOTOR2;
}STRUCT_BATTERY_t;

/* monitoring structure */
typedef STRUCT_BATTERY_t STRUCT_BATTERY_MNTR_t;

/* ------------------------------------------------------------- --
   function prototypes
-- ------------------------------------------------------------- */
void API_BATTERY_START(void);
bool API_BATTERY_GET_MNTR(STRUCT_BATTERY_MNTR_t* monitoring);

/* ------------------------------------------------------------- --
   end of file
-- ------------------------------------------------------------- */
#endif /* BATTERY_INC_API_BATTERY_H_ */
