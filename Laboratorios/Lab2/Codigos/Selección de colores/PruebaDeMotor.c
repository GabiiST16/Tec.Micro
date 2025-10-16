#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

// Valores calculados para el pulso del servo
#define SERVO_MIN 90 // Pulso para 0 grados (1ms)
#define SERVO_MAX 600 // Pulso para 180 grados (2ms)

void servo_timer1_init(void) {
	// Configurar el pin de salida para el servo PB1
	DDRB |= (1 << PB1);

	// Configurar Timer1 en modo Fast PWM con TOP en ICR1
	TCCR1A = (1 << COM1A1) | (1 << WGM11);
	
	// WGM13, WGM12: Completa la configuración del Modo 
	// CS11 y CS10: Preescaler de 64 
	TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11) | (1 << CS10);

	// Fijar el valor TOP para obtener un período de 20ms (50 Hz)
	// Con preescaler 64
	ICR1 = 4999;
}

// Función para mover el servo a un ángulo específico (0-180)
void servo_set_angle(uint8_t angle) {
	// Mapear el ángulo (0-180) al rango de pulsos (SERVO_MIN - SERVO_MAX)
	uint16_t pulse_width = SERVO_MIN + ((float)angle / 180.0) * (SERVO_MAX - SERVO_MIN);
	
	// Cargar el nuevo valor de pulso en el registro de comparación
	OCR1A = pulse_width;
}

int main(void) {
	servo_timer1_init();

	// Mover el servo a una posición inicial (90 grados)
	servo_set_angle(90);
	_delay_ms(1000); // Dar tiempo al servo para que llegue

	while (1) {
		// Mover a 0 grados
		servo_set_angle(0);
		_delay_ms(2000);

		// Mover a 90 grados
		servo_set_angle(90);
		_delay_ms(2000);

		// Mover a 180 grados
		servo_set_angle(180);
		_delay_ms(2000);

		// Barrido suave de 0 a 180 grados
		for (uint8_t angle = 0; angle < 180; angle++) {
			servo_set_angle(angle);
			_delay_ms(15);
		}

		// Barrido suave de 180 a 0 grados
		for (uint8_t angle = 180; angle > 0; angle--) {
			servo_set_angle(angle);
			_delay_ms(15);
		}
	}
}