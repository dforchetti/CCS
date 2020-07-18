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
#include <string.h>
#include "driverlib/systick.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"


#define PERIODO 1000            // 1000 miisegundos
#define TRUE    1
#define FALSE   0

extern volatile uint32_t milis, tanterior;

extern volatile bool flagCONVERSION_1S, flagCONVERSION_10S, flagENVIO;

void actualizaTiempo(int segundos);

void SycTickInt();

void SysTickbegin();


#endif /* TICK_H_ */
