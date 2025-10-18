#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>

#define LED 6
#define WIDTH 8
#define HEIGHT 8
#define NUM_LEDS (WIDTH * HEIGHT)

// Buffer global para almacenar los colores de los LEDs
uint8_t leds[NUM_LEDS][3];

//Prototipos de Funciones 
void sendBit(uint8_t bitVal);
void sendByte(uint8_t byte);
void show(uint8_t (*colors)[3]);
void setLedRGB(uint8_t (*leds)[3], int ledIndex, uint8_t r, uint8_t g, uint8_t b);
void fillAllLedsRGB(uint8_t r, uint8_t g, uint8_t b); 

int main(void){
	DDRD |= (1 << LED); // Configura el pin del LED como salida
	
	while (1){
		// Poner todos los LEDs en color ROJO
		fillAllLedsRGB(255, 0, 0);
		show(leds);
		_delay_ms(1000);

		// Poner todos los LEDs en color VERDE
		fillAllLedsRGB(0, 255, 0);
		show(leds);
		_delay_ms(1000);
        
        // Poner todos los LEDs en color AZUL
		fillAllLedsRGB(0, 0, 255);
		show(leds);
		_delay_ms(1000);
	}
}

// --- Definiciones de Funciones ---

/**
 * @brief Envía un solo bit a la tira de LEDs con el timing correcto.
 */
void sendBit(uint8_t bitVal){
    // Tiempos ajustados para F_CPU = 16MHz para ser compatibles con WS2812B
	if(bitVal){ // Enviar un '1'
		PORTD |= (1 << LED);
		asm volatile ( 
		"nop\n\t""nop\n\t""nop\n\t""nop\n\t"
		"nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t"
		); // ~0.6 us
		PORTD &= ~ (1 << LED);
        asm volatile ( 
		"nop\n\t""nop\n\t""nop\n\t""nop\n\t"
		"nop\n\t""nop\n\t" 
		); // ~0.4 us
	}else { // Enviar un '0'
		PORTD |= (1 << LED);
		asm volatile ( 
		"nop\n\t""nop\n\t""nop\n\t"
		); // ~0.2 us
		PORTD &= ~(1 << LED);
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
	cli(); // Deshabilita interrupciones para no afectar el timing
	for (int i = 0; i < NUM_LEDS; i++){
		sendByte(colors[i][1]); // Verde
		sendByte(colors[i][0]); // Rojo
		sendByte(colors[i][2]); // Azul
	}
	sei(); // Rehabilita interrupciones
	_delay_us(60); // Envía una señal de reset para que los LEDs muestren los colores
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