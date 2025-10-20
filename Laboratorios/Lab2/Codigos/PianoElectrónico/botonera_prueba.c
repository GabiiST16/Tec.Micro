#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

volatile uint8_t pin;

void guardar_pin(){
	pin = 0;

	if (PIND & (1<<PD2)){
		pin = 1;
	}
	else if (PIND & (1<<PD3)){
		pin = 2;
	}
	else if (PIND & (1<<PD4)){
		pin = 3;
	}
	else if (PIND & (1<<PD5)){
		pin = 4;
	}
	else if (PIND & (1<<PD6)){
		pin = 5;
	}
	else if (PIND & (1<<PD7)){
		pin = 6;
	}
	else if (PINB & (1<<PB2)){
		pin = 7;
	}
	else if (PINB & (1<<PB3)){
		pin = 8;
	}
}

void evaluar_boton1(){
	if (PIND & (1<<PD2)) {
		_delay_ms(20);
		if (PIND & (1<<PD2)) {
			PORTB |= (1<<PB5);
			while (PIND & (1<<PD2));
			_delay_ms(20);
			PORTB &= ~(1<<PB5);
		}
	}
}
void evaluar_boton2(){
	if (PIND & (1<<PD3)) {
		_delay_ms(20);
		if (PIND & (1<<PD3)) {
			PORTB |= (1<<PB5);
			while (PIND & (1<<PD3));
			_delay_ms(20);
			PORTB &= ~(1<<PB5);
		}
	}
}
void evaluar_boton3(){
	if (PIND & (1<<PD4)) {
		_delay_ms(20);
		if (PIND & (1<<PD4)) {
			PORTB |= (1<<PB5);
			while (PIND & (1<<PD4));
			_delay_ms(20);
			PORTB &= ~(1<<PB5);
		}
	}
}
void evaluar_boton4(){
	if (PIND & (1<<PD5)) {
		_delay_ms(20);
		if (PIND & (1<<PD5)) {
			PORTB |= (1<<PB5);
			while (PIND & (1<<PD5));
			_delay_ms(20);
			PORTB &= ~(1<<PB5);
		}
	}
}
void evaluar_boton5(){
	if (PIND & (1<<PD6)) {
		_delay_ms(20);
		if (PIND & (1<<PD6)) {
			PORTB |= (1<<PB5);
			while (PIND & (1<<PD6));
			_delay_ms(20);
			PORTB &= ~(1<<PB5);
		}
	}
}
void evaluar_boton6(){
	if (PIND & (1<<PD7)) {
		_delay_ms(20);
		if (PIND & (1<<PD7)) {
			PORTB |= (1<<PB5);
			while (PIND & (1<<PD7));
			_delay_ms(20);
			PORTB &= ~(1<<PB5);
		}
	}
}
void evaluar_boton7(){
	if (PINB & (1<<PB2)) {
		_delay_ms(20);
		if (PINB & (1<<PB2)) {
			PORTB |= (1<<PB5);
			while (PINB & (1<<PB2));
			_delay_ms(20);
			PORTB &= ~(1<<PB5);
		}
	}
}
void evaluar_boton8(){
	if (PINB & (1<<PB3)) {
		_delay_ms(20);
		if (PINB & (1<<PB3)) {
			PORTB |= (1<<PB5);
			while (PINB & (1<<PB3));
			_delay_ms(20);
			PORTB &= ~(1<<PB5);
		}
	}
}

int main(void){
	DDRD &= ~((1<<PD2)|(1<<PD3)|(1<<PD4)|(1<<PD5)|(1<<PD6)|(1<<PD7));
	PORTD &= ~((1<<PD2)|(1<<PD3)|(1<<PD4)|(1<<PD5)|(1<<PD6)|(1<<PD7));

	DDRB &= ~((1<<PB2)|(1<<PB3));
	PORTB &= ~((1<<PB2)|(1<<PB3));

	DDRB |= (1<<PB5);
	PORTB &= ~(1<<PB5);

	pin = 0;

	while(1){
		guardar_pin();
		switch (pin){
			case 1: evaluar_boton1(); break;
			case 2: evaluar_boton2(); break;
			case 3: evaluar_boton3(); break;
			case 4: evaluar_boton4(); break;
			case 5: evaluar_boton5(); break;
			case 6: evaluar_boton6(); break;
			case 7: evaluar_boton7(); break;
			case 8: evaluar_boton8(); break;
			default: break;
		}
		_delay_ms(5);
	}
}
