#include <avr/io.h>
#include <stdint.h>

#define F_CPU   16000000UL
#define BAUD    9600
#define UBRRVAL 103

uint8_t R16;
volatile uint8_t R17;

void USART_TX(void);
void USART_RX(void);
void USART_SendString(const char *s);
void PB6_ON(void);
void ALL_OFF(void);
void PB5i(void);
void PB4i(void);
void PBall(void);
void led(void);

int main(void)
{
    DDRB = 0x3F;
    UBRR0L = (uint8_t)(UBRRVAL & 0xFF);
    UBRR0H = (uint8_t)(UBRRVAL >> 8);
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

    USART_SendString("seleccione 1 led, 1 0 2 3 4 5:\r\n");
    for (;;) {
        USART_RX();
        switch (R17) {
            case '1': PB6_ON(); break;
            case '0': ALL_OFF(); break;
            case '2': PB5i(); break;
            case '3': PB4i(); break;
            case '4': PBall(); break;
            case '5': led(); break;
            default: USART_TX(); break;
        }
    }
    return 0;
}

void USART_TX(void)
{
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = R17;
}

void USART_RX(void)
{
    while (!(UCSR0A & (1 << RXC0)));
    R17 = UDR0;
}

void USART_SendString(const char *s)
{
    while (*s) {
        R17 = (uint8_t)*s++;
        USART_TX();
    }
}

void PB6_ON(void)
{
    PORTB = 0x01;
    USART_SendString("ON\r\n");
}

void ALL_OFF(void)
{
    PORTB = 0x02;
    USART_SendString("OFF\r\n");
}

void PB5i(void)
{
    PORTB = 0x04;
}

void PB4i(void)
{
    PORTB = 0x08;
}

void PBall(void)
{
    PORTB = 0x10;
}

void led(void)
{
    PORTB = 0x20;
}
