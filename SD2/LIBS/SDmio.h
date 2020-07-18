/*
 * SD.h
 *
 *  Created on: 28 feb. 2019
 *      Author: dforchetti
 */

#ifndef LIBS_SD_H_
#define LIBS_SD_H_


typedef struct ARCHIVO ARCHIVO;

struct ARCHIVO{

    FIL         LOG;
    FIL         logfile;
    FIL         *Archivo;
    UINT        bw;
    FATFS       g_sFatFs;
    FRESULT     iFResult;
    char        nombre[15];
    char        datos[100];

    };

ARCHIVO ARCHIVO_DATOS;


void reverse(char s[]);

void itoa(int n, char s[]);

bool escribeArchivo_s   (ARCHIVO *ARCHIVO)  ;

bool crearArchivo_s     (ARCHIVO *ARCHIVO)  ;

bool iniciaSD_s         (ARCHIVO *ARCHIVO)  ;


#endif /* LIBS_SD_H_ */
