#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
void set_pines(){
	DDRD |= (1<<PD2) | (1<<PD3) | (1<<PD4) | (1<<PD5) | (1<<PD6) | (1<<PD7);
	PORTD &= ~((1 << PD2) | (1 << PD3) | (1 << PD4) | (1 << PD5) | (1 << PD6) | (1 << PD7));
}
void limpieza(){
	PORTD &= ~((1 << PD2) | (1 << PD3) | (1 << PD4) | (1 << PD5) | (1 << PD6) | (1 << PD7));
}
void pos(){
	PORTD |= (1<<PD7);
	_delay_ms(10000);
	limpieza();
	PORTD |= (1<<PD4);
	_delay_ms(5000);
	limpieza();
}
void bajarsel(){
	PORTD |= (1<<PD2) | (0<<PD3);
	_delay_ms(1000);
}
void subirsel(){
	PORTD |= (1<<PD3) | (0<<PD2);
	_delay_ms(1000);
	
}
void izq(uint16_t mst){
	limpieza();
	PORTD |= (1<<PD7);
	while (mst--) {
		_delay_ms(1.0);
	}
	limpieza();
}
void der(uint16_t mst){
	limpieza();
	PORTD |= (1<<PD6);
	while (mst--) {
		_delay_ms(1.0);
	}
	limpieza();
}
void ab(uint16_t mst){
	limpieza();
	PORTD |= (1<<PD4);
	while (mst--) {
		_delay_ms(1.0);
	}
	limpieza();
}
void ar(uint16_t mst){
	limpieza();
	PORTD |= (1<<PD5);
	while (mst--) {
		_delay_ms(1.0);
	}
	limpieza();
}
void diag_ab_der(uint16_t mst){
	limpieza();
	PORTD |= (1<<PD4) | (1<<PD6);
	  while (mst--) {
		  _delay_ms(1.0);
	  }
	  limpieza();
}
void diag_ab_IZQ(uint16_t mst){
	limpieza();
	PORTD |= (1<<PD4) | (1<<PD7);
	while (mst--) {
		_delay_ms(1.0);
	}
	limpieza();
}
void diag_AR_der(uint16_t mst){
	limpieza();
	PORTD |= (1<<PD5) | (1<<PD6);
	while (mst--) {
		_delay_ms(1.0);
	}
	limpieza();
}
void diag_AR_IZQ(uint16_t mst){
	limpieza();
	PORTD |= (1<<PD5) | (1<<PD7);
	while (mst--) {
		_delay_ms(1.0);
	}
	limpieza();
}
void equis(){
	bajarsel();
	diag_ab_der(3000);
	subirsel();
	diag_AR_IZQ(1500);
	diag_AR_der(1500);
	bajarsel();
	diag_ab_IZQ(3000);
}
void triangulo(){
	bajarsel();
	ab(3000);
	der(3000);
	diag_AR_IZQ(3000);
}
int main(void){
	set_pines();
	pos();
	triangulo();
    while(1)
    {
        //TODO:: Please write your application code 
    }
}