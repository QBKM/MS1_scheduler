/** ************************************************************* *
 * @file        SYS_master.c
 * @brief       
 * 
 * @date        2021-09-02
 * @author      Quentin Bakrim (quentin.bakrim@hotmail.fr)
 * 
 * Mines Space
 * 
 * ************************************************************* **/

#include "SYS_master.h"

#include "FreeRTOS.h"
#include "task.h"

/* ------------------------------------------------------------- --
   types
-- ------------------------------------------------------------- */
typedef enum
{
    E_NO_PHASE,
    E_PHASE_WAIT,
    E_PHASE_ASCEND,
    E_PHASE_DEPLOY,
    E_PHASE_DESCEND,
    E_PHASE_LAND
}PHASE_t;

TaskHandle_t master_handle;
volatile PHASE_t SYS_phase = E_NO_PHASE;

static void master_handler(void* parameters);
void SYS_AEROCONTACT_IT_handle(void);

static void master_handler(void* parameters)
{
    while(1)
    {
        if(SYS_phase == E_PHASE_WAIT)
        {

        }

    }
}

uint8_t init_master(void)
{
    BaseType_t status;

    SYS_phase = E_PHASE_WAIT;

    status = xTaskCreate(master_handler, "task_master", configMINIMAL_STACK_SIZE, NULL, 5, &master_handle);

    return status;
}

void SYS_AEROCONTACT_IT_handle(void)
{
    SYS_phase = E_PHASE_ASCEND;

    //xTaskNotifyFromISR(master_handle, 0, eNoAction, pdFALSE);
}