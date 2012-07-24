/* -*- Mode: C; tab-width: 4; c-basic-offset: 4 -*- */
#include "lagtest_datatypes.h"
#define analogPin 0
#define LEDPin 13

volatile epoch_dtype epoch=0;
volatile uint8_t ADCvalue;    // Global variable, set to volatile if used with ISR

volatile timed_sample_t adc_sample;

ISR(ADC_vect)
{
    adc_sample.value = ADCH;
    adc_sample.epoch = epoch;
    adc_sample.ticks = TCNT1;
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

    pinMode(LEDPin, OUTPUT);
    digitalWrite(LEDPin, 0);

    // start serial port at 115200 bps:
    Serial.begin(115200);

    setup_timer1();
    setup_adc();
}

static inline void send_data(const timed_sample_t samp, const char header) {
    const char * buf;
    Serial.write(header);
    buf = (const char*)&(samp);
    uint8_t chksum=0;
    for (uint8_t i=0; i< sizeof(timed_sample_t); i++) {
        chksum += buf[i];
        Serial.write(buf[i]);
    }
    Serial.write(chksum);
}

void loop() {
    static timed_sample_t adc_copy;

    uint8_t SaveSREG = SREG;   // save interrupt flag
    cli(); // disable interrupts

        adc_copy.value = adc_sample.value;
        adc_copy.epoch = adc_sample.epoch;
        adc_copy.ticks = adc_sample.ticks;

    SREG = SaveSREG; // restore interrupt flags

    send_data(adc_copy,'H');

    if (Serial.available() >= 2) {
        static char cmd;
        static char value;

        cmd = Serial.read();
        value = Serial.read();
        digitalWrite(LEDPin, 1);

        if (cmd=='P') {

            static timed_sample_t timestamp_request;

            timestamp_request.value = value;

            uint8_t SaveSREG_ = SREG;   // save interrupt flag
            cli(); // disable interrupts

                timestamp_request.epoch = epoch;
                timestamp_request.ticks = TCNT1;

            SREG = SaveSREG_; // restore interrupt flags
            send_data(timestamp_request,'P');
        }

    }
}

