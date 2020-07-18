/*
 * INA226.c
 *
 *  Created on: 12 jul. 2020
 *      Author: daniel
 */
#include "inc/hw_memmap.h"
#include "INA226.h"
#include "I2Cmio.h"

void Ina226_Init(uint8_t Rshunt){

    float   MaximaCorrienteEsperada =   1.0,      //  [Amper]
            CAL                     =   0.0;      //  Registro de Calibración

    write16(INA226_CONF , RESET | AVG | VBCT | VSCT | MODE);

    CAL =   0.00512 / ( Rshunt / 1000.0 * MaximaCorrienteEsperada / 32768 ) ;  //  2 /\ 15 = 32.768

    write16(INA226_CALIBRATE , CAL);

}

float lee_corriente(void){

    int16_t i16corriente = 0;

    i16corriente   =   read16(INA226_CORRIENTE);

    return  (float) i16corriente * ESCALA_CORRIENTE ;   // [mm A]

}


float lee_voltaje(void){

    int16_t i16voltaje = 0;

    i16voltaje   =   read16(INA226_VOLTAJE);

    return  (float) i16voltaje * ESCALA_VOLTAJE  ;   // [V]

}


float lee_potencia(void){

    int16_t i16potencia = 0;

    i16potencia   =   read16(INA226_POTENCIA);

    return  (float) i16potencia * ESCALA_POTENCIA;   // [W]

}

