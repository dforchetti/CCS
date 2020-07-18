#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "ctype.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"


uint32_t contador=0;

char text[100];

void PrintString( uint32_t UARTX_BASE, volatile char *str);

void UARTIntHandler(void)
{
    uint32_t ui32Status;
    uint32_t letra;

    ui32Status = UARTIntStatus(UART0_BASE, true); //get interrupt status

    UARTIntClear(UART0_BASE, ui32Status); //clear the asserted interrupts

    while(UARTCharsAvail(UART0_BASE)) //loop while there are chars
    {
        letra=UARTCharGetNonBlocking(UART0_BASE);
        if(letra=='x'){
            PrintString(UART0_BASE,"menú \r\n");
        }
        UARTCharPutNonBlocking(UART0_BASE,toupper(letra)); //echo character
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2); //blink LED
        SysCtlDelay(SysCtlClockGet() / (1000 * 3)); //delay ~1 msec
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0); //turn off LED
    }
}

main(void) {

	SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); //enable GPIO port for LED
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2); //enable pin for LED PF2

    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    IntRegister(INT_UART0, UARTIntHandler);

    IntMasterEnable(); //enable processor interrupts
    IntEnable(INT_UART0); //enable the UART interrupt
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT); //only enable RX and TX interrupts

    PrintString(UART0_BASE,"prueba");

    while (1)
    {
        contador++;
        sprintf(text, "contador = %d\n\r", contador);
        PrintString(UART0_BASE,text);
        SysCtlDelay(SysCtlClockGet()/3);
    }

}

void PrintString( uint32_t UARTX_BASE, volatile char *str)
{
    uint32_t i=0;

     for( i=0 ; str[i]!='\0'; i++)
           {
               UARTCharPut(UARTX_BASE, str[i]);
           }

}




