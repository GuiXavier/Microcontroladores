#include <xc.h>

// Configurações do microcontrolador
#pragma config FOSC = HS       // Oscilador de alta velocidade
#pragma config WDTE = OFF      // Watchdog Timer desativado
#pragma config PWRTE = OFF     // Power-up Timer desativado
#pragma config CP = OFF        // Proteção de código desativada

#define _XTAL_FREQ 20000000    // Frequência do oscilador (20 MHz)

#define LED PORTCbits.RC2      // Define o LED na porta RC2

// Inicializa Timer0 para gerar um atraso de 1 segundo
void inicializar_Timer0() {
    TMR0 = 6;                  // Valor inicial para atingir ~1s com prescaler 1:256
    OPTION_REG = 0b00000111;   // Prescaler 1:256, Timer0 como temporizador
    INTCONbits.TMR0IF = 0;     // Zera a flag do Timer0
}

// Função para gerar um atraso de 1 segundo utilizando Timer0
void atraso_Timer0() {
    unsigned char contador = 0; // Conta os transbordamentos
    while (contador < 80) {     // 61 transbordamentos para 1 segundo
        while (!INTCONbits.TMR0IF); // Espera Timer0 transbordar
        INTCONbits.TMR0IF = 0;      // Reseta a flag do Timer0
        TMR0 = 6;                  // Recarrega Timer0 para próxima contagem
        contador++;
    }
}

void main() {
    TRISCbits.TRISC2 = 0;  // Configura RC2 como saída para o LED
    LED = 0;               // Inicializa LED apagado

    inicializar_Timer0();  // Configura Timer0

    while (1) {
        LED = !LED;        // Inverte o estado do LED (Liga/Desliga)
        atraso_Timer0();   // Aguarda 1 segundo antes da próxima mudança de estado
    }
}
