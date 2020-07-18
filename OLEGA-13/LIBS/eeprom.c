/*
 * eeprom.c
 *
 *  Created on: 2 may. 2019
 *      Author: dforchetti
 */

#include "eeprom.h"

extern volatile uint32_t    DEBUG   ;   // codigo de error
extern volatile uint32_t    CONFIG  ;   // codigo configuración


uint32_t DEBUG_ACTUAL;
uint32_t DEBUG_ANTERIOR;


uint32_t causa_reset = 0 , err;

void inicia_eeprom(void)
{
    WDT

#ifdef VERBOSE
    UARTprintf(" -- inicia_EEPROM --\n");
#endif

    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_EEPROM0);

    err = ROM_EEPROMInit();

    while (err != EEPROM_INIT_OK)
    {
        err = ROM_EEPROMInit(); // EEPROM start
    }

    causa_reset = ROM_SysCtlResetCauseGet();

    switch (causa_reset)
    {
    case SYSCTL_CAUSE_HSRVREQ:
        UARTprintf("SYSCTL_CAUSE_HSRVREQ");
        break;
    case SYSCTL_CAUSE_HIB:
        UARTprintf("SYSCTL_CAUSE_HIB");
        break;
    case SYSCTL_CAUSE_WDOG1:
        UARTprintf("SYSCTL_CAUSE_HSRVREQ");
        break;
    case SYSCTL_CAUSE_SW:
        UARTprintf("SYSCTL_CAUSE_WDOG1");
        break;
    case SYSCTL_CAUSE_WDOG0:
        UARTprintf(" SYSCTL_CAUSE_WDOG0");
        break;
    case SYSCTL_CAUSE_BOR:
        UARTprintf("SYSCTL_CAUSE_BOR");
        break;
    case SYSCTL_CAUSE_POR:
        UARTprintf("SYSCTL_CAUSE_POR");
        break;
    case SYSCTL_CAUSE_EXT:
        UARTprintf("SYSCTL_CAUSE_EXT");
        break;
    default:
        UARTprintf("NO_RESET");
        break;
    }

    DEBUG_ANTERIOR = 0x00000000;    // resetea la variable para tener certeza de la lectura del nuevo DEBUG

    ROM_EEPROMRead(&DEBUG_ANTERIOR, 0x400, sizeof(DEBUG_ANTERIOR));

    UARTprintf("\n## DEBUG ANTERIOR %x", DEBUG_ANTERIOR);

    ROM_EEPROMProgram((uint32_t *) &DEBUG, 0x400, sizeof(DEBUG)); // graba el debug actual

    DEBUG = DEBUG_ANTERIOR;

    //ROM_SysCtlReset();

}



