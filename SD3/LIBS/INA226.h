/*
 * INA226.h
 *
 *  Created on: 12 jul. 2020
 *      Author: daniel
 */
#include <stdbool.h>
#include <stdint.h>

#ifndef LIB_INA226_H_
#define LIB_INA226_H_

#define INA226_ADDRESS      0X41
#define INA226_CONF         0X00
#define INA226_VOLTAJE      0X02
#define INA226_POTENCIA     0X03
#define INA226_CORRIENTE    0X01    //0X01
#define INA226_CALIBRATE    0X05

#define RESET               0x0000          //  0000 0000 0000 0000b
#define AVG                 0x0000          //  0000 0000 0000 0000b
#define VBCT                0x0100          //  0000 0001 0000 0000b    // 1.1 ms tiempo de conversion de voltaje
#define VSCT                0x0020          //  0000 0000 0010 0000b    // 1.1 ms tiempo de conversion de corriente
#define MODE                0x0007          //  0000 0000 0111 0000b    // conversion continua

#define ESCALA_CORRIENTE    1/21
#define ESCALA_VOLTAJE      1/800
#define ESCALA_POTENCIA     1


#define I2C_BASE_INA226     I2C1_BASE
#define read8(x)            I2C_GetReg_8(I2C_BASE_INA226, INA226_ADDRESS, x)
#define write8(x,y)         I2C_SetReg_8(I2C_BASE_INA226, INA226_ADDRESS, x , y)
#define read16(x)           I2C_GetReg_16(I2C_BASE_INA226, INA226_ADDRESS, x)
#define write16(x,y)        I2C_SetReg_16(I2C_BASE_INA226, INA226_ADDRESS, x , y)


void    Ina226_Init     (uint8_t Rshunt);
float   lee_corriente   (void);
float   lee_voltaje     (void);
float   lee_potencia    (void);


#endif /* LIB_INA226_H_ */
