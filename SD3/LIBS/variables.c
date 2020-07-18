/*
 * variables.c
 *
 *  Created on: 26 mar. 2019
 *      Author: dforchetti
 */
#include "string.h"
#include "variables.h"
#include "dht22_mio.h"

extern volatile uint32_t milis;

extern char datos[];

struct hora HORA_TIVA    ;

volatile struct hora HORA_GPS     ;

int WDT_TIMEOUT = 0X00FFFFFF;

DHT_TypeDef th;

void entero      (float *IN, NEntero *OUT){

    //  convierte a entero a xn

    OUT->ENTERO     = (int32_t)  *IN ;

    OUT->FRACCION   = (int32_t) (*IN * 100.0f);

    OUT->FRACCION = OUT->FRACCION - (OUT->ENTERO * 100);

    if ( OUT->FRACCION < 0 )   OUT->FRACCION *= -1;

}


void IniciaVariable(variables *DATO){

    DATO->n         = 0 ;
    DATO->xn        = 0 ;
    DATO->suma      = 0 ;
    DATO->suma2     = 0 ;
    DATO->max       = 0 ;
    DATO->min       = 9999;//DATO->max ;   // considera el minimo del proximo período
    DATO->t_anterior= milis ;

}


void reverse(char s[])
{

    int i, j;
    char c;

    for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}





void itoa(int n, char s[])
{
    int i, sign;

    if ((sign = n) < 0)  /* record sign */
        n = -n;          /* make n positive */
    i = 0;
    do {       /* generate digits in reverse order */
        s[i++] = n % 10 + '0';   /* get next digit */
    } while ((n /= 10) > 0);     /* delete it */
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    reverse(s);
}



void SUM_MAX_MIN    (volatile variables *DATO){

    DATO->suma  += DATO->xn ;                                           // agrega un dato más a la sumatoria

    DATO->suma2 += DATO->xn * DATO->xn ;                                // agrega un dato más a la sumatoria cuadrática-> https://es.wikipedia.org/wiki/Desviaci%C3%B3n_t%C3%ADpica

    DATO->max    = ( DATO->xn > DATO->max ) ? DATO->xn : DATO->max ;    // determina el maximo

    DATO->min    = ( DATO->xn < DATO->min ) ? DATO->xn : DATO->min ;    // determina el minimo

}



