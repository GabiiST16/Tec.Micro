#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


#define baud 9600
#define ubr 103 
#define LED 6   
#define WIDTH 8
#define HEIGHT 8
#define NUM_LEDS (WIDTH * HEIGHT)


uint8_t leds[NUM_LEDS][3];

volatile char comando_recibido = 0; 

// USART (Recepción por Interrupción)

void USART_Init(unsigned int ubrr);
void USART_Transmit(unsigned char data);
void USART_SendString(const char *str);
ISR(USART_RX_vect);

void sendBit(uint8_t bitVal);
void sendByte(uint8_t byte);
void show(uint8_t (*colors)[3]);
void setLedRGB(uint8_t (*leds_buf)[3], int ledIndex, uint8_t r, uint8_t g, uint8_t b);
void fillAllLedsRGB(uint8_t r, uint8_t g, uint8_t b);
void probarColores(void);

int main(void) {
    // pin de la matriz PD6 
    DDRD |= (1 << LED);
    
    USART_Init(ubr);
    
    // Habilitar interrupciones globales
    sei(); 
    
    // Saludo inicial
    USART_SendString("\r\n Sistema de Prueba de Matriz  \r\n");
    USART_SendString("Envie '0' para probar los colores.\r\n");
    
    while (1) {
        
        if (comando_recibido == '0') {
            
            comando_recibido = 0; 
            
            USART_SendString("Comando '0' recibido. Ejecutando prueba...\r\n");
            
            probarColores();
            
            USART_SendString("Prueba finalizada. Envie '0' de nuevo.\r\n");
        }
        
    }
}

void USART_Init(unsigned int ubrr)
{
    UBRR0H = (unsigned char)(ubrr >> 8);
    UBRR0L = (unsigned char)ubrr;
    // Habilitar RX, TX, y la Interrupción de Recepción (RXCIE0)
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); 
}
void USART_Transmit(unsigned char data)
{
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = data;
}

void USART_SendString(const char *str)
{
    while (*str)
    {
        USART_Transmit(*str++);
    }
}
ISR(USART_RX_vect) {
    char cmd = UDR0; // Leer carácter

    if (cmd == '0') { 
        comando_recibido = '0'; // Poner el "flag"
    }
}

void sendBit(uint8_t bitVal)
{
    if (bitVal) {
        PORTD |= (1 << LED);
        asm volatile("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t");
        PORTD &= ~(1 << LED);
        asm volatile("nop\n\t""nop\n\t""nop\n\t""nop\n\t");
    } else {
        PORTD |= (1 << LED);
        asm volatile("nop\n\t""nop\n\t""nop\n\t");
        PORTD &= ~(1 << LED);
        asm volatile("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t");
    }
}
void sendByte(uint8_t byte)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        sendBit(byte & (1 << (7 - i)));
    }
}

void show(uint8_t (*colors)[3])
{
    cli(); // Deshabilitar interrupciones durante el envío
    for (int i = 0; i < NUM_LEDS; i++)
    {
        sendByte(colors[i][1]); // Verde
        sendByte(colors[i][0]); // Rojo
        sendByte(colors[i][2]); // Azul
    }
    sei(); // Habilitar interrupciones
    _delay_us(60);
}

void setLedRGB(uint8_t (*leds_buf)[3], int ledIndex, uint8_t r, uint8_t g, uint8_t b)
{
    if (ledIndex < 0 || ledIndex >= NUM_LEDS) return;
    leds_buf[ledIndex][0] = r;
    leds_buf[ledIndex][1] = g;
    leds_buf[ledIndex][2] = b;
}

void fillAllLedsRGB(uint8_t r, uint8_t g, uint8_t b)
{
    for (int i = 0; i < NUM_LEDS; i++)
    {
        setLedRGB(leds, i, r, g, b); 
    }
}

void probarColores(void) {
    fillAllLedsRGB(150, 0, 0); // Rojo
    show(leds);
    _delay_ms(500); 
    
    fillAllLedsRGB(0, 150, 0); // Verde
    show(leds);
    _delay_ms(500); 
    
    fillAllLedsRGB(0, 0, 150); // Azul
    show(leds);
    _delay_ms(500);
    
    fillAllLedsRGB(0, 0, 0); // Apagar
    show(leds);
}