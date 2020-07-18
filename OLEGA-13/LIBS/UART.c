/*
 * Inicia_UART.c
 *
 *  Created on: 09/08/2016
 *      Author: dforchetti
 */

#include <UART.h>


#define UART_AVL            UART1_BASE
#define UART_AVL_BAUDRATE   9600


//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

extern void UARTStdioIntHandler(void);


void
UART_AVL_IntHandler(void)
{
    uint32_t ui32Status;

    //
    // Get the interrrupt status.
    //
    ui32Status = UARTIntStatus(UART1_BASE, true);

    //
    // Clear the asserted interrupts.
    //
    UARTIntClear(UART1_BASE, ui32Status);

    //
    // Loop while there are characters in the receive FIFO.
    //
    while(UARTCharsAvail(UART1_BASE))
    {
        //
        // Read the next character from the UART and write it back to the UART.
        //
        UARTCharPutNonBlocking(UART1_BASE, UARTCharGetNonBlocking(UART1_BASE));

    }
}




void iniciaAVL_UART(void)
{

#ifdef VERBOSE
    UARTprintf(" -- inicia AVL_UART    --\n");
#endif

    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

    GPIOPinConfigure(GPIO_PC4_U1RX);

    GPIOPinConfigure(GPIO_PC5_U1TX);

    GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5);


    //UARTClockSourceSet(UART1_BASE, UART_CLOCK_PIOSC);

    //UARTStdioConfig(1, 115200, SysCtlClockGet());


    UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(),UART_AVL_BAUDRATE,//115200,
                                (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                                        UART_CONFIG_PAR_NONE));

 //   IntRegister(INT_UART1, UART_AVL_IntHandler);
    IntRegister(INT_UART1, UARTStdioIntHandler);


}








void PrintString( uint32_t UARTX_BASE, volatile char *str)
{
	uint32_t i=0;

	 for( i=0 ; str[i]!='\0'; i++)
	       {
	    	   UARTCharPut(UARTX_BASE, str[i]);
	       }

}


void PrintAVL(char *str)
{

	PrintString(UART_AVL, (char*) "\n\r (AVL) ");
    PrintString(UART_AVL, (char*) str);
    PrintString(UART_AVL, "\n\r");

    //PrintString(UART1_BASE  , (char *) "\n\r (AVL) hola \n\r" ) ;


}




void UARTSend(const uint8_t *pui8Buffer, uint32_t ui32Count)
{
    //
    // Loop while there are more characters to send.
    //
    while(ui32Count--)
    {
        //
        // Write the next character to the UART.
        //
        UARTCharPutNonBlocking(UART1_BASE, *pui8Buffer++);
    }
}




uint8_t IniciaUART( uint32_t UARTX_BASE )
{

	if (UARTX_BASE==UART0_BASE)
	{
		SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

	    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

	    GPIOPinConfigure(GPIO_PA0_U0RX);

	    GPIOPinConfigure(GPIO_PA1_U0TX);

	    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	    UARTConfigSetExpClk( UART0_BASE ,
	                         SysCtlClockGet(),
	                         UART0_BAUDRATE,
	                         ( UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE)
	                       );
	}


	if (UARTX_BASE==UART1_BASE)
	{
	    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);

	    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

	    GPIOPinConfigure(GPIO_PC4_U1RX);

	    GPIOPinConfigure(GPIO_PC5_U1TX);

	    GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5);

	    UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), UART1_BAUDRATE,//115200,
	                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
	                                UART_CONFIG_PAR_NONE));
	}


	if (UARTX_BASE==UART2_BASE)
	    {
	        SysCtlPeripheralEnable(SYSCTL_PERIPH_UART2);

	        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

	        GPIOPinConfigure(GPIO_PD6_U2RX);

	        GPIOPinConfigure(GPIO_PD7_U2TX);

	        GPIOPinTypeUART(GPIO_PORTD_BASE, GPIO_PIN_6 | GPIO_PIN_7);

	        UARTConfigSetExpClk(UART2_BASE, SysCtlClockGet(), UART2_BAUDRATE,
	                            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
	                                    UART_CONFIG_PAR_NONE));
	    }


    if (UARTX_BASE==UART3_BASE)
    {
        SysCtlPeripheralEnable(SYSCTL_PERIPH_UART3);

        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

        GPIOPinConfigure(GPIO_PC6_U3RX);

        GPIOPinConfigure(GPIO_PC7_U3TX);

        GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_6 | GPIO_PIN_7);

        UARTConfigSetExpClk(UART3_BASE, SysCtlClockGet(), UART3_BAUDRATE,//115200,
                            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                                    UART_CONFIG_PAR_NONE));
    }

	return 0;
}



//*****************************************************************************
//
// subrutina especifica para UARTstdio, UARTStdioConfig se usa para configurar la librería
//
//
//*****************************************************************************



void ConfigureUART(void)
{
    //
    // Enable the GPIO Peripheral used by the UART.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    //
    // Enable UART0
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    //
    // Configure GPIO Pins for UART mode.
    //
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    //
    // Initialize the UART for console I/O.
    //

    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
    //
    // Initialize the UART for console I/O.
    //
    UARTStdioConfig(0, UART0_BAUDRATE, 16000000);//SysCtlClockGet());

    IntRegister(INT_UART0, UARTStdioIntHandler);

}



