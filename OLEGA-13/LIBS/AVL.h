/*
 * AVL.h
 *
 *  Created on: 4 mar. 2019
 *      Author: Daniel
 */

/*
 * se escriben las rutinas para acomodar los datos para el envío al AVL o SIM800
 */


#ifndef LIBS_AVL_H_
#define LIBS_AVL_H_


#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "string.h"
#include "fatfs/src/ff.h"
#include "variables.h"
#include "UART.h"
#include "variables.h"


extern char nombre[];

extern FIL *Archivo;


extern struct hora HORA_TIVA;

extern volatile struct hora HORA_GPS;

void reverse(char s[]);

void itoa(int n, char s[]);

void AVL(void);

void iniciaAVL(void);

void UART1IntHandler(void);

void ajusta_viento(void);
void ajusta_direccion(void);
void ajusta_lluvia(void);
void ajusta_humedad(void);
void ajusta_temperatura(void);
void ajusta_humedad22(void);
void ajusta_temperatura22(void);
void ajusta_luz(void);
void ajusta_gel(void);
void ajusta_lipo(void);
void ajusta_lat_long(void);



#endif /* LIBS_AVL_H_ */
