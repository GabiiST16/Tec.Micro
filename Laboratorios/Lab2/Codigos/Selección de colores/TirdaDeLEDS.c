#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>

#define LED 6
#define WIDTH 8
#define HEIGHT 8
#define NUM_LEDS (WIDTH * HEIGHT)

uint8_t leds[NUM_LEDS][3]; 

void sendBit(uint8_t bitVal);
void sendByte(uint8_t byte);
void show(uint8_t (*colors)[3]);
void setLedRGB(uint8_t (*leds)[3], int ledIndex, uint8_t r, uint8_t g, uint8_t b);
void fillAllLedsRGB(uint8_t r, uint8_t g, uint8_t b);

int main(void){
	DDRD |= (1 << LED);
	
	while (1){
		// 1. Poner todos los LEDs en color ROJO
		fillAllLedsRGB(255, 0, 0);
		show(leds); // Muestra el color en la tira
		_delay_ms(1000);

		// 2. Poner todos los LEDs en color VERDE
		fillAllLedsRGB(0, 255, 0);
		show(leds); // Muestra el color en la tira
		_delay_ms(1000);
		
		// 3. Poner todos los LEDs en color AZUL
		fillAllLedsRGB(135, 235, 206);
		show(leds); // Muestra el color en la tira
		_delay_ms(1000);
	}
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