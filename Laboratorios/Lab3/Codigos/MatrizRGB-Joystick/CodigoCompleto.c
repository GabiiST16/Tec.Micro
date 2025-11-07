#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>

// Definiciones de la Matriz LED
#define PIN_MATRIZ 6    // Pin de datos (PD6)
#define ANCHO 8
#define ALTO 8
#define TOTAL_LEDS (ANCHO * ALTO)

// Definiciones del Joystick (ADC)
#define PIN_JOY_X 0     // Joystick X en ADC0 (PC0)
#define PIN_JOY_Y 1     // Joystick Y en ADC1 (PC1)
#define PIN_BOTON PC3   // Botón del Joystick en PC3
#define PIN_SEMILLA_ADC 2
// Buffer global para los colores de la matriz
uint8_t matriz[TOTAL_LEDS][3];

// Prototipos de Funciones
void sendBit(uint8_t bitVal);
void sendByte(uint8_t byte);
void show(uint8_t (*colors)[3]);
void setLedRGB(uint8_t (*leds)[3], int ledIndex, uint8_t r, uint8_t g, uint8_t b);
void fillAllLedsRGB(uint8_t r, uint8_t g, uint8_t b);

int coord_a_indice(uint8_t x, uint8_t y);
void ADC_Iniciar(void);
uint8_t ADC_Leer(uint8_t canal);
void colorAleatorio(uint8_t* r, uint8_t* g, uint8_t* b);
unsigned int obtenerSemillaADC(void);

int main(void){
	// Configuración de pines
	DDRD |= (1 << PIN_MATRIZ);
	DDRC &= ~(1 << PIN_BOTON);
	PORTC |= (1 << PIN_BOTON);
	
	DDRC &= ~(1 << PIN_SEMILLA_ADC); 
	PORTC &= ~(1 << PIN_SEMILLA_ADC); 
	
	// Iniciar periféricos
	ADC_Iniciar();

	// SECCIÓN DE SEMILLA ALEATORIA
	// Obtenemos la semilla leyendo el ruido del pin ADC2
	unsigned int semilla = obtenerSemillaADC();
	srand(semilla); 
	

	int led_central = coord_a_indice(4, 5);
	setLedRGB(matriz, led_central, 50, 50, 50);
	show(matriz);
	
	uint8_t valor_x, valor_y;
	uint8_t boton_presionado;
	uint8_t r, g, b;
	int indiceLed;
	uint8_t pos_x = 4;
	uint8_t pos_y = 4;

	// Obtener el primer color
	colorAleatorio(&r, &g, &b);

	while (1){
		// 1. Leer el Joystick
		valor_x = ADC_Leer(PIN_JOY_X);
		valor_y = ADC_Leer(PIN_JOY_Y);
		boton_presionado = !(PINC & (1 << PIN_BOTON));

		// Lógica de Movimiento
		if (valor_x > 200) {
			if (pos_x < (ANCHO - 1)) pos_x++;
			} else if (valor_x < 50) {
			if (pos_x > 0) pos_x--;
		}

		if (valor_y > 200) { // Mover Abajo
			if (pos_y < (ALTO - 1)) pos_y++;
			} else if (valor_y < 50) { // Mover Arriba
			if (pos_y > 0) pos_y--;
		}
		
		// Lógica del Botón (Cambio de Color)
		if (boton_presionado) {
			colorAleatorio(&r, &g, &b);
			_delay_ms(50); // Anti-rebote
			while(!(PINC & (1 << PIN_BOTON))); // Esperar a que se suelte
		}

		// Actualizar la Pantalla
		indiceLed = coord_a_indice(pos_x, pos_y);
		fillAllLedsRGB(0, 0, 0);
		setLedRGB(matriz, indiceLed, r, g, b);
		show(matriz);

		// Control de Velocidad
		_delay_ms(100);
	}
}

// Funciones de la Matriz

void sendBit(uint8_t bitVal){
	if(bitVal){ // Enviar un '1'
		PORTD |= (1 << PIN_MATRIZ);
		asm volatile (
		"nop\n\t""nop\n\t""nop\n\t""nop\n\t"
		"nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t"
		); // ~0.6 us
		PORTD &= ~ (1 << PIN_MATRIZ);
		asm volatile (
		"nop\n\t""nop\n\t""nop\n\t""nop\n\t"
		"nop\n\t""nop\n\t"
		); // ~0.4 us
		}else { // Enviar un '0'
		PORTD |= (1 << PIN_MATRIZ);
		asm volatile (
		"nop\n\t""nop\n\t""nop\n\t"
		); // ~0.2 us
		PORTD &= ~(1 << PIN_MATRIZ);
		asm volatile (
		"nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t"
		"nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t"
		); // ~0.6 us
	}
}

void sendByte(uint8_t byte){
	for(uint8_t i = 0; i < 8; i++){
		sendBit(byte & (1 << (7-i)));
	}
}

void show(uint8_t (*colors)[3]){
	cli(); // Deshabilita interrupciones
	for (int i = 0; i < TOTAL_LEDS; i++){
		sendByte(colors[i][1]); // Verde
		sendByte(colors[i][0]); // Rojo
		sendByte(colors[i][2]); // Azul
	}
	sei(); // Rehabilita interrupciones
	_delay_us(60);
}

void setLedRGB(uint8_t (*leds)[3], int ledIndex, uint8_t r, uint8_t g, uint8_t b){
	if (ledIndex < 0 || ledIndex >= TOTAL_LEDS) return;
	leds[ledIndex][0]=r;
	leds[ledIndex][1]=g;
	leds[ledIndex][2]=b;
}

void fillAllLedsRGB(uint8_t r, uint8_t g, uint8_t b){
	for(int i = 0; i < TOTAL_LEDS; i++){
		setLedRGB(matriz, i, r, g, b);
	}
}
// Funciones del Joystick
int coord_a_indice(uint8_t x, uint8_t y) {
	if (y >= ALTO || x >= ANCHO) return 0;
	int indice = (y * ANCHO) + x;
	return indice;
}

void ADC_Iniciar(void) {
	ADMUX = (1 << REFS0) | (1 << ADLAR);
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint8_t ADC_Leer(uint8_t canal) {
	ADMUX = (ADMUX & 0xF0) | (canal & 0x0F);
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC));
	return ADCH;
}

void colorAleatorio(uint8_t* r, uint8_t* g, uint8_t* b) {
	*r = rand() % 256;
	*g = rand() % 256;
	*b = rand() % 256;
}
unsigned int obtenerSemillaADC(void) {
	unsigned int semilla = 0;
	
	// Sumamos 32 lecturas del pin flotante.
	// La suma de este ruido es una gran semilla aleatoria.
	for (uint8_t i = 0; i < 32; i++) {
		// Leemos el valor de 8 bits del pin de ruido
		semilla += ADC_Leer(PIN_SEMILLA_ADC);
		// Pequeña demora para permitir que el ruido varíe
		_delay_us(10);
	}
	return semilla;
}