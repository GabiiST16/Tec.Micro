#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>

// Pines del Sensor
#define TRIG_PIN    PD2
#define ECHO_PIN    PD3
#define TRIG_PORT   PORTD
#define TRIG_DDR    DDRD
#define ECHO_PIN_REG PIND

// Pin del LED PWM
#define LED_PIN    PD6
#define LED_DDR     DDRD

// Configuración UART
#define BAUD 9600
#define ubr 103

// Lógica del LED
#define maxdist     80  // Distancia máxima para el brillo del LED
#define distcambio    8   // Cada cuantos cm cambia el brillo
#define cantcambios          10  // Número de niveles de intensidad

// Prototipos de funciones
void hc_sr04_init(void);
uint16_t hc_sr04_get_distance(void);
void timer1_init(void);
void USART_Init(unsigned int ubrr);
void USART_SendString(const char *str);
void pwm_init(void);
void LED_PWM(uint16_t distance);


// Array con 11 valores (nivel 0 = apagado, niveles 1-10 = brillo creciente)
const uint8_t brightness_map[cantcambios + 1] = {
	0, 8, 18, 32, 50, 75, 105, 145, 190, 230, 255
};


// Inicialización del Sensor
void hc_sr04_init() {
	TRIG_DDR |= (1 << TRIG_PIN);
	DDRD &= ~(1 << ECHO_PIN);
	TRIG_PORT &= ~(1 << TRIG_PIN);
}

// Inicialización del Timer1
void timer1_init() {
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1 = 0;
}

// Inicialización de USART
void USART_Init(unsigned int ubrr) {
	UBRR0H = (unsigned char)(ubrr >> 8);
	UBRR0L = (unsigned char)ubrr;
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

// Envío de cadenas por USART
void USART_SendString(const char* str) {
	while (*str) {
		while (!(UCSR0A & (1 << UDRE0)));
		UDR0 = *str++;
	}
}

// Inicializa Timer0 para PWM enPD6 (OC0A)
void pwm_init(void) {
	LED_DDR |= (1 << LED_PIN);
	TCCR0A = (1 << COM0A1) | (1 << WGM01) | (1 << WGM00);
	TCCR0B = (1 << CS01) | (1 << CS00);
	OCR0A = 0;
}


//Mapea la distancia usando la tabla de brillo
void LED_PWM(uint16_t distance) {
	uint8_t brillo;

	if (distance >= maxdist) {
		brillo = 0; // Apagado
		} else {
		uint8_t step = distance / distcambio;
		//más cerca = más brillo
		brillo = cantcambios - step;
	}

	// Asigna el valor de la tabla de consulta al registro PWM
	// brillo será el índice del array (de 0 a 10)
	OCR0A = brightness_map[brillo];
}


// Medir distancia en centímetros (retorna uint16_t)
uint16_t hc_sr04_get_distance() {
	uint16_t pulse_ticks = 0;
	
	TRIG_PORT &= ~(1 << TRIG_PIN);
	_delay_us(2);
	TRIG_PORT |= (1 << TRIG_PIN);
	_delay_us(10);
	TRIG_PORT &= ~(1 << TRIG_PIN);

	uint16_t timeout_counter = 0;
	while (!(ECHO_PIN_REG & (1 << ECHO_PIN))) {
		if (++timeout_counter > 20000) return 0xFFFF;
		_delay_us(2);
	}

	TCNT1 = 0;
	TCCR1B = (1 << CS11);

	while ((ECHO_PIN_REG & (1 << ECHO_PIN))) {
		if (TCNT1 > 60000) {
			TCCR1B = 0;
			return 0xFFFF;
		}
	}

	TCCR1B = 0;
	pulse_ticks = TCNT1;

	float distance_cm = (pulse_ticks * 0.5f) / 58.0f;
	
	return (uint16_t)(distance_cm + 0.5f);
}


// Función Principal
int main(void) {
	USART_Init(ubr);
	hc_sr04_init();
	timer1_init();
	pwm_init();

	char buffer[40];
	USART_SendString("Sistema de medicion iniciado.\n");

	while (1) {
		uint16_t distance = hc_sr04_get_distance();

		if (distance == 0xFFFF) {
			USART_SendString("Error de lectura del sensor.\n");
			LED_PWM(maxdist);
			} else {
			snprintf(buffer, sizeof(buffer), "Distancia: %u cm\n", distance);
			USART_SendString(buffer);
			LED_PWM(distance);
		}
		
		_delay_ms(100);
	}

	return 0;
}