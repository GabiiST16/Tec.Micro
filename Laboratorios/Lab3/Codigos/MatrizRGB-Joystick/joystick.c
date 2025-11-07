#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

// Definiciónes para USART (de tu código)
#define baud 9600
#define ubr 103

// --- Definiciones del Joystick ---
#define PIN_JOY_X 0     // Joystick X en ADC0 (PC0)
#define PIN_JOY_Y 1     // Joystick Y en ADC1 (PC1)
#define PIN_BOTON PC3   // Botón del Joystick en PC3

// Prototipos de funciones
void USART_Init(unsigned int ubrr);
void USART_Transmit(unsigned char data);
void USART_SendString(const char *str);
void ADC_Init(void);
uint8_t ADC_Read(uint8_t channel);


int main(void)
{
	uint8_t valor_x;
	uint8_t valor_y;
	uint8_t boton_presionado;
	
	// Buffer para convertir los números a texto
	// "%u" para un uint8_t (0-255) necesita máx 3 chars + '\0' = 4
	char buffer_texto[5];

	// Inicializaciones
	USART_Init(ubr);
	ADC_Init();
	
	// Configuración del botón (PC3) como entrada con pull-up
	DDRC &= ~(1 << PIN_BOTON);   // Configura PC3 como entrada
	PORTC |= (1 << PIN_BOTON);  // Activa el pull-up interno en PC3

	// Mensaje de bienvenida
	USART_SendString("Monitor de Joystick iniciado.\r\n");
	_delay_ms(500);

	while (1)
	{
		// 1. Leer los valores del Joystick
		valor_x = ADC_Read(PIN_JOY_X);
		valor_y = ADC_Read(PIN_JOY_Y);
		boton_presionado = !(PINC & (1 << PIN_BOTON));

		// --- Impresión usando sprintf (como en tu ejemplo) ---
		
		// 2. Enviar valor X
		USART_SendString("X: ");
		sprintf(buffer_texto, "%u", valor_x); 
		USART_SendString(buffer_texto);

		// 3. Enviar valor Y
		USART_SendString("  |  Y: ");
		sprintf(buffer_texto, "%u", valor_y); 
		USART_SendString(buffer_texto);

		// 4. Enviar estado del Botón
		USART_SendString("  |  Boton: ");
		sprintf(buffer_texto, "%u", boton_presionado); 
		USART_SendString(buffer_texto);

		// Salto de línea
		USART_SendString("\r\n");
		
		// Pequeña demora
		_delay_ms(200);
	}
}

void ADC_Init(void)
{
	ADMUX = (1 << REFS0) | (1 << ADLAR);
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint8_t ADC_Read(uint8_t channel)
{
	ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC))
	;
	return ADCH;
}

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