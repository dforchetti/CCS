/*
 * SD.c
 *
 *  Created on: 28 feb. 2019
 *      Author: dforchetti
 */

#include <stdint.h>
#include <stdbool.h>
#include "driverlib/rom.h"
#include "fatfs/src/ff.h"
#include "string.h"
#include "utils/uartstdio.h"
#include "sd_fs.h"

#include "SDmio.h"

char texto[200];

char datos[200];

extern struct ARCHIVO ARCHIVO_DATOS;


void reverse(char s[])
{

    int i, j;
    char c;

    for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

void itoa(int n, char s[])
{
    int i, sign;

    if ((sign = n) < 0)  /* record sign */
        n = -n;          /* make n positive */
    i = 0;
    do {       /* generate digits in reverse order */
        s[i++] = n % 10 + '0';   /* get next digit */
    } while ((n /= 10) > 0);     /* delete it */
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    reverse(s);
}



bool iniciaSD_s(ARCHIVO *ARCHIVO){      // función con argumentos variables
    //
    // Mount the file system, using logical disk 0.
    //

    (*ARCHIVO).Archivo=&(*ARCHIVO).LOG;

    strcpy((*ARCHIVO).datos,"NADA");

    (*ARCHIVO).iFResult = f_mount(0, &(*ARCHIVO).g_sFatFs);

    if((*ARCHIVO).iFResult!= FR_OK){

        UARTprintf("f_mount error: %s\n", StringFromFResult((*ARCHIVO).iFResult));


        return false;
        }

    else{

        crearArchivo_s(ARCHIVO);

        strcpy(datos,__DATE__);
        strcat(datos," ; ");
        strcat(datos,__TIME__);
        strcat(datos, "\n\r");


        escribeArchivo_s(ARCHIVO);


        return true;

        }

}




bool escribeArchivo_s(ARCHIVO *ARCHIVO)
{

   (*ARCHIVO).iFResult = f_open((*ARCHIVO).Archivo, (*ARCHIVO).nombre, FA_WRITE | FA_WRITE);    //FA_OPEN_ALWAYS) ;

   f_lseek((*ARCHIVO).Archivo,  (*(*ARCHIVO).Archivo).fsize );                                  // Move to end of the file to append data

   strcpy(texto, "192.0.0.1 usuario [ 01/04/2019 :");
//   strcat(texto, HORA_TIVA.text);
   strcat(texto, "- 0700] ");
   strcat(texto, ARCHIVO->datos);
   strcat(texto, "\n\r");

   ARCHIVO->iFResult=f_write(ARCHIVO->Archivo, texto, strlen(texto), &ARCHIVO->bw );

   //(*ARCHIVO).iFResult=f_write((*ARCHIVO).Archivo, (*ARCHIVO).datos, strlen((*ARCHIVO).datos), &(*ARCHIVO).bw );

   f_close((*ARCHIVO).Archivo);

   if((*ARCHIVO).iFResult == FR_OK){

       UARTprintf("numero de datos escritos  %u\n\r",(*ARCHIVO).bw);
       return true;
   }

   else {

       UARTprintf("ERROR al escribir el archivo\n\r");
       return false;
   }

}


bool crearArchivo_s(ARCHIVO *ARCHIVO)
{

    unsigned int  indice = 0;

    char Sindice [10];
    char name_ [10];

    unsigned int intentos = 1;

    while (intentos <= 2) // intenta hasta diez veces abrir el archivo
    {

    strcpy(name_,(*ARCHIVO).nombre);

    itoa(indice,Sindice);

    if (indice<1000)   strcat(name_, "0");
    if (indice<100)    strcat(name_, "0");
    if (indice<10)     strcat(name_, "0");

    strcat(name_, Sindice);

    strcat(name_, ".TXT");

    (*ARCHIVO).iFResult = f_open((*ARCHIVO).Archivo, name_, FA_WRITE | FA_CREATE_NEW); //FA_CREATE_NEW FA_OPEN_ALWAYS) ; // Open file - If nonexistent, create

    if((*ARCHIVO).iFResult == FR_OK)
        {
            f_lseek((*ARCHIVO).Archivo,  (*(*ARCHIVO).Archivo).fsize );    /* Move to end of the file to append data */

            //f_write(fp, datos, strlen(datos), &bw);

            UARTprintf("Nombre: %s",name_);

            UARTprintf("  El archivo pesa: %u\n",(*(*ARCHIVO).Archivo).fsize);

            f_close((*ARCHIVO).Archivo);

            strcpy((*ARCHIVO).nombre,name_);   // devuelve el nombre del archivo

            break;      // si el archivo pudo abrirse sale del bucle while
        }
    else if ((*ARCHIVO).iFResult == FR_EXIST){

        indice++;

       // break;
    }
    else
        {
        UARTprintf("\n\r no se pudo abrir el archivo intento %u ",intentos);

        UARTprintf("  ERROR %u \n\r",(*ARCHIVO).iFResult);

        intentos++;

        ROM_SysCtlDelay(10000);

        }
    }

if ((*ARCHIVO).iFResult == FR_OK)
{




    return true;
}
else
    {




    return false;
    }

}

