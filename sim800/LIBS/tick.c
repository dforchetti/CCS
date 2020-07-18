/*
 * tick.c
 *
 *  Created on: 21 dic. 2018
 *      Author: dforchetti
 */


#include "tick.h"
#include <stdint.h>
#include "utils/uartstdio.h"

extern disk_timerproc();

volatile uint32_t
    diferencia          = 0 ,
    milis               = 0 ,
    milis2              = 0 ,
    tanterior_1S        = 0 ,
    tanterior_10S       = 0 ,
    tanterior_ENVIO     = 0 ,
    t_ENVIO             = 5 * 60 * 1000,    // minutos * segundos * milisegundos
    //t_ENVIO             =       10 * 1000,    // minutos * segundos * milisegundos
    contador_timeproc   = 0 ;               // contador necesario para el FatFs

volatile bool

    flagCONVERSION_1S   = FALSE ,           // flag 1 segundo
    flagCONVERSION_10S  = FALSE ,           // flag 10 segundos
    flagENVIO           = FALSE ;           // flag 5 minutos


void SycTickInt()
{
    int     t_extra     =   0 ;

    milis++;                                                // micros++ if period set to F_CPU/1000000

    diferencia = milis - tanterior_1S;		                // código para generar la base de tiempo de 1 segundo sin desfase
    
    if ( diferencia >= 1000)
    {
		t_extra = diferencia % 1000;	                    // calcula el exceso sobre los 1000 ms
        tanterior_1S = milis - t_extra;	                    // fija el el valor de timepo anterior para la proxima cuenta de 1 segundo
		flagCONVERSION_1S = TRUE;                           //	arranca una conversion si es necesario cada 1 segundo
    }

    diferencia = milis - tanterior_10S;

    if (diferencia >= 10000)
    {
        t_extra = diferencia % 10000;                       // calcula el exceso sobre los 1000 ms
        tanterior_10S = milis - t_extra;                    // fija el el valor de timepo anterior para la proxima cuenta de 1 segundo
        flagCONVERSION_10S = TRUE;                          // arranca una conversion si es necesario cada 1 segundo
    }

    diferencia = milis - tanterior_ENVIO;

    if (diferencia >= t_ENVIO)
    {
        t_extra = diferencia % t_ENVIO;                     // calcula el exceso sobre los 1000 ms
        tanterior_ENVIO = milis - t_extra;                  // fija el el valor de timepo anterior para la proxima cuenta de 1 segundo
        flagENVIO = TRUE;                                   // arranca una conversion si es necesario cada 1 segundo
    }


    /* Este código lo necesita la rutina del FatFs   */

    contador_timeproc++;
    if (contador_timeproc == 10){
        disk_timerproc();                                   // Configure SysTick for a 100Hz interrupt.
        contador_timeproc = 0;                              // The FatFs driver wants a 10 ms tick.
    }

}


void SysTickbegin()
{

    ROM_SysTickEnable();
    ROM_SysTickPeriodSet(SysCtlClockGet()/1000);            // for miliseconds, F_CPU/1000000 for microseconds
    SysTickIntRegister(SycTickInt);
    ROM_SysTickIntEnable();
}
