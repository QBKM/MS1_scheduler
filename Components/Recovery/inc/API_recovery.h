/** ************************************************************* *
 * @file        API_recovery.h
 * @brief       
 * 
 * @date        2021-10-11
 * @author      Quentin Bakrim (quentin.bakrim@hotmail.fr)
 * 
 * Mines Space
 * 
 * ************************************************************* **/

#ifndef RECOVERY_INC_API_RECOVERY_H_
#define RECOVERY_INC_API_RECOVERY_H_

/* ------------------------------------------------------------- --
   includes
-- ------------------------------------------------------------- */
#include "FreeRTOS.h"
#include "queue.h"

/* ------------------------------------------------------------- --
   types
-- ------------------------------------------------------------- */
/* List of commands available for this API. 
 * -> Stop command      : will stop the motors 
 * -> Open/Close command: will run the motors until reach the end */
typedef enum
{
    E_CMD_NONE,
    E_CMD_STOP,
    E_CMD_OPEN,
    E_CMD_CLOSE
}ENUM_CMD_ID_t;

/* ------------------------------------------------------------- --
   variables
-- ------------------------------------------------------------- */
QueueHandle_t QueueHandle_recovery;

/* ------------------------------------------------------------- --
   functions
-- ------------------------------------------------------------- */
void API_RECOVERY_START(void);

/* ------------------------------------------------------------- --
   end of file
-- ------------------------------------------------------------- */

#endif /* RECOVERY_INC_API_RECOVERY_H_ */
