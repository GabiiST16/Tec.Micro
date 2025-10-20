#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <stdint.h>

#define BAUD    9600
#define UBRRVAL 103

volatile uint8_t pin;
volatile uint8_t R17;

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
    set_botones();

    UBRR0L = (uint8_t)(UBRRVAL & 0xFF);
    UBRR0H = (uint8_t)(UBRRVAL >> 8);
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

    USART_SendString("Listado de canciones:\r\n");
    USART_SendString("1)JINGLE BELLS\r\n");
    USART_SendString("2)Estrellita\r\n");

    while(1)
    {
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

        
        if (UCSR0A & (1 << RXC0)) {
            R17 = UDR0; 
        } else {
            R17 = 0;    
        }

     
        switch (R17){
            case '1': JINGLE_BELLS(); break;
            case '2': estrellita(); break;
            default: break; 
        }
        _delay_ms(5);
    }
}

/* notas */
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

void set_botones(){
    DDRD &= ~((1<<PD2)|(1<<PD3)|(1<<PD4)|(1<<PD5)|(1<<PD6)|(1<<PD7));
    PORTD &= ~((1<<PD2)|(1<<PD3)|(1<<PD4)|(1<<PD5)|(1<<PD6)|(1<<PD7));
    DDRB &= ~((1<<PB2)|(1<<PB3));
    PORTB &= ~((1<<PB2)|(1<<PB3));
}

void guardar_pin(){
    pin = 0;
    if (PIND & (1<<PD2)) pin = 1;
    else if (PIND & (1<<PD3))
	 pin = 2;
    else if (PIND & (1<<PD4)) 
		pin = 3;
    else if (PIND & (1<<PD5))
		pin = 4;
    else if (PIND & (1<<PD6)) 
		pin = 5;
    else if (PIND & (1<<PD7)) 
		pin = 6;
    else if (PINB & (1<<PB2)) 
		pin = 7;
    else if (PINB & (1<<PB3)) 
		pin = 8;
}

void evaluar_boton1(){
    if (PIND & (1<<PD2)) {
        _delay_ms(20);
        if (PIND & (1<<PD2)) {
            nota_do();
            while (PIND & (1<<PD2));
            _delay_ms(20);
            silencio();
        }
    }
}
void evaluar_boton2(){
    if (PIND & (1<<PD3)) {
        _delay_ms(20);
        if (PIND & (1<<PD3)) {
            nota_re();
            while (PIND & (1<<PD3));
            _delay_ms(20);
            silencio();
        }
    }
}
void evaluar_boton3(){
    if (PIND & (1<<PD4)) {
        _delay_ms(20);
        if (PIND & (1<<PD4)) {
            nota_mi();
            while (PIND & (1<<PD4));
            _delay_ms(20);
            silencio();
        }
    }
}
void evaluar_boton4(){
    if (PIND & (1<<PD5)) {
        _delay_ms(20);
        if (PIND & (1<<PD5)) {
            nota_fa();
            while (PIND & (1<<PD5));
            _delay_ms(20);
            silencio();
        }
    }
}
void evaluar_boton5(){
    if (PIND & (1<<PD6)) {
        _delay_ms(20);
        if (PIND & (1<<PD6)) {
            nota_sol();
            while (PIND & (1<<PD6));
            _delay_ms(20);
            silencio();
        }
    }
}
void evaluar_boton6(){
    if (PIND & (1<<PD7)) {
        _delay_ms(20);
        if (PIND & (1<<PD7)) {
            nota_la();
            while (PIND & (1<<PD7));
            _delay_ms(20);
            silencio();
        }
    }
}
void evaluar_boton7(){
    if (PINB & (1<<PB2)) {
        _delay_ms(20);
        if (PINB & (1<<PB2)) {
            nota_si();
            while (PINB & (1<<PB2));
            _delay_ms(20);
            silencio();
        }
    }
}
void evaluar_boton8(){
    if (PINB & (1<<PB3)) {
        _delay_ms(20);
        if (PINB & (1<<PB3)) {
            nota_do_agudo();
            while (PINB & (1<<PB3));
            _delay_ms(20);
            silencio();
        }
    }
}

void USART_TX(void)
{
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = R17;
}

void USART_SendString(const char *s)
{
    while (*s) {
        R17 = (uint8_t)*s++;
        USART_TX();
    }
}
