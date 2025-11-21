/* slave_spi_servo_led_buzzer */

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <stdint.h>

/* Actuadores */
#define LED_DDR   DDRD
#define LED_PORT  PORTD
#define LED_PIN   PD6

#define BUZ_DDR   DDRB
#define BUZ_PORT  PORTB
#define BUZ_PIN   PB0

#define SERVO_DDR DDRB
#define SERVO_PIN PB1   // OC1A

/* Inicialización */
static void actuators_init(void){
    LED_DDR |= (1<<LED_PIN);
    BUZ_DDR |= (1<<BUZ_PIN);
    SERVO_DDR |= (1<<SERVO_PIN);

    LED_PORT &= ~(1<<LED_PIN);
    BUZ_PORT &= ~(1<<BUZ_PIN);
}

/* Servo (Timer1) */
static void servo_init(void){
    TCCR1A = (1<<COM1A1)|(1<<WGM11);
    TCCR1B = (1<<WGM13)|(1<<WGM12)|(1<<CS11);
    ICR1 = 40000;
    OCR1A = 3000;
}

static void servo_write_deg(uint8_t deg){
    if(deg > 180) deg = 180;
    uint16_t counts = 2000 + (uint16_t)((deg * 2000UL + 90) / 180);
    if(counts < 2000) counts = 2000;
    if(counts > 4000) counts = 4000;
    OCR1A = counts;
}

/* SPI Esclavo */
volatile uint8_t cmd = 0;
volatile uint8_t idx = 0;

static void spi_slave_init(void){
    DDRB &= ~((1<<PB2)|(1<<PB3)|(1<<PB5));
    DDRB |= (1<<PB4);
    SPCR = (1<<SPE)|(1<<SPIE);
    SPDR = 0x00;
    sei();
}

#define ACT_LED    (1<<0)
#define ACT_BUZZER (1<<1)

static void apply_mask(uint8_t m){
    if(m & ACT_LED) LED_PORT |=  (1<<LED_PIN);
    else            LED_PORT &= ~(1<<LED_PIN);

    if(m & ACT_BUZZER) BUZ_PORT |=  (1<<BUZ_PIN);
    else               BUZ_PORT &= ~(1<<BUZ_PIN);
}

#define CMD_SET_ACT   0xA1
#define CMD_SET_SERVO 0xA2
#define CMD_PING      0xB1

ISR(SPI_STC_vect){
    uint8_t in  = SPDR;
    uint8_t out = 0x00;

    if(cmd == 0){
        cmd = in;
        idx = 0;

        if(cmd == CMD_PING){
            out = 0x55;
            cmd = 0;
        }
    }
    else if(cmd == CMD_SET_ACT){
        if(idx == 0){
            apply_mask(in);
            out = 0x55;
            cmd = 0;
        }
        idx++;
    }
    else if(cmd == CMD_SET_SERVO){
        if(idx == 0){
            servo_write_deg(in);
            out = 0x55;
            cmd = 0;
        }
        idx++;
    }
    else{
        out = 0x00;
        cmd = 0;
    }

    SPDR = out;
}

/* MAIN */
int main(void){
    actuators_init();
    servo_init();
    spi_slave_init();

    while(1){
        // Todo se maneja por interrupciones
    }
}
