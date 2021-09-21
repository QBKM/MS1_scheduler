/** ************************************************************* *
 * @file        API_bme280.c
 * @brief
 *
 * @date        2021-08-30
 * @author      Quentin Bakrim (quentin.bakrim@hotmail.fr)
 *
 * Mines Space
 *
 * ************************************************************* **/

/* ------------------------------------------------------------- --
   include
-- ------------------------------------------------------------- */
#include "API_bme280.h"
#include "bme280.h"

#include "FreeRTOS.h"
#include "task.h"

/* ------------------------------------------------------------- --
   defines
-- ------------------------------------------------------------- */
#define BME280_DATA_PERIOD     10

/* ------------------------------------------------------------- --
   variable
-- ------------------------------------------------------------- */
BMP280_data_t bmp280_data = {0};
TaskHandle_t bmp280_handle;

/* ------------------------------------------------------------- --
   prototypes
-- ------------------------------------------------------------- */
static void data_i2c_handler(void* parameters);

/* ------------------------------------------------------------- --
   tasks
-- ------------------------------------------------------------- */
/** ************************************************************* *
 * @brief
 *
 * @param       parameters
 * ************************************************************* **/
static void data_i2c_handler(void* parameters)
{
    TickType_t last_wakeup_time = xTaskGetTickCount();

    while(1)
    {
        bmp280_data = BMP280_Get_Data();
      /*icm_data = icm_get.... */

        xTaskDelayUntil(&last_wakeup_time, pdMS_TO_TICKS(BME280_DATA_PERIOD));
    }
}

/* ------------------------------------------------------------- --
   functions
-- ------------------------------------------------------------- */
/** ************************************************************* *
 * @brief       init the battery and return 1 if OK
 *
 * @return      uint8_t
 * ************************************************************* **/
uint8_t init_sensors(void)
{
    BaseType_t status;

    BMP280_Init();

    status = xTaskCreate(data_i2c_handler, "task_bmp280", configMINIMAL_STACK_SIZE, NULL, 3, &bmp280_handle);

    return status;
}

