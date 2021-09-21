/** ************************************************************* *
 * @file        API_bme280.h
 * @brief       
 * 
 * @date        2021-08-30
 * @author      Quentin Bakrim (quentin.bakrim@hotmail.fr)
 * 
 * Mines Space
 * 
 * ************************************************************* **/

#ifndef SENSORS_BME280_INC_TASKS_BME280_H_
#define SENSORS_BME280_INC_TASKS_BME280_H_

#include "stdint.h"

typedef struct
{
	float temperature;
	float pressure;
}BMP280_api_t;

uint8_t init_bmp280(void);
BMP280_api_t get_bmp280_data(void);

#endif /* SENSORS_BME280_INC_TASKS_BME280_H_ */
