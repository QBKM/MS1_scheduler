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
#define TASK_PRIORITY_APP_AEROCONTACT   (uint32_t)5     /* Application */
#define TASK_PRIORITY_APP_WINDOWS       (uint32_t)5     /* Application */
#define TASK_PRIORITY_SENSORS_MPU6050   (uint32_t)4     /* Sensors */
#define TASK_PRIORITY_SENSORS_BMP280    (uint32_t)4     /* Sensors */
#define TASK_PRIORITY_APP_USER_BUTTONS  (uint32_t)4     /* Application */
#define TASK_PRIORITY_APP_RECOVERY      (uint32_t)3     /* Application */
#define TASK_PRIORITY_APP_PAYLOAD       (uint32_t)3     /* Application */
#define TASK_PRIORITY_RECOVERY          (uint32_t)3     /* Recovery */
#define TASK_PRIORITY_PAYLOAD           (uint32_t)3     /* Payload */
#define TASK_PRIORITY_APP_MONITORING    (uint32_t)2     /* Application */
#define TASK_PRIORITY_AUDIO_BUZZER      (uint32_t)1     /* Audio */
#define TASK_PRIORITY_BATTERY           (uint32_t)1     /* Battery */
#define TASK_PRIORITY_HMI               (uint32_t)1     /* HMI */

#endif /* _MS1_CONFIG_H_ */
/* ------------------------------------------------------------- --
   end of file
-- ------------------------------------------------------------- */