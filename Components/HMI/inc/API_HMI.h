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
   types
-- ------------------------------------------------------------- */
/* The id is use by the API to identify which type of data is sent */
typedef uint8_t TYPE_HMI_ID_t;

/* ------------------------------------------------------------- --
   defines
-- ------------------------------------------------------------- */ 
/* default value */
#define HMI_ID_NONE                 (TYPE_HMI_ID_t)0x00

/* application IDs */
#define HMI_ID_APP_PHASE            (TYPE_HMI_ID_t)0x10
#define HMI_ID_APP_AEROC            (TYPE_HMI_ID_t)0x11
#define HMI_ID_APP_WINDOW           (TYPE_HMI_ID_t)0x12
#define HMI_ID_APP_RECOV_APOGEE     (TYPE_HMI_ID_t)0x13

/* sensor IDs */
#define HMI_ID_SENS_IMU_AX          (TYPE_HMI_ID_t)0x20
#define HMI_ID_SENS_IMU_AY          (TYPE_HMI_ID_t)0x21
#define HMI_ID_SENS_IMU_AZ          (TYPE_HMI_ID_t)0x22
#define HMI_ID_SENS_IMU_GX          (TYPE_HMI_ID_t)0x23
#define HMI_ID_SENS_IMU_GY          (TYPE_HMI_ID_t)0x24
#define HMI_ID_SENS_IMU_GZ          (TYPE_HMI_ID_t)0x25
#define HMI_ID_SENS_IMU_X_KALMAN    (TYPE_HMI_ID_t)0x26
#define HMI_ID_SENS_IMU_Y_KALMAN    (TYPE_HMI_ID_t)0x27
#define HMI_ID_SENS_IMU_TEMP        (TYPE_HMI_ID_t)0x28
#define HMI_ID_SENS_IMU_ERROR       (TYPE_HMI_ID_t)0x29
#define HMI_ID_SENS_BARO_PRESS      (TYPE_HMI_ID_t)0x2A
#define HMI_ID_SENS_BARO_TEMP       (TYPE_HMI_ID_t)0x2B
#define HMI_ID_SENS_BARO_ERROR      (TYPE_HMI_ID_t)0x2C

/* monitoring IDs */
#define HMI_ID_MNTR_BAT_SEQ         (TYPE_HMI_ID_t)0x30
#define HMI_ID_MNTR_BAT_MOTOR1      (TYPE_HMI_ID_t)0x31
#define HMI_ID_MNTR_BAT_MOTOR2      (TYPE_HMI_ID_t)0x32

/* recovery IDs */
#define HMI_ID_RECOV_LAST_CMD       (TYPE_HMI_ID_t)0x40
#define HMI_ID_RECOV_STATUS         (TYPE_HMI_ID_t)0x41

/* payload IDs */
#define HMI_ID_PAYLOAD_LAST_CMD     (TYPE_HMI_ID_t)0x50
#define HMI_ID_PAYLOAD_STATUS       (TYPE_HMI_ID_t)0x51

/* ------------------------------------------------------------- --
   function prototypes
-- ------------------------------------------------------------- */
void API_HMI_START(void);
void API_HMI_SEND_DATA(TYPE_HMI_ID_t dataID, const char *fmt, ...);

/* ------------------------------------------------------------- --
   end of file
-- ------------------------------------------------------------- */

#endif /* HMI_INC_API_HMI_H_ */
