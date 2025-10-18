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
void izq(){
	limpieza();
	PORTD |= (1<<PD7);
}
void der(){
	limpieza();
	PORTD |= (1<<PD6);
}
void ab(){
	limpieza();
	PORTD |= (1<<PD4);
}
void ar(){
	limpieza();
	PORTD |= (1<<PD5);
}
void diag_ab_der(){
	limpieza();
	PORTD |= (1<<PD4) | (1<<PD6);
}
void diag_ab_IZQ(){
	limpieza();
	PORTD |= (1<<PD4) | (1<<PD7);
}
void diag_AR_der(){
	limpieza();
	PORTD |= (1<<PD5) | (1<<PD6);
}
void diag_AR_IZQ(){
	limpieza();
	PORTD |= (1<<PD5) | (1<<PD7);
}
void equis(){
	bajarsel();
	diag_ab_der();
	_delay_ms(3000);
	limpieza();
	subirsel();
	diag_AR_IZQ();
	_delay_ms(1500);
	limpieza();
	diag_AR_der();
	_delay_ms(1500);
	limpieza();
	bajarsel();
	diag_ab_IZQ();
	_delay_ms(3000);
	limpieza();
}
int main(void){
	set_pines();
	pos();
	equis();
    while(1)
    {
        //TODO:: Please write your application code 
    }
}