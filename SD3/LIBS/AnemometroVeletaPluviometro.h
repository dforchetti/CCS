/*
 * VelDirPluv.h
 *
 *  Created on: 28 ene. 2019
 *      Author: Daniel
 */
#include "variables.h"
#include "inicio.h"


#ifndef LIBS_ANEMOMETROVELETAPLUVIOMETRO_H_
#define LIBS_ANEMOMETROVELETAPLUVIOMETRO_H_

#define CONVIERTE_ADC   ROM_ADCProcessorTrigger(ADC1_BASE, 0);


void VelDirPluvIntHandler(void);

void VelDirPluvInicio(void);

void ADC1InterruptHandler(void);

void calculaDireccionViento(variables *);

void ADC1InterruptHandler(void);


#endif /* LIBS_ANEMOMETROVELETAPLUVIOMETRO_H_ */
