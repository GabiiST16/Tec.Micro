#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#define baud 9600
#define ubr 103

// Función para inicializar el USART 
void USART_Init(unsigned int ubrr) {
    // 1. Fijar el baud rate
    UBRR0H = (unsigned char)(ubrr >> 8);
    UBRR0L = (unsigned char)ubrr;

    // 2. Habilitar el RECEPTOR (RX) y el TRANSMISOR (TX)
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);

    // 3. Fijar el formato de la trama: 8 datos, 1 bit de parada
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

// Función para recibir un byte
unsigned char USART_Receive(void) {
    // Espera a que se reciba un dato (la bandera RXC0 se ponga en 1)
    while (!(UCSR0A & (1 << RXC0)));
    
    // Devuelve el dato recibido del buffer
    return UDR0;
}

// Función para enviar un byte (sin cambios)
void USART_Transmit(unsigned char data) {
    // Espera a que el buffer de transmisión esté vacío
    while (!(UCSR0A & (1 << UDRE0)));
    
    // Pone el dato en el buffer y lo envía
    UDR0 = data;
}

// Función para enviar una cadena de texto (string)
void USART_SendString(const char* str) {
    while (*str) {
        USART_Transmit(*str++);
    }
}

int main(void) {
    // Inicializa el USART
    USART_Init(ubr);
    
    // Envía un mensaje de bienvenida una sola vez
    USART_SendString("Sistema de Eco listo. Escribe algo...\r\n");
    
    unsigned char received_char;
    
    while (1) {
        // 1. Espera y recibe un carácter
        received_char = USART_Receive();
        
        // 2. Envía de vuelta (hace eco) el mismo carácter que recibió
        USART_Transmit(received_char);
        
        // Opcional: Si presionas 'Enter' (retorno de carro), envía una nueva línea también.
        if (received_char == '\r') {
            USART_Transmit('\n');
        }
    }
}