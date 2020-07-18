/*
 * TIVA-OLEGA-008.H
 *
 *  Created on: 25 mar. 2019
 *      Author: dforchetti
 */

#ifndef VARIABLES_H_
#define VARIABLES_H_

#include "stdint.h"
#include "fatfs/src/ff.h"
#include "fatfs/src/diskio.h"
#include "AVL.h"
#include "sd_fs.h"
#include "string.h"


/// Codigos de Error de DEBUG
///
/// DEBUG = |SD|SHT21|WDT|EEPROM|RTC|X|X|PLUV|
///
///     {0}     :   Significa que NO HAY ERROR.
///     {1..F}  :   Significa algun tipo de error.
///

#define ERR_NO_ERROR                0x00000000  // No hay Error en la SD

#define ERR_NO_SD_ERROR             0x0FFFFFFF  // No hay Error en la SD
#define ERR_NO_SD                   0x10000000  // tarjeta deshabilitada
#define ERR_SD_MOUNT                0x20000000  // no pudo montarse la tarjeta SD
#define ERR_SD_CREA_ARCHIVO         0x40000000  // no pudo montarse la tarjeta SD
#define ERR_SD_ESCRITURA            0x80000000  // no pudo escribirse el archivo SD

#define ERR_NO_SHT21_ERROR          0xF0FFFFFF  //
#define ERR_SHT21_DESHABILITADO     0x01000000  // no hay error en el sensor SHT21
#define ERR_SHT21_ERROR             0x02000000  // no pudo leerse el sensor SHT21

#define ERR_NO_WDT_ERROR            0xFF0FFFFF  // no hay error en el WDT
#define ERR_WDT_DESHABILITADO       0x00100000  // WDT Deshabilitado
#define ERR_WDT_DISPARADO           0x00200000  // WDT Diparado

#define ERR_NO_EEPROM_ERROR         0xFFF0FFFF  // no hay error en la EEPROM
#define ERR_EEPROM_DESHABILITADO    0x00010000  // EEPROM deshabilitada

#define ERR_NO_RTC_ERROR            0xFFFF0FFF  // no hay error en el RTC
#define ERR_RTC_DESHABILITADO       0x00001000  // el RTC está dehabilitado

#define ERR_NO_DHT22_ERROR          0xFFFFF0FF  // no hay error en el RTC
#define ERR_DHT22_DESHABILITADO     0x00000100  // el RTC está dehabilitado


#define ERR_PLUVIOMETRO_OK          0x00000000  // El pluviometro funciona correctamente
#define ERR_GOTA_NO_PLUVIOMETRO     0x00000001  // sensó la caida de gota pero el pluviometro no acusa datos nuevos


/// Códigos de Configuración del sistema
/// CONFIG = |SD|SHT21|WDT|EEPROM|RTC|X|X|PLUV|

#define CFG_TODO_ON                 0XFFFFFFFF  //  TODAS LAS FUNCIONES ENCENDIDAS
#define CFG_TODO_OFF                0X00000000  //  TODAS LAS FUNCIONES APAGADAS

#define CFG_SD_ON                   0x10000000  //  funciones de la SD      activadas
#define CFG_SD_OFF                  0x0FFFFFFF  //  funciones de la SD      apagadas
#define CFG_SHT21_ON                0x01000000  //  funciones de la SHT21   activadas
#define CFG_SHT21_OFF               0xF0FFFFFF  //  funciones de la SHT21   apagadas
#define CFG_WDT_ON                  0x00100000  //  funciones de la WDT     activadas
#define CFG_WDT_OFF                 0xFF0FFFFF  //  funciones de la WDT     apagadas
#define CFG_EEPROM_ON               0x00010000  //  funciones de la EEPROM  activadas
#define CFG_EEPROM_OFF              0xFFF0FFFF  //  funciones de la EEPROM  apagadas
#define CFG_RTC_ON                  0x00001000  //  funciones de la RTC     activadas
#define CFG_RTC_OFF                 0xFFFF0FFF  //  funciones de la RTC     apagadas
#define CFG_DHT22_ON                0x00000100  //  funciones de la DHT22   activadas
#define CFG_DHT22_OFF               0xFFFFF0FF  //  funciones de la DHT22   apagadas

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

#ifndef     reverse_
#define     reverse_
void reverse(char s[]);
//#endif

#ifndef     itoa_
#define     itoa_
void itoa(int n, char s[]);
#endif


#endif /* VARIABLES_H_ */
