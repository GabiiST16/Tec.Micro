#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>

#define KP_NUM 4
#define KP_DEN 10

#define KI_NUM 5
#define KI_DEN 100

#define DEAD_BAND 2

#define MIN_PWM 100

#define MAX_INTEGRAL 5000
#define MIN_INTEGRAL -5000

#define DIR1_PORT PORTD
#define DIR1_DDR  DDRD
#define DIR1_PIN  PD6

#define DIR2_PORT PORTD
#define DIR2_DDR  DDRD
#define DIR2_PIN  PD5

#define PWM_DDR   DDRB
#define PWM_PIN   PB1

void usart_init();
void usart_transmit_string(const char* str);
void adc_init();
uint16_t adc_read(uint8_t channel);
void pwm_init();
void pwm_set(uint8_t value);
void direction_init();
void set_direction_stop();
void set_direction_horario();
void set_direction_antihorario();

static int32_t integral_error = 0;

int main(void) {
    usart_init();
    adc_init();
    pwm_init();
    direction_init();
    
    int16_t ref_val = 0;
    int16_t actual_val = 0;
    int16_t error = 0;
    
    int16_t p_term = 0;
    int16_t i_term = 0;
    int16_t calc_pwm = 0;
    
    uint8_t pwm_out = 0;
    const char* direccion = "Detenido";
    char serial_buffer[80];

    set_direction_stop();
    pwm_set(0);

    while (1) {
        ref_val = adc_read(0);
        actual_val = adc_read(1);
        
        error = ref_val - actual_val;

        if (abs(error) > DEAD_BAND) {
            p_term = (int16_t)(((int32_t)error * KP_NUM) / KP_DEN);
            
            integral_error += error;
            
            if (integral_error > MAX_INTEGRAL) {
                integral_error = MAX_INTEGRAL;
            } else if (integral_error < MIN_INTEGRAL) {
                integral_error = MIN_INTEGRAL;
            }
            
            i_term = (int16_t)(((int32_t)integral_error * KI_NUM) / KI_DEN);

            calc_pwm = p_term + i_term;

            pwm_out = (uint8_t)abs(calc_pwm);

            if (pwm_out > 255) {
                pwm_out = 255;
            }
            if (pwm_out < MIN_PWM) {
                pwm_out = MIN_PWM;
            }
            
            if (calc_pwm > 0) {
                set_direction_horario();
                direccion = "Horario";
            } else {
                set_direction_antihorario();
                direccion = "Antihorario";
            }
            
            pwm_set(pwm_out);

        } else {
            set_direction_stop();
            pwm_set(0);
            pwm_out = 0;
            direccion = "Detenido";
            
            integral_error = 0;
        }

        sprintf(serial_buffer, "%d,%d,%d,%s\r\n", ref_val, actual_val, pwm_out, direccion);
        usart_transmit_string(serial_buffer);
        
        _delay_ms(50);
    }
}

#define BAUD 9600
#define UBRR_VAL ((F_CPU / 16 / BAUD) - 1)

void usart_init() {
    UBRR0H = (uint8_t)(UBRR_VAL >> 8);
    UBRR0L = (uint8_t)(UBRR_VAL);
    UCSR0B = (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void usart_transmit_char(char data) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = data;
}

void usart_transmit_string(const char* str) {
    while (*str) {
        usart_transmit_char(*str++);
    }
}

void adc_init() {
    ADMUX = (1 << REFS0);
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint16_t adc_read(uint8_t channel) {
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
    return ADC;
}

void pwm_init() {
    PWM_DDR |= (1 << PWM_PIN);
    TCCR1A = (1 << WGM10) | (1 << COM1A1);
    TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10);
    OCR1A = 0;
}

void pwm_set(uint8_t value) {
    OCR1A = value;
}

void direction_init() {
    DIR1_DDR |= (1 << DIR1_PIN);
    DIR2_DDR |= (1 << DIR2_PIN);
}

void set_direction_stop() {
    DIR1_PORT &= ~(1 << DIR1_PIN);
    DIR2_PORT &= ~(1 << DIR2_PIN);
}

void set_direction_horario() {
    DIR1_PORT |= (1 << DIR1_PIN);
    DIR2_PORT &= ~(1 << DIR2_PIN);
}

void set_direction_antihorario() {
    DIR1_PORT &= ~(1 << DIR1_PIN);
    DIR2_PORT |= (1 << DIR2_PIN);
}