/*
 * BQ32000.c
 *
 *  Created on: 12 jul. 2020
 *      Author: daniel
 */
#include "BQ32000.h"
#define BCD2DEC(x) (x/16*10) + (x%16)

#define I2C_BASE        I2C1_BASE
#define read8(x)        I2C_GetReg_8(I2C_BASE, BQ32000_ADDRESS, x);
#define write8(x,y)     I2C_SetReg_8(I2C_BASE, BQ32000_ADDRESS, x , y);





/*
#ifdef ACTUALIZA_RTC
            UARTprintf("\n -.actualizando fecha y hora .-\n")
            minutos   =   I2C_SetReg_8(I2C1_BASE, BQ32000_ADDRESS, BQ32000_MINUTOS  ,BUILD_MIN  );
            horas     =   I2C_SetReg_8(I2C1_BASE, BQ32000_ADDRESS, BQ32000_HORAS    ,BUILD_HOUR );
            dia       =   I2C_SetReg_8(I2C1_BASE, BQ32000_ADDRESS, BQ32000_DIA      ,BUILD_DAY  );
            mes       =   I2C_SetReg_8(I2C1_BASE, BQ32000_ADDRESS, BQ32000_MES      ,BUILD_MONTH);
            year      =   I2C_SetReg_8(I2C1_BASE, BQ32000_ADDRESS, BQ32000_YEAR     ,0x20       );

#endif
*/

void actualiza_RTC(void){
    //UARTprintf("\n -.actualizando fecha y hora .-\n");

    HORA.segundos   =   write8(BQ32000_SEGUNDOS,HORA.segundos   );
    HORA.minutos    =   write8(BQ32000_MINUTOS ,HORA.minutos    );
    HORA.horas      =   write8(BQ32000_HORAS   ,HORA.horas      );
    FECHA.dia       =   write8(BQ32000_DIA     ,FECHA.dia       );
    FECHA.mes       =   write8(BQ32000_MES     ,FECHA.mes       );
    FECHA.year      =   write8(BQ32000_YEAR    ,FECHA.year      );

}
void lee_tiempo(void){
/*
    HORA.segundos   =   read8(BQ32000_SEGUNDOS);
    HORA.segundos   =   BCD2DEC(HORA.segundos);
    HORA.minutos    =   read8(BQ32000_MINUTOS );
    HORA.minutos    =   BCD2DEC(HORA.minutos);
    HORA.horas      =   read8(BQ32000_HORAS   );
    HORA.horas      =   BCD2DEC(HORA.horas );
    FECHA.dia       =   read8( BQ32000_DIA    );
    FECHA.dia       =   BCD2DEC(FECHA.dia);
    FECHA.mes       =   read8(BQ32000_MES     );
    FECHA.mes       =   BCD2DEC(FECHA.mes);
    FECHA.year      =   read8(BQ32000_YEAR    );
    FECHA.year      =   BCD2DEC(FECHA.year );
  */
    HORA.segundos  =  BCD2DEC( I2C_GetReg_8(I2C1_BASE, BQ32000_ADDRESS, BQ32000_SEGUNDOS ));
    HORA.minutos   =  BCD2DEC( I2C_GetReg_8(I2C1_BASE, BQ32000_ADDRESS, BQ32000_MINUTOS  ));
    HORA.horas     =  BCD2DEC( I2C_GetReg_8(I2C1_BASE, BQ32000_ADDRESS, BQ32000_HORAS    ));
    FECHA.dia      =  BCD2DEC( I2C_GetReg_8(I2C1_BASE, BQ32000_ADDRESS, BQ32000_DIA      ));
    FECHA.mes      =  BCD2DEC( I2C_GetReg_8(I2C1_BASE, BQ32000_ADDRESS, BQ32000_MES      ));
    FECHA.year     =  BCD2DEC( I2C_GetReg_8(I2C1_BASE, BQ32000_ADDRESS, BQ32000_YEAR     ));

}


void actualizaTiempo(int segundos)
{
    char    aux[4];

    HORA.segundos = HORA.segundos + segundos;

    if( HORA.segundos >= 60 )
    {
        HORA.segundos = HORA.segundos - 60 ;

        if (++HORA.minutos >= 60)
        {
            HORA.minutos = 0 ;

            if (++HORA.horas == 24)
                {
                HORA.horas = 0;
                }
        }
    }

    strcpy(HORA.text,"");
    if ( HORA.horas < 10 )   strcat(HORA.text,"0");

    itoa(HORA.horas, aux);
    strcat(HORA.text,aux);
    strcat(HORA.text,":");

    if( HORA.minutos < 10 )   strcat(HORA.text,"0") ;

    itoa(HORA.minutos, aux);
    strcat(HORA.text,aux);
    strcat(HORA.text,":");

    if ( HORA.segundos < 10 )   strcat(HORA.text,"0") ;
    itoa(HORA.segundos, aux);

    strcat(HORA.text,aux);
    strcat(HORA.text,"\0");

}

