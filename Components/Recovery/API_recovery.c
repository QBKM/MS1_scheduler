/** ************************************************************* *
 * @file        API_recovery.c
 * @brief       
 * 
 * @date        2021-10-11
 * @author      Quentin Bakrim (quentin.bakrim@hotmail.fr)
 * 
 * Mines Space
 * 
 * ************************************************************* **/


/* ------------------------------------------------------------- --
   include
-- ------------------------------------------------------------- */
#include "API_recovery.h"
#include "freeRtos.h"
#include "task.h"
#include "gpio.h"
#include "tim.h"
#include "queue.h"

#include "MS1_config.h"

/* ------------------------------------------------------------- --
   defines
-- ------------------------------------------------------------- */
#define RECOVERY_CCR2_M1        3840u   /* 80% PWM (ARR = 4800) */
#define RECOVERY_CCR2_M2        3840u   /* 80% PWM (ARR = 4800) */

/* ------------------------------------------------------------- --
   handles
-- ------------------------------------------------------------- */
TaskHandle_t TaskHandle_recovery;
QueueHandle_t QueueHandle_recov_cmd;
QueueHandle_t QueueHandle_recov_mntr;

/* ------------------------------------------------------------- --
   variables
-- ------------------------------------------------------------- */
static STRUCT_RECOV_t recov_mntr = {0};

/* ------------------------------------------------------------- --
   prototypes
-- ------------------------------------------------------------- */
static void handler_recovery(void* parameters);

static void process_cmd(ENUM_RECOV_CMD_t cmd);
static void check_position(void);

/* ============================================================= ==
   tasks functions
== ============================================================= */
/** ************************************************************* *
 * @brief       This task manage the recovery system with the 
 *              opening or closing features. The task need to 
 *              receive command from queue to operate.
 *              Please check at the ENUM_CMD_ID_t enum to send
 *              commands
 * 
 * @param       parameters 
 * ************************************************************* **/
static void handler_recovery(void* parameters)
{
    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    
    ENUM_RECOV_CMD_t cmd = E_CMD_RECOV_NONE;

    while(1)
    {
        /* check for new command */
        if(xQueueReceive(QueueHandle_recov_cmd, &cmd, (TickType_t)0)) 
        {
            process_cmd(cmd);
        }

        /* check if the system has reach the end */
        check_position();

        /* wait until next task period */
        vTaskDelayUntil(&xLastWakeTime, TASK_PERIOD_RECOVERY);
    }
}

/** ************************************************************* *
 * @brief       
 * 
 * @param       cmd 
 * ************************************************************* **/
static void process_cmd(ENUM_RECOV_CMD_t cmd)
{
    switch(cmd)
    {
        case E_CMD_RECOV_OPEN :
            /* run motors clockwise */
            HAL_GPIO_WritePin(DIR_M1_GPIO_Port, DIR_M1_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(DIR_M2_GPIO_Port, DIR_M2_Pin, GPIO_PIN_SET);

            /* enable the pwm */
            HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
            HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);

            /* enable the motors */
            HAL_GPIO_WritePin(EN_M1_GPIO_Port, EN_M1_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(EN_M1_GPIO_Port, EN_M2_Pin, GPIO_PIN_SET);

            /* update system structure */
            recov_mntr.status 	= E_STATUS_RECOV_RUNNING;
            recov_mntr.last_cmd = cmd;
            break;

        case E_CMD_RECOV_CLOSE :
            /* run motors anti-clockwise */
            HAL_GPIO_WritePin(DIR_M1_GPIO_Port, DIR_M1_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(DIR_M2_GPIO_Port, DIR_M2_Pin, GPIO_PIN_RESET);

            /* enable the pwm */
            HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
            HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);

            /* enable the motors */
            HAL_GPIO_WritePin(EN_M1_GPIO_Port, EN_M1_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(EN_M1_GPIO_Port, EN_M2_Pin, GPIO_PIN_SET);
            
            /* update system structure */
            recov_mntr.status 	= E_STATUS_RECOV_RUNNING;
            recov_mntr.last_cmd = cmd;
            break;

        case E_CMD_RECOV_STOP :
            /* diasable the motors */
            HAL_GPIO_WritePin(EN_M1_GPIO_Port, EN_M1_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(EN_M1_GPIO_Port, EN_M2_Pin, GPIO_PIN_RESET);

            /* disable the pwm */
            HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
            HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_3);

            /* update system structure */
            recov_mntr.status 	= E_STATUS_RECOV_STOP;
            recov_mntr.last_cmd = cmd;

        default :
            break;
    }

    /* update monitoring queue */
    xQueueSend(QueueHandle_recov_mntr, &recov_mntr, (TickType_t)0);
}

/** ************************************************************* *
 * @brief       
 * 
 * ************************************************************* **/
static void check_position(void)
{
    /* check if the system has reach the open point */
    if(HAL_GPIO_ReadPin(END_11_GPIO_Port, END_11_Pin) == GPIO_PIN_RESET
    || HAL_GPIO_ReadPin(END_12_GPIO_Port, END_12_Pin) == GPIO_PIN_RESET)
    {
        /* diasable the motors */
        HAL_GPIO_WritePin(EN_M1_GPIO_Port, EN_M1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(EN_M1_GPIO_Port, EN_M2_Pin, GPIO_PIN_RESET);

        /* disable the pwm */
        HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
        HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_3);

        /* update system structure */
        recov_mntr.status = E_STATUS_RECOV_OPEN;

        /* update monitoring queue */
        xQueueSend(QueueHandle_recov_mntr, &recov_mntr, (TickType_t)0);
    }

    /* check if the system has reach the close point */
    if(HAL_GPIO_ReadPin(END_12_GPIO_Port, END_21_Pin) == GPIO_PIN_RESET
    || HAL_GPIO_ReadPin(END_12_GPIO_Port, END_22_Pin) == GPIO_PIN_RESET)
    {
        /* diasable the motors */
        HAL_GPIO_WritePin(EN_M1_GPIO_Port, EN_M1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(EN_M1_GPIO_Port, EN_M2_Pin, GPIO_PIN_RESET);

        /* disable the pwm*/
        HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
        HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_3);

        
        /* update system structure */
        recov_mntr.status = E_STATUS_RECOV_CLOSE;

        /* update monitoring queue */
        xQueueSend(QueueHandle_recov_mntr, &recov_mntr, (TickType_t)0);
    }
}

/* ============================================================= ==
   public functions
== ============================================================= */
/** ************************************************************* *
 * @brief       init and start the recovery task
 * 
 * ************************************************************* **/
void API_RECOVERY_START(void)
{
    BaseType_t status;

    /* init the main structure */
    recov_mntr.last_cmd   = E_CMD_RECOV_NONE;
    recov_mntr.status     = E_STATUS_RECOV_NONE;

    /* init the motors pwm dutycycle */
    TIM2->CCR2 = RECOVERY_CCR2_M1;
    TIM3->CCR3 = RECOVERY_CCR2_M2;

    /* create the queues */
    QueueHandle_recov_cmd  = xQueueCreate(1, sizeof(ENUM_RECOV_CMD_t));
    QueueHandle_recov_mntr = xQueueCreate(1, sizeof(STRUCT_RECOV_MNTR_t));
    
    /* create the task */
    status = xTaskCreate(handler_recovery, "task_recovery", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_RECOVERY, &TaskHandle_recovery);
    configASSERT(status == pdPASS);
}

/** ************************************************************* *
 * @brief       send a command to the recovery task
 * 
 * @param       cmd 
 * ************************************************************* **/
void API_RECOVERY_SEND_CMD(ENUM_RECOV_CMD_t command)
{
    xQueueSend(QueueHandle_recov_cmd, &command, (TickType_t)0);
}

/** ************************************************************* *
 * @brief       get the recovery status
 * 
 * @param       monitoring 
 * @return      true    new status received
 * @return      false   nothing received
 * ************************************************************* **/
bool API_RECOVERY_GET_MNTR(STRUCT_RECOV_MNTR_t* monitoring)
{
    return (xQueueReceive(QueueHandle_recov_mntr, monitoring, (TickType_t)0)) ? true : false;
}

/* ------------------------------------------------------------- --
   end of file
-- ------------------------------------------------------------- */
