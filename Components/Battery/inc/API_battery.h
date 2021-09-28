/** ************************************************************* *
 * @file        API_baterry.h
 * @brief       
 * 
 * @date        2021-08-16
 * @author      Quentin Bakrim (quentin.bakrim@hotmail.fr)
 * 
 * Mines Space
 * 
 * ************************************************************* **/

#ifndef SUPPLY_BATTERY_INC_API_BATTERY_H_
#define SUPPLY_BATTERY_INC_API_BATTERY_H_

/* ------------------------------------------------------------- --
   includes
-- ------------------------------------------------------------- */
#include "stdint.h"

#include "FreeRTOS.h"
#include "queue.h"

/* ------------------------------------------------------------- --
   types
-- ------------------------------------------------------------- */
typedef struct STRUCT_battery_t
{
    uint16_t   period;
    float      volt_sch;
    float      volt_motor;
}API_BATTERY_t;

/* ------------------------------------------------------------- --
   variables
-- ------------------------------------------------------------- */
QueueHandle_t QueueHandle_battery;

/* ------------------------------------------------------------- --
   functions
-- ------------------------------------------------------------- */
void API_BATTERY_START(void);

/* ------------------------------------------------------------- --
   end of file
-- ------------------------------------------------------------- */
#endif /* SUPPLY_BATTERY_INC_API_BATTERY_H_ */
