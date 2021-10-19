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
typedef uint8_t TYPE_HMI_LENGTH_t;

typedef union 
{
    uint8_t* txt;
    uint8_t  u8;
    uint16_t u16;
    uint32_t u32;
    float    f32;
}UNION_HMI_DATA_t;

typedef enum 
{
    E_HMI_TEXT,
    E_HMI_UINT8,
    E_HMI_UINT16,
    E_HMI_UINT32,
    E_HMI_FLOAT,
}ENUM_HMI_DATA_TYPE_t;

/* ------------------------------------------------------------- --
   function prototypes
-- ------------------------------------------------------------- */
void API_HMI_START(void);
void API_HMI_SEND_DATA(TYPE_HMI_ID_t  dataID, const char *fmt, ...);

/* ------------------------------------------------------------- --
   end of file
-- ------------------------------------------------------------- */

#endif /* HMI_INC_API_HMI_H_ */
