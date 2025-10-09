#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h> 
#include <stdio.h> 
//Definiciónes para USART
#define baud 9600
#define ubr 103

// Prototipos de funciones
void USART_Init(unsigned int ubrr);
void USART_Transmit(unsigned char data);
void USART_SendString(const char* str);

void ADC_Init(void);
uint16_t ADC_Read(uint8_t channel);


int main(void) {
    uint16_t adc_value;
    char buffer[6]; // Buffer para almacenar el valor
    USART_Init(ubr);
    ADC_Init();
    //Mensaje de bienvenida
    USART_SendString("Sistema de lectura de fotorresistencia iniciado.\r\n");
    _delay_ms(500);
    
    while (1) {
        //Lee el valor del canal ADC 0 (donde conectamos la LDR)
        adc_value = ADC_Read(0);
        
        //Convierte el valor numérico
       sprintf(buffer, "%u", adc_value);// Convierte a base 10
        
        //Envía el valor a través de USART
        USART_SendString("Valor del sensor: ");
        USART_SendString(buffer);
        USART_SendString("\r\n"); // Envía un retorno de carro y nueva línea
        
        //Espera un tiempo antes de la siguiente lectura
        _delay_ms(500); 
    }
}

// USART
void USART_Init(unsigned int ubrr) {
    // 1. Fijar el baud rate
    UBRR0H = (unsigned char)(ubrr >> 8);
    UBRR0L = (unsigned char)ubrr;

    // 2. Habilitar el RECEPTOR (RX) y el TRANSMISOR (TX)
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);

    // 3. Fijar el formato de la trama: 8 datos, 1 bit de parada
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void USART_Transmit(unsigned char data) {
    // Espera a que el buffer de transmisión esté vacío
    while (!(UCSR0A & (1 << UDRE0)));
    
    // Pone el dato en el buffer y lo envía
    UDR0 = data;
}

void USART_SendString(const char* str) {
    while (*str) {
        USART_Transmit(*str++);
    }
}


// Funciones para el ADC
void ADC_Init(void) {
    // ADMUX: Seleccionar AVCC (5V) como voltaje de referencia
    ADMUX = (1 << REFS0);

    // ADCSRA: Habilitar el ADC y fijar el preescaler en 128
    // Reloj ADC = 16MHz / 128 = 125kHz (dentro del rango recomendado)
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint16_t ADC_Read(uint8_t channel) {
    // Seleccionar el canal del ADC (limpiando los 4 bits inferiores y asignando el nuevo canal)
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);

    // Iniciar la conversión
    ADCSRA |= (1 << ADSC);

    // Esperar a que la conversión termine (el bit ADSC se pondrá a 0)
    while (ADCSRA & (1 << ADSC));

    // Devolver el resultado de 10 bits
    return ADC;
}