/*
 * VelDirPluv.c
 *
 *  Created on: 28 ene. 2019
 *      Author: Daniel
 */

#include <stdbool.h>
#include <stdint.h>


#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "driverlib/gpio.h"

#include "driverlib/adc.h"
#include "AnemometroVeletaPluviometro.h"



#define VELDIRPLUV_GPIO_PERIPH     SYSCTL_PERIPH_GPIOD
#define VELDIRPLUV_GPIO_BASE       GPIO_PORTD_BASE

#define NUM_ENTRADAS            3
#define ENTRADA_VEL             GPIO_PIN_3
#define ENTRADA_ANE             GPIO_PIN_2
#define ENTRADA_PLUV            GPIO_PIN_1

#define NORTE       3140
#define NOR_ESTE    1840
#define ESTE        375
#define SUR_ESTE    750
#define SUR         1146
#define SUR_OESTE   2500
#define OESTE       3700
#define NOR_OESTE   3550

#define DELTA       150

volatile uint32_t ui32ADC1Value[5];

volatile uint32_t EstadoEntradas = 0x00000000;//ENTRADA_ANE | ENTRADA_PLUV;

extern volatile uint32_t milis;

extern variables v_viento, lluvia, d_viento, humedad , humedad_dht22, temperatura, temperatura_dht22, gota, vBat,
luz, vLipo;





void ADC1InterruptHandler(void)
{
    //ledAux3(ON); //PE4

    ROM_ADCIntClear(ADC1_BASE, 0);

    ROM_ADCSequenceDataGet(ADC1_BASE, 0,(uint32_t *) ui32ADC1Value);

    // 24 microsegundos tarda el codigo hasta acá

    vBat.xn     =   (float) ( ui32ADC1Value[0]*0.003911*0.55)    ;   //  3.3/4096.0*4.855);
    gota.xn     =   (float) ( ui32ADC1Value[1]*3.3/4096.0)  ;
    luz.xn      =   (float) ( ui32ADC1Value[2]*3.3/4096.0)  ;
    vLipo.xn    =   (float) ((((float)ui32ADC1Value[3])-((float)ui32ADC1Value[4]))*0.001127); // si no se pone float delante de las variables medidas calcula mal la resta

    vBat.n++    ;
    gota.n++    ;
    luz.n++     ;
    vLipo.n++   ;

    SUM_MAX_MIN(&gota);
    SUM_MAX_MIN(&vBat);
    SUM_MAX_MIN(&luz);
    SUM_MAX_MIN(&vLipo);

    //ledAux3(OFF); //PE4
}




//*****************************************************************************
//
// The UART interrupt handler.
//
//*****************************************************************************

volatile uint32_t   t_actual=0, diff=0;

volatile uint32_t   cont_ane=0, cont_pluv=0;

volatile uint32_t   interrupt_status=0;

void
VelDirPluvIntHandler(void)
{
    //ledAux4(ON);
    /// acordate pedazo de pelotudo que la entrada es activa en bajo!!!

    EstadoEntradas = ROM_GPIOPinRead(VELDIRPLUV_GPIO_BASE, ENTRADA_ANE | ENTRADA_PLUV);     //  Lee el estado actual de las entradas

    interrupt_status = GPIOIntStatus(VELDIRPLUV_GPIO_BASE , ENTRADA_ANE | ENTRADA_PLUV);

    GPIOIntClear(VELDIRPLUV_GPIO_BASE, ENTRADA_ANE | ENTRADA_PLUV);                         //  Borra los flags de interrupcion de las entradas


    t_actual = milis;                                                                   //  Guarda el valor de la variable temporal

    if (interrupt_status == ENTRADA_ANE) //(EstadoEntradas & ENTRADA_ANE) == 0)
    {
        cont_ane++;

        diff = t_actual - v_viento.t_anterior;              // mide el tiempo entre un pulso y otro

        if (( diff < 1000) & (diff!=0))                     // 1000 ms = 1Hz->2.4 km/h
        {
            v_viento.xn = 2400.0 / diff;                    // Calcula la velocidad de viento como: Velocidad= K/DTiempo ..... 2400/1000 = 2.4 km/h
        }

        else  v_viento.xn = 0;                                                    // Si el tiempo enre pulsos es mayor a 1000 se devuelve 0 de velocidad.


        v_viento.n++ ;                                                          // incrementa el contador de datos

        v_viento.t_anterior =   t_actual  ;                                     // guarda registro del tiempo actual

        SUM_MAX_MIN(&v_viento);
    }


    if (interrupt_status==ENTRADA_PLUV)//(EstadoEntradas & ENTRADA_PLUV) == 0)
    {
/*
        diff = t_actual - lluvia.t_anterior;

        cont_pluv++;

        if (diff!=0)
        {

            lluvia.xn = (float)1.0/(t_actual-lluvia.t_anterior);                             // mm/seg

            lluvia.n++;                                                             // incrementa el indice del dato (numero de datos usados para los calculos)

            SUM_MAX_MIN(&lluvia);

        }

        lluvia.t_anterior = t_actual;
        */

        lluvia.n++;
        lluvia.n=lluvia.n % 1000;       // cuenta hasta 1000 vuelcos del pluviometro
        lluvia.xn = lluvia.n;
    }

	EstadoEntradas = 0xFFFFFFFF;       // Resetea el estado de las entradas digitales a 1 (son activas en bajo)

   // ledAux4(OFF);
}

//*****************************************************************************
//
// Define los pines necesarios para la lectura de pluviometro anemometro y veleta
//
//*****************************************************************************

int contador_inicia=0;

void
VelDirPluvInicio(void)
{
#ifdef VERBOSE
    UARTprintf(" -- inicia VelDirPLuv    --\n");
#endif

    /// Enable the GPIO port to which the signals are connected

    ROM_SysCtlPeripheralEnable(VELDIRPLUV_GPIO_PERIPH);

    while(!ROM_SysCtlPeripheralReady(VELDIRPLUV_GPIO_PERIPH))
    {
    }

    /// registro el handler asociado a las interrupciones de los GPIO

    GPIOIntRegister(VELDIRPLUV_GPIO_BASE,VelDirPluvIntHandler);

    ROM_GPIOIntTypeSet(VELDIRPLUV_GPIO_BASE, ENTRADA_ANE | ENTRADA_PLUV , GPIO_FALLING_EDGE );

    ROM_GPIODirModeSet(VELDIRPLUV_GPIO_BASE, ENTRADA_ANE | ENTRADA_PLUV, GPIO_DIR_MODE_HW ); // esta linea es fundamental

    ROM_GPIOPadConfigSet(VELDIRPLUV_GPIO_BASE, ENTRADA_ANE | ENTRADA_PLUV,GPIO_STRENGTH_12MA, GPIO_PIN_TYPE_OD) ;


    ROM_GPIOPinTypeGPIOInput(VELDIRPLUV_GPIO_BASE, ENTRADA_ANE | ENTRADA_PLUV );

    EstadoEntradas = ROM_GPIOPinRead(VELDIRPLUV_GPIO_BASE, ENTRADA_ANE | ENTRADA_PLUV );


    GPIOIntEnable(VELDIRPLUV_GPIO_BASE, ENTRADA_ANE | ENTRADA_PLUV );


    /// Conversion AD para la Veleta ADC0

    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

    //
    // Wait for the ADC4 module to be ready.
    //
    while(!ROM_SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0))  {  }


    // Enable the first sample sequencer to capture the value of channel 4 when the processor trigger occurs.

    ROM_ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);

    ROM_ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_IE | ADC_CTL_END | ADC_CTL_CH4);    // ADC_CTL_CH4 es el pin PD7

    ROM_ADCSequenceEnable(ADC0_BASE, 1);


    /// Conversion ADC1

    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC1);
    //
    // Wait for the ADC4 module to be ready.
    //
    while(!ROM_SysCtlPeripheralReady(SYSCTL_PERIPH_ADC1))
    {
    }

    ///

    ROM_ADCSequenceConfigure(ADC1_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);

    ROM_ADCSequenceStepConfigure(ADC1_BASE, 0, 0, ADC_CTL_CH0);                                 // ADC_CTL_CH0 es el pin PE3    //  VBAT
    ROM_ADCSequenceStepConfigure(ADC1_BASE, 0, 1, ADC_CTL_CH1);                                 // ADC_CTL_CH1 es el pin PE2    //  LLUVIA
    ROM_ADCSequenceStepConfigure(ADC1_BASE, 0, 2, ADC_CTL_CH2);                                 // ADC_CTL_CH2 es el pin PE1    //  LUZ LDR
    ROM_ADCSequenceStepConfigure(ADC1_BASE, 0, 3, ADC_CTL_CH7);                                 // ADC_CTL_CH2 es el pin PD0    //  VBAT+   (AUX1)
    ROM_ADCSequenceStepConfigure(ADC1_BASE, 0, 4, ADC_CTL_CH8 | ADC_CTL_IE | ADC_CTL_END);      // ADC_CTL_CH2 es el pin PE5    //  VBAT-   (AUX2)

    ROM_ADCSequenceEnable(ADC1_BASE, 0);

    ADCIntRegister(ADC1_BASE, 0, ADC1InterruptHandler);

    ROM_ADCIntEnable(ADC1_BASE, 0);


}

//*****************************************************************************
//
// Calcula la direccion
//
//*****************************************************************************

volatile uint32_t veleta[1];

int direccion = -1;

void calculaDireccionViento(variables *DATOS)
{
    // Medición de la dirección de viento

    ROM_ADCIntClear(ADC0_BASE, 1);

    ROM_ADCProcessorTrigger(ADC0_BASE, 1);

    while (!ROM_ADCIntStatus(ADC0_BASE, 1, false)) { }

    ROM_ADCSequenceDataGet(ADC0_BASE, 1,(uint32_t *) veleta);

    if      (( veleta[0] >= ( NORTE      -DELTA )) & ( veleta[0] <= ( NORTE      +DELTA ))) direccion = 0;    // NORTE
    else if (( veleta[0] >= ( NOR_ESTE   -DELTA )) & ( veleta[0] <= ( NOR_ESTE   +DELTA ))) direccion = 1;    // NorEste
    else if (( veleta[0] >= ( ESTE       -DELTA )) & ( veleta[0] <= ( ESTE       +DELTA ))) direccion = 2;    // ESTE
    else if (( veleta[0] >= ( SUR_ESTE   -DELTA )) & ( veleta[0] <= ( SUR_ESTE   +DELTA ))) direccion = 3;    // SurEste
    else if (( veleta[0] >= ( SUR        -DELTA )) & ( veleta[0] <= ( SUR        +DELTA ))) direccion = 4;    // SUR
    else if (( veleta[0] >= ( SUR_OESTE  -DELTA )) & ( veleta[0] <= ( SUR_OESTE  +DELTA ))) direccion = 5;    // SurOeste
    else if (( veleta[0] >= ( OESTE      -DELTA )) & ( veleta[0] <= ( OESTE      +DELTA ))) direccion = 6;    // OESTE
    else if (( veleta[0] >= ( NOR_OESTE  -DELTA )) & ( veleta[0] <= ( NOR_OESTE  +DELTA ))) direccion = 7;    // NorOeste
    else direccion = -1;


    DATOS->xn   =   (float)direccion   ;                                   // Guarda el ultimo valor de direccion medida

    if (direccion!=-1)                                                  // descarta los datos donde la medicion de viento fue errada
    {
        DATOS->n++;                                                     // Incrementa el numero de datos usados

        SUM_MAX_MIN(DATOS);
    }


}


