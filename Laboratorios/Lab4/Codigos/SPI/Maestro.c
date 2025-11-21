/*
   Maestro:
     - DHT11 en PD2
     - Pulsador en PD3
     - IR en PD4
     - SPI Maestro -> Esclavo (servo, LED, buzzer)
     - LCD 16x2 I2C en dirección 0x27
*/

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/* Pines de sensores */
#define DHT_PIN   PD2
#define BTN_PIN   PD3
#define IR_PIN    PD4

/* SPI Master */
static void spi_master_init(void) {
    DDRB |= (1<<PB2) | (1<<PB3) | (1<<PB5);
    DDRB &= ~(1<<PB4);
    PORTB |= (1<<PB2);
    SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPR0);
}

static inline void spi_select(void){ PORTB &= ~(1<<PB2); }
static inline void spi_release(void){ PORTB |=  (1<<PB2); }

static uint8_t spi_txrx(uint8_t data){
    SPDR = data;
    while(!(SPSR & (1<<SPIF)));
    return SPDR;
}

/* DHT11 */
static inline void dht_line_output(void){ DDRD |= (1<<DHT_PIN); }
static inline void dht_line_input_pullup(void){ DDRD &= ~(1<<DHT_PIN); PORTD |= (1<<DHT_PIN); }
static inline void dht_drive_low(void){ PORTD &= ~(1<<DHT_PIN); }
static inline uint8_t dht_read_pin(void){ return (PIND & (1<<DHT_PIN)) ? 1 : 0; }

static bool dht11_read(uint8_t *hum, uint8_t *temp){
    uint8_t d[5] = {0};

    dht_line_output();
    dht_drive_low();
    _delay_ms(18);

    dht_line_input_pullup();
    _delay_us(40);

    uint16_t t=0;

    while(dht_read_pin()){ if(++t>220) return false; _delay_us(1);}
    t=0; while(!dht_read_pin()){ if(++t>220) return false; _delay_us(1);}
    t=0; while(dht_read_pin()){ if(++t>220) return false; _delay_us(1);}

    for(uint8_t i=0;i<40;i++){
        t=0; while(!dht_read_pin()){ if(++t>220) return false; _delay_us(1); }
        _delay_us(30);
        uint8_t bit = dht_read_pin();
        t=0; while(dht_read_pin()){ if(++t>220) break; _delay_us(1); }

        d[i/8] <<= 1;
        d[i/8] |= bit;
    }

    if((uint8_t)(d[0]+d[1]+d[2]+d[3]) != d[4]) return false;

    *hum  = d[0];
    *temp = d[2];
    return true;
}

/* Entradas */
static inline void inputs_init(void){
    DDRD &= ~((1<<BTN_PIN)|(1<<IR_PIN));
    PORTD |= (1<<BTN_PIN);
}

static inline bool button_pressed(void){ return (PIND & (1<<BTN_PIN)) == 0; }
static inline bool ir_detected(void){ return (PIND & (1<<IR_PIN)) == 0; }

/* Protocolo SPI */
#define CMD_SET_ACT   0xA1
#define CMD_SET_SERVO 0xA2

#define ACT_LED     (1<<0)
#define ACT_BUZZER  (1<<1)

/* Lógica de sensores → actuadores */
static void decide_actions(uint8_t okDHT, uint8_t T, uint8_t H,
                           bool btn, bool ir,
                           uint8_t *mask, uint8_t *angle)
{
    uint8_t m = 0;

    if(btn) m |= ACT_BUZZER;
    if(okDHT && (T >= 28 || H >= 70)) m |= ACT_LED;

    uint8_t a = ir ? 150 : 30;

    *mask = m;
    *angle = a;
}

/* I2C + LCD */
#define LCD_ADDR       0x27
#define LCD_COMMAND    0
#define LCD_DATA       1
#define LCD_BACKLIGHT  0x08
#define EN             0x04

/* I2C básico */
static void i2c_init(void){
    TWSR = 0x00;
    TWBR = 72; // 100kHz approx
}

static void i2c_start(void){
    TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
    while(!(TWCR & (1<<TWINT)));
}

static void i2c_write(uint8_t data){
    TWDR = data;
    TWCR = (1<<TWINT)|(1<<TWEN);
    while(!(TWCR & (1<<TWINT)));
}

static void i2c_stop(void){
    TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
}

/* LCD via PCF8574 */
static void lcd_i2c_write(uint8_t data, uint8_t mode){
    uint8_t high = data & 0xF0;
    uint8_t low  = (data << 4) & 0xF0;

    i2c_start();
    i2c_write(LCD_ADDR << 1);

    i2c_write(high | mode | LCD_BACKLIGHT | EN);
    i2c_write(high | mode | LCD_BACKLIGHT);

    i2c_write(low  | mode | LCD_BACKLIGHT | EN);
    i2c_write(low  | mode | LCD_BACKLIGHT);

    i2c_stop();
}

static void lcd_i2c_command(uint8_t cmd){
    lcd_i2c_write(cmd, LCD_COMMAND);
    _delay_us(50);
}

static void lcd_i2c_clear(void){
    lcd_i2c_command(0x01);
    _delay_ms(2);
}

static void lcd_i2c_init(void){
    _delay_ms(50);

    lcd_i2c_write(0x30,LCD_COMMAND); _delay_ms(5);
    lcd_i2c_write(0x30,LCD_COMMAND); _delay_us(200);
    lcd_i2c_write(0x30,LCD_COMMAND); _delay_us(200);
    lcd_i2c_write(0x20,LCD_COMMAND);

    lcd_i2c_command(0x28);
    lcd_i2c_command(0x0C);
    lcd_i2c_command(0x06);
    lcd_i2c_clear();
}

/*  ESTA ERA LA FUNCIÓN QUE HABÍA QUE CAMBIAR  */
static void lcd_i2c_write_string(const char *str){
    while(*str){
        lcd_i2c_write((uint8_t)*str, LCD_DATA);
        str++;
    }
}

/* MAIN */
int main(void){
    spi_master_init();
    inputs_init();
    i2c_init();
    lcd_i2c_init();

    uint8_t T=0, H=0;
    char buf[17];

    /* Mensaje de arranque */
    lcd_i2c_command(0x80);
    lcd_i2c_write_string("Sistema I2C+DHT");

    lcd_i2c_command(0xC0);
    lcd_i2c_write_string("Maestro listo");
    _delay_ms(1000);

    while(1){
        bool ok  = dht11_read(&H, &T);
        bool btn = button_pressed();
        bool ir  = ir_detected();

        uint8_t mask, angle;
        decide_actions(ok, T, H, btn, ir, &mask, &angle);

        /* LCD - línea 1 */
        lcd_i2c_command(0x80);
        if(ok){
            snprintf(buf, sizeof(buf), "T:%2uC H:%2u%%", T, H);
            lcd_i2c_write_string(buf);
        } else {
            lcd_i2c_write_string("DHT ERROR      ");
        }

        /* LCD - línea 2 */
        lcd_i2c_command(0xC0);
        snprintf(buf, sizeof(buf), "BTN=%d IR=%d   ", btn?1:0, ir?1:0);
        lcd_i2c_write_string(buf);

        /* Enviar comandos al esclavo */
        spi_select();
        spi_txrx(CMD_SET_ACT);
        spi_txrx(mask);
        spi_release();

        _delay_ms(2);

        spi_select();
        spi_txrx(CMD_SET_SERVO);
        spi_txrx(angle);
        spi_release();

        _delay_ms(500);
    }
}
