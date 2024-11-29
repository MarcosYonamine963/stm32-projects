#ifndef ACTION_H_
#define ACTION_H_

#include <stdint.h>

typedef enum
{
    AC_COMPLETED,
    AC_RUNNING,
    AC_ERROR
}action_status_t;

void Action_Config(void);
void Action_SM(void);

action_status_t Action_Execute(uint8_t action, uint8_t *step);
uint32_t Action_Get(uint32_t addr, uint8_t evt, uint8_t* action);
uint16_t Event_Get_Cmd(uint8_t evt, uint8_t *msg, uint16_t *len);
uint8_t Send_Event_Message(uint8_t *step);
uint8_t ExecutaAcao(uint8_t numeroDaAcao, uint8_t evento);
uint8_t GetNumAcoes(uint8_t numero_do_evento);
void GetAcoes(uint8_t numero_do_evento, uint8_t numero_de_acoes, uint8_t *vetorAcoes);



#endif /* ACTION_H_ */
