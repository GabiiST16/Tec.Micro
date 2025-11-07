#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>  
#include <stdlib.h> 
#include <string.h> 

// Definiciones de USART y ADC
#define baud 9600
#define ubr 103
#define SENSOR_ADC_CHAN 1 // El sensor está en A1 (Canal 1)

// Definición de Pines
#define CAL_PORT PORTD
#define CAL_DDR  DDRD
#define CAL_PIN  PD6  // Calefactor en D6

#define VENT_IN1_PORT PORTB
#define VENT_IN1_DDR  DDRB
#define VENT_IN1_PIN  PB1    // Ventilador IN1 en D9

#define VENT_IN2_PORT PORTB
#define VENT_IN2_DDR  DDRB
#define VENT_IN2_PIN  PB0    // Ventilador IN2 en D8

#define VENT_PWM_DDR  DDRB
#define VENT_PWM_PIN  PB3    // Ventilador ENA (PWM) en D11

volatile unsigned long g_millis = 0;
unsigned long tiempoPrevio = 0;
const long intervalo = 1000;

float temperaturaC = 0.0;
int   accionActual = 0;
float idealBajo = 23.0;
float idealAlto = 30.0;

// INICIO DE MODIFICACIÓN (Máquina de Estados)
// 0=Apagado, 1=Calentando, 2=Enfriando(Bajo), 3=Enfriando(Medio), 4=Enfriando(Alto)
int estado_sistema = 1; // Empezar en estado "Calentando"
// FIN DE MODIFICACIÓN

char serial_buffer[100];
#define CMD_BUFFER_SIZE 20
static char cmd_buffer[CMD_BUFFER_SIZE];
static uint8_t cmd_index = 0;

// MÓDULOS DE PERIFÉRICOS
void USART_Init(unsigned int ubrr) {
	UBRR0H = (unsigned char)(ubrr >> 8); UBRR0L = (unsigned char)ubrr;
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}
void USART_Transmit(unsigned char data) {
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = data;
}
void USART_SendString(const char* str) {
	while (*str) { USART_Transmit(*str++); }
}


void ADC_Init(void) {
	ADMUX = (1 << REFS0) | (1 << ADLAR);
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}
uint8_t ADC_Read(uint8_t channel) {
	ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC));
	return ADCH;
}


void init_millis(void) {
	TCCR0A = (1 << WGM01); TCCR0B = (1 << CS01) | (1 << CS00);
	OCR0A = 249; TIMSK0 = (1 << OCIE0A);
}
ISR(TIMER0_COMPA_vect) {
	g_millis++;
}


void pwm_init(void) {
	VENT_PWM_DDR |= (1 << VENT_PWM_PIN);
	TCCR2A = (1 << WGM21) | (1 << WGM20) | (1 << COM2A1);
	TCCR2B = (1 << CS22); OCR2A = 0;
}
void set_pwm_ventilador(uint8_t duty_cycle) {
	OCR2A = duty_cycle;
}

// FUNCIONES DE LÓGICA DE CONTROL

void leerTemperatura(void) {
	uint8_t valor_raw = ADC_Read(SENSOR_ADC_CHAN);
	float voltaje = ((float)valor_raw * 5.0) / 255.0;
	temperaturaC = voltaje * 100.0;
}

// INICIO DE MODIFICACIÓN
void aplicarLogicaControl(void) {
	
	switch (estado_sistema) {
		
		// ESTADO 1: CALENTANDO
		case 1:
		accionActual = 1; // 1 = Calefactor
		CAL_PORT |= (1 << CAL_PIN); // Calefactor ON
		set_pwm_ventilador(0);      // Ventilador OFF
		
		// Condición de salida:
		if (temperaturaC > (idealAlto + 10.0)) { // Supera el límite de peligro
			estado_sistema = 4; // Pasar a Enfriar (Alto)
		}
		break;
		
		// ESTADO 4: ENFRIANDO (ALTO)
		case 4:
		accionActual = 4; // 4 = Fan Alto
		CAL_PORT &= ~(1 << CAL_PIN); // Calefactor OFF
		set_pwm_ventilador(255);      // Ventilador ALTO
		
		// Condición de salida:
		if (temperaturaC <= (idealAlto + 10.0)) {
			estado_sistema = 3; // Pasar a Enfriar (Medio)
		}
		break;
		
		// ESTADO 3: ENFRIANDO (MEDIO)
		case 3:
		accionActual = 3; // 3 = Fan Medio
		set_pwm_ventilador(170);      // Ventilador MEDIO
		
		// Condición de salida:
		if (temperaturaC <= (idealAlto + 5.0)) {
			estado_sistema = 2; // Pasar a Enfriar (Bajo)
		}
		break;
		
		// ESTADO 2: ENFRIANDO (BAJO)
		case 2:
		accionActual = 2; // 2 = Fan Bajo
		set_pwm_ventilador(85);       // Ventilador BAJO
		
		// Condición de salida:
		if (temperaturaC <= idealAlto) {
			estado_sistema = 0; // Pasar a Apagado
		}
		break;
		
		// ESTADO 0: APAGADO (ESPERANDO A ENFRIAR)
		case 0:
		accionActual = 0; // 0 = Apagado
		set_pwm_ventilador(0);      // Ventilador OFF
		
		// Condición de salida:
		if (temperaturaC < idealBajo) {
			estado_sistema = 1; // Reiniciar el ciclo, volver a calentar
		}
		break;
	}
}
void enviarDatosSerial(void) {
	
	sprintf(serial_buffer,
	"%.2f,%d,%.2f,%.2f\r\n",
	temperaturaC,
	accionActual,
	idealBajo,
	idealAlto);
	USART_SendString(serial_buffer);
}


void procesarComando(void) {
	if (cmd_buffer[0] == 'S' || cmd_buffer[0] == 's') {
		char* token;
		token = strtok(cmd_buffer, ",");
		token = strtok(NULL, ",");
		if (token != NULL) { idealBajo = strtof(token, NULL); }
		token = strtok(NULL, ",");
		if (token != NULL) { idealAlto = strtof(token, NULL); }
		
		sprintf(serial_buffer, "\r\nOK: Nuevo rango: %.2f - %.2f C\r\n", idealBajo, idealAlto);
		USART_SendString(serial_buffer);
	}
	cmd_index = 0;
	cmd_buffer[0] = '\0';
}
void revisarComandosSerial(void) {
	if (UCSR0A & (1 << RXC0)) {
		char c = UDR0;
		if (c == '\r' || c == '\n') {
			if (cmd_index > 0) {
				cmd_buffer[cmd_index] = '\0';
				procesarComando();
				} else { cmd_index = 0; }
			}
			else if ((c == '\b' || c == 0x7F) && cmd_index > 0) { cmd_index--; }
			else if (cmd_index < (CMD_BUFFER_SIZE - 1)) { cmd_buffer[cmd_index++] = c; }
		}
	}

	int main(void) {
		init_millis();
		USART_Init(ubr);
		ADC_Init();
		pwm_init();
		
		CAL_DDR      |= (1 << CAL_PIN);
		VENT_IN1_DDR |= (1 << VENT_IN1_PIN);
		VENT_IN2_DDR |= (1 << VENT_IN2_PIN);
		
		CAL_PORT &= ~(1 << CAL_PIN);
		VENT_IN1_PORT |= (1 << VENT_IN1_PIN);
		VENT_IN2_PORT &= ~(1 << VENT_IN2_PIN);
		
		sei();

		USART_SendString("\r\nSistema de Control (MODO DEMO AUTO)\r\n");
		sprintf(serial_buffer, "Rango inicial: %.2f - %.2f C\r\n", idealBajo, idealAlto);
		USART_SendString(serial_buffer);
		USART_SendString("Defina un nuevo rango en Python...\r\n");

		while (1) {
			revisarComandosSerial();
			
			if (g_millis - tiempoPrevio >= intervalo) {
				tiempoPrevio = g_millis;
				
				leerTemperatura();
				aplicarLogicaControl();
				
				enviarDatosSerial();
			}
		}
	}