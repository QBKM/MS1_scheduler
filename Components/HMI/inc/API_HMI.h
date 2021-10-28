/** ************************************************************* *
 * @file        API_HMI.h
 * @brief       
 * 
 * @date        2021-10-16
 * @author      Quentin Bakrim (quentin.bakrim@hotmail.fr)
 * 
 * Mines Space
 * 
 * ************************************************************* **/

#ifndef HMI_INC_API_HMI_H_
#define HMI_INC_API_HMI_H_

/* ------------------------------------------------------------- --
   includes
-- ------------------------------------------------------------- */
#include "stdint.h"

/* ------------------------------------------------------------- --
   defines
-- ------------------------------------------------------------- */ 
/* default value */
#define HMI_ID_NONE                 (uint8_t)0x00

/* application IDs */
#define HMI_ID_APP_PHASE            (uint8_t)0x10
#define HMI_ID_APP_AEROC            (uint8_t)0x11
#define HMI_ID_APP_WINDOW           (uint8_t)0x12
#define HMI_ID_APP_RECOV_TO         (uint8_t)0x13

/* sensor IDs */
#define HMI_ID_SENS_IMU_AX          (uint8_t)0x20
#define HMI_ID_SENS_IMU_AY          (uint8_t)0x21
#define HMI_ID_SENS_IMU_AZ          (uint8_t)0x22
#define HMI_ID_SENS_IMU_GX          (uint8_t)0x23
#define HMI_ID_SENS_IMU_GY          (uint8_t)0x24
#define HMI_ID_SENS_IMU_GZ          (uint8_t)0x25
#define HMI_ID_SENS_IMU_TEMP        (uint8_t)0x26
#define HMI_ID_SENS_BARO_PRESS      (uint8_t)0x27
#define HMI_ID_SENS_BARO_TEMP       (uint8_t)0x28

/* monitoring IDs */
#define HMI_ID_MONIT_BAT_SEQ_V      (uint8_t)0x30
#define HMI_ID_MONIT_BAT_SEQ_A      (uint8_t)0x31
#define HMI_ID_MONIT_BAT_MOTOR1_V   (uint8_t)0x32
#define HMI_ID_MONIT_BAT_MOTOR1_A   (uint8_t)0x33
#define HMI_ID_MONIT_BAT_MOTORj2_V  (uint8_t)0x34
#define HMI_ID_MONIT_BAT_MOTOR2_A   (uint8_t)0x35

/* ------------------------------------------------------------- --
   types
-- ------------------------------------------------------------- */
/* The id is use by the API to identify which type of data is sent */
typedef uint8_t TYPE_HMI_ID_t;

/* ------------------------------------------------------------- --
   function prototypes
-- ------------------------------------------------------------- */
void API_HMI_START(uint32_t priority);
void API_HMI_SEND_DATA(TYPE_HMI_ID_t dataID, const char *fmt, ...);

/* ------------------------------------------------------------- --
   end of file
-- ------------------------------------------------------------- */

#endif /* HMI_INC_API_HMI_H_ */
