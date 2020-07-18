/*
 * inicio.h
 *
 *  Created on: 26 mar. 2019
 *      Author: dforchetti
 */

#ifndef LIBS_INICIO_H_
#define LIBS_INICIO_H_

#include "variables.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "GPIOs.h"
#include "SHT21mio.h"
#include "tick.h"
#include "VelDirPluv.h"
#include "driverlib/watchdog.h"

#include <time.h>
#include "driverlib/hibernate.h"
#include "inc/hw_hibernate.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "eeprom.h"

#define PERIODO_RTC 10  //segundos

/// Códigos de Configuración del sistema
/// CONFIG = | SD | SHT21 | WATCHDOG | EEPROM | RTC | X | X | PLUV |

#define OFF false
#define ON  true


#define SD          OFF
#define SHT21       ON
#define WATCHDOG    ON
#define EEPROM      OFF
#define RTC         OFF
#define LIPO        OFF
#define DHT22       ON




/// VERBOSE se usa para modificar la salida por el terminal UART0 de depuracion

#define VERBOSE
//#undef VERBOSE

/// MACROS para activar el WDT


#define WDT_TIMEOUT     68000000//0x03FFFFFF//0x00000FFF //0x000FFFFF

#define WDT            if(CONFIG & CFG_WDT_ON) ROM_WatchdogReloadSet(WATCHDOG0_BASE, WDT_TIMEOUT);

//#define WDT             // sirve para hacer que falle el WDT

#define ERR_EEPROM     if(CONFIG & CFG_EEPROM_ON) ROM_EEPROMProgram((uint32_t *) &DEBUG, 0x400, sizeof(DEBUG)); // graba el debug actual

/// MACROS para medición de temperatura y humedad usando SHT21

#define INICIA_SHT21            if (CONFIG & CFG_SHT21_ON)      iniciaSHT21         ()                          ;
#define LEE_HUMEDAD_SHT21       if ((CONFIG & CFG_SHT21_ON)&&!(DEBUG & ERR_SHT21_ERROR))             \
                                                                leeHumedadSHT21     ((variables *)&humedad    ) ;
#define LEE_TEMPERATURA_SHT21   if ((CONFIG & CFG_SHT21_ON)&&!(DEBUG & ERR_SHT21_ERROR))             \
                                                                leeTemperaturaSHT21 ((variables *)&temperatura) ;
#define LEE_DHT22               if (CONFIG & CFG_DHT22_ON) {    dht_readTH(&th);                      \
                                                                temperatura_dht22.xn=th.celsius_temp; \
                                                                humedad_dht22.xn    =th.humidity;     \
                                                                temperatura_dht22.n++;                \
                                                                humedad_dht22.n++;                    \
                                                                SUM_MAX_MIN(&temperatura_dht22);      \
                                                                SUM_MAX_MIN(&humedad_dht22);          \
                                                                UARTprintf(" T= %d.%d H= %d.%d\r\n",  \
                                                                           (int)th.celsius_temp,      \
                                                                           ((int)th.celsius_temp)%10, \
                                                                           (int)th.humidity,          \
                                                                           ((int)th.humidity)%10);}

void inicia_WDT(void);
void inicia_RTC(void);
void WatchdogIntHandler(void);
void HibernateHandler(void);
void inicio(void);

#endif /* LIBS_INICIO_H_ */
