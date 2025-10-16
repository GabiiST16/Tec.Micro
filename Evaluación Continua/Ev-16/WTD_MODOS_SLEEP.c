#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>

// Función para entrar en modo de suspensión y ser despertado por el Watchdog Timer
void enter_sleep(uint8_t wdt_prescaler) {
    // Configura el Watchdog Timer para despertar el micro
    wdt_enable(wdt_prescaler); // Establece el tiempo (ej: WDTO_8S para 8 segundos)
    WDTCSR |= (1 << WDIE); // Habilita la interrupción del Watchdog Timer

    sleep_enable(); // Habilita la funcionalidad de suspensión
    sei();          // Habilita las interrupciones globales
    sleep_cpu();    // Pone el microcontrolador a dormir
    sleep_disable();// Deshabilita la suspensión al despertar
    wdt_disable();  // Deshabilita el Watchdog Timer para evitar reinicios
}

int main(void) {

    DDRB |= (1 << DDB0) | (1 << DDB1) | (1 << DDB2) | (1 << DDB3) | (1 << DDB4);

    while (1) {
        // --- MODO ACTIVO ---
        // Encender los 5 LEDs (poner a HIGH los pines PB0 a PB4)
        PORTB |= (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3) | (1 << PB4);
        _delay_ms(5000);

        // Apagar los LEDs
        PORTB &= ~((1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3) | (1 << PB4));

        // --- CICLO DE SUSPENSIÓN (30 segundos) ---

        // 1. MODO IDLE (10 segundos)
        // El modo menos profundo. La CPU se detiene pero los periféricos siguen activos.
        set_sleep_mode(SLEEP_MODE_IDLE);
        enter_sleep(WDTO_8S);
        enter_sleep(WDTO_2S);

        // 2. MODO ADC NOISE REDUCTION (10 segundos)
        // Detiene la CPU y los relojes IO para reducir el ruido en el ADC.
        set_sleep_mode(SLEEP_MODE_ADC);
        enter_sleep(WDTO_8S);
        enter_sleep(WDTO_2S);

        // 3. MODO POWER-DOWN (10 segundos)
        // El modo de más bajo consumo. Casi todo se desactiva.
        set_sleep_mode(SLEEP_MODE_PWR_DOWN);
        enter_sleep(WDTO_8S);
        enter_sleep(WDTO_2S);
    }
}

// Vector de interrupción para el Watchdog Timer.
ISR(WDT_vect) {
    // Simplemente despertar y continuar.
}