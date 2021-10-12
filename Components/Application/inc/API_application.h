/** ************************************************************* *
 * @file        API_application.h
 * @brief       
 * 
 * @date        2021-09-29
 * @author      Quentin Bakrim (quentin.bakrim@hotmail.fr)
 * 
 * Mines Space
 * 
 * ************************************************************* **/

#ifndef APPLICATION_INC_API_APPLICATION_H_
#define APPLICATION_INC_API_APPLICATION_H_

/* ------------------------------------------------------------- --
   types
-- ------------------------------------------------------------- */
typedef enum
{
    E_PHASE_WAIT,
    E_PHASE_ASCEND,
    E_PHASE_DESCEND,
    E_PHASE_END
}ENUM_PHASE_t;

/* ------------------------------------------------------------- --
   functions
-- ------------------------------------------------------------- */
void API_APPLICATION_START(void);
void API_APPLICATION_CALLBACK(void);

#endif /* APPLICATION_INC_API_APPLICATION_H_ */
