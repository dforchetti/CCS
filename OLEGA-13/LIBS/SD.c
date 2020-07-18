/*
 * SD.c
 *
 *  Created on: 28 feb. 2019
 *      Author: dforchetti
 */

#include "variables.h"
#include <stdint.h>
#include <stdbool.h>
#include "fatfs/src/ff.h"
#include "string.h"
#include "utils/uartstdio.h"

#include "SD.h"

extern volatile uint32_t    DEBUG   ;   // codigo de error
extern volatile uint32_t    CONFIG  ;   // codigo configuración


extern char datos[];

char texto[200];

extern struct hora HORA_TIVA;


bool iniciaSD_s(ARCHIVO *ARCHIVO){      // función con argumentos variables
    //
    // Mount the file system, using logical disk 0.
    //
#ifdef VERBOSE
    UARTprintf(" -- inicia iniciaSD_s    --\n");
#endif


    (*ARCHIVO).Archivo=&(*ARCHIVO).LOG;

    strcpy((*ARCHIVO).datos,"NADA");

    (*ARCHIVO).iFResult = f_mount(0, &(*ARCHIVO).g_sFatFs);

    if((*ARCHIVO).iFResult!= FR_OK){

        UARTprintf("f_mount error: %s\n", StringFromFResult((*ARCHIVO).iFResult));

        DEBUG |= ERR_SD_MOUNT ;

        if (CONFIG & CFG_EEPROM_ON) ROM_EEPROMProgram((uint32_t *) &DEBUG, 0x400, sizeof(DEBUG)); // graba el debug actual

        return false;
        }

    else{

        crearArchivo_s(ARCHIVO);

        strcpy(datos,__DATE__);
        strcat(datos," ; ");
        strcat(datos,__TIME__);
        strcat(datos, "\n\r");

        WDT

        escribeArchivo_s(ARCHIVO);

        DEBUG &= ERR_SD_MOUNT ;

        if (CONFIG & CFG_EEPROM_ON) ROM_EEPROMProgram((uint32_t *) &DEBUG, 0x400, sizeof(DEBUG)); // graba el debug actual

        return true;

        }

}


bool escribeArchivo_s(ARCHIVO *ARCHIVO)
{
    WDT

   (*ARCHIVO).iFResult = f_open((*ARCHIVO).Archivo, (*ARCHIVO).nombre, FA_WRITE | FA_WRITE);    //FA_OPEN_ALWAYS) ;

   f_lseek((*ARCHIVO).Archivo,  (*(*ARCHIVO).Archivo).fsize );                                  // Move to end of the file to append data

   strcpy(texto, "192.0.0.1 usuario [ 01/04/2019 :");
   strcat(texto, HORA_TIVA.text);
   strcat(texto, "- 0700] ");
   strcat(texto, ARCHIVO->datos);
   strcat(texto, "\n\r");

   ARCHIVO->iFResult=f_write(ARCHIVO->Archivo, texto, strlen(texto), &ARCHIVO->bw );

   //(*ARCHIVO).iFResult=f_write((*ARCHIVO).Archivo, (*ARCHIVO).datos, strlen((*ARCHIVO).datos), &(*ARCHIVO).bw );

   f_close((*ARCHIVO).Archivo);

   if((*ARCHIVO).iFResult == FR_OK){

    //   UARTprintf("numero de datos escritos  %u\n\r",(*ARCHIVO).bw);


       DEBUG |= ERR_SD_ESCRITURA;

       if (CONFIG & CFG_EEPROM_ON) ROM_EEPROMProgram((uint32_t *) &DEBUG, 0x400, sizeof(DEBUG)); // graba el debug actual

       return true;

   }

   else {

       UARTprintf("ERROR al escribir el archivo\n\r");


       DEBUG &= ERR_SD_ESCRITURA;

       if (CONFIG & CFG_EEPROM_ON) ROM_EEPROMProgram((uint32_t *) &DEBUG, 0x400, sizeof(DEBUG)); // graba el debug actual

       return false;
   }

}


bool crearArchivo_s(ARCHIVO *ARCHIVO)
{
    WDT

    unsigned int  indice = 0;

    char Sindice [10];
    char name_ [10];

    unsigned int intentos = 1;

    while (intentos <= 2) // intenta hasta diez veces abrir el archivo
    {

    WDT     // feed de dog

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

        WDT

       // break;
    }
    else
        {
        UARTprintf("\n\r no se pudo abrir el archivo intento %u ",intentos);

        UARTprintf("  ERROR %u \n\r",(*ARCHIVO).iFResult);

        WDT

        intentos++;

        ROM_SysCtlDelay(10000);

        }
    }

if ((*ARCHIVO).iFResult == FR_OK)
{
    DEBUG &= ERR_SD_CREA_ARCHIVO;

    if (CONFIG & CFG_EEPROM_ON) ROM_EEPROMProgram((uint32_t *) &DEBUG, 0x400, sizeof(DEBUG)); // graba el debug actual

    return true;
}
else
    {
    DEBUG |= ERR_SD_CREA_ARCHIVO;

    if (CONFIG & CFG_EEPROM_ON) ROM_EEPROMProgram((uint32_t *) &DEBUG, 0x400, sizeof(DEBUG)); // graba el debug actual

    return false;
    }

}

