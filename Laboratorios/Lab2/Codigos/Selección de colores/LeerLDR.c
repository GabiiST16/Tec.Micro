#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
//Definiciónes para USART
#define baud 9600
#define ubr 103
//Definicion de colores
#define ADC_AZULinf 158
#define ADC_AZULsup 172
#define ADC_VERDEinf 130
#define ADC_VERDEsup 145
#define ADC_AMARILLOinf 74
#define ADC_AMARILLOsup 100
#define ADC_CELESTEinf 100
#define ADC_CELESTEsup 129
#define AZUL_ESPERADO 163
#define CELESTE_ESPERADO 120
#define VERDE_ESPERADO 135
#define AMARILLO_ESPERADO 99

#define SERVO_MIN 90 // Pulso para 0 grados (1ms)
#define SERVO_MAX 600 // Pulso para 180 grados (2ms)
#define LED 6
#define WIDTH 8
#define HEIGHT 8
#define NUM_LEDS (WIDTH * HEIGHT)

uint8_t leds[NUM_LEDS][3];
typedef enum {
	ROJO, VERDE, AZUL, AMARILLO, VIOLETA, CELESTE, NINGUNO,
} color_detectado;

color_detectado color;

// Prototipos de funciones
color_detectado ADCaColor(uint16_t adc_value);
void USART_Init(unsigned int ubrr);
void USART_Transmit(unsigned char data);
void USART_SendString(const char* str);
void servo_timer1_init(void);
void servo_set_angle(uint8_t angle);
void ADC_Init(void);
uint8_t ADC_Read(uint8_t channel);
void sendBit(uint8_t bitVal);
void sendByte(uint8_t byte);
void show(uint8_t (*colors)[3]);
void setLedRGB(uint8_t (*leds)[3], int ledIndex, uint8_t r, uint8_t g, uint8_t b);
void fillAllLedsRGB(uint8_t r, uint8_t g, uint8_t b);

int main(void) {
	DDRD |= (1 << LED);
	uint16_t adc_average;
	uint32_t adc_sum = 0;
	char buffer[6]; // Buffer para almacenar el valor
	int16_t diferencia;
	USART_Init(ubr);
	ADC_Init();
	servo_timer1_init();
	//Mensaje de bienvenida
	USART_SendString("Sistema de lectura de fotorresistencia iniciado.\r\n");
	_delay_ms(500);
	
	while (1) {
		// 1. Reinicia la suma a cero en cada ciclo del while
		adc_sum = 0;
		
		// 2. Bucle FOR para leer el ADC
		for (uint8_t i = 0; i < 16; i++) {
			// Lee el valor del ADC y lo añade a la suma total
			adc_sum += ADC_Read(0);
		}
		adc_average = adc_sum / 16;
		
		color = ADCaColor(adc_average);
		sprintf(buffer, "%u", adc_average);
		USART_SendString("Valor ADC: ");
		USART_SendString(buffer);
		
		USART_SendString(" - Color detectado: ");
		// Usamos un switch para imprimir el nombre del color
		switch (color) {
			case VERDE:
			USART_SendString("VERDE");
			USART_SendString("\r");
			sprintf(buffer, "%u", VERDE_ESPERADO);
			USART_SendString("Valor esperado: ");
			USART_SendString(buffer);
			USART_SendString("\r");
			diferencia = adc_average - VERDE_ESPERADO; 
			sprintf(buffer, "%d", diferencia);
			USART_SendString("Diferencia: ");
			USART_SendString(buffer);
			servo_set_angle(45);
			fillAllLedsRGB(0,255, 0);
			show(leds);
			break;
			case AZUL:
			USART_SendString("AZUL");
			USART_SendString("\r");
			sprintf(buffer, "%u", AZUL_ESPERADO);
			USART_SendString("Valor esperado: ");
			USART_SendString(buffer);
			USART_SendString("\r");
			diferencia = adc_average - AZUL_ESPERADO;
			sprintf(buffer, "%d", diferencia);
			USART_SendString("Diferencia: ");
			USART_SendString(buffer);
			servo_set_angle(180);
			fillAllLedsRGB(0, 0, 255);
			show(leds);
			break;
			case AMARILLO:
			USART_SendString("AMARILLO");
			USART_SendString("\r");
			sprintf(buffer, "%u", AMARILLO_ESPERADO);
			USART_SendString("Valor esperado: ");
			USART_SendString(buffer);
			USART_SendString("\r");
			diferencia = adc_average - AMARILLO_ESPERADO;
			sprintf(buffer, "%d", diferencia);
			USART_SendString("Diferencia: ");
			USART_SendString(buffer);
			servo_set_angle(0);
			fillAllLedsRGB(255, 255, 0);
			show(leds);
			break;
			case CELESTE:
			USART_SendString("CELESTE");
			USART_SendString("\r");
			sprintf(buffer, "%u", CELESTE_ESPERADO);
			USART_SendString("Valor esperado: ");
			USART_SendString(buffer);
			USART_SendString("\r");
			diferencia = adc_average - CELESTE_ESPERADO;
			sprintf(buffer, "%d", diferencia);
			USART_SendString("Diferencia: ");
			USART_SendString(buffer);
			servo_set_angle(135);
			fillAllLedsRGB(135, 235, 206);
			show(leds);
			break;
			default:
			USART_SendString("Desconocido");
			break;
		}
		USART_SendString("\r\n");
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
	ADMUX = (1 << REFS0) | (1 << ADLAR);

	// ADCSRA: Habilitar el ADC y fijar el preescaler en 128
	// Reloj ADC = 16MHz / 128 = 125kHz (dentro del rango recomendado)
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint8_t ADC_Read(uint8_t channel) {
	ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC));
	return ADCH;
}

color_detectado ADCaColor(uint16_t adc_value){
	if (adc_value >= ADC_AZULinf && adc_value <= ADC_AZULsup){
		return AZUL;
	}
	else if (adc_value >= ADC_VERDEinf && adc_value <= ADC_VERDEsup){
		return VERDE;
	}
	else if (adc_value >= ADC_AMARILLOinf && adc_value <= ADC_AMARILLOsup){
		return AMARILLO;
	}
	else if (adc_value >= ADC_CELESTEinf && adc_value <= ADC_CELESTEsup){
		return CELESTE;
	}
	else {
		return NINGUNO;
	}
}
void servo_timer1_init(void) {
	// Configurar el pin de salida para el servo PB1
	DDRB |= (1 << PB1);

	// Configurar Timer1 en modo Fast PWM con TOP en ICR1
	TCCR1A = (1 << COM1A1) | (1 << WGM11);
	
	// WGM13, WGM12: Completa la configuración del Modo
	// CS11 y CS10: Preescaler de 64
	TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11) | (1 << CS10);

	// Fijar el valor TOP para obtener un período de 20ms (50 Hz)
	// Con preescaler 64
	ICR1 = 4999;
}
// Función para mover el servo a un ángulo específico (0-180)
void servo_set_angle(uint8_t angle) {
	// Mapear el ángulo (0-180) al rango de pulsos (SERVO_MIN - SERVO_MAX)
	uint16_t pulse_width = SERVO_MIN + ((float)angle / 180.0) * (SERVO_MAX - SERVO_MIN);
	
	// Cargar el nuevo valor de pulso en el registro de comparación
	OCR1A = pulse_width;
}
void sendBit(uint8_t bitVal){
	if(bitVal){
		PORTD |= (1 << LED);
		asm volatile (
		"nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t"
		"nop\n\t""nop\n\t""nop\n\t""nop\n\t"
		);
		PORTD &= ~ (1 << LED);
		asm volatile(
		"nop\n\t""nop\n\t""nop\n\t""nop\n\t"
		);
		}else {
		PORTD |= (1 << LED);
		asm volatile(
		"nop\n\t""nop\n\t""nop\n\t"
		);
		PORTD &= ~(1 << LED);
		asm volatile(
		"nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t"
		"nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t"
		);
	}
}
void sendByte(uint8_t byte){
	for(uint8_t i = 0; i < 8; i++){
		sendBit(byte & (1 << (7-i)));
	}
}

void show(uint8_t (*colors)[3]){
	cli();
	for (int i = 0; i < NUM_LEDS; i++){
		sendByte(colors[i][1]);
		sendByte(colors[i][0]);
		sendByte(colors[i][2]);
	}
	sei();
	_delay_us(60);
}

void setLedRGB(uint8_t (*leds)[3], int ledIndex, uint8_t r, uint8_t g, uint8_t b){
	if (ledIndex < 0 || ledIndex >= NUM_LEDS) return;
	leds[ledIndex][0]=r;
	leds[ledIndex][1]=g;
	leds[ledIndex][2]=b;
}
void fillAllLedsRGB(uint8_t r, uint8_t g, uint8_t b){
	for(int i = 0; i < NUM_LEDS; i++){
		setLedRGB(leds, i, r, g, b);
	}
}