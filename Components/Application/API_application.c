/** ************************************************************* *
 * @file        API_application.c
 * @brief       
 * 
 * @date        2021-09-29
 * @author      Quentin Bakrim (quentin.bakrim@hotmail.fr)
 * 
 * Mines Space
 * 
 * ************************************************************* **/

/* ------------------------------------------------------------- --
   includes
-- ------------------------------------------------------------- */
#include "stdbool.h"
#include "stdint.h"

#include "API_application.h"
#include "API_buzzer.h"

//#include "config.h"

/* ------------------------------------------------------------- --
   defines
-- ------------------------------------------------------------- */
#define APPLICATION_DEFAULT_AEROCONTACT_ID 0x08000000

/* ------------------------------------------------------------- --
   types
-- ------------------------------------------------------------- */
typedef struct
{
    volatile bool       flag;
    volatile uint32_t   triggerDate;
}STRUCT_AEROCONTACT_t;

/* ------------------------------------------------------------- --
   variables
-- ------------------------------------------------------------- */
TaskHandle_t TaskHandle_application;

static ENUM_PHASE_t phase;

STRUCT_AEROCONTACT_t aerocontact;
STRUCT_BUZZER_t buzzer;

/* ------------------------------------------------------------- --
   prototypes
-- ------------------------------------------------------------- */
static void handler_application(void* parameters);

/* ------------------------------------------------------------- --
   functions
-- ------------------------------------------------------------- */
/** ************************************************************* *
 * @brief       
 * 
 * ************************************************************* **/
static void handler_application(void* parameters)
{
    uint32_t notify_id;

    //STRUCT_BARO_t pressure = {0};

    while(1)
    {
        /* check task notify */
        if(xTaskNotifyWait(0,0, &notify_id, 0) == pdTRUE)
        {
            /* check aerocontact */
            if(notify_id & APPLICATION_DEFAULT_AEROCONTACT_ID)
            {
                phase = E_PHASE_ASCEND;
                buzzer.dutycycle = 0.1;
                buzzer.period = 100;
                xQueueSend(QueueHandle_buzzer, &buzzer, 0);
            }
        }

        //xTaskNotify(TaskHandle_sensors, 0, eNoAction);

        //xqueuereceive ihm
            //if btn pressed, notify recovery
        //xqueuereceive battery
            //if battery ok/ko => data + ihm

        /* receive data from API_SENSORS */
        //xQueueReceive(QueueHandle_sensors, &pressure, pdMS_TO_TICKS(10)); 

        //xqueuesend recovery
            //if new cmd, send it to recovery (ihm btn or algo)
        //xqueuesend payload
            //if new cmd, send it to recovery (ihm btn or algo)

        //xqueuesend flash
        //xqueuesend radio
        //xqueuesend ihm
            //if status == ko => send msg + led
    }
}

/** ************************************************************* *
 * @brief       
 * 
 * ************************************************************* **/
void API_APPLICATION_START(void)
{
    BaseType_t status;

    phase = E_PHASE_WAIT;

    aerocontact.flag = false;
    aerocontact.triggerDate = 0xFFFFFFFF;
    
    status = xTaskCreate(handler_application, "task_application", configMINIMAL_STACK_SIZE, NULL, 3, &TaskHandle_application);

    configASSERT(status == pdPASS);
}

/** ************************************************************* *
 * @brief       
 * 
 * ************************************************************* **/
void API_APPLICATION_CALLBACK(void)
{
    if(aerocontact.flag == false)
    {
        aerocontact.flag = true;
        aerocontact.triggerDate = xTaskGetTickCountFromISR();

        xTaskNotifyFromISR(TaskHandle_application, APPLICATION_DEFAULT_AEROCONTACT_ID, eSetBits, pdFALSE);
    }
}

