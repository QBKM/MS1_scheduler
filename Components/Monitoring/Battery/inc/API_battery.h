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

/* ------------------------------------------------------------- --
   types
-- ------------------------------------------------------------- */
typedef struct STRUCT_battery_t
{
    uint16_t   period;
    float      volt_sch;
    float      volt_motor;
}STRUCT_battery_t;

/* ------------------------------------------------------------- --
   functions
-- ------------------------------------------------------------- */
uint8_t init_battery(void);
void 	set_battery_period(uint16_t period);

#endif /* SUPPLY_BATTERY_INC_API_BATTERY_H_ */

/* ------------------------------------------------------------- --
   end of file
-- ------------------------------------------------------------- */
