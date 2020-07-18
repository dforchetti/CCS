//*****************************************************************************
//
//  DFORCHETTI  10/01/2020
//
//*****************************************************************************
#include "stdbool.h"
#include "stdint.h"
#include "string.h"

#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/fpu.h"
#include "fatfs/src/ff.h"
#include "fatfs/src/diskio.h"
//uart
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include "inc/hw_memmap.h"

#include "LIBS/uartstdio.h"
#include "LIBS/SDmio.h"
#include "driverlib/uart.h"
//uart


#include "LIBS/tick.h"

char MSG[100],aux[100];

uint32_t contador=0;

char datos[];

void InitConsole(void);

int main(void)
{
    SysTickbegin();

    ROM_FPUEnable();

    ROM_FPULazyStackingEnable();

    ROM_SysCtlClockSet( SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    InitConsole();

    strcpy(ARCHIVO_DATOS.nombre, "EME");

    iniciaSD_s( &ARCHIVO_DATOS );


    while (1)
    {
        contador++;

        itoa((int32_t) (contador ), MSG);

        UARTprintf("\n dato: %s  ",MSG);

        escribeArchivo_s(&ARCHIVO_DATOS);

        SysCtlDelay(SysCtlClockGet()/3);
    }

}


void InitConsole(void)
{

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);


    UARTStdioConfig(0, 115200, 16000000);//SysCtlClockGet());

    extern void UARTStdioIntHandler(void);

    IntRegister(INT_UART0, UARTStdioIntHandler);

    UARTprintf("\033[2J"); // borra la pantalla \n nueva linea \r inicio de linea

    UARTprintf("\n\r -- SD --\n\r");
}


