#include <xc.h>

// Configuração do microcontrolador
#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config CP = OFF

#define _XTAL_FREQ 20000000 // Oscilador de 20 MHz

// Variáveis globais
volatile unsigned int tempo_motor = 0;
volatile unsigned char motor_ativo = 0;

// Rotina de interrupção
void __interrupt() interrupcao(void) {
    // Timer1 controla o tempo do motor
    if (PIR1bits.TMR1IF) {
        PIR1bits.TMR1IF = 0; // Limpa a flag
        TMR1H = 0x0B; // Recarga para 50ms
        TMR1L = 0xDC;

        if (motor_ativo) {
            tempo_motor--;
            if (tempo_motor == 0) {
                motor_ativo = 0;
                PORTDbits.RD0 = 0; // Desliga o motor
                T1CONbits.TMR1ON = 0; // Para o Timer1
            }
        }
    }
    
    // Botão RB0 (aciona motor por 2s)
    if (INTCONbits.INTF) {
        INTCONbits.INTF = 0; // Limpa a flag
        if (!motor_ativo) {
            tempo_motor = 40; // 2s (40 * 50ms)
            motor_ativo = 1;
            PORTDbits.RD0 = 1; // Liga o motor
            T1CONbits.TMR1ON = 1; // Inicia o Timer1
        }
    }

    // Botão RB1 (aciona motor por 4s)
    if (INTCONbits.RBIF) {
        INTCONbits.RBIF = 0; // Limpa a flag
        if (PORTBbits.RB1 && !motor_ativo) {
            tempo_motor = 80; // 4s (80 * 50ms)
            motor_ativo = 1;
            PORTDbits.RD0 = 1; // Liga o motor
            T1CONbits.TMR1ON = 1; // Inicia o Timer1
        }
    }
}

void main(void) {
    // Configuração dos pinos
    TRISD = 0x00; // PORTD como saída
    TRISB = 0x03; // RB0 e RB1 como entrada
    PORTD = 0x00; // Inicializa motor desligado

    // Configuração do Timer1
    T1CON = 0x31; // Prescaler 1:8
    TMR1H = 0x0B; // Valor de recarga (50ms)
    TMR1L = 0xDC;

    // Configuração das interrupções
    INTCON = 0b11011000; // Habilita interrupção global, periférica, RB0 e RB1
    PIE1 = 0b00000001;   // Habilita interrupção do Timer1
    PIR1bits.TMR1IF = 0; // Limpa a flag do Timer1

    while (1);
}
