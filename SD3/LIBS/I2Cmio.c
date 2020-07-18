/*
 * bq32000.c
 *
 *  Created on: 2 jul. 2020
 *      Author: daniel
 */


#include <stdbool.h>
#include <stdint.h>
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "inc/hw_i2c.h"
#include "driverlib/i2c.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "sensorlib/i2cm_drv.h"
#include "sensorlib/sht21.h"
#include "I2Cmio.h"
#include "tick.h"



extern bool flag_TIMEOUT1;
extern uint32_t flag_count;

bool Espera(uint32_t ui32BaseAddress){

    flag_TIMEOUT1 = false;

    while(!(I2CMasterBusy(ui32BaseAddress)) & !flag_TIMEOUT1);
    while(  I2CMasterBusy(ui32BaseAddress)  & !flag_TIMEOUT1);

    if(flag_TIMEOUT1) {

        flag_count++;

        return false;
    }

    else return true;

}



void    I2C_Init(void)
{
    SysCtlPeripheralDisable (SYSCTL_PERIPH_I2C1);
    SysCtlPeripheralReset   (SYSCTL_PERIPH_I2C1);
    SysCtlPeripheralEnable  (SYSCTL_PERIPH_I2C1);

    SysCtlPeripheralEnable  (SYSCTL_PERIPH_GPIOA);

    GPIOPinConfigure(GPIO_PA6_I2C1SCL);
    GPIOPinConfigure(GPIO_PA7_I2C1SDA);

    GPIOPinTypeI2CSCL(GPIO_PORTA_BASE, GPIO_PIN_6);
    GPIOPinTypeI2C(GPIO_PORTA_BASE, GPIO_PIN_7);

    // Wait for the Peripheral to be ready for programming
    //
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_I2C1));
    //
    // Initialize and Configure the Master Module
    //
    I2CMasterInitExpClk(I2C1_BASE, SysCtlClockGet(), false);


}



uint8_t I2C_GetReg_8(uint32_t ui32BaseAddress, uint8_t direccion, uint8_t registro)
{
    uint8_t  ui8Sec;
    //
    // Write to the Config Register of BQ3200
    //

    I2CMasterSlaveAddrSet(ui32BaseAddress, direccion, WRITE);

    I2CMasterDataPut(ui32BaseAddress, registro);
    I2CMasterControl(ui32BaseAddress,I2C_MASTER_CMD_SINGLE_SEND);

    while(!(I2CMasterBusy(ui32BaseAddress)));
    while(I2CMasterBusy(ui32BaseAddress));

    SysCtlDelay(10000);

    I2CMasterSlaveAddrSet(ui32BaseAddress, direccion, READ);

    I2CMasterDataPut(ui32BaseAddress, registro);
    I2CMasterControl(ui32BaseAddress,I2C_MASTER_CMD_SINGLE_RECEIVE);

    while(!(I2CMasterBusy(ui32BaseAddress)));
    while(I2CMasterBusy(ui32BaseAddress));
    ui8Sec = I2CMasterDataGet(ui32BaseAddress);

    //ui8Sec=ui8Sec & 0x7F;

 //   ui8Sec=(ui8Sec/16*10) + (ui8Sec%16);

    return ui8Sec;

}


bool I2C_SetReg_8(uint32_t ui32BaseAddress, uint8_t direccion, uint8_t registro, uint8_t DATO)
{
    //
    // Write to the Config Register of BQ3200
    //

    I2CMasterSlaveAddrSet(ui32BaseAddress, direccion, WRITE);

    I2CMasterDataPut(ui32BaseAddress, registro);
    I2CMasterControl(ui32BaseAddress,I2C_MASTER_CMD_BURST_SEND_START);

    while(!(I2CMasterBusy(ui32BaseAddress)));
    while(I2CMasterBusy(ui32BaseAddress));

    I2CMasterDataPut(ui32BaseAddress, DATO);
    I2CMasterControl(ui32BaseAddress,I2C_MASTER_CMD_BURST_SEND_FINISH);

    while(!(I2CMasterBusy(ui32BaseAddress)));
    while(I2CMasterBusy(ui32BaseAddress));

    return true;

}


uint16_t I2C_GetReg_16(uint32_t ui32BaseAddress, uint8_t direccion, uint8_t registro)
{
    uint8_t  ui8ConvRegHighByte;
    uint8_t  ui8ConvRegLowByte;
    uint16_t ui16Resultado;
    //
    // Write to the Config Register of BQ3200
    //

    I2CMasterSlaveAddrSet(ui32BaseAddress, direccion, WRITE);

    I2CMasterDataPut(ui32BaseAddress, registro);                    //Escribe el numero de regístro que quiere leer
    I2CMasterControl(ui32BaseAddress,I2C_MASTER_CMD_SINGLE_SEND);

    while(!(I2CMasterBusy(ui32BaseAddress)));
    while(I2CMasterBusy(ui32BaseAddress));

    SysCtlDelay(10000);

    I2CMasterSlaveAddrSet(ui32BaseAddress, direccion, READ);

    I2CMasterControl(ui32BaseAddress,I2C_MASTER_CMD_BURST_RECEIVE_START);


    while(!(I2CMasterBusy(ui32BaseAddress)));
    while(I2CMasterBusy(ui32BaseAddress));
    ui8ConvRegHighByte = I2CMasterDataGet(ui32BaseAddress);

    I2CMasterControl(ui32BaseAddress,I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
    while(!(I2CMasterBusy(ui32BaseAddress)));
    while(I2CMasterBusy(ui32BaseAddress));
    ui8ConvRegLowByte = I2CMasterDataGet(ui32BaseAddress);


    ui16Resultado       = ((ui8ConvRegHighByte<<8)|ui8ConvRegLowByte);


    return ui16Resultado;

}

bool I2C_SetReg_16(uint32_t ui32BaseAddress, uint8_t direccion, uint8_t registro, uint16_t DATO)
{

    uint8_t ui8DATOLow, ui8DATOHigh;
    //
    // Write to the Config Register of BQ3200
    //

    ui8DATOHigh= DATO >> 8;
    ui8DATOLow = DATO & 0x00FF;

    I2CMasterSlaveAddrSet(ui32BaseAddress, direccion, WRITE);

    I2CMasterDataPut(ui32BaseAddress, registro);
    I2CMasterControl(ui32BaseAddress,I2C_MASTER_CMD_BURST_SEND_START);

    while(!(I2CMasterBusy(ui32BaseAddress)));
    while(I2CMasterBusy(ui32BaseAddress));

    I2CMasterDataPut(ui32BaseAddress, ui8DATOHigh);
    I2CMasterControl(ui32BaseAddress,I2C_MASTER_CMD_BURST_SEND_CONT);

    while(!(I2CMasterBusy(ui32BaseAddress)));
    while(I2CMasterBusy(ui32BaseAddress));

    I2CMasterDataPut(ui32BaseAddress, ui8DATOLow);
    I2CMasterControl(ui32BaseAddress,I2C_MASTER_CMD_BURST_SEND_FINISH);

    while(!(I2CMasterBusy(ui32BaseAddress)));
    while(I2CMasterBusy(ui32BaseAddress));

    return true;

}

uint16_t I2C_GetReg_24(uint32_t ui32BaseAddress, uint8_t direccion, uint8_t registro)
{
    uint8_t  ui8ConvChecksum;
    uint8_t  ui8ConvRegHighByte;
    uint8_t  ui8ConvRegLowByte;
    uint16_t ui16Resultado;
    //
    // Write to the Config Register of BQ3200
    //

    I2CMasterSlaveAddrSet(ui32BaseAddress, direccion, WRITE);

    I2CMasterDataPut(ui32BaseAddress, registro);                    //Escribe el numero de regístro que quiere leer
    I2CMasterControl(ui32BaseAddress,I2C_MASTER_CMD_SINGLE_SEND);

    while(!(I2CMasterBusy(ui32BaseAddress)));
    while(I2CMasterBusy(ui32BaseAddress));

   // SysCtlDelay(10000);
    SysCtlDelay(SysCtlClockGet() / (30 * 3));

    I2CMasterSlaveAddrSet(ui32BaseAddress, direccion, READ);

    I2CMasterControl(ui32BaseAddress,I2C_MASTER_CMD_BURST_RECEIVE_START);

    while(!(I2CMasterBusy(ui32BaseAddress)));
    while(I2CMasterBusy(ui32BaseAddress));
    ui8ConvRegHighByte = I2CMasterDataGet(ui32BaseAddress);

    I2CMasterControl(ui32BaseAddress,I2C_MASTER_CMD_BURST_RECEIVE_CONT);

    while(!(I2CMasterBusy(ui32BaseAddress)));
    while(I2CMasterBusy(ui32BaseAddress));
    ui8ConvRegLowByte = I2CMasterDataGet(ui32BaseAddress);


    I2CMasterControl(ui32BaseAddress,I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
    while(!(I2CMasterBusy(ui32BaseAddress)));
    while(I2CMasterBusy(ui32BaseAddress));
    ui8ConvChecksum = I2CMasterDataGet(ui32BaseAddress);




    ui16Resultado       = ((ui8ConvRegHighByte<<8)|ui8ConvRegLowByte);


    return ui16Resultado;

}

