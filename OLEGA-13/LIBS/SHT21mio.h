/*
 * SHT21.h
 *
 *  Created on: 13 feb. 2019
 *      Author: dforchetti
 */

#ifndef LIBS_SHT21MIO_H_
#define LIBS_SHT21MIO_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "utils/uartstdio.h"
#include "sensorlib/hw_sht21.h"
#include "sensorlib/i2cm_drv.h"
#include "sensorlib/sht21.h"
#include "SHT21mio.h"
#include "variables.h"
#include "SD.h"
#include "AVL.h"




extern ARCHIVO ARCHIVO_DATOS, ARCHIVO_LOG;


//*****************************************************************************
// Define SHT21 I2C Address.
//*****************************************************************************

#define SHT21_I2C_ADDRESS  0x40

//*****************************************************************************
// Global instance structure for the I2C master driver.
//*****************************************************************************

tI2CMInstance g_sI2CInst;

//*****************************************************************************
// Global instance structure for the SHT21 sensor driver.
//*****************************************************************************

tSHT21 g_sSHT21Inst;

//*****************************************************************************
// Global new data flag to alert main that TMP006 data is ready.
//*****************************************************************************

volatile uint_fast8_t g_vui8DataFlag;

//*****************************************************************************
// Global new error flag to store the error condition if encountered.
//*****************************************************************************

volatile uint_fast8_t g_vui8ErrorFlag;

void SHT21AppCallback(void *pvCallbackData, uint_fast8_t ui8Status);

void SHT21AppErrorHandler(char *pcFilename, uint_fast32_t ui32Line);

void SHT21I2CIntHandler(void);

bool SHT21AppI2CWait(char *pcFilename, uint_fast32_t ui32Line);

void iniciaSHT21(void);

void leeHumedadSHT21    (variables *);

void leeTemperaturaSHT21(variables *);

#endif /* LIBS_SHT21MIO_H_ */
