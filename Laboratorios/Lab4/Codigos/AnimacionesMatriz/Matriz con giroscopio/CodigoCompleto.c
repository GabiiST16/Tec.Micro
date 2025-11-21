#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdint.h> 
#include <util/twi.h> 

#define PIN_MATRIZ 6    // Pin de datos PD6
#define ANCHO 8
#define ALTO 8
#define TOTAL_LEDS (ANCHO * ALTO)


#define PIN_SEMILLA_ADC 2 

#define PIN_BOTON_COLOR PB0
#define PUERTO_BOTON    PORTB
#define DDR_BOTON       DDRB
#define PIN_REG_BOTON   PINB


#define MPU6050_ADDR      0x68 
#define MPU_SCL_PIN       PC5  // Pin SCL 
#define MPU_SDA_PIN       PC4  // Pin SDA 


#define REG_PWR_MGMT_1    0x6B 
#define REG_ACCEL_XOUT_H  0x3B 


#define UMBRAL_INCLINACION 4000




uint8_t matriz[TOTAL_LEDS][3];


void sendBit(uint8_t bitVal);
void sendByte(uint8_t byte);
void show(uint8_t (*colors)[3]);
void setLedRGB(uint8_t (*leds)[3], int ledIndex, uint8_t r, uint8_t g, uint8_t b);
void fillAllLedsRGB(uint8_t r, uint8_t g, uint8_t b);


int coord_a_indice(uint8_t x, uint8_t y);
void ADC_Iniciar(void);
uint8_t ADC_Leer(uint8_t canal);
void colorAleatorio(uint8_t* r, uint8_t* g, uint8_t* b);
unsigned int obtenerSemillaADC(void);


void I2C_Init(void);
void I2C_Start(void);
void I2C_Stop(void);
void I2C_Escribir(uint8_t data);
uint8_t I2C_LeerACK(void);
uint8_t I2C_LeerNACK(void);
void MPU6050_EscribirByte(uint8_t reg, uint8_t data);
void MPU6050_LeerBytes(uint8_t reg, uint8_t* buffer, uint8_t len);
void MPU6050_Init(void);
void MPU6050_LeerAcelerometro(int16_t* ax, int16_t* ay, int16_t* az);



int main(void){
	
	DDRD |= (1 << PIN_MATRIZ); 
	

	DDR_BOTON &= ~(1 << PIN_BOTON_COLOR);
	PUERTO_BOTON |= (1 << PIN_BOTON_COLOR);
	

	DDRC &= ~(1 << PIN_SEMILLA_ADC);
	PORTC &= ~(1 << PIN_SEMILLA_ADC);
	

	ADC_Iniciar(); 
	I2C_Init();   
	MPU6050_Init(); 

	
	unsigned int semilla = obtenerSemillaADC();
	srand(semilla);
	
	
	int led_central = coord_a_indice(4, 5);
	setLedRGB(matriz, led_central, 50, 50, 50);
	show(matriz);
	

	int16_t accl_x, accl_y, accl_z; 
	uint8_t boton_presionado;
	uint8_t r, g, b;
	int indiceLed;
	uint8_t pos_x = 4;
	uint8_t pos_y = 4;


	colorAleatorio(&r, &g, &b);

	while (1){
		//Leer el MPU-6050
		MPU6050_LeerAcelerometro(&accl_x, &accl_y, &accl_z);

		//Leer el botón
		boton_presionado = !(PIN_REG_BOTON & (1 << PIN_BOTON_COLOR));


		
		if (accl_y > UMBRAL_INCLINACION) { 
			if (pos_y > 0) pos_y--;
			} else if (accl_y < -UMBRAL_INCLINACION) { 
			if (pos_y < (ALTO - 1)) pos_y++;
		}

		if (accl_x > UMBRAL_INCLINACION) {
			if (pos_x < (ANCHO - 1)) pos_x++;
			} else if (accl_x < -UMBRAL_INCLINACION) { 
			if (pos_x > 0) pos_x--;
		}
		

		if (boton_presionado) {
			colorAleatorio(&r, &g, &b);
			_delay_ms(50); // Anti-rebote
			while(!(PIN_REG_BOTON & (1 << PIN_BOTON_COLOR))); // Esperar a que se suelte
		}

		// Actualizar la Pantalla
		indiceLed = coord_a_indice(pos_x, pos_y);
		fillAllLedsRGB(0, 0, 0);
		setLedRGB(matriz, indiceLed, r, g, b);
		show(matriz);

		// Control de Velocidad
		_delay_ms(100);
	}
}


void sendBit(uint8_t bitVal){
	if(bitVal){ // Enviar un '1'
		PORTD |= (1 << PIN_MATRIZ);
		asm volatile (
		"nop\n\t""nop\n\t""nop\n\t""nop\n\t"
		"nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t"
		); // ~0.6 us
		PORTD &= ~ (1 << PIN_MATRIZ);
		asm volatile (
		"nop\n\t""nop\n\t""nop\n\t""nop\n\t"
		"nop\n\t""nop\n\t"
		); // ~0.4 us
		}else { // Enviar un '0'
		PORTD |= (1 << PIN_MATRIZ);
		asm volatile (
		"nop\n\t""nop\n\t""nop\n\t"
		); // ~0.2 us
		PORTD &= ~(1 << PIN_MATRIZ);
		asm volatile (
		"nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t"
		"nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t"
		); // ~0.6 us
	}
}

void sendByte(uint8_t byte){
	for(uint8_t i = 0; i < 8; i++){
		sendBit(byte & (1 << (7-i)));
	}
}

void show(uint8_t (*colors)[3]){
	cli(); // Deshabilita interrupciones
	for (int i = 0; i < TOTAL_LEDS; i++){
		sendByte(colors[i][1]); // Verde
		sendByte(colors[i][0]); // Rojo
		sendByte(colors[i][2]); // Azul
	}
	sei(); // Rehabilita interrupciones
	_delay_us(60);
}

void setLedRGB(uint8_t (*leds)[3], int ledIndex, uint8_t r, uint8_t g, uint8_t b){
	if (ledIndex < 0 || ledIndex >= TOTAL_LEDS) return;
	leds[ledIndex][0]=r;
	leds[ledIndex][1]=g;
	leds[ledIndex][2]=b;
}

void fillAllLedsRGB(uint8_t r, uint8_t g, uint8_t b){
	for(int i = 0; i < TOTAL_LEDS; i++){
		setLedRGB(matriz, i, r, g, b);
	}
}


int coord_a_indice(uint8_t x, uint8_t y) {
	if (y >= ALTO || x >= ANCHO) return 0;
	int indice = (y * ANCHO) + x;
	return indice;
}

void ADC_Iniciar(void) {
	ADMUX = (1 << REFS0) | (1 << ADLAR);
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint8_t ADC_Leer(uint8_t canal) {
	ADMUX = (ADMUX & 0xF0) | (canal & 0x0F);
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC));
	return ADCH;
}

void colorAleatorio(uint8_t* r, uint8_t* g, uint8_t* b) {
	*r = rand() % 256;
	*g = rand() % 256;
	*b = rand() % 256;
}
unsigned int obtenerSemillaADC(void) {
	unsigned int semilla = 0;
	for (uint8_t i = 0; i < 32; i++) {
		semilla += ADC_Leer(PIN_SEMILLA_ADC);
		_delay_us(10);
	}
	return semilla;
}



// Inicializa el TWI (I2C) con una velocidad de 100kHz
void I2C_Init(void) {
	TWSR = 0; // Sin pre-escalador
	TWBR = ((F_CPU / 100000UL) - 16) / 2;
	TWCR = (1 << TWEN); // Habilitar TWI
}

// Envía una condición de START
void I2C_Start(void) {
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT))); // Esperar a que se complete
}

// Envía una condición de STOP
void I2C_Stop(void) {
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

// Escribe un byte en el bus I2C
void I2C_Escribir(uint8_t data) {
	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT))); // Esperar a que se complete
}

// Lee un byte del bus y envía ACK (para seguir leyendo)
uint8_t I2C_LeerACK(void) {
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
	while (!(TWCR & (1 << TWINT))); // Esperar a que se complete
	return TWDR;
}

// Lee un byte del bus y envía NACK (para terminar la lectura)
uint8_t I2C_LeerNACK(void) {
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT))); // Esperar a que se complete
	return TWDR;
}

// Escribe un solo byte en un registro del MPU-6050
void MPU6050_EscribirByte(uint8_t reg, uint8_t data) {
	I2C_Start();
	I2C_Escribir((MPU6050_ADDR << 1) | 0); // Dirección + Bit de Escritura (0)
	I2C_Escribir(reg);                     // Dirección del registro
	I2C_Escribir(data);                    // Dato a escribir
	I2C_Stop();
}

// Lee múltiples bytes de registros del MPU-6050
void MPU6050_LeerBytes(uint8_t reg, uint8_t* buffer, uint8_t len) {
	I2C_Start();
	I2C_Escribir((MPU6050_ADDR << 1) | 0); // Dirección + Bit de Escritura (0)
	I2C_Escribir(reg);                     // Apuntar al registro inicial
	
	I2C_Start(); // Repeated Start
	I2C_Escribir((MPU6050_ADDR << 1) | 1); // Dirección + Bit de Lectura (1)
	
	for (uint8_t i = 0; i < len; i++) {
		if (i == len - 1) {
			buffer[i] = I2C_LeerNACK(); // Último byte se lee con NACK
			} else {
			buffer[i] = I2C_LeerACK();  // Bytes intermedios se leen con ACK
		}
	}
	I2C_Stop();
}

void MPU6050_Init(void) {
	// Escribe 0x00 en el registro PWR_MGMT_1 (0x6B) para despertar el sensor
	MPU6050_EscribirByte(REG_PWR_MGMT_1, 0x00);
	_delay_ms(100); // Esperar a que el sensor se estabilice
}

// Lee los 3 ejes del acelerómetro
void MPU6050_LeerAcelerometro(int16_t* ax, int16_t* ay, int16_t* az) {
	uint8_t buffer[6];
	
	MPU6050_LeerBytes(REG_ACCEL_XOUT_H, buffer, 6);
	
	*ax = (int16_t)(buffer[0] << 8) | buffer[1];
	*ay = (int16_t)(buffer[2] << 8) | buffer[3];
	*az = (int16_t)(buffer[4] << 8) | buffer[5];
}