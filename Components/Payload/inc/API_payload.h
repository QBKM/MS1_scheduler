/** ************************************************************* *
 * @file        API_payload.h
 * @brief       
 * 
 * @date        2021-12-08
 * @author      Quentin Bakrim (quentin.bakrim@hotmail.fr)
 * 
 * Mines Space
 * 
 * ************************************************************* **/
#ifndef PAYLOAD_INC_API_PAYLOAD_H_
#define PAYLOAD_INC_API_PAYLOAD_H_

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
    E_CMD_PL_NONE,
    E_CMD_PL_STOP,
    E_CMD_PL_OPEN,
    E_CMD_PL_CLOSE
}ENUM_PAYLOAD_CMD_t;

/* List of system status */
typedef enum
{
    E_STATUS_PL_NONE,          /* default state */
    E_STATUS_PL_STOP,          /* state when the system is stop */
    E_STATUS_PL_RUNNING,       /* state when the system is running */
    E_STATUS_PL_OPEN,          /* state when the system is opened */
    E_STATUS_PL_CLOSE          /* state when the system is closed */
}ENUM_PAYLOAD_STATUS_t;

/* main structure */
typedef struct
{
    ENUM_PAYLOAD_CMD_t last_cmd;       /* last command running */
    ENUM_PAYLOAD_STATUS_t status;      /* current status of the system */
}STRUCT_PAYLOAD_t;

/* monitoring structure */
typedef STRUCT_PAYLOAD_t STRUCT_PAYLOAD_MNTR_t;

/* ------------------------------------------------------------- --
   function propotypes
-- ------------------------------------------------------------- */
void API_PAYLOAD_START(void);
void API_PAYLOAD_SEND_CMD(ENUM_PAYLOAD_CMD_t command);
bool API_PAYLOAD_GET_MNTR(STRUCT_PAYLOAD_MNTR_t* monitoring);

/* ------------------------------------------------------------- --
   end of file
-- ------------------------------------------------------------- */

#endif /* PAYLOAD_INC_API_PAYLOAD_H_ */
