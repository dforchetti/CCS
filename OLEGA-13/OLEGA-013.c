//*****************************************************************************
//
//  OLEGA-012.c
//
//  DFORCHETTI  10/01/2020
//
//
//*****************************************************************************

#include <dht22_mio.h>
#include "variables.h"
#include "mediciones.h"
#include "inicio.h"

volatile variables v_viento, lluvia, d_viento, humedad , humedad_dht22, temperatura, temperatura_dht22, gota, vBat,
        luz, vLipo;

extern volatile uint32_t DEBUG;   // codigo de error
extern volatile uint32_t CONFIG;   // codigo configuración

volatile DHT_TypeDef th;

ARCHIVO ARCHIVO_DATOS, ARCHIVO_LOG;

volatile uint32_t ui32Loop;

int modo = 1;
int contador = 0, contador2 =0;

volatile int32_t    aux3=0,aux4=0;

char MSG_AUX[400];



int main(void)
{
    SysTickbegin();
    inicio();


    blink(&ledRojo);
   // blink(&ledAzul);
  //  blink(&ledVerde);


    ROM_SysCtlDelay((ROM_SysCtlClockGet()*18)/3000);

    UARTprintf("%i",ROM_SysCtlClockGet());

    contador=0;


/*
    uint32_t  tiempo=0;
    milis=0;
    SysTickPeriodSet(0);


    WDT
    while(true){

        tiempo = SysTickPeriodGet();

    }
*/
    //para que falle el WDT

    while (1)
    {
        WDT
        /*      // muestra un "icono" de progreso
         UARTprintf("%c",icon[j++]);
         UARTprintf("\b");
         if (j >3) j= 0;
         */
        // controla el blink del led

        if (contador == 80)
            ledAzul(ON);
        if (contador == 100)
        {
            ledAzul(OFF);
            contador = 0;
        }

        contador++;

        aux3 = ROM_GPIOPinRead(GPIO_PORTE_BASE, AUX3);      // lee la entrada AUX3

        aux4 = ROM_GPIOPinRead(GPIO_PORTB_BASE, AUX4);      // lee la entrada AUX4

        if (!(aux3 && AUX3)) ledRojo(ON);   // ROM_SysCtlResetCauseGet();     // es activo por bajo
        else ledRojo(OFF);

          ROM_ADCProcessorTrigger(ADC1_BASE, 0);

        if (flagCONVERSION_1S)
        {

            WDT

            AVL();

            WDT

            LEE_DHT22

            WDT

            LEE_HUMEDAD_SHT21

            WDT

            LEE_TEMPERATURA_SHT21

            WDT

            calculaDireccionViento((variables *) &d_viento); // mide y calcula la dirección del viento

            flagCONVERSION_1S = FALSE;
        }
        /*
         ledAux3(ON);
*/
         if (flagCONVERSION_10S)
         {

         //calculaDireccionViento((variables *)&d_viento); // mide y calcula la dirección del viento

         flagCONVERSION_10S = FALSE;
         }



        ROM_SysCtlDelay(20000);
    }

}
