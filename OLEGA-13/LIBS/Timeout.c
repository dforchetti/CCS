/*
 * Timeout.c
 *
 *  Created on: 17 ene. 2020
 *      Author: daniel
 */

#include "Timeout.h"

/*
 ==============================================================================
       código para el TIMEOUT
 ==============================================================================
 */

volatile bool  flag_timeout=   false;
volatile int   contador_timeout = 0 ;



void TIMEOUTinit(void){

    ROM_SysCtlPeripheralEnable(TIMEOUT_TIMER_PERIPH);

    ROM_TimerConfigure(TIMEOUT_TIMER, TIMER_CFG_A_PERIODIC);

    TIMEOUT_PERIOD(ROM_SysCtlClockGet()/2)      // por defecto carga un timeout de 1/2 seg

    TimerIntRegister(TIMEOUT_TIMER, TIMER_A, TimerTIMEOUT);

    ROM_TimerIntEnable(TIMEOUT_TIMER, TIMER_TIMA_TIMEOUT);

    ROM_TimerEnable(TIMEOUT_TIMER, TIMER_A);


}

void TimerTIMEOUT(void)
{

    ROM_TimerIntClear(TIMEOUT_TIMER, TIMER_TIMA_TIMEOUT);

    flag_timeout = true;

    if (flag_timeout) contador_timeout++;

}


