#include <xc.h>

// Configuração do microcontrolador PIC16F877
#pragma config FOSC = HS   // Oscilador High Speed
#pragma config WDTE = OFF  // Watchdog Timer desativado
#pragma config PWRTE = OFF // Power-up Timer desativado
#pragma config CP = OFF    // Proteção de Código desativada

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
    // Verifica se a interrupção foi gerada pelo Timer1
    if (PIR1bits.TMR1IF) {
        PIR1bits.TMR1IF = 0; // Limpa flag de interrupção
        TMR1H = 0x85; // Valor correto para 50ms no PIC16F877
        TMR1L = 0xEE;

        if (motor_ativo) {
            tempo_motor--;
            if (tempo_motor == 0) {
                motor_ativo = 0;
                PORTDbits.RD0 = 0; // Desliga o motor
                T1CONbits.TMR1ON = 0; // Para o Timer1
            }
        }
    }

    // Interrupção do botão RB0 (borda de subida)
    if (INTCONbits.INTF) {
        INTCONbits.INTF = 0; // Limpa flag de interrupção externa
        if (!motor_ativo) { // Só ativa se o motor não estiver rodando
            tempo_motor = 40; // 2s (40 ciclos de 50ms)
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

    while (1) {
        // Verifica manualmente (Polling) se RB1 foi pressionado
        if (PORTBbits.RB1 == 1 && !motor_ativo) {
            __delay_ms(50); // Debounce para evitar leituras erradas
            if (PORTBbits.RB1 == 1) { // Confirmação após debounce
                tempo_motor = 80; // 4s (80 ciclos de 50ms)
                motor_ativo = 1;
                PORTDbits.RD0 = 1; // Liga o motor
                T1CONbits.TMR1ON = 1; // Inicia o Timer1
                while (PORTBbits.RB1 == 1); // Espera soltar o botão
            }
        }
    }
}

void configurar_pinos(void) {
    TRISD = 0x00; // PORTD como saída
    TRISBbits.TRISB0 = 1; // RB0 como entrada
    TRISBbits.TRISB1 = 1; // RB1 como entrada
    PORTD = 0x00; // Inicializa motor desligado
}

void configurar_timer1(void) {
    T1CON = 0x31; // Timer1 ligado, Prescaler 1:8
    TMR1H = 0x85; // Recarga correta para 50ms no PIC16F877
    TMR1L = 0xEE;
    PIR1bits.TMR1IF = 0;
    PIE1bits.TMR1IE = 1;
}

void configurar_interrupcoes(void) {
    INTCONbits.GIE = 1;  // Habilita interrupções globais
    INTCONbits.PEIE = 1; // Habilita interrupções periféricas
    INTCONbits.INTE = 1; // Habilita interrupção externa no RB0
    INTCONbits.INTF = 0; // Limpa a flag de interrupção externa
}
