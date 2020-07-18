/*
 * BME280.c
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
#include "BME280mio.h"
#include "I2Cmio.h"
#include "tick.h"





uint16_t    dig_T1  =   0,
            dig_T2  =   0,
            dig_T3  =   0,
            dig_H1  =   0,
            dig_H2  =   0,
            dig_H3  =   0,
            dig_H4  =   0,
            dig_H5  =   0,
            dig_H6  =   0,
            dig_P1  =   0,
            dig_P2  =   0,
            dig_P3  =   0,
            dig_P4  =   0,
            dig_P5  =   0,
            dig_P6  =   0,
            dig_P7  =   0,
            dig_P8  =   0,
            dig_P9  =   0,
            AUX     =   0;

uint32_t t_fine=0;



#define I2C_BASE        I2C1_BASE
#define read8(x)        I2C_GetReg_8(I2C_BASE, BME280_ADDRESS, x);
#define write8(x,y)     I2C_SetReg_8(I2C_BASE, BME280_ADDRESS, x , y);

volatile uint32_t int_cont=0;

void I2CHandler(void)
{
    int_cont++;
    I2CMasterIntClear(I2C_BASE);

}

bool BM280_Convertir(void){

    I2CMasterSlaveAddrSet(I2C_BASE,BME280_ADDRESS, WRITE);

    I2CMasterDataPut(I2C_BASE, BME280_REGISTER_CONTROL);
    I2CMasterControl(I2C_BASE,I2C_MASTER_CMD_BURST_SEND_START);

    Espera(I2C_BASE);

    I2CMasterDataPut(I2C_BASE, 0X25);    // 001 001 01 = 0x25;
    I2CMasterControl(I2C_BASE,I2C_MASTER_CMD_BURST_SEND_FINISH);

    Espera(I2C_BASE);

    return true;
}

bool BME280_Inicio(void)
{

    I2CIntRegister(I2C_BASE,I2CHandler);

    I2CMasterTimeoutSet(I2C_BASE,0x7D);        // 0x7d 20ms timeout en 100khz

    //I2CMasterIntEnable(I2C1_BASE);

    I2CMasterIntEnableEx(I2C_BASE,I2C_MASTER_INT_TIMEOUT);

    uint8_t    _sensorID    =   0,
    Status      =   0;
    // check if sensor, i.e. the chip ID is correct
    _sensorID = read8(BME280_REGISTER_CHIPID);
    if (_sensorID != 0x60)
    return false;

    // reset the device using soft-reset
    // this makes sure the IIR is off, etc.
    write8(BME280_REGISTER_SOFTRESET, 0xB6);

    // wait for chip to wake up.
    SysCtlDelay(SysCtlClockGet() / (30 * 3));

    // if chip is still reading calibration, delay
    do {

    Status = read8(BME280_REGISTER_STATUS);

    SysCtlDelay(1000);;


    } while ((Status & (1 << 0)) != 0);


    BME280_LeeCoeficientes(); // read trimming parameters, see DS 4.2.2

    // making sure sensor is in sleep mode before setting configuration
    // as it otherwise may be ignored
    write8(BME280_REGISTER_CONTROL, MODE_SLEEP);



    write8(BME280_REGISTER_CONTROLHUMID, 0x00);

        // inactive duration (standby time) in normal mode
        // 000 = 0.5 ms
        // 001 = 62.5 ms
        // 010 = 125 ms
        // 011 = 250 ms
        // 100 = 500 ms
        // 101 = 1000 ms
        // 110 = 10 ms
        // 111 = 20 ms


        // filter settings
        // 000 = filter off
        // 001 = 2x filter
        // 010 = 4x filter
        // 011 = 8x filter
        // 100 and above = 16x filter


        // unused - don't set

        write8(BME280_REGISTER_CONFIG, 0x00);

    // temperature oversampling
    // 000 = skipped
    // 001 = x1
    // 010 = x2
    // 011 = x4
    // 100 = x8
    // 101 and above = x16


    // pressure oversampling
    // 000 = skipped
    // 001 = x1
    // 010 = x2
    // 011 = x4
    // 100 = x8
    // 101 and above = x16


    // device mode
    // 00       = sleep
    // 01 or 10 = forced
    // 11       = normal



    write8(BME280_REGISTER_CONTROL, 0X01);


    //      delay(100);
    SysCtlDelay(1000);


    BME280_LeeCoeficientes();

    return true;
}


bool BME280_LeeCoeficientes(void){


    dig_T1 =             read8(BME280_REGISTER_DIG_T1+1);
    dig_T1 = dig_T1<<8 | read8(BME280_REGISTER_DIG_T1  );
    dig_T2 =             read8(BME280_REGISTER_DIG_T2+1);
    dig_T2 = dig_T2<<8 | read8(BME280_REGISTER_DIG_T2  );
    dig_T3 =             read8(BME280_REGISTER_DIG_T3+1);
    dig_T3 = dig_T3<<8 | read8(BME280_REGISTER_DIG_T3  );


    dig_H1 =    read8( BME280_REGISTER_DIG_T3);

    dig_H2 =             read8(BME280_REGISTER_DIG_H2+1);
    dig_H2 = dig_H2<<8 | read8(BME280_REGISTER_DIG_H2  );

    dig_H3 =    read8( BME280_REGISTER_DIG_H3);

    dig_H4 =    read8( BME280_REGISTER_DIG_H4);

    dig_H4 <<= 4;

    AUX =       read8(  BME280_REGISTER_DIG_H5);

    dig_H4 = dig_H4 | (AUX & 0x0f);

    dig_H5 = AUX>>4;    // 0xE5[7:4]

    AUX =       read8( BME280_REGISTER_DIG_H5+1);

    dig_H5 = dig_H5 |  (AUX << 4);

    dig_H6 =    read8(   BME280_REGISTER_DIG_H6);


    dig_P1 =             read8(BME280_REGISTER_DIG_P1+1);
    dig_P1 = dig_P1<<8 | read8(BME280_REGISTER_DIG_P1  );

    dig_P2 =             read8(BME280_REGISTER_DIG_P2+1);
    dig_P2 = dig_P2<<8 | read8(BME280_REGISTER_DIG_P2  );

    dig_P3 =             read8(BME280_REGISTER_DIG_P3+1);
    dig_P3 = dig_P3<<8 | read8(BME280_REGISTER_DIG_P3  );

    dig_P4 =             read8(BME280_REGISTER_DIG_P4+1);
    dig_P4 = dig_P4<<8 | read8(BME280_REGISTER_DIG_P4  );

    dig_P5 =             read8(BME280_REGISTER_DIG_P5+1);
    dig_P5 = dig_P5<<8 | read8(BME280_REGISTER_DIG_P5  );

    dig_P6 =             read8(BME280_REGISTER_DIG_P6+1);
    dig_P6 = dig_P6<<8 | read8(BME280_REGISTER_DIG_P6  );

    dig_P7 =             read8(BME280_REGISTER_DIG_P7+1);
    dig_P7 = dig_P7<<8 | read8(BME280_REGISTER_DIG_P7  );

    dig_P8 =             read8(BME280_REGISTER_DIG_P8+1);
    dig_P8 = dig_P8<<8 | read8(BME280_REGISTER_DIG_P8  );

    dig_P9 =             read8(BME280_REGISTER_DIG_P9+1);
    dig_P9 = dig_P9<<8 | read8(BME280_REGISTER_DIG_P9  );

    return true;

}



float BME280_GetTemp(void)
{
    uint32_t adc_T,var1,var2;
    float T;

    //*************************************************************************
    // Lee MSB 0xFA
    //*************************************************************************

    I2CMasterSlaveAddrSet(I2C_BASE,BME280_ADDRESS, WRITE);

    I2CMasterDataPut(I2C_BASE, 0XFA);                    //Escribe el numero de regístro que quiere leer
    I2CMasterControl(I2C_BASE,I2C_MASTER_CMD_SINGLE_SEND);

    Espera(I2C_BASE);

   // SysCtlDelay(10000);
    SysCtlDelay(SysCtlClockGet() / (30 * 3));

    I2CMasterSlaveAddrSet(I2C_BASE, BME280_ADDRESS, READ);

    I2CMasterControl(I2C_BASE,I2C_MASTER_CMD_SINGLE_RECEIVE);

    Espera(I2C_BASE);

    adc_T = I2CMasterDataGet(I2C_BASE);

    adc_T<<=8;

    SysCtlDelay(SysCtlClockGet() / (30 * 3));

   //*************************************************************************
   // Lee LSB 0xFB
   //*************************************************************************

   I2CMasterSlaveAddrSet(I2C_BASE,BME280_ADDRESS, WRITE);

   I2CMasterDataPut(I2C_BASE, 0XFB);                    //Escribe el numero de regístro que quiere leer
   I2CMasterControl(I2C_BASE,I2C_MASTER_CMD_SINGLE_SEND);

   Espera(I2C_BASE);

   SysCtlDelay(SysCtlClockGet() / (30 * 3));

   I2CMasterSlaveAddrSet(I2C_BASE, BME280_ADDRESS, READ);

   I2CMasterDataPut(I2C_BASE, 0XFB);
   I2CMasterControl(I2C_BASE,I2C_MASTER_CMD_SINGLE_RECEIVE);

   Espera(I2C_BASE);

   adc_T |= I2CMasterDataGet(I2C_BASE);

   adc_T<<=8;

   //*************************************************************************
   // Lee LSB 0xFC
   //*************************************************************************

   I2CMasterDataPut(I2C_BASE, 0XFC);                    //Escribe el numero de regístro que quiere leer
   I2CMasterSlaveAddrSet(I2C_BASE, BME280_ADDRESS, READ);

   I2CMasterControl(I2C_BASE,I2C_MASTER_CMD_SINGLE_RECEIVE);

   Espera(I2C_BASE);

   adc_T |= I2CMasterDataGet(I2C_BASE);

   adc_T>>=4;

   var1 = ((( (adc_T >> 3) - ((int32_t)dig_T1 << 1))) * ((int32_t)dig_T2))>>11;
   var2 = (((((adc_T >> 4) - ((int32_t)dig_T1)) *((adc_T>>4) - ((int32_t)dig_T1))) >> 12) *((int32_t)dig_T3))>>14;

   t_fine = var1 + var2;

   T = (t_fine * 5 + 128) >> 8;

   return T /100 ;


}


float BME280_GetHum(void)
{
    uint32_t adc_H;

    int32_t v_x1_u32r;

    float h;

    I2CMasterSlaveAddrSet(I2C_BASE, BME280_ADDRESS, WRITE);

    I2CMasterDataPut(I2C_BASE, 0XFE); //Escribe el numero de regístro que quiere leer
    I2CMasterControl(I2C_BASE, I2C_MASTER_CMD_SINGLE_SEND);

    Espera(I2C_BASE);

    // SysCtlDelay(10000);
    SysCtlDelay(SysCtlClockGet() / (30 * 3));

    I2CMasterSlaveAddrSet(I2C_BASE, BME280_ADDRESS, READ);

    I2CMasterControl(I2C_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);

    Espera(I2C_BASE);

    adc_H = I2CMasterDataGet(I2C_BASE);

    adc_H <<= 8;

    SysCtlDelay(SysCtlClockGet() / (30 * 3));

    I2CMasterSlaveAddrSet(I2C_BASE, BME280_ADDRESS, WRITE);

    I2CMasterDataPut(I2C_BASE, 0XFD); //Escribe el numero de regístro que quiere leer
    I2CMasterControl(I2C_BASE, I2C_MASTER_CMD_SINGLE_SEND);

    Espera(I2C_BASE);

    SysCtlDelay(SysCtlClockGet() / (30 * 3));

    I2CMasterSlaveAddrSet(I2C_BASE, BME280_ADDRESS, READ);

    I2CMasterControl(I2C_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);

    Espera(I2C_BASE);

    adc_H = I2CMasterDataGet(I2C_BASE);

    adc_H <<= 8;

    v_x1_u32r = (t_fine - ((int32_t) 76800));

    v_x1_u32r = (((((adc_H << 14) - (((int32_t) dig_H4) << 20) - (((int32_t) dig_H5) * v_x1_u32r)) + ((int32_t) 16384)) >> 15)
                * (((((((v_x1_u32r * ((int32_t) dig_H6)) >> 10)* (((v_x1_u32r * ((int32_t) dig_H3)) >> 11)
                + ((int32_t) 32768))) >> 10) + ((int32_t) 2097152)) * ((int32_t) dig_H2) + 8192) >> 14));

    v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((int32_t) dig_H1)) >> 4));

    v_x1_u32r = (v_x1_u32r < 0) ? 0 : v_x1_u32r;

    v_x1_u32r = (v_x1_u32r > 419430400) ? 419430400 : v_x1_u32r;

    h = (v_x1_u32r >> 12);

    return h / 1024.0;
}

float BME280_GetPres(void)
{
    int64_t adc_P,var1,var2 ,p;

    //*************************************************************************
    // Lee MSB 0xFA
    //*************************************************************************

    I2CMasterSlaveAddrSet(I2C_BASE,BME280_ADDRESS, WRITE);

    I2CMasterDataPut(I2C_BASE, 0XF7);                    //Escribe el numero de regístro que quiere leer
    I2CMasterControl(I2C_BASE,I2C_MASTER_CMD_SINGLE_SEND);

    Espera(I2C_BASE);

   // SysCtlDelay(10000);
    SysCtlDelay(SysCtlClockGet() / (30 * 3));

    I2CMasterSlaveAddrSet(I2C_BASE, BME280_ADDRESS, READ);

    I2CMasterControl(I2C_BASE,I2C_MASTER_CMD_SINGLE_RECEIVE);

    Espera(I2C_BASE);

    adc_P = I2CMasterDataGet(I2C_BASE);

    adc_P<<=8;

    SysCtlDelay(SysCtlClockGet() / (30 * 3));

   //*************************************************************************
   // Lee LSB 0xFB
   //*************************************************************************

   I2CMasterSlaveAddrSet(I2C_BASE,BME280_ADDRESS, WRITE);

   I2CMasterDataPut(I2C_BASE, 0XF7);                    //Escribe el numero de regístro que quiere leer
   I2CMasterControl(I2C_BASE,I2C_MASTER_CMD_SINGLE_SEND);

   Espera(I2C_BASE);

   SysCtlDelay(SysCtlClockGet() / (30 * 3));

   I2CMasterSlaveAddrSet(I2C_BASE, BME280_ADDRESS, READ);

   I2CMasterDataPut(I2C_BASE, 0XF8);
   I2CMasterControl(I2C_BASE,I2C_MASTER_CMD_SINGLE_RECEIVE);

   Espera(I2C_BASE);

   adc_P |= I2CMasterDataGet(I2C_BASE);

   adc_P<<=8;

   //*************************************************************************
   // Lee LSB 0xFC
   //*************************************************************************

   I2CMasterDataPut(I2C_BASE, 0XF9);                    //Escribe el numero de regístro que quiere leer
   I2CMasterSlaveAddrSet(I2C_BASE, BME280_ADDRESS, READ);

   I2CMasterControl(I2C_BASE,I2C_MASTER_CMD_SINGLE_RECEIVE);

   Espera(I2C_BASE);

   adc_P |= I2CMasterDataGet(I2C_BASE);

   adc_P>>=4;

    var1 = ((int64_t)t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)dig_P6;
    var2 = var2 + ((var1 * (int64_t)dig_P5) << 17);
    var2 = var2 + (((int64_t)dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)dig_P3) >> 8) +
         ((var1 * (int64_t)dig_P2) << 12);
    var1 =
      (((((int64_t)1) << 47) + var1)) * ((int64_t)dig_P1) >> 33;

    if (var1 == 0) {
    return 0; // avoid exception caused by division by zero
    }
    p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;

    var1 = (((int64_t)dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)dig_P8) * p) >> 19;

    p = ((p + var1 + var2) >> 8) + (((int64_t)dig_P7) << 4);

    return (float)p / 256 / 100.0F;


}

