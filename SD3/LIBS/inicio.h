/*
 * inicio.h
 *
 *  Created on: 26 mar. 2019
 *      Author: dforchetti
 */

#ifndef LIBS_INICIO_H_
#define LIBS_INICIO_H_

#include "stdbool.h"
#include "stdint.h"
#include "string.h"
#include "ctype.h"

#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/fpu.h"
#include "driverlib/watchdog.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/uart.h"

#include "fatfs/src/ff.h"
#include "fatfs/src/diskio.h"
//uart

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"

#include "uartstdio.h"
#include "variables.h"
#include "tick.h"
#include "variables.h"
#include "AnemometroVeletaPluviometro.h"
#include "SDmio.h"
#include "I2Cmio.h"
#include "BME280mio.h"
#include "BQ32000.h"
#include "INA226.h"
#include "dht22_mio.h"
#include "GPIOs.h"


extern float    corriente , voltaje , potencia , ftemperatura , fhumedad , presion ;
extern uint32_t cont , err , estado ;
extern uint8_t  i ;
extern variables v_viento, lluvia, d_viento, humedad , humedad_dht22, temperatura, temperatura_dht22, gota, vBat, luz, vLipo;
extern ARCHIVO ARCHIVO_DATOS, ARCHIVO_LOG;
extern  DHT_TypeDef th;
extern bool FLAG_dato_nuevo;

extern char MSG[100],aux[100],ENTRADA[40];;

extern char datos[100];


#define OFF false
#define ON  true

#define SD          OFF
#define SHT21       ON
#define WATCHDOG    ON
#define EEPROM      OFF
#define RTC         OFF
#define LIPO        OFF
#define DHT22       ON

/// MACROS para activar el WDT

#define WDT_TIMEOUT     68000000//0x03FFFFFF//0x00000FFF //0x000FFFFF

#define WDT            ROM_WatchdogReloadSet(WATCHDOG0_BASE, WDT_TIMEOUT);

//#define WDT             // sirve para hacer que falle el WDT


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
void WatchdogIntHandler(void);
void inicio(void);
void InitConsole(void);
void float_to_char(float f,char MSG[]);

#endif /* LIBS_INICIO_H_ */
