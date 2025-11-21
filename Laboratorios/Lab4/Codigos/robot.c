// main.c  — Control de 2 motores DC por UART (comandos tipo Arduino)
// uC: ATmega328P @ 16 MHz (Arduino Uno)

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>

// =======================
// Mapeo de pines Arduino
// =======================
// MOTOR 1
// M1A = D5  -> PD5
// M1B = D6  -> PD6
// MOTOR 2
// M2A = D7  -> PD7
// M2B = D8  -> PB0

#define M1A_DDR   DDRD
#define M1A_PORT  PORTD
#define M1A_PIN   PD5

#define M1B_DDR   DDRD
#define M1B_PORT  PORTD
#define M1B_PIN   PD6

#define M2A_DDR   DDRD
#define M2A_PORT  PORTD
#define M2A_PIN   PD7

#define M2B_DDR   DDRB
#define M2B_PORT  PORTB
#define M2B_PIN   PB0

// Pines PWM (velocidad)
// V1 = D3 -> PD3 (OC2B)
// V2 = D11 -> PB3 (OC2A)
#define V1_DDR    DDRD
#define V1_PIN    PD3      // OC2B

#define V2_DDR    DDRB
#define V2_PIN    PB3      // OC2A

// =======================
// UART a 9600 bps
// =======================
void timer1_init_servo() {
	// PB1 (OC1A) como salida
	DDRB |= (1<<PB1);

	// Fast PWM, TOP en ICR1, no invertido en OC1A, prescaler 8
	TCCR1A = (1<<COM1A1) | (1<<WGM11);
	TCCR1B = (1<<WGM13) | (1<<WGM12) | (1<<CS11);

	// 20 ms → 40000 ticks (0.5us/tick) → ICR1 = 39999
	ICR1 = 39999;
}

static void uart_init(void) {
	// 9600 bps @ 16 MHz: UBRR = 103
	uint16_t ubrr = 103;
	UBRR0H = (uint8_t)(ubrr >> 8);
	UBRR0L = (uint8_t)(ubrr & 0xFF);

	// Habilitar RX y TX
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	// Formato: 8 bits datos, 1 stop, sin paridad
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

static void uart_tx_char(char c) {
	while (!(UCSR0A & (1 << UDRE0))) {
		// esperar buffer libre
	}
	UDR0 = c;
}

static char uart_rx_char(void) {
	while (!(UCSR0A & (1 << RXC0))) {
		// esperar dato
	}
	return UDR0;
}

static uint8_t uart_available(void) {
	return (UCSR0A & (1 << RXC0));
}

static void uart_print(const char *s) {
	while (*s) {
		uart_tx_char(*s++);
	}
}

static void uart_println(const char *s) {
	uart_print(s);
	uart_tx_char('\r');
	uart_tx_char('\n');
}

// =======================
// PWM en Timer2 (OC2A/OC2B)
// =======================
static void pwm_init(uint8_t duty) {
	// Configurar pines OC2A (PB3) y OC2B (PD3) como salida
	V1_DDR |= (1 << V1_PIN);  // PD3
	V2_DDR |= (1 << V2_PIN);  // PB3

	// Timer2 en Fast PWM, no-inverting en OC2A y OC2B
	// WGM22:0 = 3 (Fast PWM 0-255)
	TCCR2A = (1 << WGM20) | (1 << WGM21)
	| (1 << COM2A1) | (1 << COM2B1);  // OC2A/OC2B no-inverting
	TCCR2B = (1 << CS22);   // prescaler = 64 (como analogWrite)

	OCR2A = duty;   // PWM en D11
	OCR2B = duty;   // PWM en D3
}

static void pwm_set_speed(uint8_t duty) {
	OCR2A = duty;
	OCR2B = duty;
}

// =======================
// Funciones de motor
// =======================
static void adelante(void) {
	// M1: A=1, B=0
	M1A_PORT |=  (1 << M1A_PIN);
	M1B_PORT &= ~(1 << M1B_PIN);
	// M2: A=1, B=0
	M2A_PORT |=  (1 << M2A_PIN);
	M2B_PORT &= ~(1 << M2B_PIN);
}

static void atras(void) {
	// M1: A=0, B=1
	M1A_PORT &= ~(1 << M1A_PIN);
	M1B_PORT |=  (1 << M1B_PIN);
	// M2: A=0, B=1
	M2A_PORT &= ~(1 << M2A_PIN);
	M2B_PORT |=  (1 << M2B_PIN);
}

static void derecha(void) {
	// Motor izquierdo adelante, derecho atrás (ejemplo)
	M1A_PORT |=  (1 << M1A_PIN);
	M1B_PORT &= ~(1 << M1B_PIN);

	M2A_PORT &= ~(1 << M2A_PIN);
	M2B_PORT |=  (1 << M2B_PIN);
}

static void izquierda(void) {
	// Motor izquierdo atrás, derecho adelante (ejemplo)
	M1A_PORT &= ~(1 << M1A_PIN);
	M1B_PORT |=  (1 << M1B_PIN);

	M2A_PORT |=  (1 << M2A_PIN);
	M2B_PORT &= ~(1 << M2B_PIN);
}

static void detener(void) {
	M1A_PORT &= ~(1 << M1A_PIN);
	M1B_PORT &= ~(1 << M1B_PIN);
	M2A_PORT &= ~(1 << M2A_PIN);
	M2B_PORT &= ~(1 << M2B_PIN);
}
void servo_write(uint8_t angle) {
	// 0° → 1000us, 180° → 2000us
	uint16_t pulse_us = 1000 + (angle * 1000UL)/180;
	uint16_t ticks = pulse_us * 2;  // 0.5us por tick
	OCR1A = ticks;
}

// =======================
// Programa principal
// =======================
int main(void) {
	// --- Configuración de pines de motores como salida ---
	M1A_DDR |= (1 << M1A_PIN);
	M1B_DDR |= (1 << M1B_PIN);
	M2A_DDR |= (1 << M2A_PIN);
	M2B_DDR |= (1 << M2B_PIN);

	// Inicia detenido
	detener();

	// Velocidad inicial
	uint8_t vel = 128;
	pwm_init(vel);
	timer1_init_servo();

	// UART
	uart_init();
	uart_println("Sistema iniciado");

	// Buffer para comando tipo "F", "G", "#200", etc.
	char cmd_buf[32];
	uint8_t idx = 0;

	while (1) {
		if (uart_available()) {
			char c = uart_rx_char();

			if (c != '\n' && c != '\r') {
				if (idx < sizeof(cmd_buf) - 1) {
					cmd_buf[idx++] = c;
				}
				} else {
				// Fin de línea: procesar comando
				cmd_buf[idx] = '\0';
				idx = 0;

				if (cmd_buf[0] == '\0') {
					continue; // línea vacía
				}

				char inicial = cmd_buf[0];

				switch (inicial) {
					case 'F':
					adelante();
					uart_println("Adelante");
					break;
					case 'G':
					atras();
					uart_println("Atras");
					break;
					case 'R':
					derecha();
					uart_println("Derecha");
					break;
					case 'L':
					izquierda();
					uart_println("Izquierda");
					break;
					case 'S':
					detener();
					uart_println("Detener");
					break;
					case 'M':
					servo_write(90);
					servo_write(0);
					uart_println("disparo");
					break;
					case '#': {
						// Desde índice 1 hasta el final es el número
						int nueva_vel = atoi(&cmd_buf[1]);  // 0–255
						if (nueva_vel < 0)   nueva_vel = 0;
						if (nueva_vel > 255) nueva_vel = 255;
						vel = (uint8_t)nueva_vel;
						pwm_set_speed(vel);
						uart_println("Velocidad actualizada");
						break;
					}
					default:
					uart_println("Comando no valido");
					break;
				}
			}
		}

		// Podés poner aquí otras tareas en el loop
		// _delay_ms(1);  // pequeño descanso si querés
	}
}