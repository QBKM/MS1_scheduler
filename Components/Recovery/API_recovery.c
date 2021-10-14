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

/* ------------------------------------------------------------- --
   defines
-- ------------------------------------------------------------- */
#define RECOVERY_DEFAULT_PERIOD_TASK    10u     /* [ms] */ 
#define RECOVERY_DEFAULT_CCR2_M1        3840u   /* 80% PWM (ARR = 4800) */
#define RECOVERY_DEFAULT_CCR2_M2        3840u   /* 80% PWM (ARR = 4800) */

/* ------------------------------------------------------------- --
   types
-- ------------------------------------------------------------- */
/* List of system status */
typedef enum
{
    E_STATUS_NONE,          /* default state */
    E_STATUS_STOP,          /* state when the system is stop */
    E_STATUS_RUNNING,       /* state when the system is running */
    E_STATUS_OPEN,          /* state when the system is opened */
    E_STATUS_CLOSE          /* state when the system is closed */
}ENUM_RECOVERY_STATUS_t;

/* main structure */
typedef struct
{
    ENUM_CMD_ID_t last_cmd;             /* last command running */
    ENUM_RECOVERY_STATUS_t status;      /* current status of the system */
}STRUCT_recovery_t;

/* ------------------------------------------------------------- --
   handles
-- ------------------------------------------------------------- */
TaskHandle_t TaskHandle_recovery;
QueueHandle_t QueueHandle_recovery;

/* ------------------------------------------------------------- --
   variable
-- ------------------------------------------------------------- */
static STRUCT_recovery_t recovery = {0};

/* ------------------------------------------------------------- --
   prototypes
-- ------------------------------------------------------------- */
static void handler_recovery(void* parameters);

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
    
    ENUM_CMD_ID_t cmd = E_CMD_NONE;

    while(1)
    {
        /* check for new command */
        if(xQueueReceive(QueueHandle_recovery, &cmd, (TickType_t)0))
        {
            switch(cmd)
            {
                case E_CMD_OPEN :
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
                    recovery.status 	= E_STATUS_RUNNING;
                    recovery.last_cmd 	= cmd;
                    break;

                case E_CMD_CLOSE :
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
                    recovery.status 	= E_STATUS_RUNNING;
                    recovery.last_cmd 	= cmd;
                    break;

                case E_CMD_STOP :
                    /* diasable the motors */
                    HAL_GPIO_WritePin(EN_M1_GPIO_Port, EN_M1_Pin, GPIO_PIN_RESET);
                    HAL_GPIO_WritePin(EN_M1_GPIO_Port, EN_M2_Pin, GPIO_PIN_RESET);

                    /* disable the pwm */
                    HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
                    HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_3);

                    /* update system structure */
                    recovery.status 	= E_STATUS_STOP;
                    recovery.last_cmd 	= cmd;

                default :
                    break;
            }
        }

        /* check if the system as reach the open point */
        if(HAL_GPIO_ReadPin(END11_GPIO_Port, END11_Pin) == GPIO_PIN_SET
        || HAL_GPIO_ReadPin(END12_GPIO_Port, END12_Pin) == GPIO_PIN_SET)
        {
            /* diasable the motors */
            HAL_GPIO_WritePin(EN_M1_GPIO_Port, EN_M1_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(EN_M1_GPIO_Port, EN_M2_Pin, GPIO_PIN_RESET);

            /* disable the pwm */
            HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
            HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_3);

            /* update system structure */
            recovery.status = E_STATUS_OPEN;
        }

        /* check if the system as reach the open or close point */
        if(HAL_GPIO_ReadPin(END12_GPIO_Port, END21_Pin) == GPIO_PIN_SET
        || HAL_GPIO_ReadPin(END12_GPIO_Port, END22_Pin) == GPIO_PIN_SET)
        {
            /* diasable the motors */
            HAL_GPIO_WritePin(EN_M1_GPIO_Port, EN_M1_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(EN_M1_GPIO_Port, EN_M2_Pin, GPIO_PIN_RESET);

            /* disable the pwm*/
            HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
            HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_3);
            
            /* update system structure */
             recovery.status = E_STATUS_CLOSE;
        }

        /* wait until next task period */
        vTaskDelayUntil(xLastWakeTime, pdMS_TO_TICKS(RECOVERY_DEFAULT_PERIOD_TASK));
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
    recovery.last_cmd   = E_CMD_NONE;
    recovery.status     = E_STATUS_NONE;

    /* init the motors pwm dutycycle */
    htim2.Instance->CCR2 = RECOVERY_DEFAULT_CCR2_M1;
    htim3.Instance->CCR2 = RECOVERY_DEFAULT_CCR2_M2;

    /* create the queue */
    QueueHandle_recovery = xQueueCreate(1, sizeof(ENUM_CMD_ID_t));
    
    /* create the task */
    status = xTaskCreate(handler_recovery, "task_recovery", configMINIMAL_STACK_SIZE, NULL, 3, &TaskHandle_recovery);
    configASSERT(status == pdPASS);
}

/** ************************************************************* *
 * @brief       send a command to the recovery task
 * 
 * @param       cmd 
 * ************************************************************* **/
void API_RECOVERY_SEND_CMD(ENUM_CMD_ID_t cmd)
{
    xQueueSend(QueueHandle_recovery, &cmd, 0);
}

/* ------------------------------------------------------------- --
   end of file
-- ------------------------------------------------------------- */
