/*
 * API_sensors.h
 *
 *  Created on: Sep 18, 2021
 *      Author: Quent
 */

#ifndef SENSORS_INC_API_SENSORS_H_
#define SENSORS_INC_API_SENSORS_H_

#include "stdint.h"

typedef struct
{
	float temperature;
	float pressure;
}BMP280_api_t;

#endif /* SENSORS_INC_API_SENSORS_H_ */
