/*
 * GPIO.c
 *
 *  Created on: 12 feb. 2019
 *      Author: dforchetti
 */

#include <stdbool.h>
#include <stdint.h>

#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "inc/hw_nvic.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/gpio.h"
#include "GPIOs.h"




/*
 * Configuracion inicial de las señales digitales
 */

void ledAzul(int estado /*ON/OFF*/) //PC6
    {
    if (estado == ON)  ROM_GPIOPinWrite(GPIO_PORTC_BASE, LED_AZUL, LED_AZUL);
    if (estado == OFF) ROM_GPIOPinWrite(GPIO_PORTC_BASE, LED_AZUL, 0);
    }

void ledVerde(int estado /*ON/OFF*/)    //PC7
    {
    if (estado == ON)  ROM_GPIOPinWrite(GPIO_PORTC_BASE, LED_VERDE, LED_VERDE);
    if (estado == OFF) ROM_GPIOPinWrite(GPIO_PORTC_BASE, LED_VERDE, 0);
    }

void ledRojo(int estado /*ON/OFF*/) //PD7
    {
    if (estado == ON)  ROM_GPIOPinWrite(GPIO_PORTD_BASE, LED_ROJO, LED_ROJO);
    if (estado == OFF) ROM_GPIOPinWrite(GPIO_PORTD_BASE, LED_ROJO, 0);
    }

void ledAux3(int estado /*ON/OFF*/) //PE4
    {
    if (estado == ON)  ROM_GPIOPinWrite(GPIO_PORTE_BASE, AUX3, AUX3);
    if (estado == OFF) ROM_GPIOPinWrite(GPIO_PORTE_BASE, AUX3, 0);
    }

void ledAux4(int estado /*ON/OFF*/) //PE4
    {
    if (estado == ON)  ROM_GPIOPinWrite(GPIO_PORTB_BASE, AUX4, AUX4);
    if (estado == OFF) ROM_GPIOPinWrite(GPIO_PORTB_BASE, AUX4, 0);
    }



void AUX3IntHandler(void)
{

    //Resetea por API
    SysCtlReset();

    // resetea por registro

    HWREG(NVIC_APINT) = NVIC_APINT_VECTKEY | NVIC_APINT_SYSRESETREQ;
    //debería hacer saltar el WDT si falla lo demás
    while(true){

    }
}



void configuraGPIO(void)
{
    ///*************************************************************************************
    ///  Configura la salida LED_ROJO
    ///  la salida PD7 esta protegida porque comparte la funcion NMI
    ///
    ///*************************************************************************************


    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

    while(!ROM_SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOD))   {  }

    HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTD_BASE + GPIO_O_CR) |= LED_ROJO;                 //0x01;
    HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = 0;

    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, LED_ROJO)       ;
    ROM_GPIOPinWrite(GPIO_PORTD_BASE, LED_ROJO, 0);



    ///*************************************************************************************
    ///
    ///  Puerto C, pines PC7 y PC6  LED_VERDE y LED_AZUL
    ///
    ///*************************************************************************************

    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

    while(!ROM_SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOC))  {   }

    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, LED_VERDE | LED_AZUL )       ;

    ROM_GPIOPinWrite(GPIO_PORTC_BASE, LED_AZUL, 0);
    ROM_GPIOPinWrite(GPIO_PORTC_BASE, LED_VERDE, 0);



    ///*************************************************************************************
    ///
    ///  Puerto E, pines PE4 AUX3
    ///
    ///*************************************************************************************

    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

    while(!ROM_SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE))  {   }

    //  ROM_GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, AUX3 )       ;       // Configurar como salida

    ROM_GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, AUX3);

    ROM_GPIODirModeSet(GPIO_PORTE_BASE, AUX3, GPIO_DIR_MODE_HW );

    ROM_GPIOPadConfigSet(GPIO_PORTE_BASE,AUX3,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
    //ROM_GPIOPadConfigSet(VELDIRPLUV_GPIO_BASE, ENTRADA_ANE | ENTRADA_PLUV,GPIO_STRENGTH_12MA, GPIO_PIN_TYPE_OD) ;

    ROM_GPIOIntTypeSet(GPIO_PORTE_BASE,AUX3,GPIO_FALLING_EDGE);

    ROM_GPIOPinWrite(GPIO_PORTE_BASE,  AUX3 , 1);


        /// registro el handler asociado a las interrupciones de los GPIO

        GPIOIntRegister(GPIO_PORTE_BASE , AUX3IntHandler);

        GPIOIntEnable(GPIO_PORTE_BASE, AUX3 );

    ///*************************************************************************************
    ///
    ///  Puerto B, pines PB4 AUX4
    ///
    ///*************************************************************************************

    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    while(!ROM_SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB))  {   }

    //ROM_GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, AUX4 )       ;     // Configurar como salida

    ROM_GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, AUX4);

    ROM_GPIODirModeSet(GPIO_PORTB_BASE, AUX4, GPIO_DIR_MODE_HW );

    ROM_GPIOIntTypeSet(GPIO_PORTB_BASE,AUX4,GPIO_FALLING_EDGE);     //  Configura el pin como generador de interrupcion por flanco descendente

    ROM_GPIOPadConfigSet(GPIO_PORTB_BASE,AUX4,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);

    ROM_GPIOPinWrite(GPIO_PORTB_BASE,  AUX4 , 1);


}

void blink(void (*funcion)(int)){
    int i=0;
    int velocidad = 2;
    WDT
    for(i=1 ; i < 100; i=i+velocidad)
     {
        (*funcion)(ON);
        ROM_SysCtlDelay(i*1000);
        (*funcion)(OFF);
        ROM_SysCtlDelay((101-i)*1000);
      }
    for(i=100 ; i > 1; i=i-velocidad)
         {
            (*funcion)(ON);
            ROM_SysCtlDelay(i*1000);
            (*funcion)(OFF);
            ROM_SysCtlDelay((101-i)*1000);
          }

}

