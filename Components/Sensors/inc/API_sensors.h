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

/* ------------------------------------------------------------- --
   types
-- ------------------------------------------------------------- */
typedef struct 
{
   uint8_t TODO;
}STRUCT_SENSORS_t ;

typedef STRUCT_SENSORS_t STRUCT_SENSORS_MNTR_t;
/* ------------------------------------------------------------- --
   function propotypes
-- ------------------------------------------------------------- */
void API_SENSORS_START(void);
bool API_SENSORS_GET_MNTR(STRUCT_SENSORS_MNTR_t* monitoring);

/* ------------------------------------------------------------- --
   end of file
-- ------------------------------------------------------------- */

#endif /* RECOVERY_INC_API_SENSORS_H_ */
