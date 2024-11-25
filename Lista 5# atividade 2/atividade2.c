#include <xc.h>
#define _XTAL_FREQ 20000000 // Define a frequ�ncia do oscilador em 20 MHz

// CONFIGURA��ES DO MICROCONTROLADOR
#pragma config FOSC = HS        // Oscilador HS (alta velocidade)
#pragma config WDTE = OFF       // Watchdog Timer desativado
#pragma config PWRTE = OFF      // Power-up Timer desativado
#pragma config BOREN = OFF      // Brown-out Reset desativado
#pragma config LVP = OFF        // Programa��o em baixa tens�o desativada
#pragma config CPD = OFF        // Prote��o do c�digo na EEPROM desativada
#pragma config WRT = OFF        // Escrita na mem�ria Flash desativada
#pragma config CP = OFF         // Prote��o do c�digo na mem�ria Flash desativada

// SEQU�NCIA DO MOTOR DE PASSO (4 FASES)
unsigned char stepSequence[] = {0x09, 0x0C, 0x06, 0x03}; // Sequ�ncia para 4 fases
unsigned char stepIndex = 0;   // �ndice para controle da sequ�ncia
unsigned char sentido = 0;     // 0 = Anti-hor�rio, 1 = Hor�rio

void main(void) {
    // CONFIGURA��O DAS PORTAS
    TRISD = 0x00;       // PORTD como sa�da (motor de passo)
    TRISB = 0xFF;       // PORTB como entrada (bot�o em RB0)
    PORTD = 0x00;       // Inicializa PORTD em 0
    unsigned char botaoAnterior = 1; // Armazena o estado anterior do bot�o

    // LOOP PRINCIPAL
    while (1) {
        // LEITURA DO BOT�O (COM DEBOUNCE)
        if (PORTBbits.RB0 == 0 && botaoAnterior == 1) {
            __delay_ms(50); // Atraso para debounce
            if (PORTBbits.RB0 == 0) {
                sentido = !sentido; // Alterna o sentido
                while (PORTBbits.RB0 == 0); // Aguarda o bot�o ser solto
            }
        }
        botaoAnterior = PORTBbits.RB0; // Atualiza o estado anterior do bot�o

        // CONTROLE DO MOTOR DE PASSO
        if (sentido == 1) { // Hor�rio
            stepIndex = (stepIndex + 1) % 4;
        } else { // Anti-hor�rio
            stepIndex = (stepIndex == 0) ? 3 : stepIndex - 1;
        }

        PORTD = stepSequence[stepIndex]; // Atualiza a sequ�ncia no PORTD
        __delay_ms(200);                 // Atraso para controle da velocidade do motor
    }
}
