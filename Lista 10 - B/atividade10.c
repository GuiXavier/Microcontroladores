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

// Protótipos de funções
void configurar_pinos(void);
void configurar_timer1(void);
void configurar_interrupcoes(void);

// Rotina de interrupção
void __interrupt() interrupcao(void) {
    // Timer1 controla o tempo do motor
    if (PIR1bits.TMR1IF) {
        PIR1bits.TMR1IF = 0;
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
        INTCONbits.INTF = 0;
        if (!motor_ativo) {
            tempo_motor = 20; // 2s (40 * 50ms)
            motor_ativo = 1;
            PORTDbits.RD0 = 1; // Liga o motor
            T1CONbits.TMR1ON = 1; // Inicia o Timer1
        }
    }

    // Botão RB1 (aciona motor por 4s)
    if (INTCONbits.RBIF) {
        INTCONbits.RBIF = 0;
        if (PORTBbits.RB1 && !motor_ativo) {
            tempo_motor = 40; // 4s (80 * 50ms)
            motor_ativo = 1;
            PORTDbits.RD0 = 1; // Liga o motor
            T1CONbits.TMR1ON = 1; // Inicia o Timer1
        }
    }
}

void main(void) {
    configurar_pinos();
    configurar_timer1();
    configurar_interrupcoes();
    while (1);
}

void configurar_pinos(void) {
    TRISD = 0x00; // PORTD como saída
    //PORTB = 0x01;
    TRISBbits.TRISB0 = 1; // RB0 como entrada
    TRISBbits.TRISB1 = 1; // RB1 como entrada
    PORTD = 0x00; // Inicializa motor desligado
}

void configurar_timer1(void) {
    T1CON = 0x31; // Prescaler 1:8
    TMR1H = 0x0B; // Valor de recarga (50ms)
    TMR1L = 0xDC;
    PIR1bits.TMR1IF = 0;
    PIE1bits.TMR1IE = 1;
}

void configurar_interrupcoes(void) {
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    INTCONbits.INTE = 1;
    INTCONbits.INTF = 0;
    INTCONbits.RBIE = 1;
    INTCONbits.RBIF = 0;
}
