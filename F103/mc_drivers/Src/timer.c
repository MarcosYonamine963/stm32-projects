/*
 * timer.c
 *
 *  Created on: Jul 17, 2024
 *  Author: Mc
 */

#include "stm32f1xx.h"
#include "timer.h"

#define TICK    8000U // Sys clock is 8 Mhz. Set timer clk to 1kHz
#define FALSE   0
#define TRUE    1

/*
 * @brief Struct utilizada para gerenciar os timers.
 *
 */
typedef struct
{
    uint8_t flag;               /**< Flag de ativacao do timer.  */
    uint8_t timer_mode;               /**< Tipo do Timer. ALWAYS ou ONCE.  */
    uint32_t timeout;           /**< Timeout do Timer  */
    uint32_t timer;             /**< Contagem do Timer  */
    CallbackFunc_t callback;    /**< Callback para a funcao que deve ser executada quando o timer estourar */
}timer_t;


/**
 * @brief Array de structs de timers.
 *
 */
timer_t timers[TIMER_N_MAX];

volatile uint32_t systick_time = 0;
volatile uint32_t delay_time = 0;

/**
 * @brief Funcao de inicializacao dos Timers. Configura a interrupcao do SysTick
 * e zera a flag de todos os timers.
 *
 */
void Timer_Init(void)
{
    /* https://www.keil.com/pack/doc/CMSIS_Dev/Core/html/group__system__init__gr.html */
    SysTick_Config(TICK);

    // inicia sem contar nenhum timer ate que ele seja chamado
    for (uint8_t i = 0; i < TIMER_N_MAX; i++)
    {
        timers[i].flag = 0;
    }

}// end Timer_Init

/**
 * @brief Funcao de tratamento da interrupcao do SysTick. Ocorre a cada 1ms e
 * incrementa a contagem de todos os timers.
 *
 */
void SysTick_Handler(void)
{
    systick_time++;
    delay_time++;
    for (uint8_t i = 0; i < TIMER_N_MAX; i++)
    {
        timers[i].timer++;
    }
}// end SysTick_Handler



/**
 * @brief Funcao para configurar e iniciar um timer.
 *
 * @param i Index do timer no array de timers.
 * @param time Tempo em ms de timeout do timer.
 * @param callback Funcao de callback do timer.
 * @param type Tipo de execucao do timer. ALWAYS ou ONCE.
 */
void Timer_Set(uint8_t i, uint32_t time, CallbackFunc_t callback, uint8_t timer_mode)
{
    timers[i].flag = 1;
    timers[i].timeout = time;
    timers[i].callback = callback;
    timers[i].timer_mode = timer_mode;
    timers[i].timer = 0;
}

/**
 * @brief Funcao para desativar um timer.
 *
 * @param n_timer Index do timer no array de timers.
 * @return uint8_t Retorna status da operacao.
 */
uint8_t Timer_Clear(uint8_t n_timer)
{
    if ((n_timer >= 0) && (n_timer < TIMER_N_MAX))
    {
        timers[n_timer].flag = 0;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/**
 * @brief Maquina de estados dos timer. Checa se ocorreu timeout em algum timer
 * e executa a funcao de callback.
 *
 */
void Timer_SM(void)
{
    for (uint8_t i=0; i<TIMER_N_MAX; i++)
    {
        if (timers[i].flag == 1)
        {
            if (timers[i].timer >= timers[i].timeout)
            {
                if (timers[i].timer_mode == TIMER_MODE_ONCE)
                {
                    timers[i].flag = 0;
                }
                else if (timers[i].timer_mode == TIMER_MODE_ALWAYS)
                {
                    timers[i].timer = 0;
                }

                (timers[i].callback)();
            }
        }
    }
}// end Timer_SM


/**
 * @brief Funcao de delay, bloqueia o funcionamento do programa pelo tempo
 * especificado.
 *
 * @param t Quantidade de milisegundos que a funcao deve executar.
 *
 * @note Funcao bloqueante: nao recomendado para tempo muito longo
 */
void Delay_ms(uint32_t t)
{
    delay_time = 0;
    while (delay_time < t)
    {
        asm volatile("nop");
    }
}// end Delay_ms

