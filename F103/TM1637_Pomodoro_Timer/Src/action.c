#include "action.h"
#include "leds.h"
#include "buttons.h"
#include "stm32f103drivers/circular_buffer.h"

//#define TABLE_SIZE  0xC8    /* 200 BYTES */


// Size of Action Table
#define ACTION_QTY 10

typedef enum
{
    EVENT_BUTTON0_PRESS = 0,
    EVENT_BUTTON0_RELEASE,
    EVENT_BUTTON0_LONG_PRESS,
    EVENT_BUTTON0_LONG_RELEASE,
    EVENT_BUTTON1_PRESS,
    EVENT_BUTTON1_RELEASE,
    EVENT_BUTTON1_LONG_PRESS,
    EVENT_BUTTON1_LONG_RELEASE,

    EVENT_QTY
}event_list_e;

typedef enum
{
    ACTION_SEND_EVENT_MESSAGE = 0,
    ACTION_LEDS_ONBOARD_LED_ON,
    ACTION_LEDS_ONBOARD_LED_OFF,
    ACTION_LEDS_ONBOARD_LED_TOGGLE,
    ACTION_LEDS_RED_LED_ON,
    ACTION_LEDS_RED_LED_OFF,
    ACTION_LEDS_RED_LED_TOGGLE,
    ACTION_LEDS_GREEN_LED_ON,
    ACTION_LEDS_GREEN_LED_OFF,
    ACTION_LEDS_GREEN_LED_TOGGLE
}action_list_e;


typedef uint8_t (*CallbackAction_t)(uint8_t*);

// Buffer circular de fila de acoes
static circ_buffer_t action_circ_buffer = { {0}, 0, 0 };

static CallbackAction_t action_table[ACTION_QTY] = {0};

static uint8_t action_quant_for_event[EVENT_QTY];

static uint8_t action_evt_conf_vect[ EVENT_QTY*3 ]; // Aprox. 3 acoes por evento
static uint8_t action_evt_conf_len = 0;

// Callbacks dos Botoes
static void Action_Button0_Press_Callback(void);
static void Action_Button0_Release_Callback(void);
static void Action_Button0_Long_Press_Callback(void);
static void Action_Button0_Long_Release_Callback(void);

static void Action_Button1_Press_Callback(void);
static void Action_Button1_Release_Callback(void);
static void Action_Button1_Long_Press_Callback(void);
static void Action_Button1_Long_Release_Callback(void);

void Action_Config(void)
{
    // Configura a Tabela de Acoes
    action_table[ACTION_SEND_EVENT_MESSAGE] = 0; // TODO
    action_table[ACTION_LEDS_ONBOARD_LED_ON] = Leds_Onboard_Led_ON;
    action_table[ACTION_LEDS_ONBOARD_LED_OFF] = Leds_Onboard_Led_OFF;
    action_table[ACTION_LEDS_ONBOARD_LED_TOGGLE] = Leds_Onboard_Led_Toggle;
    action_table[ACTION_LEDS_RED_LED_ON] = Leds_Red_Led_ON;
    action_table[ACTION_LEDS_RED_LED_OFF] = Leds_Red_Led_OFF;
    action_table[ACTION_LEDS_RED_LED_TOGGLE] = Leds_Red_Led_Toggle;
    action_table[ACTION_LEDS_GREEN_LED_ON] = Leds_Green_Led_ON;
    action_table[ACTION_LEDS_GREEN_LED_OFF] = Leds_Green_Led_OFF;
    action_table[ACTION_LEDS_GREEN_LED_TOGGLE] = Leds_Green_Led_Toggle;

    // Configura callbacks dos Botoes
    Button_Config(BUTTON0,  Action_Button0_Press_Callback, \
                            Action_Button0_Release_Callback, \
                            Action_Button0_Long_Press_Callback, \
                            Action_Button0_Long_Release_Callback);

    Button_Config(BUTTON1,  Action_Button1_Press_Callback, \
                            Action_Button1_Release_Callback, \
                            Action_Button1_Long_Press_Callback, \
                            Action_Button1_Long_Release_Callback);

    // Configura a quantidade  de acoes para os eventos
    action_quant_for_event[EVENT_BUTTON0_PRESS] = 2;
    action_quant_for_event[EVENT_BUTTON0_RELEASE] = 1;
    action_quant_for_event[EVENT_BUTTON0_LONG_PRESS] = 0;
    action_quant_for_event[EVENT_BUTTON0_LONG_RELEASE] = 0;

    action_quant_for_event[EVENT_BUTTON1_PRESS] = 2;
    action_quant_for_event[EVENT_BUTTON1_RELEASE] = 1;
    action_quant_for_event[EVENT_BUTTON1_LONG_PRESS] = 0;
    action_quant_for_event[EVENT_BUTTON1_LONG_RELEASE] = 0;

    // Conf acoes para os eventos
    uint8_t i = 0;
    action_evt_conf_vect[i++] = EVENT_BUTTON0_PRESS;
    action_evt_conf_vect[i++] = ACTION_LEDS_GREEN_LED_ON;
    action_evt_conf_vect[i++] = EVENT_BUTTON0_PRESS;
    action_evt_conf_vect[i++] = ACTION_LEDS_ONBOARD_LED_ON;

    action_evt_conf_vect[i++] = EVENT_BUTTON0_RELEASE;
    action_evt_conf_vect[i++] = ACTION_LEDS_GREEN_LED_OFF;

    action_evt_conf_vect[i++] = EVENT_BUTTON1_PRESS;
    action_evt_conf_vect[i++] = ACTION_LEDS_RED_LED_ON;
    action_evt_conf_vect[i++] = EVENT_BUTTON1_PRESS;
    action_evt_conf_vect[i++] = ACTION_LEDS_ONBOARD_LED_OFF;

    action_evt_conf_vect[i++] = EVENT_BUTTON1_RELEASE;
    action_evt_conf_vect[i++] = ACTION_LEDS_RED_LED_OFF;

    action_evt_conf_len = i;
}


void Action_SM(void)
{
    uint8_t dummy;
    uint8_t act_idx;
    if(BUFFER_OK == Buffer_Read(&action_circ_buffer, &act_idx))
    {
        (action_table[act_idx])(&dummy);
    }
}

//==========   Callbacks dos Botoes   ===============// // TODO

static void Action_Button0_Press_Callback(void)
{
//    uint8_t act_quant = 0;

    for (uint8_t i = 0; i < action_evt_conf_len; i+=2)
    {
        if(action_evt_conf_vect[i] == EVENT_BUTTON0_PRESS)
        {
//            act_quant++;
            Buffer_Write(&action_circ_buffer, action_evt_conf_vect[i+1]);
        }
    }

//    if(act_quant == action_quant_for_event[EVENT_BUTTON0_PRESS])
//    {
//        // OK
//        return;
//    }

}

static void Action_Button0_Release_Callback(void)
{
    for (uint8_t i = 0; i < action_evt_conf_len; i+=2)
    {
        if(action_evt_conf_vect[i] == EVENT_BUTTON0_RELEASE)
        {
            Buffer_Write(&action_circ_buffer, action_evt_conf_vect[i+1]);
        }
    }
}

static void Action_Button0_Long_Press_Callback(void)
{
    for (uint8_t i = 0; i < action_evt_conf_len; i+=2)
    {
        if(action_evt_conf_vect[i] == EVENT_BUTTON0_LONG_PRESS)
        {
            Buffer_Write(&action_circ_buffer, action_evt_conf_vect[i+1]);
        }
    }
}

static void Action_Button0_Long_Release_Callback(void)
{
    for (uint8_t i = 0; i < action_evt_conf_len; i+=2)
    {
        if(action_evt_conf_vect[i] == EVENT_BUTTON0_LONG_RELEASE)
        {
            Buffer_Write(&action_circ_buffer, action_evt_conf_vect[i+1]);
        }
    }
}


static void Action_Button1_Press_Callback(void)
{
    for (uint8_t i = 0; i < action_evt_conf_len; i+=2)
    {
        if(action_evt_conf_vect[i] == EVENT_BUTTON1_PRESS)
        {
            Buffer_Write(&action_circ_buffer, action_evt_conf_vect[i+1]);
        }
    }
}

static void Action_Button1_Release_Callback(void)
{
    for (uint8_t i = 0; i < action_evt_conf_len; i+=2)
    {
        if(action_evt_conf_vect[i] == EVENT_BUTTON1_RELEASE)
        {
            Buffer_Write(&action_circ_buffer, action_evt_conf_vect[i+1]);
        }
    }
}

static void Action_Button1_Long_Press_Callback(void)
{
    for (uint8_t i = 0; i < action_evt_conf_len; i+=2)
    {
        if(action_evt_conf_vect[i] == EVENT_BUTTON1_LONG_PRESS)
        {
            Buffer_Write(&action_circ_buffer, action_evt_conf_vect[i+1]);
        }
    }
}

static void Action_Button1_Long_Release_Callback(void)
{
    for (uint8_t i = 0; i < action_evt_conf_len; i+=2)
    {
        if(action_evt_conf_vect[i] == EVENT_BUTTON1_LONG_RELEASE)
        {
            Buffer_Write(&action_circ_buffer, action_evt_conf_vect[i+1]);
        }
    }
}

