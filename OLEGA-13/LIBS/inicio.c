/*
 * inicio.c
 *
 *  Created on: 26 mar. 2019
 *      Author: dforchetti
 */

#include <dht22_mio.h>
#include "inicio.h"
#include "GPIOs.h"

extern variables v_viento, lluvia, d_viento, humedad , humedad_dht22, temperatura, temperatura_dht22, gota, vBat,
luz, vLipo;

extern ARCHIVO ARCHIVO_DATOS, ARCHIVO_LOG;

volatile uint32_t DEBUG = ERR_NO_ERROR;   // codigo de error
volatile uint32_t CONFIG = CFG_TODO_OFF;   // codigo configuración

extern DHT_TypeDef th;

void inicio(void)
{

    ROM_FPUEnable();

    ROM_FPULazyStackingEnable();

    ROM_SysCtlClockSet( SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

   //ROM_SysCtlClockSet( SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);



    if ( WATCHDOG == ON)
        {
        CONFIG |= CFG_WDT_ON;
        DEBUG &= ERR_NO_WDT_ERROR ;
        }
    else
        {
        CONFIG &= CFG_WDT_OFF;
        DEBUG |= ERR_WDT_DESHABILITADO;
        }

    if (CONFIG & CFG_WDT_ON)            inicia_WDT()    ;


    /// configura el comportamiento en funcion de los macros definidos o
    /// en términos del codigo de DEBUG que se haya guardado en la EEPROM
    /// antes del reset previo
    ///
    /// Códigos de Configuración del sistema
    ///
    /// CONFIG = | SD | SHT21 | WATCHDOG | EEPROM | RTC | X | X | PLUV |

    if ( SD == ON){
        CONFIG |= CFG_SD_ON;
        DEBUG &= ERR_NO_SD_ERROR ;
    }
    else
        {
        CONFIG &= CFG_SD_OFF;
        DEBUG &= ERR_NO_SD_ERROR ;
        DEBUG |= ERR_NO_SD ;
        }


    if ( SHT21 == ON)
        {
        CONFIG |= CFG_SHT21_ON;
        DEBUG &= ERR_NO_SHT21_ERROR ;
        }
    else
        {
        CONFIG &= CFG_SHT21_OFF;
        DEBUG|=ERR_SHT21_DESHABILITADO;
        }



    if ( EEPROM == ON)
        {
        CONFIG |= CFG_EEPROM_ON;
        DEBUG &= ERR_NO_EEPROM_ERROR;
        }
    else
        {
        CONFIG &= CFG_EEPROM_OFF;
        DEBUG |= ERR_EEPROM_DESHABILITADO;
        }

    if ( RTC == ON)
        {
        CONFIG |= CFG_RTC_ON;
        DEBUG &= ERR_NO_RTC_ERROR;
        }
    else
        {
        CONFIG &= CFG_RTC_OFF;
        DEBUG |= ERR_RTC_DESHABILITADO;
        }

    if ( DHT22 == ON)
    {
        CONFIG |= CFG_DHT22_ON;
        DEBUG &= ERR_NO_DHT22_ERROR;
    }
    else
    {
        CONFIG &= CFG_DHT22_OFF;
        DEBUG |= ERR_DHT22_DESHABILITADO;
    }


    ConfigureUART();

    UARTprintf("\n\r -- OLEGA-010 --\n\r");

  //  UARTprintf("\033[2J"); // borra la pantalla \n nueva linea \r inicio de linea

    configuraGPIO();

    ledAzul(OFF);

    ledVerde(OFF);

    ledRojo(OFF);



    if (CONFIG & CFG_EEPROM_ON)         inicia_eeprom() ;

    if (CONFIG & CFG_RTC_ON)            inicia_RTC()    ;

    if (CONFIG & CFG_DHT22_ON)
        {

            dht_init()      ;
            th.celsius_temp= 0;
            th.humidity = 0;
            SysCtlDelay((SysCtlClockGet()/3)*2);
        }




    iniciaAVL_UART();

    iniciaAVL();

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

    /*    strcpy(HORA_TIVA.hora,__DATE__);
     strcat(datos," ; ");
     strcat(datos,__TIME__);
     strcat(datos, "\n\r");
     */

    if (CONFIG & CFG_SD_ON)
    {
    strcpy(ARCHIVO_DATOS.nombre, "EME");

    iniciaSD_s( &ARCHIVO_DATOS );

    strcpy(ARCHIVO_LOG.nombre, "LOG"); //ARCHIVO para guardar los eventos de error

    iniciaSD_s(&ARCHIVO_LOG);

    }


    if ((CONFIG & CFG_SHT21_ON) && !(DEBUG & ERR_SHT21_ERROR))      iniciaSHT21();





}

void inicia_WDT(void) {

#ifdef VERBOSE
    UARTprintf(" -- inicia_WDT    --\n");
#endif

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

void inicia_RTC(void)
{

#ifdef  VERBOSE

        UARTprintf(" -- inicia_RTC    --\n");

#endif

        ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_HIBERNATE);

        while (!ROM_SysCtlPeripheralReady(SYSCTL_PERIPH_HIBERNATE))
        {
        }

        ROM_HibernateEnableExpClk(SYSCTL_OSC_EXT32); //SysCtlClockGet());

        ROM_HibernateClockConfig(HIBERNATE_OSC_HIGHDRIVE);

        ROM_HibernateRTCSet(0);

        // Set the match 0 register for 30 seconds from now.
        HibernateRTCMatchSet(0, ROM_HibernateRTCGet() + PERIODO_RTC);
        // Set up interrupts on the Hibernation module to enable the RTC match
        // 0 interrupt. Clear all pending interrupts and register the
        // interrupt handler.
        HibernateIntRegister(HibernateHandler);

        ROM_HibernateIntClear(
                HIBERNATE_INT_PIN_WAKE | HIBERNATE_INT_LOW_BAT
                        | HIBERNATE_INT_RTC_MATCH_0);

        ROM_HibernateIntEnable(HIBERNATE_INT_RTC_MATCH_0);
        ROM_HibernateRTCEnable();
    }


void HibernateHandler(void)
{
    uint32_t ui32Status;

    // Get the interrupt status and clear any pending interrupts.
    ui32Status = ROM_HibernateIntStatus(1);

    ROM_HibernateIntClear(ui32Status);

    // Process the RTC match 0 interrupt.
    if (ui32Status & HIBERNATE_INT_RTC_MATCH_0)
    {
        //
        // RTC match 0 interrupt actions go here.
        //

        //      tiempo = HibernateRTCGet();

        //       actualizaTiempo( &Hora, PERIODO);

//        contador++;

    }

    HibernateRTCMatchSet(0, ROM_HibernateRTCGet() + PERIODO);
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

