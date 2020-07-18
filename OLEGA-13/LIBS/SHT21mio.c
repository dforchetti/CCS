/*
 * SHT21mio.c
 *
 *  Created on: 20 feb. 2019
 *      Author: dforchetti
 */
#include "SHT21mio.h"
#include "sensorlib/sht21.h"

extern volatile uint32_t    CONFIG, DEBUG;


//*****************************************************************************
//
// SHT21 Sensor callback function.  Called at the end of SHT21 sensor driver
// transactions. This is called from I2C interrupt context. Therefore, we just
// set a flag and let main do the bulk of the computations and display.
//
//*****************************************************************************
void
SHT21AppCallback(void *pvCallbackData, uint_fast8_t ui8Status)
{
    //
    // If the transaction succeeded set the data flag to indicate to
    // application that this transaction is complete and data may be ready.
    //
    if(ui8Status == I2CM_STATUS_SUCCESS)
    {
        g_vui8DataFlag = 1;
    }

    //
    // Store the most recent status in case it was an error condition
    //
    g_vui8ErrorFlag = ui8Status;
}

//*****************************************************************************
//
// TMP006 Application error handler.
//
//*****************************************************************************
void
SHT21AppErrorHandler(char *pcFilename, uint_fast32_t ui32Line)
{
    //
    // Set terminal color to red and print error status and locations
    //
    UARTprintf("\033[31;1m");
    UARTprintf("Error: %d, File: %s, Line: %d\n"
               "See I2C status definitions in utils\\i2cm_drv.h\n",
               g_vui8ErrorFlag, pcFilename, ui32Line);

    //
    // Return terminal color to normal
    //
    UARTprintf("\033[0m");

    if (CONFIG & CFG_SD_ON)
    {
    strcpy(ARCHIVO_LOG.datos,"Error i2cm  \n\r");

    escribeArchivo_s(&ARCHIVO_LOG);
    }

    DEBUG = ERR_SHT21_ERROR;
    //ROM_EEPROMProgram((uint32_t *) &DEBUG, 0x400, sizeof(DEBUG)); // graba el debug actual

    // Go to sleep wait for interventions.  A more robust application could
    // attempt corrective actions here.

   /* while(1)
    {
        ROM_SysCtlSleep();
    }*/
}

//*****************************************************************************
//
// Called by the NVIC as a result of I2C3 Interrupt. I2C3 is the I2C connection
// to the SHT21.
//
//*****************************************************************************
void
SHT21I2CIntHandler(void)
{
    //
    // Pass through to the I2CM interrupt handler provided by sensor library.
    // This is required to be at application level so that I2CMIntHandler can
    // receive the instance structure pointer as an argument.
    //
    I2CMIntHandler(&g_sI2CInst);
}

//*****************************************************************************
//
// Function to wait for the SHT21 transactions to complete.
//
//*****************************************************************************
int timeout_=0;

bool
SHT21AppI2CWait(char *pcFilename, uint_fast32_t ui32Line)
{
    bool return_ = true;
    //
    // Put the processor to sleep while we wait for the I2C driver to
    // indicate that the transaction is complete.
    //
    timeout_=10;

    while((g_vui8DataFlag == 0) && (g_vui8ErrorFlag == 0)&&(timeout_!=0))
    {
        //ROM_SysCtlSleep();
        ROM_SysCtlDelay(50000);
        timeout_--;
        UARTprintf("wait");
    }

    if (timeout_==0)
    {
        UARTprintf("TIMEOUT");

        return_ = false;

        g_vui8ErrorFlag = 1;
    }
    //
    // If an error occurred call the error handler immediately.
    //
    if(g_vui8ErrorFlag)
    {
        SHT21AppErrorHandler(pcFilename, ui32Line);
    }
    else {
        DEBUG = ERR_NO_SHT21_ERROR;

        //ROM_EEPROMProgram((uint32_t *) &DEBUG, 0x400, sizeof(DEBUG)); // graba el debug actual

        ERR_EEPROM

    }
    //
    // clear the data flag for next use.
    //

    g_vui8DataFlag = 0;

    return return_;
}


void iniciaSHT21(void)
{
#ifdef VERBOSE
    UARTprintf(" -- inicia SHT21    --\n");
#endif
        //
        // The I2C3 peripheral must be enabled before use.
        //
        ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C1);
        ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

        //
        // Configure the pin muxing for I2C3 functions on port D0 and D1.
        // This step is not necessary if your part does not support pin muxing.
        //
        ROM_GPIOPinConfigure(GPIO_PA6_I2C1SCL);
        ROM_GPIOPinConfigure(GPIO_PA7_I2C1SDA);
        //
        // Select the I2C function for these pins.  This function will also
        // configure the GPIO pins pins for I2C operation, setting them to
        // open-drain operation with weak pull-ups.  Consult the data sheet
        // to see which functions are allocated per pin.
        //
        GPIOPinTypeI2CSCL(GPIO_PORTA_BASE, GPIO_PIN_6);
        GPIOPinTypeI2C(GPIO_PORTA_BASE, GPIO_PIN_7);

        //
        // Keep only some parts of the systems running while in sleep mode.
        // UART0 is the virtual serial port
        // TIMER0, TIMER1 and WTIMER5 are used by the RGB driver
        // I2C3 is the I2C interface to the TMP006
        //
        ROM_SysCtlPeripheralClockGating(true);
        ROM_SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_UART0);
        ROM_SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_TIMER0);
        ROM_SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_TIMER1);
        ROM_SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_I2C1);
        ROM_SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_WTIMER5);

        //
        // Enable interrupts to the processor.
        //
        ROM_IntMasterEnable();

        IntRegister(INT_I2C1 ,SHT21I2CIntHandler);
        //
        // Initialize I2C3 peripheral.
        //
        I2CMInit(&g_sI2CInst, I2C1_BASE, INT_I2C1, 0xff, 0xff, ROM_SysCtlClockGet());
        //
        // Initialize the SHT21
        //
        SHT21Init(&g_sSHT21Inst, &g_sI2CInst, SHT21_I2C_ADDRESS, SHT21AppCallback, &g_sSHT21Inst);
        //
        // Wait for the I2C transactions to complete before moving forward
        //
        SHT21AppI2CWait(__FILE__, __LINE__);
        //
        // Delay for 20 milliseconds for SHT21 reset to complete itself.
        // Datasheet says reset can take as long 15 milliseconds.
        //
        ROM_SysCtlDelay(ROM_SysCtlClockGet() / (50 * 3));

}



void leeHumedadSHT21 (variables *DATA)
{
            // Write the command to start a humidity measurement

            SHT21Write(&g_sSHT21Inst, SHT21_CMD_MEAS_RH, g_sSHT21Inst.pui8Data, 0, SHT21AppCallback, &g_sSHT21Inst);

            // Wait for the I2C transactions to complete before moving forward

            SHT21AppI2CWait(__FILE__, __LINE__);

            // Wait 33 milliseconds before attempting to get the result. Datasheet claims this can take as long as 29 milliseconds

            ROM_SysCtlDelay(ROM_SysCtlClockGet() / (30 * 3));

            // Get the raw data from the sensor over the I2C bus

            SHT21DataRead(&g_sSHT21Inst, SHT21AppCallback, &g_sSHT21Inst);

            // Wait for the I2C transactions to complete before moving forward

            SHT21AppI2CWait(__FILE__, __LINE__);

            // Get a copy of the most recent raw data in floating point format.

            SHT21DataHumidityGetFloat(&g_sSHT21Inst, &DATA->xn);

            DATA->n++;

            SUM_MAX_MIN(DATA);

            UARTprintf(" -- lee  SHT21 adentro --\n");


           // ROM_SysCtlDelay(ROM_SysCtlClockGet() / 3);
}


void leeTemperaturaSHT21(variables *DATA)
{
            // Write the command to start a temperature measurement

            SHT21Write(&g_sSHT21Inst, SHT21_CMD_MEAS_T, g_sSHT21Inst.pui8Data, 0, SHT21AppCallback, &g_sSHT21Inst);

            // Wait for the I2C transactions to complete before moving forward

            SHT21AppI2CWait(__FILE__, __LINE__);

            // Wait 100 milliseconds before attempting to get the result. Datasheet claims this can take as long as 85 milliseconds

            ROM_SysCtlDelay( ROM_SysCtlClockGet() / (10 * 3) );

            // Read the conversion data from the sensor over I2C.

            SHT21DataRead(&g_sSHT21Inst, SHT21AppCallback, &g_sSHT21Inst);

            // Wait for the I2C transactions to complete before moving forward

            SHT21AppI2CWait(__FILE__, __LINE__);

            // Get the most recent temperature result as a float in celcius.

            SHT21DataTemperatureGetFloat(&g_sSHT21Inst, &DATA->xn);

            DATA->n++;

            SUM_MAX_MIN(DATA);

         //   ROM_SysCtlDelay(ROM_SysCtlClockGet() / 3);
}


