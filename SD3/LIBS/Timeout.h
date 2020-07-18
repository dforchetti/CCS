/*
 * Timeout.h
 *
 *  Created on: 17 ene. 2020
 *      Author: daniel
 */

#include <stdint.h>
#include <stdbool.h>
#include "driverlib/rom.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"


#ifndef DRIVERS_TIMEOUT_H_
#define DRIVERS_TIMEOUT_H_

extern volatile bool  flag_timeout ;
extern volatile int   contador_timeout  ;

#define TIMEOUT_TIMER_PERIPH    SYSCTL_PERIPH_TIMER4
#define TIMEOUT_TIMER           TIMER4_BASE
#define TIMEOUT_PERIOD(x)       ROM_TimerLoadSet(TIMEOUT_TIMER, TIMER_A, x);
#define TIMEOUT(x)              flag_timeout     = false;                       \
                                contador_timeout = 0    ;                       \
                                ROM_TimerLoadSet(TIMEOUT_TIMER, TIMER_A, x);

void TimerTIMEOUT(void);
void TIMEOUTinit(void);

#endif /* DRIVERS_TIMEOUT_H_ */
