/*
 * tick.h
 *
 *  Created on: 21 dic. 2018
 *      Author: dforchetti
 */

#ifndef TICK_H_
#define TICK_H_

#include <stdint.h>
#include <stdbool.h>
#include "driverlib/systick.h"
#include "driverlib/sysctl.h"



#define PERIODO 1000            // 1000 miisegundos
#define TRUE    1
#define FALSE   0



extern volatile uint32_t milis, tanterior;

extern volatile bool flagCONVERSION_1S, flagCONVERSION_10S, flagENVIO;

extern void SycTickInt();
extern void SysTickbegin();

#endif /* TICK_H_ */
