/** ************************************************************* *
 * @file        MS1_config.h
 * @brief       
 * 
 * @date        2021-11-29
 * @author      Quentin Bakrim (quentin.bakrim@hotmail.fr)
 * 
 * Mines Space
 * 
 * ************************************************************* **/

#ifndef _MS1_CONFIG_H_
#define _MS1_CONFIG_H_

/* ------------------------------------------------------------- --
   includes
-- ------------------------------------------------------------- */
#include "stdint.h"

/* ------------------------------------------------------------- --
   defines
-- ------------------------------------------------------------- */
/* TASK PRIORITIES */
#define TASK_PRIORITY_SENSORS           (uint32_t)5     /* Sensors */
#define TASK_PRIORITY_APPLICATION       (uint32_t)4     /* Application */
#define TASK_PRIORITY_RECOVERY          (uint32_t)3     /* Recovery */
#define TASK_PRIORITY_PAYLOAD           (uint32_t)3     /* Payload */
#define TASK_PRIORITY_BATTERY           (uint32_t)2     /* Battery */
#define TASK_PRIORITY_BUZZER            (uint32_t)1     /* Audio */
#define TASK_PRIORITY_HMI               (uint32_t)1     /* HMI */

/* TASK PERIOD DELAY */                                 /* [RTOS tick = 10ms/tick] */
#define TASK_PERIOD_APPLICATION         (uint32_t)1     /* [RTOS tick] */
#define TASK_PERIOD_RECOVERY            (uint32_t)10    /* [RTOS tick] */
#define TASK_PERIOD_BATTERY             (uint32_t)100   /* [RTOS tick] */

#endif /* _MS1_CONFIG_H_ */
/* ------------------------------------------------------------- --
   end of file
-- ------------------------------------------------------------- */