/*
 * VelDirPluv.h
 *
 *  Created on: 28 ene. 2019
 *      Author: Daniel
 */

#ifndef LIBS_VELDIRPLUV_H_
#define LIBS_VELDIRPLUV_H_

#include "variables.h"
#include "mediciones.h"
#include "inicio.h"

extern volatile uint32_t CONFIG;



void VelDirPluvIntHandler(void);

void VelDirPluvInicio(void);

void ADC1InterruptHandler(void);

void calculaDireccionViento(variables *);

void ADC1InterruptHandler(void);



#endif /* LIBS_VELDIRPLUV_H_ */
