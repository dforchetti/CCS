/*
===============================================================================
 Name        : main.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/


#include <stdint.h>
#include <stdbool.h>
#include "driverlib/rom.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_timer.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/watchdog.h"
#include "driverlib/sysctl.h"
#include "Timeout.h"


#ifndef __DHT_H
#define __DHT_H

// default is DHT11, comment this define for DHT22 = RHT03
 //#define DHT11

#define DHT_PORT_PERIPH SYSCTL_PERIPH_GPIOE
#define DHT_PORT        GPIO_PORTE_BASE
#define DHT_PIN         GPIO_PIN_0
//#define DHT_PIN GPIO_PIN_5

#define DHT_TIM_PERIPH	SYSCTL_PERIPH_TIMER5
#define DHT_TIM_BASE	TIMER5_BASE
#define DHT_TIM			TIMER_A

#define MCU_CLOCK       ROM_SysCtlClockGet()
#define DHT_WAIT_18ms   ((MCU_CLOCK*18)/3000)
#define DHT_WAIT_20us   ((MCU_CLOCK*2)/300000-70) // -70 sirve para aproximarse mas a los 20 useg

#define DHT_TIMEOUT     ((MCU_CLOCK*9)/100000) // 90us
#define DHT_TIME_BIT_1  ((MCU_CLOCK*7)/100000) // 70us

#define DON()		    ROM_GPIOPinTypeGPIOOutput(DHT_PORT, DHT_PIN)
#define DIN()		    ROM_GPIOPinTypeGPIOInput(DHT_PORT, DHT_PIN)

#define DHT_DATA(__DATA__)	ROM_GPIOPinWrite(DHT_PORT, DHT_PIN, (__DATA__))
#define DHT_READ	        ROM_GPIOPinRead(DHT_PORT, DHT_PIN)


typedef struct {
    float celsius_temp;
    float humidity;
} DHT_TypeDef;


// Interface
void dht_init(void);

uint8_t dht_readTH(DHT_TypeDef *);
void    DHTIntHandler(void);
void    Timer5IntHandler(void);

#endif

