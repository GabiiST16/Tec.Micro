/*
 * PRUEBA DEL SENSOR LM35 CON FLOATS (sprintf)
 * Muestra el valor crudo, el voltaje y la temperatura en °C.
 */

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h> // Para sprintf()

// --- Definiciones de USART y ADC ---
#define baud 9600
#define ubr 103
#define SENSOR_ADC_CHAN 1 // El sensor está en A1 (Canal 1)

// --- Buffer para enviar la línea ---
char buffer[80]; // Aumentamos el buffer por si acaso

// --- 1. MÓDULO USART (Tu código) ---
void USART_Init(unsigned int ubrr)
{
    UBRR0H = (unsigned char)(ubrr >> 8);
    UBRR0L = (unsigned char)ubrr;
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void USART_Transmit(unsigned char data)
{
    while (!(UCSR0A & (1 << UDRE0)))
        ;
    UDR0 = data;
}

void USART_SendString(const char *str)
{
    while (*str)
    {
        USART_Transmit(*str++);
    }
}
// --- Fin Módulo USART ---

// --- 2. MÓDULO ADC (Tu código - 8 bits) ---
void ADC_Init(void)
{
    ADMUX = (1 << REFS0) | (1 << ADLAR);                               // 5V Ref, Resultado 8-bit (ADCH)
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Prescaler 128
}

uint8_t ADC_Read(uint8_t channel)
{
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F); // Seleccionar canal
    ADCSRA |= (1 << ADSC);                     // Iniciar conversión
    while (ADCSRA & (1 << ADSC))
        ;        // Esperar
    return ADCH; // Devolver los 8 bits altos
}
// --- Fin Módulo ADC ---

// --- 3. PROGRAMA PRINCIPAL ---
int main(void)
{
    // Inicializar periféricos
    USART_Init(ubr);
    ADC_Init();

    USART_SendString("\r\n--- Prueba del Sensor LM35 (con Floats) ---\r\n");

    uint8_t valor_raw;
    float voltaje;
    float temperaturaC;

    while (1)
    {
        // 1. Leer el valor del sensor (0-255)
        valor_raw = ADC_Read(SENSOR_ADC_CHAN);

        // 2. Calcular voltaje (float)
        // (valor_raw / 255.0) -> Porcentaje de la lectura
        // * 5.0 -> Voltaje (basado en la referencia de 5V)
        voltaje = ((float)valor_raw * 5.0) / 255.0;

        // 3. Calcular temperatura (float)
        // El LM35 da 10mV (0.01V) por grado Celsius
        temperaturaC = voltaje * 100.0;

        // 4. Formatear la cadena de salida con floats
        sprintf(buffer,
                "Crudo: %u | Voltaje: %.2f V | Temp: %.2f C\r\n",
                valor_raw,
                voltaje,
                temperaturaC);

        // 5. Enviar la cadena formateada
        USART_SendString(buffer);

        _delay_ms(500); // Esperar medio segundo
    }
}