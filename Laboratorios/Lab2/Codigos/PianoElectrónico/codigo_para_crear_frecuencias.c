#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
void crear_frecuencia(float freq, uint16_t duty){
	uint32_t top32;
	uint16_t top;

	DDRB |= (1<<PB1); 

	top32 = (uint32_t)( (float)F_CPU / (8.0f * freq) ) - 1u;
	if (top32 > 0xFFFFu) top = 0xFFFFu;
	else top = (uint16_t)top32;

	ICR1 = top;

	OCR1A = (uint16_t)( ((uint32_t)(top + 1) * duty) / 100u );
	TCCR1A = (1<<COM1A1) | (1<<WGM11);
	TCCR1B = (1<<WGM13) | (1<<WGM12) | (1<<CS11);
}


int main(void)
{
    while(1)
    {
        crear_frecuencia(252.0,50);
		_delay_ms(2500);
		crear_frecuencia(294.0,50);
		_delay_ms(2500);
		crear_frecuencia(330.0, 50);
		_delay_ms(2500);
		crear_frecuencia(349.0, 50);
		_delay_ms(2500);
		crear_frecuencia(392.0, 50);
		_delay_ms(2500);
		crear_frecuencia(440.0, 50);
		_delay_ms(2500);
		crear_frecuencia(494.0, 50);
		_delay_ms(2500);
		crear_frecuencia(523.0, 50);
		_delay_ms(2500);
		//TODO:: Please write your application code 
    }
}