/*
 * TIVA-OLEGA-008.H
 *
 *  Created on: 25 mar. 2019
 *      Author: dforchetti
 */

#ifndef VARIABLES_H_
#define VARIABLES_H_

#include "variables.h"
#include "stdint.h"

//extern DHT_TypeDef;


typedef struct{
    int ENTERO;
    int FRACCION;
} NEntero;


typedef struct variables variables;

struct variables {
    int                     n               ;   // numero de datos promediados
    float                   xn              ;   // variable en formato float
    float                   suma            ;   // valor accumulativo de xn
    float                   suma2           ;   // valor accumulativo de xn^2
    float                   max             ;   // valor accumulativo de xn
    float                   min             ;   // valor accumulativo de xn
    int                     t_anterior      ;
    };


variables v_viento, lluvia, d_viento, humedad , humedad_dht22, temperatura, temperatura_dht22, gota, vBat, luz, vLipo;


struct hora{
    int hora;
    int minuto;
    int segundo;
    char text[9];
};

//void media_movil    (variables *DATO);

void SUM_MAX_MIN    (volatile variables *DATO);

void entero         (float *IN, NEntero *OUT);

void IniciaVariable (variables *DATO);

void itoa(int n, char s[]);

#ifndef REVERSE_
#define REVERSE_
void reverse(char s[]);
#endif

#ifndef ITOA_
#define ITOA_
void itoa(int n, char s[]);
#endif





#endif /* VARIABLES_H_ */
