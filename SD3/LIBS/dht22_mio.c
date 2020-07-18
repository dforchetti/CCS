
#include "dht22_mio.h"

float dht_readTemperature(void);
float dht_readHumidity(void);
uint8_t dht_read(void);
void DHTIntHandler(void);
void Timer5IntHandler(void);


volatile uint8_t data_buffer[6];
volatile uint32_t dht_timing;
volatile int32_t start_timer;

volatile bool quit_timing   = false;
volatile bool ERROR         = false;

volatile int contadorTimer5 = 0;


void Timer5IntHandler(){

    ROM_TimerIntClear(TIMER5_BASE, TIMER_TIMA_TIMEOUT);
    contadorTimer5++;
}


void dht_init(void) {

	ROM_SysCtlPeripheralEnable(DHT_PORT_PERIPH);

    // set interrupt for counter
	GPIOIntEnable(DHT_PORT, DHT_PIN);
	ROM_GPIOIntTypeSet(DHT_PORT, DHT_PIN, GPIO_BOTH_EDGES); // config interrupt both edges
	GPIOIntRegister(DHT_PORT, DHTIntHandler);


    ROM_SysCtlPeripheralEnable(DHT_TIM_PERIPH);
    ROM_TimerConfigure(DHT_TIM_BASE, TIMER_CFG_ONE_SHOT_UP);
    ROM_TimerLoadSet(DHT_TIM_BASE, DHT_TIM,10000);
    TimerIntRegister(DHT_TIM_BASE, DHT_TIM, Timer5IntHandler);
    ROM_TimerIntEnable(DHT_TIM_BASE, TIMER_TIMA_TIMEOUT);
    ROM_TimerEnable(DHT_TIM_BASE, DHT_TIM);

    TIMEOUTinit();


	DON();
	DHT_DATA(DHT_PIN);

}


// RETURN:
// 1 = success reading from sensor;
// 0 = error on reading from sensor;
// also return T and H

uint8_t dht_readTH(DHT_TypeDef *values) {

    if (dht_read()==1) {

        // temperature
        values->celsius_temp = dht_readTemperature();

        // humidity
        values->humidity = dht_readHumidity();
      // }

        return 1;
    }

    values->celsius_temp = 80;
    values->humidity     = 100;

    return 0;
}



float dht_readTemperature() {

    float t;

#ifdef DHT11
    t = data_buffer[2];
#else

    t = data_buffer[2] & 0x7F;
    t *= 256;
    t += data_buffer[3];
    t /= 10.0;

    if (data_buffer[2] & 0x80)
        t *= -1.0;

    if (t > 80.0) {
    	t = 80.0;
    }
    if (t < -40.0) {
    	t = -40.0;
    }


#endif

    return t;
}


float dht_readHumidity() {

    float h;

#ifdef DHT11
    h = data_buffer[0];
#else
    h = data_buffer[0];
    h *= 256;
    h += data_buffer[1];
    h /= 10.0;

    if (h > 100.0) {
    	h = 100.0;
    }
    if (h < 0.0) {
    	h = 0.0;
    }

#endif

    return h;
}


uint8_t dht_read(void) {
    uint8_t ii, byteIdx, bitCounter;

    // clear data
    for (ii=0; ii< 5; ii++) *(data_buffer + ii) = 0;

    // send start signal low and wait 1-18 ms
    DON();

    DHT_DATA(0x00);
    ROM_SysCtlDelay(DHT_WAIT_18ms);


    // send pull up signal high and wait 20-40 us
    DHT_DATA(DHT_PIN);
    // wait at least 20us

    ROM_SysCtlDelay(DHT_WAIT_20us);


    GPIOIntEnable(DHT_PORT, DHT_PIN);

    DIN();

    TIMEOUT(300000)
    while((ROM_GPIOPinRead(DHT_PORT, DHT_PIN) == DHT_PIN) & !flag_timeout);


    // acknowledge ~80us low
    TIMEOUT(300000)
    while((ROM_GPIOPinRead(DHT_PORT, DHT_PIN) == 0x00   ) & !flag_timeout);

    // acknowledge ~80us high
    TIMEOUT(300000)
    while((ROM_GPIOPinRead(DHT_PORT, DHT_PIN) == DHT_PIN) & !flag_timeout);

    ROM_TimerLoadSet(DHT_TIM_BASE, DHT_TIM,10000);

    // gets data 5 bytes, 40 bits
    // every bit start with low level for 50us then high level last
    // 0 = 26-28us
    // 1 = 70us

    byteIdx = 0;
    bitCounter = 7;

    for (ii=0; ii<40; ii++) {

    	quit_timing = false;

        HWREG(DHT_TIM_BASE + TIMER_O_TAV) = 0;
        flag_timeout=false;
        ROM_TimerEnable(DHT_TIM_BASE, DHT_TIM);
        // do elaboration

        TIMEOUT(300000)
    	while(!quit_timing & !flag_timeout);

        // check bit (timing is 1us) 26-28 -> 0 - 70us -> 1
        if (dht_timing > DHT_TIME_BIT_1) {
            data_buffer[byteIdx] |= (1 << bitCounter);
        }

        if (bitCounter == 0) {
            bitCounter = 7;
            byteIdx++;
        } else
            bitCounter--;

    }

    // check we read 40 bits and that the checksum matches

    GPIOIntDisable(DHT_PORT, DHT_PIN);

    if ((byteIdx == 5) && (data_buffer[4] == ( (data_buffer[0] + data_buffer[1] + data_buffer[2] + data_buffer[3]) & 0xFF) ) ) {

        return 1;
    }


    return 0;

}

void DHTIntHandler(void)
{

	GPIOIntClear(DHT_PORT, DHT_PIN); // clear interrupt flag

	if (ROM_GPIOPinRead(DHT_PORT, DHT_PIN) == DHT_PIN) { // check low to high
	    ROM_TimerEnable(DHT_TIM_BASE, DHT_TIM);
		start_timer = ROM_TimerValueGet(DHT_TIM_BASE, DHT_TIM);
		quit_timing = false;
	} else { // check high to low, finish
	    ROM_TimerDisable(DHT_TIM_BASE, DHT_TIM);
	    dht_timing = ROM_TimerValueGet(DHT_TIM_BASE, DHT_TIM);
	    dht_timing -= start_timer;
    	quit_timing = true;
    	HWREG(DHT_TIM_BASE + TIMER_O_TAV) = 0;
	}

}

