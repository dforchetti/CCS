/*
 * GPIO.h
 *
 *  Created on: 12 feb. 2019
 *      Author: dforchetti
 */

#ifndef LIBS_GPIOS_H_
#define LIBS_GPIOS_H_

#define ON  true
#define OFF false


#define LED_ROJO    GPIO_PIN_7      //PD7
#define LED_VERDE   GPIO_PIN_7      //PC7
#define LED_AZUL    GPIO_PIN_6      //PC6
#define AUX1        GPIO_PIN_0      //PD0   //VBAT+
#define AUX2        GPIO_PIN_5      //PE5   //VBAT-
#define AUX3        GPIO_PIN_4      //PE4
#define AUX4        GPIO_PIN_4      //PB4


/*
 * Configuracion inicial de las señales digitales
 */

void blink(void (*funcion)(int));
void configuraGPIO(void);

void ledAzul    (int /*ON/OFF*/);
void ledVerde   (int /*ON/OFF*/);
void ledRojo    (int /*ON/OFF*/);
void ledAux3    (int /*ON/OFF*/);
void ledAux4    (int /*ON/OFF*/);


#endif /* LIBS_GPIOS_H_ */
