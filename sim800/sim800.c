//*****************************************************************************
//
//  DFORCHETTI  10/01/2020
//
//*****************************************************************************
#include "stdbool.h"
#include "stdint.h"
#include "string.h"
#include "ctype.h"

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


#include "LIBS/SDmio.h"
#include "driverlib/uart.h"
#include "LIBS/uartstdio.h"
//uart


#include "LIBS/tick.h"

extern void UARTStdioIntHandler(void);

bool FLAG_dato_nuevo  =   false;
char MSG[100],aux[100],ENTRADA[40];
uint8_t     i                =   0;
uint32_t contador=0;

char datos[];

void InitConsole(void);


/*
while(UARTCharsAvail(UART0_BASE)) //loop while there are chars
    {
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2); //blink LED

    letra = toupper(UARTCharGetNonBlocking(UART0_BASE));

    ENTRADA[i++]=letra;

    UARTCharPutNonBlocking(UART0_BASE,letra);

    SysCtlDelay(SysCtlClockGet() / (1000 * 3)); //delay ~1 msec

    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0); //turn off LED
    }
}
*/

#define CMD_BUF_SIZE            64

char g_pcCmdBuf[CMD_BUF_SIZE];

int32_t tecla;


int main(void)
{
    SysTickbegin();

    ROM_FPUEnable();

    ROM_FPULazyStackingEnable();

    ROM_SysCtlClockSet( SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    InitConsole();

    while (1)
    {
        contador++;

        UARTprintf("\ndato: %02d  ",contador);


        //
        // Get a line of text from the user.
        //
        if(UARTCharsAvail(UART0_BASE)){
                 tecla=UARTCharGetNonBlocking(UART0_BASE);//;UARTgetc();
        }


        if(tecla=='x')
        {
            UARTprintf("\n>>");
            UARTgets(g_pcCmdBuf, sizeof(g_pcCmdBuf));
            i=0;
            while(g_pcCmdBuf[i]!='\0')
            {
                g_pcCmdBuf[i]=toupper(g_pcCmdBuf[i]);
                i++;
            }
            UARTprintf("\n ..entrada: %s  ", g_pcCmdBuf);

        }





        SysCtlDelay(SysCtlClockGet()/3);

    }

    /*
        contador++;

        itoa((int32_t) (contador ), MSG);

        UARTprintf("\n dato: %s  ",MSG);

        // recibe datos del puerto serial
        if(UARTCharsAvail(UART0_BASE))
                {
                i = 0 ;
                while (UARTCharsAvail(UART0_BASE)) ENTRADA[i++] = toupper(UARTCharGet(UART0_BASE));
                UARTprintf ("\n\r entrada: %s \n\r", ENTRADA);
                FLAG_dato_nuevo = true;
                }
        if (FLAG_dato_nuevo){
            char *pointer;
            pointer = strstr(ENTRADA, "HOLA");
            if(pointer !=NULL) UARTprintf ("\n\r OK \n\r");
            FLAG_dato_nuevo =false;
        }

        SysCtlDelay(2*SysCtlClockGet()/3);
    }*/

}


void InitConsole(void)
{
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);

    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    ROM_UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    UARTStdioConfig(0, 115200,16000000);//SysCtlClockGet());

    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    extern void UARTStdioIntHandler(void);

    IntRegister(INT_UART0, UARTStdioIntHandler);



    UARTprintf("\033[2J"); // borra la pantalla \n nueva linea \r inicio de linea

    UARTprintf("\n\r -- SD --\n\r");
}






