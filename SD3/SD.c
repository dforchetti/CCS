//*****************************************************************************
//  DFORCHETTI  10/01/2020
//*****************************************************************************

#include "inicio.h"


int main(void)
{

   // inicio();

  //  SysTickbegin();

    InitConsole();

//    dht_init();

 //   strcpy(ARCHIVO_DATOS.nombre, "EME");

 //   iniciaSD_s( &ARCHIVO_DATOS );

  //  I2C_Init();

  //  Ina226_Init(50 /*Rshunt en mili ohms*/);

  //  BME280_Inicio();

  //  BM280_Convertir();


    while(1)
      {
          cont++;
          UARTprintf("dato: %02d\n",cont);

   /*       if(flagCONVERSION_1S)
          {
              flagCONVERSION_1S = false;

              CONVIERTE_ADC     // se usa para iniciar la conversion de los canales ADC.

              BM280_Convertir();

              fhumedad         = BME280_GetHum();
              ftemperatura     = BME280_GetTemp();
              presion         = BME280_GetPres();

              corriente       =   lee_corriente();
              voltaje         =   lee_voltaje();
              potencia        =   lee_potencia();

              lee_tiempo();

              dht_readTH(&th);

              UARTprintf("( %02d:%02d:%02d - %02d/%02d/20%02d ) ",HORA.horas,HORA.minutos,HORA.segundos,FECHA.dia,FECHA.mes,FECHA.year);

              float_to_char(fhumedad, MSG);
              UARTprintf ("humedad: %s ", MSG);

              float_to_char(ftemperatura, MSG);
              UARTprintf ("temperatura: %s ", MSG);

              float_to_char(presion, MSG);
              UARTprintf ("presion: %s ", MSG);

              float_to_char(corriente, MSG);
              UARTprintf ("corriente: %s ", MSG);

              float_to_char(voltaje, MSG);
              UARTprintf ("voltaje: %s ", MSG);

              float_to_char(potencia, MSG);
              UARTprintf ("potencia: %s ", MSG);

              UARTprintf ("\n");

             }
*/
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
      }

}






