#include <xc.h>

// Configurações do microcontrolador
#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config CP = OFF

#define _XTAL_FREQ 20000000 // Frequência do oscilador (20 MHz)

// Variáveis globais
volatile unsigned char estado_led_rd0 = 0;
volatile unsigned char estado_led_rd1 = 0;
volatile unsigned char estado_led_rd2 = 0;
volatile unsigned char estado_anterior_rb7 = 0;
volatile unsigned int contador_timer2 = 0;

// Rotina de interrupção
void __interrupt() interrupcao(void) {
    // Interrupção do Timer2 (pisca LED RD0)
    if (PIR1bits.TMR2IF) {
        PIR1bits.TMR2IF = 0; // Limpa a flag
        contador_timer2++;
        if (contador_timer2 >= 156) { // Aproximadamente 1s
            contador_timer2 = 0;
            estado_led_rd0 = ~estado_led_rd0;
            PORTD ^= 0x01; // Alterna RD0
        }
    }

    // Interrupção externa em RB0 (borda de subida)
    if (INTCONbits.INTF) {
        INTCONbits.INTF = 0; // Limpa a flag
        estado_led_rd1 = ~estado_led_rd1;
        PORTD ^= 0x02; // Alterna RD1
    }

    // Interrupção de mudança de estado em RB7
    if (INTCONbits.RBIF) {
        INTCONbits.RBIF = 0; // Limpa a flag
        if (PORTBbits.RB7 != estado_anterior_rb7) {
            estado_led_rd2 = ~estado_led_rd2;
            PORTD ^= 0x04; // Alterna RD2
            estado_anterior_rb7 = PORTBbits.RB7;
        }
    }
}

void main(void) {
    // Configuração dos pinos
    TRISD = 0x00; // PORTD como saída
    TRISB = 0x01; // RB0 e RB7 como entrada
    PORTD = 0x00;
    estado_anterior_rb7 = PORTBbits.RB7;

    // Configuração do Timer2
    T2CON = 0b00000111; // Prescaler 1:16, pós-escaler 1:16, liga Timer2
    PR2 = 250; // Período do Timer2

    // Configuração das interrupções
    INTCON = 0b11011000; // Habilita interrupção global, periférica, RB0 e RB7
    PIE1 = 0b00000010;   // Habilita interrupção do Timer2
    
 

    PIR1bits.TMR2IF = 0; // Limpa a flag do Timer2

    while (1);
}
