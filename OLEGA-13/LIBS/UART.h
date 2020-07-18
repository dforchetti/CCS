/*
 * Inicia_UART.h
 *
 *  Created on: 09/08/2016
 *      Author: dforchetti
 */


#ifndef _Inicia_UART_H_
#define _Inicia_UART_H_


#include <stdbool.h>
#include <stdint.h>
#include "utils/uartstdio.h"
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
//#include "utils/uartstdio.h"


#define UART0_BAUDRATE  115200
#define UART1_BAUDRATE  9600
#define UART2_BAUDRATE  9600
#define UART3_BAUDRATE  9600



//#define CABECERA "GTDAT "
#define CABECERA "(AVL)"



// Funciones Definidas

void PrintString( uint32_t UARTX_BASE, volatile char *str);
void PrintAVL(char *str);
uint8_t InicializaUART( uint32_t UARTX_BASE );
void UARTSend(const uint8_t *pui8Buffer, uint32_t ui32Count);
void IniciaCONSOLA(void);
uint8_t IniciaUART( uint32_t UARTX_BASE );
void ConfigureUART(void);
void iniciaAVL_UART(void);


#endif

