/*
 * BQ32000.H
 *
 *  Created on: 12 jul. 2020
 *      Author: daniel
 */
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "I2Cmio.h"
#include "inc/hw_memmap.h"

#define BQ32000_ADDRESS     0X68
#define BQ32000_SEGUNDOS    0X00
#define BQ32000_MINUTOS     0X01
#define BQ32000_HORAS       0X02
#define BQ32000_DIA         0X04
#define BQ32000_MES         0X05
#define BQ32000_YEAR        0X06

#ifndef LIB_BQ32000_H_
#define LIB_BQ32000_H_

struct Tiempo{
    uint8_t horas;
    uint8_t minutos;
    uint8_t segundos;
    char text[9];
};


struct Tiempo HORA;

struct Fecha{
    int dia;
    int mes;
    int year;
    char text[9];
};

struct Fecha FECHA;



void actualizaTiempo(int segundos);
void actualiza_RTC(void);
void lee_tiempo(void);
void reverse(char s[]);
void itoa(int n, char s[]);
extern void actualizaTiempo(int segundos);



#endif /* LIB_BQ32000_H_ */
