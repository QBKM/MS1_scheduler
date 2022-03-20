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
#include "stdint.h"
#include "stdbool.h"

/* ------------------------------------------------------------- --
   types
-- ------------------------------------------------------------- */
/* id */

/* ------------------------------------------------------------- --
   function propotypes
-- ------------------------------------------------------------- */
void API_DATALOGGER_START(void);
void API_DATALOGGER_SEND_CMD(ENUM_RECOV_CMD_t command);
bool API_DATALOGGER_GET_MNTR(STRUCT_RECOV_MNTR_t* monitoring);


/* ------------------------------------------------------------- --
   end of file
-- ------------------------------------------------------------- */

#endif /* RECOVERY_INC_API_RECOVERY_H_ */
