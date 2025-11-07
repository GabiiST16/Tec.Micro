
#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <util/delay.h>

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

int CLK_X(uint32_t freq_hz){
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
	sei();
	return 0;
}

void PARAR_CLK_X(void){
	TIMSK1 &= ~(1<<OCIE1A);
	TCCR1B &= ~((1<<CS12)|(1<<CS11)|(1<<CS10));
	TCCR1B &= ~((1<<WGM12));
	PORTB &= ~(1<<PB3);
	DDRB &= ~(1<<PB3);
}

int CLK_Y(uint32_t freq_hz){
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
	sei();
	return 0;
}

void PARAR_CLK_Y(void){
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
	CLK_Y(1000u);
	_delay_ms(500);
	PARAR_CLK_Y();
	CLK_X(1000u);
	_delay_ms(500);
	PARAR_CLK_X();
	
	return 0;
}
