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
	
	}
}
void nota_do(){
	crear_frecuencia(262.0, 50);
}
void nota_re(){
	crear_frecuencia(294.0, 50);
}
void nota_mi(){
	crear_frecuencia(330.0, 50);
}
void nota_fa(){
	crear_frecuencia(349.0, 50);
}
void nota_sol(){
	crear_frecuencia(392.0, 50);
}
void nota_la(){
	crear_frecuencia(440.0, 50);
}
void nota_si(){
	crear_frecuencia(494.0, 50);
}
void nota_do_agudo(){
	crear_frecuencia(523.0, 50);
}
void silencio(){
	crear_frecuencia(0.0, 0);
}
void JINGLE_BELLS(){
	nota_mi();
	_delay_ms(500);
	silencio();
	_delay_ms(100);
	nota_mi();
	_delay_ms(500);
	silencio();
	_delay_ms(100);
	nota_mi();
	_delay_ms(1000);
	silencio();
	_delay_ms(100);
	//-----
	nota_mi();
	_delay_ms(500);
	silencio();
	_delay_ms(100);
	nota_mi();
	_delay_ms(500);
	silencio();
	_delay_ms(100);
	nota_mi();
	_delay_ms(1000);
	silencio();
	_delay_ms(100);
	//------
	nota_mi();
	_delay_ms(500);
	silencio();
	_delay_ms(100);
	nota_sol();
	_delay_ms(500);
	silencio();
	_delay_ms(100);
	nota_do();
	_delay_ms(500);
	silencio();
	_delay_ms(100);
	nota_re();
	_delay_ms(500);
	silencio();
	_delay_ms(100);
	//-----
	nota_mi();
	_delay_ms(2000);
	silencio();
	_delay_ms(100);
	//-----
	nota_fa();
	_delay_ms(500);
	silencio();
	_delay_ms(100);
	nota_fa();
	_delay_ms(500);
	silencio();
	_delay_ms(100);
	nota_fa();
	_delay_ms(1000);
	silencio();
	_delay_ms(100);
	//-------
	nota_mi();
	_delay_ms(500);
	silencio();
	_delay_ms(100);
	nota_mi();
	_delay_ms(500);
	silencio();
	_delay_ms(100);
	nota_mi();
	_delay_ms(1000);
	silencio();
	_delay_ms(100);
	//-----
	nota_re();
	_delay_ms(500);
	silencio();
	_delay_ms(100);
	nota_re();
	_delay_ms(500);
	silencio();
	_delay_ms(100);
	nota_re();
	_delay_ms(500);
	silencio();
	_delay_ms(100);
	nota_mi();
	_delay_ms(500);
	silencio();
	_delay_ms(100);
	//-----
	nota_re();
	_delay_ms(1000);
	silencio();
	_delay_ms(100);
	nota_sol();
	_delay_ms(1000);
	silencio();
	_delay_ms(100);
} 
void estrellita(){
	nota_do();
	_delay_ms(500);
	silencio();
	_delay_ms(100);
	nota_do();
	_delay_ms(500);
	silencio();
	_delay_ms(100);
	nota_sol();
	_delay_ms(500);
	silencio();
	_delay_ms(100);
	nota_sol();
	_delay_ms(500);
	silencio();
	_delay_ms(100);
	//----
	nota_la();
	_delay_ms(500);
	silencio();
	_delay_ms(100);
	nota_la();
	_delay_ms(500);
	silencio();
	_delay_ms(100);
	nota_sol();
	_delay_ms(1000);
	silencio();
	_delay_ms(100);
	//----
	nota_fa();
	_delay_ms(500);
	silencio();
	_delay_ms(100);
	nota_fa();
	_delay_ms(500);
	silencio();
	_delay_ms(100);
	nota_mi();
	_delay_ms(500);
	silencio();
	_delay_ms(100);
	nota_mi();
	_delay_ms(500);
	silencio();
	_delay_ms(100);
	//------
	nota_re();
	_delay_ms(500);
	silencio();
	_delay_ms(100);
	nota_re();
	_delay_ms(500);
	silencio();
	_delay_ms(100);
	nota_do();
	_delay_ms(1000);
	silencio();
	_delay_ms(100);
	//-----
	nota_sol();
	_delay_ms(500);
	silencio();
	_delay_ms(100);
	nota_sol();
	_delay_ms(500);
	silencio();
	_delay_ms(100);
	nota_fa();
	_delay_ms(500);
	silencio();
	_delay_ms(100);
	nota_fa();
	_delay_ms(500);
	silencio();
	_delay_ms(100);
	//----
	nota_mi();
	_delay_ms(500);
	silencio();
	_delay_ms(100);
	nota_mi();
	_delay_ms(500);
	silencio();
	_delay_ms(100);
	nota_re();
	_delay_ms(1000);
	silencio();
	_delay_ms(100);
	//---
	nota_sol();
	_delay_ms(500);
	silencio();
	_delay_ms(100);
	nota_sol();
	_delay_ms(500);
	silencio();
	_delay_ms(100);
	nota_fa();
	_delay_ms(500);
	silencio();
	_delay_ms(100);
	nota_fa();
	_delay_ms(500);
	silencio();
	_delay_ms(100);
	//---
	nota_mi();
	_delay_ms(500);
	silencio();
	_delay_ms(100);
	nota_mi();
	_delay_ms(500);
	silencio();
	_delay_ms(100);
	nota_re();
	_delay_ms(1000);
	silencio();
	_delay_ms(100);
}