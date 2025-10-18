#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdbool.h>

//I2C / LCD 
#define F_SCL      100000UL     
#define LCD_ADDR   0x27         
#define LCD_SLAW   (LCD_ADDR<<1)


#define RS_BM (1<<0)
#define RW_BM (1<<1)
#define E_BM  (1<<2)
#define BL_BM (1<<3)

//TWI básico
static void twi_init(void) {
    TWSR = 0x00;     // Prescaler = 1
    TWBR = 72;       // 100 kHz @16MHz
    TWCR = (1<<TWEN);
}
static void twi_start(void) {
    TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
    while (!(TWCR & (1<<TWINT)));
}
static void twi_stop(void) {
    TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
}
static void twi_write(uint8_t data) {
    TWDR = data;
    TWCR = (1<<TWINT)|(1<<TWEN);
    while (!(TWCR & (1<<TWINT)));
}

//PCF8574 write 
static void pcf_write(uint8_t val) {
    twi_start();
    twi_write(LCD_SLAW);
    twi_write(val);
    twi_stop();
}

//LCD I2C
static void lcd_send_nibble(uint8_t nibble, uint8_t rs_mask) {
    uint8_t out = ((nibble & 0x0F) << 4) | rs_mask | BL_BM;
    pcf_write(out);
    _delay_us(1);
    pcf_write(out | E_BM);
    _delay_us(1);
    pcf_write(out);
    _delay_us(40);
}
static void lcd_write_byte(uint8_t value, uint8_t rs_mask) {
    lcd_send_nibble(value >> 4, rs_mask);
    lcd_send_nibble(value & 0x0F, rs_mask);
}
static void lcd_cmd(uint8_t cmd) {
    lcd_write_byte(cmd, 0);
    if (cmd == 0x01 || cmd == 0x02) _delay_ms(2);
    else _delay_us(50);
}
static void lcd_data(uint8_t data) {
    lcd_write_byte(data, RS_BM);
    _delay_us(50);
}
static void lcd_set_cursor(uint8_t row, uint8_t col) {
    uint8_t base = (row==0) ? 0x00 : 0x40;
    lcd_cmd(0x80 | (base + col));
}
static void lcd_print(const char *s) {
    while (*s) lcd_data((uint8_t)*s++);
}
static void lcd_clear(void){ lcd_cmd(0x01); }
static void lcd_init(void) {
    _delay_ms(20);
    lcd_send_nibble(0x03, 0);  _delay_ms(5);
    lcd_send_nibble(0x03, 0);  _delay_ms(1);
    lcd_send_nibble(0x03, 0);  _delay_us(150);
    lcd_send_nibble(0x02, 0);  _delay_us(150);
    lcd_cmd(0x28);
    lcd_cmd(0x08);
    lcd_cmd(0x01);
    lcd_cmd(0x06);
    lcd_cmd(0x0C);
}

/*                  Teclado matricial
   Filas -> PD7, PD6, PD5, PD4  (salidas, activas en 0)
   Columnas -> PB3, PB2, PB1, PB0 (entradas con pull-up)
*/
static void kpd_init(void) {
    DDRD  |=  0xF0;
    PORTD |=  0xF0;
    DDRB  &= ~0x0F;
    PORTB |=  0x0F;
}
static char keymap(uint8_t r, uint8_t c) {
    static const char tbl[4][4] = {
        {'1','2','3','A'},
        {'4','5','6','B'},
        {'7','8','9','C'},
        {'*','0','#','D'}
    };
    if (r<4 && c<4) return tbl[r][c];
    return '\0';
}
static char kpd_getkey_blocking(void) {
    const uint8_t rowBit[4] = {7,6,5,4};   
    const uint8_t colBit[4] = {0,1,2,3};   
    while (1) {
        for (uint8_t r = 0; r < 4; r++) {
            PORTD |= 0xF0;
            PORTD &= ~(1 << rowBit[r]);    // activa fila
            _delay_us(5);
            uint8_t pinb = PINB & 0x0F;    // 0 = presionada
            if (pinb != 0x0F) {
                _delay_ms(20);
                pinb = PINB & 0x0F;
                if (pinb != 0x0F) {
                    for (uint8_t c = 0; c < 4; c++) {
                        if ((pinb & (1 << colBit[c])) == 0) {
                            char ch = keymap(r, c);
                            while ((PINB & (1 << colBit[c])) == 0) { }
                            _delay_ms(10);
                            return ch;
                        }
                    }
                }
            }
        }
    }
}

//EEPROM: PIN persistente
#include <avr/eeprom.h>
uint8_t EEMEM ee_magic;
uint8_t EEMEM ee_len;
uint8_t EEMEM ee_digits[6];
uint8_t EEMEM ee_csum;

static uint8_t pass_checksum(const char* d, uint8_t n){
    uint8_t s = 0x5A ^ n;
    for(uint8_t i=0;i<n;i++) s ^= (uint8_t)d[i];
    return s;
}
static bool pass_load(char* out, uint8_t* n){
    uint8_t magic = eeprom_read_byte(&ee_magic);
    if (magic != 0xA5) return false;
    uint8_t len = eeprom_read_byte(&ee_len);
    if (len < 4 || len > 6) return false;
    for(uint8_t i=0;i<6;i++) out[i] = eeprom_read_byte(&ee_digits[i]);
    uint8_t cs = eeprom_read_byte(&ee_csum);
    if (cs != pass_checksum(out, len)) return false;
    *n = len; return true;
}
static bool pass_save(const char* d, uint8_t n){
    if (n<4 || n>6) return false;
    eeprom_update_byte(&ee_magic, 0x00); // invalidar
    eeprom_update_byte(&ee_len, n);
    for(uint8_t i=0;i<6;i++){
        uint8_t v = (i<n) ? (uint8_t)d[i] : 0xFF;
        eeprom_update_byte(&ee_digits[i], v);
    }
    eeprom_update_byte(&ee_csum, pass_checksum(d,n));
    eeprom_update_byte(&ee_magic, 0xA5); // commit
    return true;
}

//Buzzer en D3 (PD3)
#define BUZZ_DDR   DDRD
#define BUZZ_PORT  PORTD
#define BUZZ_PIN   PD3
#define BUZZER_MODE 1     // 1=ACTIVO (DC), 2=PASIVO (tono), 0=AUTO

static inline void buzzer_on(void){ BUZZ_DDR |= (1<<BUZZ_PIN); BUZZ_PORT |=  (1<<BUZZ_PIN); }
static inline void buzzer_off(void){                  BUZZ_PORT &= ~(1<<BUZZ_PIN); }

static void buzzer_dc_ms(uint16_t ms){
    buzzer_on();
    while (ms--) _delay_ms(1);
    buzzer_off();
}
static void buzzer_tone_ms(uint16_t ms){
    BUZZ_DDR |= (1<<BUZZ_PIN);
    uint16_t cycles = (uint16_t)((ms * 1000UL) / 500UL); // 2kHz = 500us período
    for(uint16_t i=0;i<cycles;i++){
        BUZZ_PORT |=  (1<<BUZZ_PIN);
        _delay_us(250);
        BUZZ_PORT &= ~(1<<BUZZ_PIN);
        _delay_us(250);
    }
}
static void beep_ms(uint16_t ms){
#if BUZZER_MODE==1
    buzzer_dc_ms(ms);
#elif BUZZER_MODE==2
    buzzer_tone_ms(ms);
#else
    if (ms < 4) ms = 4;
    uint16_t half = ms/2;
    buzzer_dc_ms(half);
    buzzer_tone_ms(ms - half);
#endif
}

//LEDs: D13 (PB5) verde, D12 (PB4) rojo 
#define LEDG     PB5
#define LEDR     PB4
#define LED_DDR  DDRB
#define LED_PORT PORTB

static inline void leds_init(void){
    LED_DDR  |= (1<<LEDG) | (1<<LEDR);
    LED_PORT &= ~((1<<LEDG) | (1<<LEDR));   // ambos apagados
}
static inline void led_green_on(void){  LED_PORT |=  (1<<LEDG); }
static inline void led_green_off(void){ LED_PORT &= ~(1<<LEDG); }
static inline void led_red_on(void){    LED_PORT |=  (1<<LEDR); }
static inline void led_red_off(void){   LED_PORT &= ~(1<<LEDR); }


#define PIN_MIN 4
#define PIN_MAX 6
typedef enum {
    ST_BOOT=0, ST_MENU, ST_ENTER, ST_VERIFY, ST_LOCKOUT,
    ST_CHANGE_ASK, ST_CHANGE_LEN, ST_CHANGE_NEW, ST_CHANGE_CONF
} state_t;

static state_t st = ST_BOOT;
static char pin_stored[PIN_MAX];   static uint8_t pin_stored_len = 0;
static char pin_buf[PIN_MAX];      static uint8_t pin_len = 0;
static char pin_new[PIN_MAX];      static uint8_t pin_new_len = 0;
static uint8_t attempts = 0;

static void ui_header(const char* l1, const char* l2){
    lcd_clear(); lcd_set_cursor(0,0); lcd_print(l1);
    lcd_set_cursor(1,0); if(l2) lcd_print(l2);
}
static void ui_menu(void){
    ui_header("1=Ingresar PIN", "2=Cambiar PIN");
    st = ST_MENU;
}
static void ui_enter_prompt(const char* msg){
    lcd_clear(); lcd_set_cursor(0,0); lcd_print(msg);
    lcd_set_cursor(1,0); lcd_print("PIN: ");
    pin_len = 0; st = ST_ENTER;
}
static void ui_mask_print(void){
    lcd_set_cursor(1,5);
    for(uint8_t i=0;i<PIN_MAX;i++) lcd_data( (i<pin_len)?'*':' ');
    lcd_set_cursor(1,5+pin_len);
}
static inline void push_digit(char d){
    if (pin_len < PIN_MAX){ pin_buf[pin_len++] = d; ui_mask_print(); }
}
static bool pin_match(const char* a, uint8_t na, const char* b, uint8_t nb){
    if (na != nb) return false;
    for(uint8_t i=0;i<na;i++) if (a[i]!=b[i]) return false;
    return true;
}
//_delay_ms necesita constante -> dividir en rebanadas fijas
static void wait_ms_with_msg(uint16_t total_ms, const char* l1, const char* l2){
    ui_header(l1,l2);
    while (total_ms >= 10) { _delay_ms(10); total_ms -= 10; }
    while (total_ms--)      { _delay_ms(1); }
}

static void app_on_key(char k){
    switch(st){
    case ST_MENU:
        if (k=='1') ui_enter_prompt("Ingresar PIN");
        else if (k=='2' || k=='A'){
            if (pass_load(pin_stored,&pin_stored_len)){
                ui_enter_prompt("PIN actual");
                st = ST_CHANGE_ASK;
            } else {
                lcd_clear(); lcd_set_cursor(0,0); lcd_print("Elige longitud");
                lcd_set_cursor(1,0); lcd_print("4,5,6 y #");
                st = ST_CHANGE_LEN;
            }
        }
        break;

    case ST_ENTER:
        if (k>='0' && k<='9') push_digit(k);
        else if (k=='*'){ if (pin_len){ --pin_len; ui_mask_print(); } }
        else if (k=='#'){
            if (pin_len < PIN_MIN){
                wait_ms_with_msg(900, "Min 4 digitos", "Intenta de nuevo");
                ui_enter_prompt("Ingresar PIN");
            } else {
                st = ST_VERIFY;
                if (pass_load(pin_stored,&pin_stored_len) &&
                    pin_match(pin_buf,pin_len,pin_stored,pin_stored_len)){
                    led_green_on();
                    beep_ms(120);
                    wait_ms_with_msg(900, "PIN Correcto", "Acceso concedido");
                    led_green_off();
                    attempts = 0; ui_menu();
                } else {
                    attempts++;
                    lcd_clear(); lcd_set_cursor(0,0); lcd_print("PIN Incorrecto");
                    lcd_set_cursor(1,0); lcd_print("Intentos: ");
                    lcd_data('0'+attempts);
                    led_red_on();
                    beep_ms(200);
                    _delay_ms(700);
                    led_red_off();
                    if (attempts>=3){
                        attempts = 0;
                        //Bloqueo 30 s con LED rojo encendido y beep por segundo 
                        led_red_on();
                        uint8_t i;
                        for(i=30;i>0;i--){
                            lcd_clear();
                            lcd_set_cursor(0,0); lcd_print("ALERTA: BLOQUEO");
                            lcd_set_cursor(1,0); lcd_print("Restan: ");
                            lcd_data('0' + (i/10));
                            lcd_data('0' + (i%10));
                            lcd_data('s');
                            beep_ms(120);
                            _delay_ms(1000);
                        }
                        led_red_off();
                        ui_menu();
                    } else {
                        ui_enter_prompt("Ingresar PIN");
                    }
                }
            }
        }
        break;

    case ST_CHANGE_ASK:  // verificar PIN actual
        if (k>='0' && k<='9') push_digit(k);
        else if (k=='*'){ if (pin_len){ --pin_len; ui_mask_print(); } else ui_menu(); }
        else if (k=='#'){
            if (!(pass_load(pin_stored,&pin_stored_len) &&
                  pin_match(pin_buf,pin_len,pin_stored,pin_stored_len))){
                led_red_on();
                beep_ms(200);
                wait_ms_with_msg(900, "Actual incorrecto", "Volviendo...");
                led_red_off();
                ui_menu();
            } else {
                lcd_clear(); lcd_set_cursor(0,0); lcd_print("Elige longitud");
                lcd_set_cursor(1,0); lcd_print("4,5,6 y #");
                st = ST_CHANGE_LEN;
            }
        }
        break;

    case ST_CHANGE_LEN:
        if (k=='4' || k=='5' || k=='6'){
            pin_new_len = (uint8_t)(k - '0');
            lcd_clear(); lcd_set_cursor(0,0); lcd_print("Nuevo PIN (");
            lcd_data(k); lcd_print("):");
            lcd_set_cursor(1,0); lcd_print("PIN: ");
            pin_len = 0; st = ST_CHANGE_NEW;
        } else if (k=='*'){ ui_menu(); }
        break;

    case ST_CHANGE_NEW:
        if (k>='0' && k<='9'){ if (pin_len < pin_new_len) push_digit(k); }
        else if (k=='*'){ if (pin_len){ --pin_len; ui_mask_print(); } else ui_menu(); }
        else if (k=='#' && pin_len == pin_new_len){
            for(uint8_t i=0;i<pin_new_len;i++) pin_new[i] = pin_buf[i];
            lcd_clear(); lcd_set_cursor(0,0); lcd_print("Confirmar PIN:");
            lcd_set_cursor(1,0); lcd_print("PIN: ");
            pin_len = 0; st = ST_CHANGE_CONF;
        }
        break;

    case ST_CHANGE_CONF:
        if (k>='0' && k<='9'){ if (pin_len < pin_new_len) push_digit(k); }
        else if (k=='*'){ if (pin_len){ --pin_len; ui_mask_print(); } else ui_menu(); }
        else if (k=='#'){
            if (pin_len == pin_new_len){
                bool same = true;
                for(uint8_t i=0;i<pin_new_len;i++)
                    if (pin_buf[i]!=pin_new[i]) { same=false; break; }
                if (same){
                    if (pass_save(pin_new, pin_new_len)){
                        led_green_on();
                        beep_ms(120);
                        wait_ms_with_msg(900, "PIN guardado", "OK");
                        led_green_off();
                        ui_menu();
                    } else {
                        led_red_on();
                        beep_ms(200);
                        wait_ms_with_msg(1200, "Error EEPROM", "Cancelado");
                        led_red_off();
                        ui_menu();
                    }
                } else {
                    led_red_on();
                    beep_ms(200);
                    wait_ms_with_msg(900, "No coincide", "Reintenta");
                    led_red_off();
                    lcd_clear(); lcd_set_cursor(0,0); lcd_print("Nuevo PIN (");
                    lcd_data('0'+pin_new_len); lcd_print("):");
                    lcd_set_cursor(1,0); lcd_print("PIN: ");
                    pin_len=0; st = ST_CHANGE_NEW;
                }
            }
        }
        break;

    default: break;
    }
}


int main(void) {
    twi_init();
    lcd_init();
    kpd_init();
    leds_init();                

    lcd_clear();
    lcd_set_cursor(0,0);
    lcd_print("Candado I2C+KB");
    lcd_set_cursor(1,0);
    lcd_print("Tecla: ");

    if (pass_load(pin_stored, &pin_stored_len)) {
        ui_menu();
    } else {
        ui_menu();
    }

    while (1) {
        char k = kpd_getkey_blocking();

        
        lcd_set_cursor(1,7);
        lcd_data((uint8_t)k);
        _delay_ms(100);
        lcd_set_cursor(1,7);
        lcd_data(' ');
        _delay_ms(50);
        lcd_set_cursor(1,7);
        lcd_data((uint8_t)k);

        app_on_key(k);
    }
}
