#include <xc.h>

// Configura��es do microcontrolador
#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config CP = OFF

#define _XTAL_FREQ 20000000 // Frequ�ncia do oscilador (20 MHz)

// Vari�veis globais
volatile unsigned char estado_led_rd0 = 0;
volatile unsigned char estado_led_rd1 = 0;
volatile unsigned char estado_led_rd2 = 0;
volatile unsigned char estado_anterior_rb7 = 0;
volatile unsigned int contador_timer2 = 0;

// Rotina de interrup��o
void __interrupt() interrupcao(void) {
    // Interrup��o do Timer2 (pisca LED RD0)
    if (PIR1bits.TMR2IF) {
        PIR1bits.TMR2IF = 0; // Limpa a flag
        contador_timer2++;
        if (contador_timer2 >= 156) { // Aproximadamente 1s
            contador_timer2 = 0;
            estado_led_rd0 = ~estado_led_rd0;
            PORTD ^= 0x01; // Alterna RD0
        }
    }

    // Interrup��o externa em RB0 (borda de subida)
    if (INTCONbits.INTF) {
        INTCONbits.INTF = 0; // Limpa a flag
        estado_led_rd1 = ~estado_led_rd1;
        PORTD ^= 0x02; // Alterna RD1
    }

    // Interrup��o de mudan�a de estado em RB7
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
    // Configura��o dos pinos
    TRISD = 0x00; // PORTD como sa�da
    TRISB = 0x01; // RB0 e RB7 como entrada
    PORTD = 0x00;
    estado_anterior_rb7 = PORTBbits.RB7;

    // Configura��o do Timer2
    T2CON = 0b00000111; // Prescaler 1:16, p�s-escaler 1:16, liga Timer2
    PR2 = 250; // Per�odo do Timer2

    // Configura��o das interrup��es
    INTCON = 0b11011000; // Habilita interrup��o global, perif�rica, RB0 e RB7
    PIE1 = 0b00000010;   // Habilita interrup��o do Timer2
    
 

    PIR1bits.TMR2IF = 0; // Limpa a flag do Timer2

    while (1);
}
