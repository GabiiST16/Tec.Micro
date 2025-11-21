#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <stdint.h>

/* ====== Actuadores ====== */
#define LED_DDR   DDRD
#define LED_PORT  PORTD
#define LED_PIN   PD6

#define BUZ_DDR   DDRB
#define BUZ_PORT  PORTB
#define BUZ_PIN   PB0

#define SERVO_DDR DDRB
#define SERVO_PIN PB1   // OC1A (D9)

/* ===== Inicialización ===== */
static void actuators_init(void){
    LED_DDR  |= (1<<LED_PIN);
    BUZ_DDR  |= (1<<BUZ_PIN);
    SERVO_DDR |= (1<<SERVO_PIN);

    LED_PORT &= ~(1<<LED_PIN);
    BUZ_PORT &= ~(1<<BUZ_PIN);
}

/* ===== Servo (Timer1) ===== */
static void servo_init(void){
    TCCR1A = (1<<COM1A1)|(1<<WGM11);
    TCCR1B = (1<<WGM13)|(1<<WGM12)|(1<<CS11); // prescaler 8
    ICR1 = 40000;   // 20 ms
    OCR1A = 3000;   // ~90°
}

static void servo_write_deg(uint8_t deg){
    if(deg > 180) deg = 180;
    uint16_t counts = 2000 + (uint16_t)((deg * 2000UL + 90) / 180);
    if(counts < 2000) counts = 2000;
    if(counts > 4000) counts = 4000;
    OCR1A = counts;
}

/* ===== I2C Esclavo ===== */
#define SLAVE_ADDR 0x12

volatile uint8_t twi_cmd = 0;
volatile uint8_t twi_idx = 0;

#define CMD_SET_ACT   0xA1
#define CMD_SET_SERVO 0xA2

#define ACT_LED    (1<<0)
#define ACT_BUZZER (1<<1)

static void apply_mask(uint8_t m){
    if(m & ACT_LED)  LED_PORT |=  (1<<LED_PIN);
    else             LED_PORT &= ~(1<<LED_PIN);

    if(m & ACT_BUZZER) BUZ_PORT |=  (1<<BUZ_PIN);
    else               BUZ_PORT &= ~(1<<BUZ_PIN);
}

static void twi_slave_init(void){
    TWAR = (SLAVE_ADDR<<1);               // dirección + GC deshabilitado
    TWCR = (1<<TWEN)|(1<<TWEA)|(1<<TWIE)|(1<<TWINT);
}

/* ISR TWI */
ISR(TWI_vect){
    uint8_t status = TWSR & 0xF8;

    switch(status){
        case 0x60: // Own SLA+W received
        case 0x68:
            twi_idx = 0;
            TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA)|(1<<TWIE);
            break;

        case 0x80: // dato recibido, ACK
        case 0x90:
        {
            uint8_t data = TWDR;
            if(twi_idx == 0){
                twi_cmd = data;     // primer byte = comando
            } else if(twi_idx == 1){
                if(twi_cmd == CMD_SET_ACT){
                    apply_mask(data);
                } else if(twi_cmd == CMD_SET_SERVO){
                    servo_write_deg(data);
                }
            }
            twi_idx++;
            TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA)|(1<<TWIE);
        }
        break;

        case 0xA0: // STOP o repeated START
            twi_cmd = 0;
            twi_idx = 0;
            TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA)|(1<<TWIE);
            break;

        default:
            TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA)|(1<<TWIE);
            break;
    }
}

/* ===== MAIN ===== */
int main(void){
    actuators_init();
    servo_init();
    twi_slave_init();
    sei();

    while(1){
        // todo se maneja en la ISR y PWM
    }
}
