#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <util/delay.h>

void setpines(void);
void limpieza(void);
void subirsel(void);
void ar(uint16_t tiemp);
void der(uint16_t tiemp);
void izq(uint16_t tiemp);
void ab(uint16_t tiemp);
void diag_ab_der(uint16_t tiemp);
void diag_ab_IZQ(uint16_t tiemp);
void diag_AR_IZQ(uint16_t tiemp);
void diag_AR_der(uint16_t tiemp);
void bajarsel(void);
void equis(void);
void triangulo(void);

static const uint16_t PRESCALERS[] = {1, 8, 64, 256, 1024};
static const uint8_t CS_BITS[][3] = {
	{0,0,1},
	{0,1,0},
	{0,1,1},
	{1,0,0},
	{1,0,1}
};

static void timer1_set(uint8_t idx){
	TCCR1B &= ~((1<<CS12)|(1<<CS11)|(1<<CS10));
	if(idx > 4) idx = 4;
	if(CS_BITS[idx][0]) TCCR1B |= (1<<CS12);
	if(CS_BITS[idx][1]) TCCR1B |= (1<<CS11);
	if(CS_BITS[idx][2]) TCCR1B |= (1<<CS10);
}

static int elegir_prescalers(uint32_t freq_hz, int *out_idx, uint16_t *out_ocr){
	if(freq_hz == 0) return -1;
	for(int i=0;i<5;++i){
		uint32_t pres = PRESCALERS[i];
		uint64_t denom = (uint64_t)pres * (uint64_t)freq_hz * 2ull;
		if(denom == 0) continue;
		uint64_t ticks = (uint64_t)F_CPU / denom;
		if(ticks == 0) continue;
		if(ticks - 1 <= 0xFFFFu){
			*out_idx = i;
			*out_ocr = (uint16_t)(ticks - 1);
			return 0;
		}
	}
	return -1;
}

int CLK_Y(uint32_t freq_hz){
	int idx;
	uint16_t ocr;
	if(elegir_prescalers(freq_hz, &idx, &ocr) != 0) return -1;
	DDRB |= (1<<PB3);
	PORTB &= ~(1<<PB3);
	TCCR1A = 0;
	TCCR1B = (1<<WGM12);
	timer1_set((uint8_t)idx);
	OCR1A = ocr;
	TIMSK1 |= (1<<OCIE1A);
	return 0;
}

void PARAR_CLK_Y(void){
	TIMSK1 &= ~(1<<OCIE1A);
	TCCR1B &= ~((1<<CS12)|(1<<CS11)|(1<<CS10));
	TCCR1B &= ~((1<<WGM12));
	PORTB &= ~(1<<PB3);
	DDRB &= ~(1<<PB3);
}

int CLK_X(uint32_t freq_hz){
	int idx;
	uint16_t ocr;
	if(elegir_prescalers(freq_hz, &idx, &ocr) != 0) return -1;
	DDRC |= (1<<PC3);
	PORTC &= ~(1<<PC3);
	TCCR1A = 0;
	TCCR1B = (1<<WGM12);
	timer1_set((uint8_t)idx);
	OCR1A = ocr;
	OCR1B = ocr;
	TIMSK1 |= (1<<OCIE1B);
	return 0;
}

void PARAR_CLK_X(void){
	TIMSK1 &= ~(1<<OCIE1B);
	PORTC &= ~(1<<PC3);
	DDRC &= ~(1<<PC3);
}

ISR(TIMER1_COMPA_vect){
	PORTB ^= (1<<PB3);
}

ISR(TIMER1_COMPB_vect){
	PORTC ^= (1<<PC3);
}

int main(void){
	setpines();
	sei();
	triangulo();
	subirsel();
	ab(500);
	equis();
	subirsel();
	ab(500);
	circulo();
	for(;;){
	}
	return 0;
}

void setpines(){
	DDRC |= (1<<PC4) | (1<<PC5) | (1<<PC0);
	DDRB |= (1<<PB4) | (1<<PB5);
	DDRD &= ~((1 << PD2) | (1 << PD3));
	PORTD |= (1 << PD2) | (1 << PD3);
	EICRA &= ~((1<<ISC00)|(1<<ISC10));
	EICRA |=  (1<<ISC01)|(1<<ISC11);
	EIMSK |= (1 << INT0) | (1 << INT1);
}

ISR(INT0_vect) {
	if( (PIND & (1<<PD2)) == 0 ) {
		limpieza();
	}
}

ISR(INT1_vect) {
	if( (PIND & (1<<PD3)) == 0 ) {
		limpieza();
	}
}

void limpieza(){
	PORTC &= ~((1<<PC4) | (1<<PC5));
	PORTB &= ~((1<<PB4) | (1<<PB5));
	PARAR_CLK_X();
	PARAR_CLK_Y();
}

void ab(uint16_t tiemp){
	limpieza();
	CLK_X(1000u);
	PORTC |= (1<<PC5) | (1<<PC4);
	while (tiemp--) {
		_delay_ms(1.0);
	}
	limpieza();
}

void ar(uint16_t tiemp){
	limpieza();
	CLK_X(1000U);
	PORTC &= ~(1<<PC4);
	PORTC |= (1<<PC5);
	while (tiemp--) {
		_delay_ms(1.0);
	}
	limpieza();
}
void der(uint16_t tiemp){
	limpieza();
	CLK_Y(1109u);
	PORTB |= (1<<PB5) | (1<<PB4);
	while (tiemp--) {
		_delay_ms(1.0);
	}
	limpieza();
	
}
void izq(uint16_t tiemp){
	limpieza();
	CLK_Y(1109u);
	PORTB &= ~(1<<PB4);
	PORTB |= (1<<PB5);
	while (tiemp--) {
		_delay_ms(1.0);
	}
	limpieza();
}
void diag_ab_der(uint16_t tiemp){
	limpieza();
	CLK_Y(1109u);
	CLK_X(1000u);
	PORTC |= (1<<PC5) | (1<<PC4);
	PORTB |= (1<<PB5) | (1<<PB4);
	while (tiemp--) {
		_delay_ms(1.0);
	}
	limpieza();
}
void diag_ab_IZQ(uint16_t tiemp){
	CLK_Y(1109u);
	CLK_X(1000u);
	PORTB |= (1<<PB5) | (1<<PB4);
	PORTC &= ~(1<<PC4);
	PORTC |= (1<<PC5);
	while (tiemp--) {
		_delay_ms(1.0);
	}
	limpieza();
}
void diag_AR_IZQ(uint16_t tiemp){
	limpieza();
	CLK_Y(1109u);
	CLK_X(1000u);
	PORTB &= ~(1<<PB4);
	PORTB |= (1<<PB5);
	PORTC &= ~(1<<PC4);
	PORTC |= (1<<PC5);
	while (tiemp--) {
		_delay_ms(1.0);
	}
	limpieza();
}
void diag_AR_der(uint16_t tiemp){
	limpieza();
	CLK_Y(1109u);
	CLK_X(1000u);
	PORTB &= ~(1<<PB4);
	PORTB |= (1<<PB5);
	PORTC |= (1<<PC5) | (1<<PC4);
	while (tiemp--) {
		_delay_ms(1.0);
	}
	limpieza();
	
}
void bajarsel(){
	PORTC &= ~(1<<PC0);
	_delay_ms(200);
}
void subirsel(){
	PORTC |= (1<<PC0);
	_delay_ms(200);
}
void triangulo(){
	bajarsel();
	ab(500);
	der(500);
	diag_AR_IZQ(500);
}
void equis(){
	bajarsel();
	diag_ab_der(500);
	subirsel();
	diag_AR_IZQ(250);
	diag_AR_der(250);
	bajarsel();
	diag_ab_IZQ(500);
}
void circulo(){
	bajarsel();
	izq(60); _delay_ms(10);
	ab(10); _delay_ms(10);
	izq(40); _delay_ms(10);
	ab(10); _delay_ms(10);
	izq(20); _delay_ms(10);
	ab(10); _delay_ms(10);
	izq(30); _delay_ms(10);
	ab(10); _delay_ms(10);
	izq(10); _delay_ms(10);
	ab(10); _delay_ms(10);
	izq(20); _delay_ms(10);
	ab(10); _delay_ms(10);
	izq(10); _delay_ms(10);
	ab(10); _delay_ms(10);
	izq(20); _delay_ms(10);
	ab(10); _delay_ms(10);
	izq(10); _delay_ms(10);
	ab(10); _delay_ms(10);
	izq(10); _delay_ms(10);
	ab(10); _delay_ms(10);
	izq(10); _delay_ms(10);
	ab(10); _delay_ms(10);
	izq(10); _delay_ms(10);
	ab(20); _delay_ms(10);
	izq(10); _delay_ms(10);
	ab(10); _delay_ms(10);
	izq(10); _delay_ms(10);
	ab(20); _delay_ms(100);
	izq(10); _delay_ms(10);
	ab(10); _delay_ms(10);
	izq(10); _delay_ms(10);
	ab(30); _delay_ms(10);
	izq(10); _delay_ms(10);
	ab(20); _delay_ms(10);
	izq(10); _delay_ms(10);
	ab(40); _delay_ms(10);
	izq(10); _delay_ms(10);
	ab(50); _delay_ms(10);

	ab(60); _delay_ms(10);
	der(10); _delay_ms(10);
	ab(40); _delay_ms(10);
	der(10); _delay_ms(10);
	ab(20); _delay_ms(10);
	der(10); _delay_ms(10);
	ab(30); _delay_ms(10);
	der(10); _delay_ms(10);
	ab(10); _delay_ms(10);
	der(10); _delay_ms(10);
	ab(20); _delay_ms(10);
	der(10); _delay_ms(10);
	ab(10); _delay_ms(10);
	der(10); _delay_ms(10);
	ab(20); _delay_ms(10);
	der(10); _delay_ms(10);
	ab(10); _delay_ms(10);
	der(10); _delay_ms(10);
	ab(10); _delay_ms(10);
	der(10); _delay_ms(10);
	ab(10); _delay_ms(10);
	der(10); _delay_ms(10);
	ab(10); _delay_ms(10);
	der(20); _delay_ms(10);
	ab(10); _delay_ms(10);
	der(10); _delay_ms(10);
	ab(10); _delay_ms(10);
	der(20); _delay_ms(10);
	ab(10); _delay_ms(10);
	der(10); _delay_ms(10);
	ab(10); _delay_ms(10);
	der(30); _delay_ms(10);
	ab(10); _delay_ms(10);
	der(20); _delay_ms(10);
	ab(10); _delay_ms(10);
	der(40); _delay_ms(10);
	ab(10); _delay_ms(10);
	der(50); _delay_ms(10);

	der(60); _delay_ms(10);
	ar(10); _delay_ms(10);
	der(40); _delay_ms(10);
	ar(10); _delay_ms(10);
	der(20); _delay_ms(10);
	ar(10); _delay_ms(10);
	der(30); _delay_ms(10);
	ar(10); _delay_ms(10);
	der(10); _delay_ms(10);
	ar(10); _delay_ms(10);
	der(20); _delay_ms(10);
	ar(10); _delay_ms(10);
	der(10); _delay_ms(10);
	ar(10); _delay_ms(10);
	der(20); _delay_ms(10);
	ar(10); _delay_ms(10);
	der(10); _delay_ms(10);
	ar(10); _delay_ms(10);
	der(10); _delay_ms(10);
	ar(10); _delay_ms(10);
	der(10); _delay_ms(10);
	ar(10); _delay_ms(10);
	der(10); _delay_ms(10);
	ar(20); _delay_ms(10);
	der(10); _delay_ms(10);
	ar(10); _delay_ms(10);
	der(10); _delay_ms(10);
	ar(20); _delay_ms(10);
	der(10); _delay_ms(10);
	ar(10); _delay_ms(10);
	der(10); _delay_ms(10);
	ar(30); _delay_ms(10);
	der(10); _delay_ms(10);
	ar(20); _delay_ms(10);
	der(10); _delay_ms(10);
	ar(40); _delay_ms(10);
	der(10); _delay_ms(10);
	ar(50); _delay_ms(10);

	ar(60); _delay_ms(10);
	izq(10); _delay_ms(10);
	ar(40); _delay_ms(10);
	izq(10); _delay_ms(10);
	ar(20); _delay_ms(10);
	izq(10); _delay_ms(10);
	ar(30); _delay_ms(10);
	izq(10); _delay_ms(10);
	ar(10); _delay_ms(10);
	izq(10); _delay_ms(10);
	ar(20); _delay_ms(10);
	izq(10); _delay_ms(10);
	ar(10); _delay_ms(10);
	izq(10); _delay_ms(10);
	ar(20); _delay_ms(10);
	izq(10); _delay_ms(10);
	ar(10); _delay_ms(10);
	izq(10); _delay_ms(10);
	ar(10); _delay_ms(10);
	izq(10); _delay_ms(10);
	ar(10); _delay_ms(10);
	izq(10); _delay_ms(10);
	ar(10); _delay_ms(10);
	izq(20); _delay_ms(10);
	ar(10); _delay_ms(10);
	izq(10); _delay_ms(10);
	ar(10); _delay_ms(10);
	izq(20); _delay_ms(10);
	ar(10); _delay_ms(10);
	izq(10); _delay_ms(10);
	ar(10); _delay_ms(10);
	izq(30); _delay_ms(10);
	ar(10); _delay_ms(10);
	izq(20); _delay_ms(10);
	ar(10); _delay_ms(10);
	izq(40); _delay_ms(10);
	ar(10); _delay_ms(10);
	izq(70); _delay_ms(10);
}