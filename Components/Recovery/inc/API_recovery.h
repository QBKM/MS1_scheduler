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
/* List of commands available for this API. 
 * -> Stop command      : will stop the motors 
 * -> Open/Close command: will run the motors until reach the end */
typedef enum
{
    E_CMD_NONE,
    E_CMD_STOP,
    E_CMD_OPEN,
    E_CMD_CLOSE
}ENUM_RECOV_CMD_t;

/* List of system status */
typedef enum
{
    E_STATUS_NONE,          /* default state */
    E_STATUS_STOP,          /* state when the system is stop */
    E_STATUS_RUNNING,       /* state when the system is running */
    E_STATUS_OPEN,          /* state when the system is opened */
    E_STATUS_CLOSE          /* state when the system is closed */
}ENUM_RECOV_STATUS_t;

/* main structure */
typedef struct
{
    ENUM_RECOV_CMD_t last_cmd;       /* last command running */
    ENUM_RECOV_STATUS_t status;      /* current status of the system */
}STRUCT_RECOV_t;

/* monitoring structure */
typedef STRUCT_RECOV_t STRUCT_RECOV_MNTR_t;

/* ------------------------------------------------------------- --
   function propotypes
-- ------------------------------------------------------------- */
void API_RECOVERY_START(void);
void API_RECOVERY_SEND_CMD(ENUM_RECOV_CMD_t command);
bool API_RECOVERY_GET_MNTR(STRUCT_RECOV_MNTR_t* monitoring);
void API_RECOVERY_CALLBACK(void);

/* ------------------------------------------------------------- --
   end of file
-- ------------------------------------------------------------- */

#endif /* RECOVERY_INC_API_RECOVERY_H_ */
