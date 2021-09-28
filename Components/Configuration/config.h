/** ************************************************************* *
 * @file        config.h
 * @brief       
 * 
 * @date        2021-09-27
 * @author      Quentin Bakrim (quentin.bakrim@hotmail.fr)
 * 
 * Mines Space
 * 
 * ************************************************************* **/

#ifndef CONFIGURATION_CONFIG_H_
#define CONFIGURATION_CONFIG_H_

/* ------------------------------------------------------------- --
   Audio
-- ------------------------------------------------------------- */
#define BUZZER_DEFAULT_PERIOD               1000
#define BUZZER_DEFAULT_DUTYCYCLE            0.015

#define BUZZER_NOTIFY_INIT_ID               0x00000001

/* ------------------------------------------------------------- --
   Battery
-- ------------------------------------------------------------- */
#define BATTERY_DEFAULT_MAX_VOLTAGE         18
#define BATTERY_DEFAULT_ADC_RANGE           1024

#define BATTERY_NOTIFY_INIT_ID              0x00000002
#define BATTERY_NOTIFY_BATT_SCHEDULER_ID    0x00001000
#define BATTERY_NOTIFY_BATT_MOTOR_ID        0x00002000

/* ------------------------------------------------------------- --
   Datalog
-- ------------------------------------------------------------- */
#define DATALOG_NOTIFY_INIT_ID              0x00000004

/* ------------------------------------------------------------- --
   HMI
-- ------------------------------------------------------------- */
#define HMI_NOTIFY_INIT_ID                  0x00000008

/* ------------------------------------------------------------- --
   Radio
-- ------------------------------------------------------------- */
#define RADIO_NOTIFY_INIT_ID                0x00000020

/* ------------------------------------------------------------- --
   Sensors
-- ------------------------------------------------------------- */
#define SENSORS_NOTIFY_INIT_ID              0x00000040

#define SENSORS_NOTIFY_IMU_ID               0x80000000
#define SENSORS_NOTIFY_PRESS_ID             0x40000000
#define SENSORS_NOTIFY_GPS_ID               0x20000000
#define SENSORS_NOTIFY_THERMO_ID            0x10000000

/* ------------------------------------------------------------- --
   System
-- ------------------------------------------------------------- */
#define SYSTEM_NOTIFY_INIT_ID               0x00000080


/* ------------------------------------------------------------- --
   List flag monitoring
-- ------------------------------------------------------------- */
/* Monitoring notify list :
 * 0x00000001   id buzzer
 * 0x00000002   id battery
 * 0x00000004   id datalog
 * 0x00000008   id hmi
 * 0x00000010   
 * 0x00000020   id radio
 * 0x00000040   id sensors
 * 0x00000080   id system
 * 0x00000100
 * 0x00000200
 * 0x00000400
 * 0x00000800
 * 0x00001000   id battery sch
 * 0x00002000   id battery motor
 * 0x00004000
 * 0x00008000
 * 0x00010000
 * 0x00020000
 * 0x00040000
 * 0x00080000
 * 0x00100000
 * 0x00200000
 * 0x00400000
 * 0x00800000
 * 0x01000000
 * 0x02000000
 * 0x04000000
 * 0x08000000
 * 0x10000000   id thermo
 * 0x20000000   id gps
 * 0x40000000   id press
 * 0x80000000   id imu
 */

#endif /* CONFIGURATION_CONFIG_H_ */
