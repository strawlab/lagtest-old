/* -*- Mode: C; tab-width: 4; c-basic-offset: 4 -*- */
#include "lagtest_datatypes.h"
#define analogPin 3

volatile epoch_dtype epoch=0;
volatile uint8_t ADCvalue;    // Global variable, set to volatile if used with ISR

volatile sample_t current_sample;

ISR(ADC_vect)
{
    current_sample.adc = ADCH;
    current_sample.epoch = epoch;
    current_sample.ticks = TCNT1;
}

ISR(TIMER1_OVF_vect)
{
    epoch++;

}

void setup_timer1() {
    cli();
    TCCR1A = 0;
    TCCR1B = 0; // normal mode

    TCCR1B |= _BV( CS11 ) | _BV( CS10 ); // clock prescaler 64

    TIMSK1 = _BV(TOIE1); // enable interrupt on timer1
    sei();
}

void setup_adc() {
    cli();

    ADMUX = 0;                // use ADC0
    ADMUX |= (1 << REFS0);    // use AVcc as the reference
    ADMUX |= (1 << ADLAR);    // Right adjust for 8 bit resolution

    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // 128 prescale for 8Mhz
    ADCSRA |= (1 << ADATE);    // Set free running mode
    ADCSRA |= (1 << ADEN);    // Enable the ADC
    ADCSRA |= (1 << ADIE);    // Enable Interrupts

    ADCSRA |= (1 << ADSC);    // Start the ADC conversion

    sei();
}

void setup() {

    // start serial port at 115200 bps:
    Serial.begin(115200);

    setup_timer1();
    setup_adc();
}

static inline void send_data(const sample_t samp) {
    const char * buf;
    Serial.write("H");
    buf = (const char*)&(samp);
    uint8_t chksum=0;
    for (uint8_t i=0; i< sizeof(sample_t); i++) {
        chksum += buf[i];
        Serial.write(buf[i]);
    }
    Serial.write(chksum);
}

void loop() {
    static sample_t sample_copy;

    uint8_t SaveSREG = SREG;   // save interrupt flag
    cli(); // disable interrupts

        sample_copy.adc = current_sample.adc;
        sample_copy.epoch = current_sample.epoch;
        sample_copy.ticks = current_sample.ticks;

    SREG = SaveSREG; // restore interrupt flags

    send_data(sample_copy);
}

