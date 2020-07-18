/*
 * AVL.c
 *
 *  Created on: 4 mar. 2019
 *      Author: Daniel
 */

#include "AVL.h"
#include "SD.h"
#include <math.h>

//#define PRUEBA

#define umbral 100

volatile long contador_mensajes = 0;

#define N_CAMPOS    47

char *campo[N_CAMPOS];

char MSG[400];

char aux[10];
float media = 0, aux_f = 0, desviacion = 0;

extern struct hora HORA_TIVA;

extern volatile uint32_t DEBUG, CONFIG;

extern ARCHIVO ARCHIVO_DATOS, ARCHIVO_LOG;

extern variables v_viento, lluvia, d_viento, humedad , humedad_dht22, temperatura, temperatura_dht22, gota, vBat,
luz, vLipo;

struct mensaje
{
    char v_actual   [5];        //  Velocidad actual                    km/h
    char v_media    [5];        //  Velocidad media ultimo minuto       km/h
    char v_maxima   [5];        //  Velocidad máxima ultimo minuto      km/h
    char v_minima   [5];        //  Velocidad media ultima hora         km/h
    char v_desv     [5];        //  Velocidad máxima ultima hora        km/h
    //
    char d_actual   [5];        //  Dirección media                     N/NE/E/SE/S/SO/O/NO
    char d_media    [5];        //  Dirección media                     N/NE/E/SE/S/SO/O/NO
    char p_acumulado[5];        //  Pluviometro acumulado               mm
    //
    char t_actual   [5];        //  Temperatura                         ºC
    char t_media    [5];        //  Temperatura                         ºC
    char t_maxima   [5];        //  Temperatura                         ºC
    char t_minima   [5];        //  Temperatura                         ºC
    char t_desv     [5];        //  Temperatura                         ºC
    //
    char h_actual   [5];        //  Humedad                             %
    char h_media    [5];        //  Humedad                             %
    char h_maxima   [5];        //  Humedad                             %
    char h_minima   [5];        //  Humedad                             %
    char h_desv     [5];        //  Humedad                             %
    //
    //
    char t_actual22 [5];        //  Temperatura                         ºC
    char t_media22  [5];        //  Temperatura                         ºC
    char t_maxima22 [5];        //  Temperatura                         ºC
    char t_minima22 [5];        //  Temperatura                         ºC
    char t_desv22   [5];        //  Temperatura                         ºC
    //
    char h_actual22 [5];        //  Humedad                             %
    char h_media22  [5];        //  Humedad                             %
    char h_maxima22 [5];        //  Humedad                             %
    char h_minima22 [5];        //  Humedad                             %
    char h_desv22   [5];        //  Humedad                             %
    //
    char l_actual   [5];        //  Radiación lumínica                  lux
    char l_media    [5];        //  Radiación lumínica                  lux
    char l_maxima   [5];        //  Radiación lumínica                  lux
    char l_minima   [5];        //  Radiación lumínica                  lux
    char l_desv     [5];        //  Radiación lumínica                  lux
    //
    char gel_actual [5];        //  voltaje batería de GEL              V
    char gel_media  [5];        //  Radiación lumínica                  lux
    char gel_maxima [5];        //  Radiación lumínica                  lux
    char gel_minima [5];        //  Radiación lumínica                  lux
    char gel_desv   [5];        //  Radiación lumínica                  lux
    //
    char lipo_actual[5];        //  voltaje batería LIPO                V
    char lipo_media [5];        //  Radiación lumínica                  lux
    char lipo_maxima[5];        //  Radiación lumínica                  lux
    char lipo_minima[5];        //  Radiación lumínica                  lux
    char lipo_desv  [5];        //  Radiación lumínica                  lux
    //
    char fecha      [10];       //  fecha                               dd/mm/yyyy
    char hora       [9];        //  hora                                hh:mm:ss
    char lat_long   [23];       //  Latitud y Longitud                  ddmm.mmmmmm-ddmm.mmmmmm
    //
    char debug      [4];        //  codigo final de debug               default: FIN.
//
};

struct mensaje mensaje = { "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
                           "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
                           "", "", "", "", "", "", "", "", "" };

void AVL(void)
{
    WDT

    int i = 0;

    /*  AT+GTDAT=gv300,0,,Dato a enviar,0,,,,FFFF$    */

 //   UARTprintf("\n\r------------------------------------------------------");

     /// AJUSTA LOS DATOS DE v_viento
   //  ajusta_viento();

     /// AJUSTA LOS DATOS DE d_viento
   //  ajusta_direccion();

     /// AJUSTA LOS DATOS DE p_acumulado
   //  ajusta_lluvia();

    /// AJUSTA LOS DATOS DE temperatura
    ajusta_temperatura();

    /// AJUSTA LOS DATOS DE humedad
   ajusta_humedad();

    /// AJUSTA LOS DATOS DE temperatura
     ajusta_temperatura22();

    /// AJUSTA LOS DATOS DE humedad
    ajusta_humedad22();


     /// AJUSTA LOS DATOS DE luz
 //   ajusta_luz();

     /// AJUSTA LOS DATOS DE BATERIA DE GEL
  //   ajusta_gel();

    /// AJUSTA LOS DATOS DE BATERIA LIPO
  //  ajusta_lipo();

    /// AJUSTA LOS DATOS DE LA FECHA
    //ajusta_fecha();
    strcpy(mensaje.fecha,"10/06/20");//__DATE__);

    /// AJUSTA LOS DATOS DE LA HORA

    strcpy(mensaje.hora, HORA_TIVA.text);

    /// AJUSTA LOS DATOS DE LA LATITUD Y LONGITUD

  //  ajusta_lat_long();

    /// AJUSTA LOS DATOS DE ERRORES

    /*DETECCION ERROR PLUVIOMETRO*/

    WDT

    if (gota.n != 0)
    {
        /* si detecta caida de gota pero el pluviometro no marca nada puede ser porque está bloqueado */

        if ((gota.suma / gota.n > umbral) && (lluvia.suma == 0))
            DEBUG |= ERR_GOTA_NO_PLUVIOMETRO;
          //  ROM_EEPROMProgram((uint32_t *) &DEBUG, 0x400, sizeof(DEBUG)); // graba el debug actual
    }

    strcpy(mensaje.debug, "\0");

    itoa((int32_t) DEBUG, aux);

    strcpy(mensaje.debug, aux);

    ROM_SysCtlDelay(10000);

//    strcpy(mensaje.lat_long,"AAAA");

    strcpy(MSG, "\0");   // borra todo lo anterior

    strcpy(MSG, "AT+GTDAT=gv300,2,,METEO2;V003;");   // carga la cabecera del mensaje si o si...sinó no lo reenvia el AVL
    //strcpy(MSG, "METEO2;V002;");   // carga la cabecera del mensaje

    for (i = 0; i < N_CAMPOS - 1; i++)
    {

#ifndef PRUEBA
        strcat(MSG, campo[i]); // carga en el string MSG los valores dentro de la estructura
#else
        strcat(MSG, "0"); // carga en el string MSG los valores dentro de la estructura
#endif

        strcat(MSG, ";");
    }
//    strcat(MSG, campo[i++]); // carga en el string MSG los valores dentro de la estructura

    strcat(MSG, ",0,,,,FFFF$\n\r");      // carga la cola del mensaje

    //UARTprintf("\033[34;1m");           // cambia el color de la terminal a rojo

    ///    The first part before the semicolon represents the text style.
    ///        00=none, 01=bold, 04=underscore, 05=blink, 07=reverse, 08=concealed.
    ///
    ///    The second and third part are the colour and the background color:
    ///        30=black, 31=red, 32=green, 33=yellow, 34=blue, 35=magenta, 36=cyan, 37=white.
    ///
    ///     Each part can be omitted, assuming starting on the left. i.e. "01" means bold, "01;31"
    ///     means bold and red. And you would get your terminal to print in colour by
    ///     escaping the instruction with \33[ and ending it with an m. 33, or 1B in hexadecimal,
    ///     is the ASCII sign "ESCAPE" (a special character in the ASCII character set). Example:




  //  UARTprintf(MSG);                            // imprime el mensaje sobre la UART0 -> DEBUG


    if (flagENVIO){                             // el tiempo de envío t_ENVIO se define en

        ledRojo(OFF);
        ledVerde(ON);
        ledAzul(OFF);

        flagENVIO = FALSE;

        contador_mensajes++;

        PrintString(UART1_BASE, (char*) MSG);       // imprime el mensaje sobre  la UART1 -> AVL
        UARTprintf("\n\r  ENVIO UART1 \n\r ");

        ledRojo(OFF);
        ledVerde(OFF);
        ledAzul(OFF);


         }


    //UARTprintf("\033[30m");              // regresa el color de la terminal a negro

    //UARTprintf("  prueba ");

  //  UARTprintf("\n\r------------------------------------------------------");




if (SD == ON)
{
    ///escribe al archivo los datos transmitidos.

    strcpy(ARCHIVO_DATOS.datos, MSG);

    escribeArchivo_s(&ARCHIVO_DATOS);

    strcpy(ARCHIVO_LOG.datos, MSG);

    escribeArchivo_s(&ARCHIVO_LOG);

}
    // Reinicia las variables a cero

    IniciaVariable(&v_viento);
 //   IniciaVariable(&lluvia);
    IniciaVariable(&d_viento);
    IniciaVariable(&humedad);
    IniciaVariable(&temperatura);
    IniciaVariable(&humedad_dht22);
    IniciaVariable(&temperatura_dht22);
    IniciaVariable(&gota);
    IniciaVariable(&vBat);
    IniciaVariable(&luz);
    IniciaVariable(&vLipo);

    DEBUG &= 0xFF000000; //  resetea los errores del sistema salvo los dos MSB asociados a ls SD

};

void iniciaAVL(void)
{

#ifdef VERBOSE
    UARTprintf(" -- inicia AVL    --\n");
#endif
    /* apunto el puntero a strings a cada campo de la estructura  */

    campo[0]  = mensaje.v_actual;
    campo[1]  = mensaje.v_media;
    campo[2]  = mensaje.v_maxima;
    campo[3]  = mensaje.v_minima;
    campo[4]  = mensaje.v_desv;
    //
    campo[5]  = mensaje.d_actual;
    campo[6]  = mensaje.d_media;
    campo[7]  = mensaje.p_acumulado;
    //
    campo[8]  = mensaje.t_actual;
    campo[9]  = mensaje.t_media;
    campo[10] = mensaje.t_maxima;
    campo[11] = mensaje.t_minima;
    campo[12] = mensaje.t_desv;
    //
    campo[13] = mensaje.h_actual;
    campo[14] = mensaje.h_media;
    campo[15] = mensaje.h_maxima;
    campo[16] = mensaje.h_minima;
    campo[17] = mensaje.h_desv;
    //
    campo[18]  = mensaje.t_actual22;
    campo[19]  = mensaje.t_media22;
    campo[20] = mensaje.t_maxima22;
    campo[21] = mensaje.t_minima22;
    campo[22] = mensaje.t_desv22;
    //
    campo[23] = mensaje.h_actual22;
    campo[24] = mensaje.h_media22;
    campo[25] = mensaje.h_maxima22;
    campo[26] = mensaje.h_minima22;
    campo[27] = mensaje.h_desv22;
    //
    campo[28] = mensaje.l_actual;
    campo[29] = mensaje.l_media;
    campo[30] = mensaje.l_maxima;
    campo[31] = mensaje.l_minima;
    campo[32] = mensaje.l_desv;
    //
    campo[33] = mensaje.gel_actual;
    campo[34] = mensaje.gel_media;
    campo[35] = mensaje.gel_maxima;
    campo[36] = mensaje.gel_minima;
    campo[37] = mensaje.gel_desv;
    //
    campo[38] = mensaje.lipo_actual;
    campo[39] = mensaje.lipo_media;
    campo[40] = mensaje.lipo_maxima;
    campo[41] = mensaje.lipo_minima;
    campo[42] = mensaje.lipo_desv;
    //
    campo[43] = mensaje.fecha;
    campo[44] = mensaje.hora;
    campo[45] = mensaje.lat_long;
    //
    campo[46] = mensaje.debug;

    int i = 0;

    for (i = 0; i < N_CAMPOS; i++)
        itoa(0, campo[i]);

    IntRegister(INT_UART1, UART1IntHandler);

}

void UART1IntHandler(void)
{
    uint32_t ui32Status;

    // Get the interrrupt status.

    ui32Status = UARTIntStatus(UART1_BASE, true);

    // Clear the asserted interrupts.

    UARTIntClear(UART1_BASE, ui32Status);
    /*

     // Loop while there are characters in the receive FIFO.

     while(UARTCharsAvail(UART3_BASE))
     {
     // Read the next character from the UART and write it back to the UART.

     UARTCharPutNonBlocking(UART3_BASE,  UARTCharGetNonBlocking(UART3_BASE));
     }
     */

    UARTprintf("interrrupt");
}

void ajusta_viento(void)
{
    /// v_actualen km/h por 100..
    /// la velocidad de viento debe devolver valores entre 0 y 150 km/h
    ///
    float MIN = 0.0, ESCALA = 1.0, MAX = 150.0;   // 50 * 100

    strcpy(mensaje.v_actual, "");                    // resetea el dato

    if (v_viento.xn < MIN || v_viento.xn > MAX)
    {
        strcpy(mensaje.v_actual, "-111\0"); // código de error en la medición de viento
    }
    else
    {
        itoa((int32_t) (v_viento.xn * 100.00f * ESCALA), aux);
        strcat(mensaje.v_actual, aux);
    }

    /// velocidad media
    /// la velocidad de viento debe devolver valores entre 0 y 50 m/s (180 km/h)

    media = v_viento.suma / v_viento.n;

    strcpy(mensaje.v_media, "");                    // resetea el dato

    if (media < MIN || media > MAX)
    {
        strcpy(mensaje.v_media, "-111\0"); // código de error en la medición de viento
    }
    else
    {
        itoa((int32_t) (media * 100.00f * ESCALA ), aux);
        strcat(mensaje.v_media, aux);
    }

    /// velocidad maxima
    /// la velocidad de viento debe devolver valores entre 0 y 50 m/s (180 km/h)

    strcpy(mensaje.v_maxima, "");                    // resetea el dato

    if (v_viento.max < MIN || v_viento.max > MAX)
    {
        strcpy(mensaje.v_maxima, "-111\0"); // código de error en la medición de viento
    }
    else
    {
        itoa((int32_t) (v_viento.max * 100.00f * ESCALA ), aux);
        strcat(mensaje.v_maxima, aux);
    }

    /// velocidad minima
    /// la velocidad de viento debe devolver valores entre 0 y 50 m/s (180 km/h)

    strcpy(mensaje.v_minima, "");                    // resetea el dato

    if (v_viento.min < MIN || v_viento.min > MAX)
    {
        strcpy(mensaje.v_minima, "-111\0"); // código de error en la medición de viento
    }

    else
    {
        itoa((int32_t) (v_viento.min * 100.00f * ESCALA ), aux);
        strcat(mensaje.v_minima, aux);
    }

    /// deviacion estandar
    /// la velocidad de viento debe devolver valores entre 0 y 50 m/s (180 km/h)

    desviacion =
            (v_viento.n > 0) ?
                    sqrt(v_viento.suma2
                            - v_viento.n * media * media / v_viento.n)
                            / v_viento.n :
                    0;

    strcpy(mensaje.v_desv, "");                    // resetea el dato

    if (desviacion < MIN || desviacion > MAX)
    {
        strcpy(mensaje.v_desv, "-111\0"); // código de error en la medición de viento
    }
    else
    {
        itoa((int32_t) (desviacion * 100.00f * ESCALA ), aux);
        strcat(mensaje.v_desv, aux);
    }
#ifdef VERBOSE
    UARTprintf("\n\r vVi n-(%i) med-(%s) max-(%s) min-(%s) des-(%s)",
               v_viento.n, mensaje.v_media, mensaje.v_maxima,
               mensaje.v_minima, mensaje.v_desv);
#endif


}

void ajusta_direccion(void)
{

    /// d_actual
    /// la dirección de viento debe devolver valores entre 0 y 7

    strcpy(mensaje.d_actual, "");                    // resetea el dato

    if (d_viento.xn < 0 || d_viento.xn > 7)
    {
        strcpy(mensaje.d_actual, "-111\0"); // código de error en la medición de viento
    }

    else
    {
        //itoa((int32_t) (d_viento.xn * 100.00f), aux);
        switch ((int)d_viento.xn)
        {
        case 0 : strcpy(mensaje.d_actual, "NN\0"); break;
        case 1 : strcpy(mensaje.d_actual, "NE\0"); break;
        case 2 : strcpy(mensaje.d_actual, "EE\0"); break;
        case 3 : strcpy(mensaje.d_actual, "SE\0"); break;
        case 4 : strcpy(mensaje.d_actual, "SS\0"); break;
        case 5 : strcpy(mensaje.d_actual, "SO\0"); break;
        case 6 : strcpy(mensaje.d_actual, "OO\0"); break;
        case 7 : strcpy(mensaje.d_actual, "NO\0"); break;
        }
    }

    /// d_media
    /// la dirección de viento debe devolver valores entre 0 y 7

    aux_f = d_viento.suma / d_viento.n;

    strcpy(mensaje.d_media, "");                    // resetea el dato

    if (aux_f < 0 || aux_f > 7)
    {
        strcpy(mensaje.d_media, "-111\0"); // código de error en la medición de viento
    }
    else
    {
        itoa((int32_t) (media * 100.00f), aux);
        strcat(mensaje.d_media, aux);
    }
#ifdef VERBOSE
        UARTprintf("\n\r dVi n-(%i) med-(%s) act-(%s) ", d_viento.n, mensaje.d_media, mensaje.d_actual);
#endif

}

void ajusta_lluvia(void)
{
    strcpy(mensaje.p_acumulado, "");                    // resetea el dato

   // aux_f = lluvia.xn * 0.2794;

    aux_f = lluvia.xn;// * 0.2794;

    if (aux_f < 0 || aux_f > 1000) // #gotas -> 0.2794 mm de lluvia  (20 mm / 5 min es una lluvia de 200 mm)
    {
        strcpy(mensaje.p_acumulado, "-111\0"); // código de error en la medición de viento
    }
    else
    {
        itoa((int32_t) (aux_f), aux);
        strcat(mensaje.p_acumulado, aux);
    }

#ifdef VERBOSE
         UARTprintf("\n\r llu n-(%i) med-(%s) ", lluvia.n, mensaje.p_acumulado);
#endif

}

void ajusta_temperatura(void)
{
    /// t_actual
    /// la temperatura devolver valores entre 0 y 50 ºC

    float MIN = 0.0, ESCALA = 1.0, MAX = 100.0;   // 50 * 100

    strcpy(mensaje.t_actual, "");                     // resetea el dato

    if (temperatura.xn < MIN || temperatura.xn > MAX)
    {
        strcpy(mensaje.t_actual, "-111\0");            // código de error
    }
    else
    {
        itoa((int32_t) (temperatura.xn * 100.00f * ESCALA), aux);
        strcat(mensaje.t_actual, aux);

    }

    /// t media
    /// la temperatura devolver valores entre 0 y 50 ºC

    media = temperatura.suma / temperatura.n;

    strcpy(mensaje.t_media, "");                      // resetea el dato
    if (media < MIN || media > MAX)
    {
        strcpy(mensaje.t_media, "-111\0");             // código de error
    }
    else
    {
        itoa((int32_t) (media * 100.00f*ESCALA), aux);
        strcat(mensaje.t_media, aux);
    }

    /// t maxima
    /// la temperatura devolver valores entre 0 y 50 ºC

    strcpy(mensaje.t_maxima, "");                    // resetea el dato

    if (temperatura.max < MIN || temperatura.max > MAX)
    {
        strcpy(mensaje.t_maxima, "-111\0"); // código de error en la medición de viento
    }
    else
    {
        itoa((int32_t) (temperatura.max * 100.00f * ESCALA), aux);
        strcat(mensaje.t_maxima, aux);
    }

    /// t minima
    /// la temperatura devolver valores entre 0 y 50 ºC

    strcpy(mensaje.t_minima, "");                     // resetea el dato

    if (temperatura.min < MIN || temperatura.min > MAX)
    {
        strcpy(mensaje.t_minima, "-111\0");            // código de error
    }
    else
    {
        itoa((int32_t) (temperatura.min * 100.00f * ESCALA ), aux);
        strcat(mensaje.t_minima, aux);
    }

    /// deviacion estandar
    /// la velocidad de viento debe devolver valores entre 0 y 50 m/s (180 km/h)

    desviacion =  (temperatura.n > 0) ?  sqrt(temperatura.suma2 - temperatura.n * media * media / temperatura.n) / temperatura.n : 0;

    strcpy(mensaje.t_desv, "");                    // resetea el dato

    if (desviacion < MIN || desviacion > MAX)
    {
        strcpy(mensaje.t_desv, "-111\0"); // código de error en la medición de viento
    }
    else
    {
        itoa((int32_t) (desviacion * 100.00f * ESCALA ), aux);
        strcat(mensaje.t_desv, aux);
    }

#ifdef VERBOSE

        UARTprintf("\n\r Tem n-(%i) med-(%s) max-(%s) min-(%s) des-(%s)",
                      temperatura.n, mensaje.t_media, mensaje.t_maxima,
                      mensaje.t_minima, mensaje.t_desv);
#endif

}

void ajusta_humedad(void)
{
    /// t_actual
    /// la humedad debe devolver valores entre 0->0% y 1.0 ->100%

    float MIN = 0.0, ESCALA = 100.0, MAX = 1.0;   // 50 * 100

    strcpy(mensaje.h_actual, "");                     // resetea el dato

    if (humedad.xn < MIN || humedad.xn > MAX)
    {
        strcpy(mensaje.h_actual, "-111\0");            // código de error
    }
    else
    {
        itoa((int32_t) (humedad.xn * 100.0 * ESCALA), aux);
        strcat(mensaje.h_actual, aux);
    }

    /// t media
    /// la humedad debe devolver valores entre 0 y 100 %

    media = (humedad.n > 0) ? humedad.suma / humedad.n : 0;

    strcpy(mensaje.h_media, "");                      // resetea el dato

    if (media < MIN || media > MAX)
    {
        strcpy(mensaje.h_media, "-111\0");             // código de error
    }

    else
    {
        itoa((int32_t) (media * 100.00f * ESCALA), aux);
        strcat(mensaje.h_media, aux);
    }

    /// t maxima
    /// la humedad debe devolver valores entre 0 y 100 %

//       humedad.max = 12.06;

    strcpy(mensaje.h_maxima, "");                    // resetea el dato

    if (humedad.max < MIN || humedad.max > MAX)
    {
        strcpy(mensaje.h_maxima, "-111\0"); // código de error en la medición de viento
    }
    else
    {
        itoa((int32_t) (humedad.max * 100.00f * ESCALA), aux);
        strcat(mensaje.h_maxima, aux);
    }

    /// t minima
    /// la humedad debe devolver valores entre 0 y 100 %

    strcpy(mensaje.h_minima, "");                     // resetea el dato

    if (humedad.min < MIN || humedad.min > MAX)
    {
        strcpy(mensaje.h_minima, "-111\0");            // código de error
    }
    else
    {
        itoa((int32_t) (humedad.min * 100.00f * ESCALA), aux);
        strcat(mensaje.h_minima, aux);
    }

    /// deviacion estandar
    /// la humedad debe devolver valores entre 0 y 100 %

    desviacion =
            (humedad.n > 0) ?
                    sqrt(humedad.suma2 - humedad.n * media * media / humedad.n)
                            / humedad.n :
                    0;

    strcpy(mensaje.h_desv, "");                    // resetea el dato

    if (desviacion < MIN || desviacion > MAX)
    {
        strcpy(mensaje.h_desv, "-111\0");          // código de error
    }
    else
    {
        itoa((int32_t) (desviacion * 100.00f * ESCALA), aux);
        strcat(mensaje.h_desv, aux);
    }

#ifdef VERBOSE
    UARTprintf("\n\r Hum n-(%i) med-(%s) max-(%s) min-(%s) des-(%s)",
                      humedad.n, mensaje.h_media, mensaje.h_maxima,
                      mensaje.h_minima, mensaje.h_desv);
#endif
}

void ajusta_temperatura22(void)
{
    /// t_actual
    /// la temperatura devolver valores entre 0 y 50 ºC

    float MIN = 0.0, ESCALA = 1.0, MAX = 70.0;   // 50 * 100

    strcpy(mensaje.t_actual22, "");                     // resetea el dato

    if (temperatura_dht22.xn < MIN || temperatura_dht22.xn > MAX)
    {
        strcpy(mensaje.t_actual22, "-111\0");            // código de error
    }
    else
    {
        itoa((int32_t) (temperatura_dht22.xn * 100.00f * ESCALA), aux);
        strcat(mensaje.t_actual22, aux);

    }

    /// t media
    /// la temperatura devolver valores entre 0 y 50 ºC

    media = temperatura_dht22.suma / temperatura_dht22.n;

    strcpy(mensaje.t_media22, "");                      // resetea el dato
    if (media < MIN || media > MAX)
    {
        strcpy(mensaje.t_media22, "-111\0");             // código de error
    }
    else
    {
        itoa((int32_t) (media * 100.00f*ESCALA), aux);
        strcat(mensaje.t_media22, aux);
    }

    /// t maxima
    /// la temperatura devolver valores entre 0 y 50 ºC

    strcpy(mensaje.t_maxima22, "");                    // resetea el dato

    if (temperatura_dht22.max < MIN || temperatura_dht22.max > MAX)
    {
        strcpy(mensaje.t_maxima22, "-111\0"); // código de error en la medición de viento
    }
    else
    {
        itoa((int32_t) (temperatura_dht22.max * 100.00f * ESCALA), aux);
        strcat(mensaje.t_maxima22, aux);
    }

    /// t minima
    /// la temperatura devolver valores entre 0 y 50 ºC

    strcpy(mensaje.t_minima22, "");                     // resetea el dato

    if (temperatura_dht22.min < MIN || temperatura_dht22.min > MAX)
    {
        strcpy(mensaje.t_minima22, "-111\0");            // código de error
    }
    else
    {
        itoa((int32_t) (temperatura_dht22.min * 100.00f * ESCALA ), aux);
        strcat(mensaje.t_minima22, aux);
    }

    /// deviacion estandar
    /// la velocidad de viento debe devolver valores entre 0 y 50 m/s (180 km/h)

    desviacion =  (temperatura_dht22.n > 0) ?  sqrt(temperatura_dht22.suma2 - temperatura_dht22.n * media * media / temperatura_dht22.n) / temperatura_dht22.n : 0;

    strcpy(mensaje.t_desv22, "");                    // resetea el dato

    if (desviacion < MIN || desviacion > MAX)
    {
        strcpy(mensaje.t_desv22, "-111\0"); // código de error en la medición de viento
    }
    else
    {
        itoa((int32_t) (desviacion * 100.00f * ESCALA ), aux);
        strcat(mensaje.t_desv22, aux);
    }

#ifdef VERBOSE

        UARTprintf("\n\r Tem22 n-(%i) med-(%s) max-(%s) min-(%s) des-(%s)",
                      temperatura_dht22.n, mensaje.t_media22, mensaje.t_maxima22,
                      mensaje.t_minima22, mensaje.t_desv22);
#endif

}

void ajusta_humedad22(void)
{
    /// t_actual
    /// la humedad debe devolver valores entre 0->0% y 1.0 ->100%

    float MIN = 0.0, ESCALA = 1.0, MAX = 99;   // 50 * 100

    strcpy(mensaje.h_actual22, "");                     // resetea el dato

    if (humedad_dht22.xn < MIN || humedad_dht22.xn > MAX)
    {
        strcpy(mensaje.h_actual22, "-111\0");            // código de error
    }
    else
    {
        itoa((int32_t) (humedad_dht22.xn * 100.0 * ESCALA), aux);
        strcat(mensaje.h_actual22, aux);
    }
    //strcat(mensaje.h_actual22, "humedad");
    /// t media
    /// la humedad debe devolver valores entre 0 y 100 %

    media = (humedad_dht22.n > 0) ? humedad_dht22.suma / humedad_dht22.n : 0;

    strcpy(mensaje.h_media22, "");                      // resetea el dato

    if (media < MIN || media > MAX)
    {
        strcpy(mensaje.h_media22, "-111\0");             // código de error
    }

    else
    {
        itoa((int32_t) (media * 100.00f * ESCALA), aux);
        strcat(mensaje.h_media22, aux);
    }

    /// t maxima
    /// la humedad debe devolver valores entre 0 y 100 %

//       humedad.max = 12.06;

    strcpy(mensaje.h_maxima22, "");                    // resetea el dato

    if (humedad_dht22.max < MIN || humedad_dht22.max > MAX)
    {
        strcpy(mensaje.h_maxima22, "-111\0"); // código de error en la medición de viento
    }
    else
    {
        itoa((int32_t) (humedad_dht22.max * 100.00f * ESCALA), aux);
        strcat(mensaje.h_maxima22, aux);
    }

    /// t minima
    /// la humedad debe devolver valores entre 0 y 100 %

    strcpy(mensaje.h_minima22, "");                     // resetea el dato

    if (humedad_dht22.min < MIN || humedad_dht22.min > MAX)
    {
        strcpy(mensaje.h_minima22, "-111\0");            // código de error
    }
    else
    {
        itoa((int32_t) (humedad_dht22.min * 100.00f * ESCALA), aux);
        strcat(mensaje.h_minima22, aux);
    }

    /// deviacion estandar
    /// la humedad debe devolver valores entre 0 y 100 %

    desviacion =
            (humedad_dht22.n > 0) ?
                    sqrt(humedad_dht22.suma2 - humedad_dht22.n * media * media / humedad_dht22.n)
                            / humedad_dht22.n :
                    0;

    strcpy(mensaje.h_desv22, "");                    // resetea el dato

    if (desviacion < MIN || desviacion > MAX)
    {
        strcpy(mensaje.h_desv22, "-111\0");          // código de error
    }
    else
    {
        itoa((int32_t) (desviacion * 100.00f * ESCALA), aux);
        strcat(mensaje.h_desv22, aux);
    }

#ifdef VERBOSE
    UARTprintf("\n\r Hum22 n-(%i) med-(%s) max-(%s) min-(%s) des-(%s)",
                      humedad_dht22.n, mensaje.h_media22, mensaje.h_maxima22,
                      mensaje.h_minima22, mensaje.h_desv22);
#endif
}

void ajusta_luz(void)
{
    /// l_actual
    /// la humedad debe devolver valores entre 0 y 100 %

    float MIN = 0.0, ESCALA = 1.0, MAX = 50.0;   // 50 * 100

    strcpy(mensaje.l_actual, "");                     // resetea el dato

    if (luz.xn < MIN || luz.xn > MAX)
    {
        strcpy(mensaje.v_actual, "-111\0");            // código de error
    }
    else
    {
        itoa((int32_t) (luz.xn * 100.00f * ESCALA ), aux);
        strcat(mensaje.l_actual, aux);
    }

    /// l_media
    /// la luz debe devolver valores entre 0 y 100 %

    media = luz.suma / luz.n;

    strcpy(mensaje.l_media, "");                      // resetea el dato

    if (media < MIN || media > MAX)
    {
        strcpy(mensaje.l_media, "-111\0");             // código de error
    }
    else
    {
        itoa((int32_t) (media * 100.00f * ESCALA ), aux);
        strcat(mensaje.l_media, aux);
    }

    /// l_maxima
    /// la luz debe devolver valores entre 0 y 100 %

    strcpy(mensaje.l_maxima, "");                    // resetea el dato

    if (luz.max < MIN || luz.max > MAX)
    {
        strcpy(mensaje.l_maxima, "-111\0"); // código de error en la medición de viento
    }
    else
    {
        itoa((int32_t) (luz.max * 100.00f * ESCALA ), aux);
        strcat(mensaje.l_maxima, aux);
    }

    /// l_minima
    /// la luz debe devolver valores entre 0 y 100 %

    strcpy(mensaje.l_minima, "");                     // resetea el dato

    if (luz.min < MIN || luz.min > MAX)
    {
        strcpy(mensaje.l_minima, "-111\0");            // código de error
    }
    else
    {
        itoa((int32_t) (luz.min * 100.00f * ESCALA ), aux);
        strcat(mensaje.l_minima, aux);
    }

    /// deviacion estandar
    /// la luz debe devolver valores entre 0 y 100 %

    desviacion =
            (luz.n > 0) ?
                    sqrt(luz.suma2 - luz.n * media * media / luz.n) / luz.n : 0;

    strcpy(mensaje.l_desv, "");                    // resetea el dato

    if (desviacion < MIN || desviacion > MAX)
    {
        strcpy(mensaje.l_desv, "-111\0");          // código de error
    }
    else
    {
        itoa((int32_t) (desviacion * 100.00f * ESCALA ), aux);
        strcat(mensaje.l_desv, aux);
    }

#ifdef VERBOSE

    UARTprintf("\n\r Luz n-(%i) med-(%s) max-(%s) min-(%s) des-(%s)",
                   luz.n, mensaje.l_media, mensaje.l_maxima,
                   mensaje.l_minima, mensaje.l_desv);
#endif
}

void ajusta_gel(void)
{
    /// gel_actual
    /// la voltage de la bateria debe devolver valores entre 0 y 100 %
    ///
    float MIN = 0.0, ESCALA = 1.0, MAX = 15.0;   // 50 * 100

    strcpy(mensaje.gel_actual, "");                     // resetea el dato

    if (vBat.xn < MIN || vBat.xn > MAX)
    {
        strcpy(mensaje.gel_actual, "-111\0");            // código de error
    }
    else
    {
        itoa((int32_t) (vBat.xn * 1000.00f * ESCALA ), aux);
        strcat(mensaje.gel_actual, aux);
    }

    /// gel_media
    /// la voltage de la bateria debe devolver valores entre 0 y 100 %

    media = vBat.suma / vBat.n;

    strcpy(mensaje.gel_media, "");                      // resetea el dato

    if (media < MIN || media > MAX)
    {
        strcpy(mensaje.gel_media, "-111\0");             // código de error
    }
    else
    {
        itoa((int32_t) (media * 1000.00f * ESCALA ), aux);
        strcat(mensaje.gel_media, aux);
    }

    /// gel_maxima
    /// la gel debe devolver valores entre 0 y 100 %

    strcpy(mensaje.gel_maxima, "");                    // resetea el dato

    if (vBat.max < MIN || vBat.max > MAX)
    {
        strcpy(mensaje.gel_maxima, "-111\0"); // código de error en la medición de viento
    }
    else
    {
        itoa((int32_t) (vBat.max * 1000.00f * ESCALA ), aux);
        strcat(mensaje.gel_maxima, aux);
    }

    /// gel_minima
    /// la gel debe devolver valores entre 0 y 100 %

    strcpy(mensaje.gel_minima, "");                     // resetea el dato

    if (vBat.min < MIN || vBat.min > MAX)
    {
        strcpy(mensaje.gel_minima, "-111\0");            // código de error
    }
    else
    {
        itoa((int32_t) (vBat.min * 1000.00f * ESCALA ), aux);
        strcat(mensaje.gel_minima, aux);
    }

    /// deviacion estandar
    /// la gel debe devolver valores entre 0 y 100 %

    desviacion =
            (vBat.n > 0) ?
                    sqrt(vBat.suma2 - vBat.n * media * media / vBat.n)
                            / vBat.n :
                    0;

    strcpy(mensaje.gel_desv, "");                    // resetea el dato

    if (desviacion < MIN || desviacion > MAX)
    {
        strcpy(mensaje.gel_desv, "-111\0");          // código de error
    }
    else
    {
        itoa((int32_t) (desviacion * 1000.00f * ESCALA ), aux);
        strcat(mensaje.gel_desv, aux);
    }

#ifdef VERBOSE
    UARTprintf("\n\r vBat n-(%i) med-(%s) max-(%s) min-(%s) des-(%s)",
               vBat.n, mensaje.gel_media, mensaje.gel_maxima,
               mensaje.gel_minima, mensaje.gel_desv);
#endif

}

void ajusta_lipo(void)
{

    /// lipo_actual
    /// el voltage de la bateria debe devolver valores entre 0 y 100 %

    if(LIPO==OFF)
    {
        strcpy(mensaje.lipo_actual  , "-111\0");
        strcpy(mensaje.lipo_media   , "-111\0");
        strcpy(mensaje.lipo_maxima  , "-111\0");
        strcpy(mensaje.lipo_minima  , "-111\0");
        strcpy(mensaje.lipo_desv    , "-111\0");
    }
    else{
    ///
    float MIN = -5.0, ESCALA = 1.0, MAX = 5.0;   // 50 * 100

    strcpy(mensaje.lipo_actual, "\0");                     // resetea el dato

    if (vLipo.xn < MIN || vLipo.xn > MAX)
    {
        strcpy(mensaje.lipo_actual, "-111\0");            // código de error
    }
    else
    {
        itoa((int32_t) (vLipo.xn * 1000.00f * ESCALA ), aux);
        strcat(mensaje.lipo_actual, aux);
    }

    /// lipo_media
    /// el voltage de la bateria debe devolver valores entre 0 y 100 %

    media = vLipo.suma / vLipo.n;

    strcpy(mensaje.lipo_media, "\0");                      // resetea el dato

    if (media < MIN || media > MAX)
    {
        strcpy(mensaje.lipo_media, "-111\0");             // código de error
    }
    else
    {
        itoa((int32_t) (media * 1000.00f * ESCALA ), aux);
        strcat(mensaje.lipo_media, aux);
    }

    /// lipo_maxima
    /// la batería de gel debe devolver valores entre 0 y 100 %

    strcpy(mensaje.lipo_maxima, "\0");                    // resetea el dato

    if (vLipo.max < MIN || vLipo.max > MAX)
    {
        strcpy(mensaje.lipo_maxima, "-111\0"); // código de error en la medición de viento
    }
    else
    {
        itoa((int32_t) (vLipo.max * 1000.00f * ESCALA), aux);
        strcat(mensaje.lipo_maxima, aux);
   }

    /// lipo_minima
    /// la batería de gel debe devolver valores entre 0 y 100 %

    strcpy(mensaje.lipo_minima, "\0");                     // resetea el dato

    if ( vLipo.min < MIN || vLipo.min > MAX)
    {
        strcpy(mensaje.lipo_minima, "-111\0");            // código de error
    }
    else
    {
        itoa((int32_t) (vLipo.min * 1000.00f * ESCALA ), aux);
        strcat(mensaje.lipo_minima, aux);
    }

    /// deviacion estandar
    /// la batería de gel debe devolver valores entre 0 y 100 %

    desviacion = (vLipo.n > 0) ? sqrt(vLipo.suma2 - vLipo.n * media * media / vLipo.n) / vLipo.n : 0;

    strcpy(mensaje.lipo_desv, "\0");                    // resetea el dato

    if ( desviacion < MIN || desviacion > MAX)
    {
        strcpy(mensaje.lipo_desv, "-111\0");          // código de error
    }
    else
    {
        itoa((int32_t) (desviacion * 1000.00f * ESCALA ), aux);
        strcat(mensaje.lipo_desv, aux);
    }
    }

#ifdef VERBOSE
    UARTprintf("\n\r LIPO n-(%i) med-(%s) max-(%s) min-(%s) des-(%s)",
               vLipo.n, mensaje.lipo_media, mensaje.lipo_maxima,
               mensaje.lipo_minima, mensaje.lipo_desv);
#endif

}

void ajusta_lat_long(void)
{
    //contador_mensajes++;

    strcpy(mensaje.lat_long, "\0");

    itoa((int32_t) contador_mensajes, aux);

    strcpy(mensaje.lat_long, aux);

}

