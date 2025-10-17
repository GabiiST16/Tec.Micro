#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

#define BAUD    9600
#define UBRRVAL 103

volatile uint8_t R17;

void set_pines(void);
void limpieza(void);
void USART_TX(void);
void USART_RX(void);
void USART_SendString(const char *s);
void pos(void);
void bajarsel(void);
void subirsel(void);
void izq(uint16_t mst);
void der(uint16_t mst);
void ab(uint16_t mst);
void ar(uint16_t mst);
void diag_ab_der(uint16_t mst);
void diag_ab_IZQ(uint16_t mst);
void diag_AR_der(uint16_t mst);
void diag_AR_IZQ(uint16_t mst);
void equis(void);
void triangulo(void);
void circulo(void);
void pintar(uint16_t largoab,uint16_t largoder);
void raton(void);
void gato(void);
void todos(void);

void set_pines(){
	DDRD |= (1<<PD2) | (1<<PD3) | (1<<PD4) | (1<<PD5) | (1<<PD6) | (1<<PD7);
	PORTD &= ~((1 << PD2) | (1 << PD3) | (1 << PD4) | (1 << PD5) | (1 << PD6) | (1 << PD7));
}
void limpieza(){
	PORTD &= ~((1 << PD2) | (1 << PD3) | (1 << PD4) | (1 << PD5) | (1 << PD6) | (1 << PD7));
}
void USART_TX(void)
{
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = R17;
}

void USART_RX(void)
{
	while (!(UCSR0A & (1 << RXC0)));
	R17 = UDR0;
}

void USART_SendString(const char *s)
{
	while (*s) {
		R17 = (uint8_t)*s++;
		USART_TX();
	}
}

void pos(){
	PORTD |= (1<<PD7);
	_delay_ms(10000);
	limpieza();
	PORTD |= (1<<PD4);
	_delay_ms(2500);
	limpieza();
}
void bajarsel(){
	PORTD |= (1<<PD2);
	_delay_ms(1000);
}
void subirsel(){
	PORTD |= (1<<PD3);
	_delay_ms(1000);
}
void izq(uint16_t mst){
	limpieza();
	PORTD |= (1<<PD7);
	while (mst--) {
		_delay_ms(1);
	}
	limpieza();
}
void der(uint16_t mst){
	limpieza();
	PORTD |= (1<<PD6);
	while (mst--) {
		_delay_ms(1);
	}
	limpieza();
}
void ab(uint16_t mst){
	limpieza();
	PORTD |= (1<<PD4);
	while (mst--) {
		_delay_ms(1);
	}
	limpieza();
}
void ar(uint16_t mst){
	limpieza();
	PORTD |= (1<<PD5);
	while (mst--) {
		_delay_ms(1);
	}
	limpieza();
}
void diag_ab_der(uint16_t mst){
	limpieza();
	PORTD |= (1<<PD4) | (1<<PD6);
	while (mst--) {
		_delay_ms(1);
	}
	limpieza();
}
void diag_ab_IZQ(uint16_t mst){
	limpieza();
	PORTD |= (1<<PD4) | (1<<PD7);
	while (mst--) {
		_delay_ms(1);
	}
	limpieza();
}
void diag_AR_der(uint16_t mst){
	limpieza();
	PORTD |= (1<<PD5) | (1<<PD6);
	while (mst--) {
		_delay_ms(1);
	}
	limpieza();
}
void diag_AR_IZQ(uint16_t mst){
	limpieza();
	PORTD |= (1<<PD5) | (1<<PD7);
	while (mst--) {
		_delay_ms(1);
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
void circulo(){
	bajarsel();
	izq(300); _delay_ms(100);
	ab(50); _delay_ms(100);
	izq(200); _delay_ms(100);
	ab(50); _delay_ms(100);
	izq(100); _delay_ms(100);
	ab(50); _delay_ms(100);
	izq(150); _delay_ms(100);
	ab(50); _delay_ms(100);
	izq(50); _delay_ms(100);
	ab(50); _delay_ms(100);
	izq(100); _delay_ms(100);
	ab(50); _delay_ms(100);
	izq(50); _delay_ms(100);
	ab(50); _delay_ms(100);
	izq(100); _delay_ms(100);
	ab(50); _delay_ms(100);
	izq(50); _delay_ms(100);
	ab(50); _delay_ms(100);
	izq(50); _delay_ms(100);
	ab(50); _delay_ms(100);
	izq(50); _delay_ms(100);
	ab(50); _delay_ms(100);
	izq(50); _delay_ms(100);
	ab(100); _delay_ms(100);
	izq(50); _delay_ms(100);
	ab(50); _delay_ms(100);
	izq(50); _delay_ms(100);
	ab(100); _delay_ms(100);
	izq(50); _delay_ms(100);
	ab(50); _delay_ms(100);
	izq(50); _delay_ms(100);
	ab(150); _delay_ms(100);
	izq(50); _delay_ms(100);
	ab(100); _delay_ms(100);
	izq(50); _delay_ms(100);
	ab(200); _delay_ms(100);
	izq(50); _delay_ms(100);
	ab(250); _delay_ms(100);

	ab(300); _delay_ms(100);
	der(50); _delay_ms(100);
	ab(200); _delay_ms(100);
	der(50); _delay_ms(100);
	ab(100); _delay_ms(100);
	der(50); _delay_ms(100);
	ab(150); _delay_ms(100);
	der(50); _delay_ms(100);
	ab(50); _delay_ms(100);
	der(50); _delay_ms(100);
	ab(100); _delay_ms(100);
	der(50); _delay_ms(100);
	ab(50); _delay_ms(100);
	der(50); _delay_ms(100);
	ab(100); _delay_ms(100);
	der(50); _delay_ms(100);
	ab(50); _delay_ms(100);
	der(50); _delay_ms(100);
	ab(50); _delay_ms(100);
	der(50); _delay_ms(100);
	ab(50); _delay_ms(100);
	der(50); _delay_ms(100);
	ab(50); _delay_ms(100);
	der(100); _delay_ms(100);
	ab(50); _delay_ms(100);
	der(50); _delay_ms(100);
	ab(50); _delay_ms(100);
	der(100); _delay_ms(100);
	ab(50); _delay_ms(100);
	der(50); _delay_ms(100);
	ab(50); _delay_ms(100);
	der(150); _delay_ms(100);
	ab(50); _delay_ms(100);
	der(100); _delay_ms(100);
	ab(50); _delay_ms(100);
	der(200); _delay_ms(100);
	ab(50); _delay_ms(100);
	der(250); _delay_ms(100);

	der(300); _delay_ms(100);
	ar(50); _delay_ms(100);
	der(200); _delay_ms(100);
	ar(50); _delay_ms(100);
	der(100); _delay_ms(100);
	ar(50); _delay_ms(100);
	der(150); _delay_ms(100);
	ar(50); _delay_ms(100);
	der(50); _delay_ms(100);
	ar(50); _delay_ms(100);
	der(100); _delay_ms(100);
	ar(50); _delay_ms(100);
	der(50); _delay_ms(100);
	ar(50); _delay_ms(100);
	der(100); _delay_ms(100);
	ar(50); _delay_ms(100);
	der(50); _delay_ms(100);
	ar(50); _delay_ms(100);
	der(50); _delay_ms(100);
	ar(50); _delay_ms(100);
	der(50); _delay_ms(100);
	ar(50); _delay_ms(100);
	der(50); _delay_ms(100);
	ar(100); _delay_ms(100);
	der(50); _delay_ms(100);
	ar(50); _delay_ms(100);
	der(50); _delay_ms(100);
	ar(100); _delay_ms(100);
	der(50); _delay_ms(100);
	ar(50); _delay_ms(100);
	der(50); _delay_ms(100);
	ar(150); _delay_ms(100);
	der(50); _delay_ms(100);
	ar(100); _delay_ms(100);
	der(50); _delay_ms(100);
	ar(200); _delay_ms(100);
	der(50); _delay_ms(100);
	ar(250); _delay_ms(100);

	ar(300); _delay_ms(100);
	izq(50); _delay_ms(100);
	ar(200); _delay_ms(100);
	izq(50); _delay_ms(100);
	ar(100); _delay_ms(100);
	izq(50); _delay_ms(100);
	ar(150); _delay_ms(100);
	izq(50); _delay_ms(100);
	ar(50); _delay_ms(100);
	izq(50); _delay_ms(100);
	ar(100); _delay_ms(100);
	izq(50); _delay_ms(100);
	ar(50); _delay_ms(100);
	izq(50); _delay_ms(100);
	ar(100); _delay_ms(100);
	izq(50); _delay_ms(100);
	ar(50); _delay_ms(100);
	izq(50); _delay_ms(100);
	ar(50); _delay_ms(100);
	izq(50); _delay_ms(100);
	ar(50); _delay_ms(100);
	izq(50); _delay_ms(100);
	ar(50); _delay_ms(100);
	izq(100); _delay_ms(100);
	ar(50); _delay_ms(100);
	izq(50); _delay_ms(100);
	ar(50); _delay_ms(100);
	izq(100); _delay_ms(100);
	ar(50); _delay_ms(100);
	izq(50); _delay_ms(100);
	ar(50); _delay_ms(100);
	izq(150); _delay_ms(100);
	ar(50); _delay_ms(100);
	izq(100); _delay_ms(100);
	ar(50); _delay_ms(100);
	izq(200); _delay_ms(100);
	ar(50); _delay_ms(100);
	izq(350); _delay_ms(100);
}
void pintar(uint16_t largoab,uint16_t largoder){
	bajarsel();
	while (largoder > 0) {
		largoder--;
		ab(largoab); _delay_ms(50);
		der(10); _delay_ms(50);
		ar(largoab); _delay_ms(50);
	}
}

void raton(){
	bajarsel(); _delay_ms(100);
	izq(1400); _delay_ms(100);
	ar(200); _delay_ms(100);
	izq(600); _delay_ms(100);
	ab(200); _delay_ms(100);
	izq(400); _delay_ms(100);
	ab(200); _delay_ms(100);
	izq(400); _delay_ms(100);
	pintar(200,25); _delay_ms(100);
	izq(400); _delay_ms(100);
	ab(400); _delay_ms(100);
	der(200); _delay_ms(100);
	ab(400); _delay_ms(100);
	der(200); _delay_ms(100);
	ab(200); _delay_ms(100);
	der(200); _delay_ms(100);
	ab(200); _delay_ms(100);
	der(400); _delay_ms(100);
	ab(200); _delay_ms(100);
	der(400); _delay_ms(100);
	ab(400); _delay_ms(100);
	der(400); _delay_ms(100);
	subirsel(); _delay_ms(100);
	izq(400); _delay_ms(100);
	bajarsel(); _delay_ms(100);
	ab(400); _delay_ms(100);
	der(200); _delay_ms(100);
	ab(200); _delay_ms(100);
	der(600); _delay_ms(100);
	ar(400); _delay_ms(100);
	der(1400); _delay_ms(100);
	ab(200); _delay_ms(100);
	der(400); _delay_ms(100);
	ar(200); _delay_ms(100);
	der(600); _delay_ms(100);
	ar(600); _delay_ms(100);
	der(400); _delay_ms(100);
	ar(400); _delay_ms(100);
	der(400); _delay_ms(100);
	ar(200); _delay_ms(100);
	der(200); _delay_ms(100);
	ar(200); _delay_ms(100);
	subirsel(); _delay_ms(100);
	ab(1600); _delay_ms(100);
	izq(1800); _delay_ms(100);
	bajarsel(); _delay_ms(100);
	ab(200); _delay_ms(100);
	der(200); _delay_ms(100);
	ab(200); _delay_ms(100);
	der(600); _delay_ms(100);
	ar(200); _delay_ms(100);
	der(200); _delay_ms(100);
	ar(400); _delay_ms(100);
	izq(400); _delay_ms(100);
	der(400); _delay_ms(100);
	ab(200); _delay_ms(100);
	der(800); _delay_ms(100);
	ar(200); _delay_ms(100);
	der(200); _delay_ms(100);
	ar(200); _delay_ms(100);
	der(200); _delay_ms(100);
	ar(200); _delay_ms(100);
	der(200); _delay_ms(100);
	ar(200); _delay_ms(100);
	der(200); _delay_ms(100);
	ar(400); _delay_ms(100);
	der(600); _delay_ms(100);
	ab(200); _delay_ms(100);
	der(200); _delay_ms(100);
	ab(200); _delay_ms(100);
	der(200); _delay_ms(100);
	ab(200); _delay_ms(100);
	der(200); _delay_ms(100);
	ab(200); _delay_ms(100);
	der(200); _delay_ms(100);
	ab(400); _delay_ms(100);
	izq(200); _delay_ms(100);
	ab(200); _delay_ms(100);
	izq(200); _delay_ms(100);
	ab(200); _delay_ms(100);
	izq(200); _delay_ms(100);
	ab(200); _delay_ms(100);
	izq(200); _delay_ms(100);
	ab(200); _delay_ms(100);
	izq(200); _delay_ms(100);
	ab(200); _delay_ms(100);
	izq(200); _delay_ms(100);
	subirsel(); _delay_ms(100);
	ar(2100); _delay_ms(100);
	izq(250); _delay_ms(100);
	bajarsel(); _delay_ms(100);
	ar(200); _delay_ms(100);
	izq(200); _delay_ms(100);
	ar(200); _delay_ms(100);
	izq(200); _delay_ms(100);
	ar(200); _delay_ms(100);
	izq(200); _delay_ms(100);
	ar(200); _delay_ms(100);
	izq(200); _delay_ms(100);
	ar(200); _delay_ms(100);
	izq(200); _delay_ms(100);
	ar(200); _delay_ms(100);
	izq(200); _delay_ms(100);
	ar(200); _delay_ms(100);
	izq(1800); _delay_ms(100);
	ab(200); _delay_ms(100);
	izq(1000); _delay_ms(100);
	der(400); _delay_ms(100);
	ar(1400); _delay_ms(100);
	der(400); _delay_ms(100);
	diag_AR_der(200); _delay_ms(100);
	ar(400); _delay_ms(100);
	diag_AR_IZQ(200); _delay_ms(100);
	izq(400); _delay_ms(100);
	ar(200); _delay_ms(100);
	izq(700); _delay_ms(100);
	subirsel(); _delay_ms(100);
	ab(400); _delay_ms(100);
	bajarsel(); _delay_ms(100);
	ab(1000); _delay_ms(100);
	subirsel(); _delay_ms(100);
	ar(1400); _delay_ms(100);
	bajarsel(); _delay_ms(100);
	izq(400); _delay_ms(100);
	subirsel(); _delay_ms(100);
	ab(400); _delay_ms(100);
	bajarsel(); _delay_ms(100);
	ab(1000); _delay_ms(100);
	subirsel(); _delay_ms(100);
	ar(1400); _delay_ms(100);
	bajarsel(); _delay_ms(100);
	izq(700); _delay_ms(100);
	ab(200); _delay_ms(100);
	izq(400); _delay_ms(100);
	diag_ab_IZQ(200); _delay_ms(100);
	ab(400); _delay_ms(100);
	diag_ab_der(200); _delay_ms(100);
	der(400); _delay_ms(100);
	ab(1200); _delay_ms(100);
	subirsel(); _delay_ms(100);
	ab(400); _delay_ms(100);
	bajarsel(); _delay_ms(100);
	pintar(200,20); _delay_ms(100);
	subirsel();
}
void gato(){
	bajarsel(); _delay_ms(100);
	izq(400); _delay_ms(100);
	subirsel(); _delay_ms(100);
	ab(400); _delay_ms(100);
	bajarsel(); _delay_ms(100);
	ab(400); _delay_ms(100);
	izq(400); _delay_ms(100);
	ar(400); _delay_ms(100);
	der(500); _delay_ms(100);
	subirsel(); _delay_ms(100);
	ar(400); _delay_ms(100);
	bajarsel(); _delay_ms(100);
	diag_AR_IZQ(600); _delay_ms(100);
	izq(200); _delay_ms(100);
	ab(1800); _delay_ms(100);
	diag_ab_der(600); _delay_ms(100);
	der(600); _delay_ms(100);
	subirsel(); _delay_ms(100);
	izq(600); _delay_ms(100);
	bajarsel(); _delay_ms(100);
	ab(400); _delay_ms(100);
	der(400); _delay_ms(100);
	subirsel(); _delay_ms(100);
	izq(400); _delay_ms(100);
	bajarsel(); _delay_ms(100);
	izq(200); _delay_ms(100);
	diag_ab_IZQ(200); _delay_ms(100);
	diag_ab_der(200); _delay_ms(100);
	der(1200); _delay_ms(100);
	diag_AR_der(400); _delay_ms(100);
	ar(200); _delay_ms(100);
	diag_AR_der(200); _delay_ms(100);
	der(200); _delay_ms(100);
	subirsel(); _delay_ms(100);
	ab(800); _delay_ms(100);
	izq(800); _delay_ms(100);
	bajarsel(); _delay_ms(100);
	der(1200); _delay_ms(100);
	diag_AR_der(200); _delay_ms(100);
	ar(200); _delay_ms(100);
	subirsel(); _delay_ms(100);
	ab(200); _delay_ms(100);
	bajarsel(); _delay_ms(100);
	der(600); _delay_ms(100);
	diag_AR_der(400); _delay_ms(100);
	ar(800); _delay_ms(100);
	diag_AR_IZQ(600); _delay_ms(100);
	izq(200); _delay_ms(100);
	diag_ab_IZQ(200); _delay_ms(100);
	ab(200); _delay_ms(100);
	diag_ab_der(200); _delay_ms(100);
	der(200); _delay_ms(100);
	diag_ab_der(200); _delay_ms(100);
	ab(400); _delay_ms(100);
	diag_ab_IZQ(200); _delay_ms(100);
	izq(400); _delay_ms(100);
	ar(200); _delay_ms(100);
	diag_AR_IZQ(400); _delay_ms(100);
	izq(400); _delay_ms(100);
	diag_AR_IZQ(200); _delay_ms(100);
	diag_ab_IZQ(600); _delay_ms(100);
	izq(400); _delay_ms(100);
	subirsel(); _delay_ms(100);
	der(400); _delay_ms(100);
	diag_AR_der(600); _delay_ms(100);
	bajarsel(); _delay_ms(100);
	ar(400); _delay_ms(100);
	subirsel(); _delay_ms(100);
	izq(400); _delay_ms(100);
	bajarsel(); _delay_ms(100);
	izq(400); _delay_ms(100);
	ar(400); _delay_ms(100);
	der(400); _delay_ms(100);
	ab(500); _delay_ms(100);
	subirsel(); _delay_ms(100);
	izq(700); _delay_ms(100);
	bajarsel(); _delay_ms(100);
	izq(200); _delay_ms(100);
	diag_ab_der(100); _delay_ms(100);
	subirsel(); _delay_ms(100);
	ar(100); _delay_ms(100);
	der(100); _delay_ms(100);
	bajarsel(); _delay_ms(100);
	diag_ab_IZQ(100); _delay_ms(100);
	ab(400); _delay_ms(100);
	der(300); _delay_ms(100);
	ar(150); _delay_ms(100);
	ab(60); _delay_ms(100);
	der(300); _delay_ms(100);
	subirsel(); _delay_ms(100);
	izq(300); _delay_ms(100);
	bajarsel(); _delay_ms(100);
	diag_AR_der(300); _delay_ms(100);
	subirsel(); _delay_ms(100);
	diag_ab_IZQ(300); _delay_ms(100);
	bajarsel(); _delay_ms(100);
	diag_ab_der(300); _delay_ms(100);
	subirsel(); _delay_ms(100);
	diag_AR_IZQ(300); _delay_ms(100);
	ab(90); _delay_ms(100);
	izq(300); _delay_ms(100);
	bajarsel(); _delay_ms(100);
	izq(300); _delay_ms(100);
	ar(150); _delay_ms(100);
	ab(60); _delay_ms(100);
	izq(300); _delay_ms(100);
	subirsel(); _delay_ms(100);
	der(300); _delay_ms(100);
	bajarsel(); _delay_ms(100);
	diag_AR_IZQ(300); _delay_ms(100);
	subirsel(); _delay_ms(100);
	diag_ab_der(300); _delay_ms(100);
	bajarsel(); _delay_ms(100);
	diag_ab_IZQ(300); _delay_ms(100);
	subirsel(); _delay_ms(100);
	diag_AR_der(300); _delay_ms(100);
	ab(90); _delay_ms(100);
	der(300); _delay_ms(100);
	ar(500); _delay_ms(100);
	der(1200); _delay_ms(100);
	bajarsel(); _delay_ms(100);
	ar(1400); _delay_ms(100);
	izq(200); _delay_ms(100);
	diag_ab_IZQ(600); _delay_ms(100);
	izq(400); _delay_ms(100);
}
void todos(){
	izq(21000);
	limpieza();
	ab(2500);
	triangulo();
	subirsel();
	der(4500);
	circulo();
	subirsel();
	der(2000);
	equis();
	subirsel();
	der(6000);
	ar(3000);
	gato();
	subirsel();
	der(3000);
	ab(5500);
	raton();
}

int main(void){
	set_pines();
	UBRR0L = (uint8_t)(UBRRVAL & 0xFF);
	UBRR0H = (uint8_t)(UBRRVAL >> 8);
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
	USART_SendString("seleccione la figura:\r\n");
	USART_SendString("triangulo-1 circulo-2 equis-3 gato-4 raton-5 todo-t:\r\n");
	while(1)
	{
		USART_RX();
		switch(R17){
			case '1': pos(); triangulo(); subirsel(); break;
			case '2': pos(); circulo(); subirsel(); break;
			case '3': pos(); equis(); subirsel(); break;
			case '4': pos(); gato(); subirsel(); break;
			case '5': pos(); raton(); subirsel(); break;
			case 't': todos(); break;
			default: USART_TX(); break;
		}
	}
	return 0;
}
