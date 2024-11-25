#include <xc.h>
#define _XTAL_FREQ 20000000 // Define a frequência do oscilador em 20 MHz

// CONFIGURAÇÕES DO MICROCONTROLADOR
#pragma config FOSC = HS        // Oscilador HS (alta velocidade)
#pragma config WDTE = OFF       // Watchdog Timer desativado
#pragma config PWRTE = OFF      // Power-up Timer desativado
#pragma config BOREN = OFF      // Brown-out Reset desativado
#pragma config LVP = OFF        // Programação em baixa tensão desativada
#pragma config CPD = OFF        // Proteção do código na EEPROM desativada
#pragma config WRT = OFF        // Escrita na memória Flash desativada
#pragma config CP = OFF         // Proteção do código na memória Flash desativada

// SEQUÊNCIA DO MOTOR DE PASSO (4 FASES)
unsigned char stepSequence[] = {0x09, 0x0C, 0x06, 0x03}; // Sequência para 4 fases
unsigned char stepIndex = 0;   // Índice para controle da sequência
unsigned char sentido = 0;     // 0 = Anti-horário, 1 = Horário

void main(void) {
    // CONFIGURAÇÃO DAS PORTAS
    TRISD = 0x00;       // PORTD como saída (motor de passo)
    TRISB = 0xFF;       // PORTB como entrada (botão em RB0)
    PORTD = 0x00;       // Inicializa PORTD em 0
    unsigned char botaoAnterior = 1; // Armazena o estado anterior do botão

    // LOOP PRINCIPAL
    while (1) {
        // LEITURA DO BOTÃO (COM DEBOUNCE)
        if (PORTBbits.RB0 == 0 && botaoAnterior == 1) {
            __delay_ms(50); // Atraso para debounce
            if (PORTBbits.RB0 == 0) {
                sentido = !sentido; // Alterna o sentido
                while (PORTBbits.RB0 == 0); // Aguarda o botão ser solto
            }
        }
        botaoAnterior = PORTBbits.RB0; // Atualiza o estado anterior do botão

        // CONTROLE DO MOTOR DE PASSO
        if (sentido == 1) { // Horário
            stepIndex = (stepIndex + 1) % 4;
        } else { // Anti-horário
            stepIndex = (stepIndex == 0) ? 3 : stepIndex - 1;
        }

        PORTD = stepSequence[stepIndex]; // Atualiza a sequência no PORTD
        __delay_ms(200);                 // Atraso para controle da velocidade do motor
    }
}
