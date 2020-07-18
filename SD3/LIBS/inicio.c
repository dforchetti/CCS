/*
 * inicio.c
 *
 *  Created on: 26 mar. 2019
 *      Author: dforchetti
 */
#include "stdint.h"
#include <dht22_mio.h>
#include "inicio.h"
#include "GPIOs.h"
#include "UART.h"
#include "AnemometroVeletaPluviometro.h"
#include "SDmio.h"


float       corriente        =   0,
            voltaje          =   0,
            potencia         =   0,
            ftemperatura     =   0,
            fhumedad         =   0,
            presion          =   0;
uint32_t    cont             =   0,
            err              =   0,
            estado           =   0;
uint8_t     i                =   0;

bool        FLAG_dato_nuevo  =   false;

char MSG[100],aux[100],ENTRADA[40];

char datos[100];



void float_to_char(float f,char MSG[]){

    char aux[10];

    itoa((int32_t) (f * 1.00f), MSG);
    strcat(MSG,".");
    itoa((int32_t) (f * 100.0f)%100, aux);
    strcat(MSG, aux);
}


void inicio(void)
{

    ROM_FPUEnable();

    ROM_FPULazyStackingEnable();

    ROM_SysCtlClockSet( SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    ConfigureUART();

    UARTprintf("\n\r -- OLEGA-010 --\n\r");

  //  UARTprintf("\033[2J"); // borra la pantalla \n nueva linea \r inicio de linea

    configuraGPIO();

    ledAzul(OFF);

    ledVerde(OFF);

    ledRojo(OFF);


    dht_init()      ;
    th.celsius_temp= 0;
    th.humidity = 0;
    SysCtlDelay((SysCtlClockGet()/3)*2);



    iniciaAVL_UART();

 //   iniciaAVL();

    IniciaVariable(&v_viento);
    IniciaVariable(&lluvia);
    IniciaVariable(&d_viento);
    IniciaVariable(&humedad);
    IniciaVariable(&temperatura);
    IniciaVariable(&humedad_dht22);
    IniciaVariable(&temperatura_dht22);
    IniciaVariable(&gota);
    IniciaVariable(&vBat);
    IniciaVariable(&luz);
    IniciaVariable(&vLipo);

    ROM_IntMasterEnable();

    ROM_IntEnable(INT_UART1);

    WDT

    VelDirPluvInicio();

    strcpy(ARCHIVO_DATOS.nombre, "EME");

    iniciaSD_s( &ARCHIVO_DATOS );

    strcpy(ARCHIVO_LOG.nombre, "LOG"); //ARCHIVO para guardar los eventos de error

    iniciaSD_s(&ARCHIVO_LOG);





}

void inicia_WDT(void) {

    // Enable the peripherals used by this example.
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_WDOG0);

    ROM_SysCtlDelay(10); // este tiempo es fundamental para que funcionen las dos instrucciones siguiente....

    // Check to see if the registers are locked, and if so, unlock them.
    if (ROM_WatchdogLockState(WATCHDOG0_BASE))
    {
        ROM_WatchdogUnlock(WATCHDOG0_BASE);
    }

    // Set the period of the watchdog timer.
    ROM_WatchdogReloadSet(WATCHDOG0_BASE, WDT_TIMEOUT); //ROM_SysCtlClockGet());

    // Enable reset generation from the watchdog timer.
    ROM_WatchdogResetEnable(WATCHDOG0_BASE);

    WatchdogIntRegister(WATCHDOG0_BASE, WatchdogIntHandler);

    ROM_WatchdogIntClear(WATCHDOG0_BASE);

    ROM_SysCtlDelay(10);

    // Enable processor interrupts.
    ROM_IntMasterEnable();

    // Enable the watchdog interrupt.
    ROM_IntEnable(INT_WATCHDOG);

    // Enable the watchdog timer.
    ROM_WatchdogEnable(WATCHDOG0_BASE);


}


void WatchdogIntHandler(void)
{

    // If we have been told to stop feeding the watchdog, return immediately
    // without clearing the interrupt.  This will cause the system to reset
    // next time the watchdog interrupt fires.
    UARTprintf(" -- RESET WDT --\n");
    int i=0;
    for(i = 0 ; i<10 ; i++){
        ledRojo(ON);
        ROM_SysCtlDelay(100000);
        ledRojo(OFF);
        ROM_SysCtlDelay(100000);

    }


    // Clear the watchdog interrupt.
    //
  //  ROM_WatchdogIntClear(WATCHDOG0_BASE);

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

    extern void UARTStdioIntHandler(void);

    IntRegister(INT_UART0, UARTStdioIntHandler);

    UARTprintf("\033[2J"); // borra la pantalla \n nueva linea \r inicio de linea

    UARTprintf("\n\r -- SD --\n\r");
}

