/*
 * bq32000.h
 *
 *  Created on: 2 jul. 2020
 *      Author: daniel
 */

#ifndef LIB_I2CMIO_H_
#define LIB_I2CMIO_H_


#ifdef __cplusplus
extern "C"
{
#endif


#define BME280_ADDRESS      0X76


//****************************************************************************
//
// I2C Read Write value
//
//****************************************************************************
#define WRITE              0x0
#define READ               0x1
//****************************************************************************

//*****************************************************************************
//
// Prototypes for the APIs.
//
//*****************************************************************************

extern void     I2C_Init(void);

extern uint8_t  I2C_GetReg_8(uint32_t ui32BaseAddress, uint8_t direccion, uint8_t registro);

extern bool     I2C_SetReg_8(uint32_t ui32BaseAddress, uint8_t direccion, uint8_t registro, uint8_t DATO);

extern uint16_t I2C_GetReg_16(uint32_t ui32BaseAddress, uint8_t direccion, uint8_t registro);

extern bool     I2C_SetReg_16(uint32_t ui32BaseAddress, uint8_t direccion, uint8_t registro, uint16_t DATO);

extern uint16_t I2C_GetReg_24(uint32_t ui32BaseAddress, uint8_t direccion, uint8_t registro);

extern bool     Espera(uint32_t ui32BaseAddress);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************


#ifdef __cplusplus
}
#endif



#endif /* LIB_I2CMIO_H_ */
