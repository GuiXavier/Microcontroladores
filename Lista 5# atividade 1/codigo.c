#include <xc.h>
#define _XTAL_FREQ 20000000

// CONFIG
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = OFF        // Low-Voltage Programming Disable
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits
#pragma config CP = OFF         // Flash Program Memory Code Protection bit

// Tabela para os segmentos do display (0-9, A-F)
unsigned char segment[] = {0x03, 0x9F, 0x25, 0x0D, 0x99, 0x49, 0x41, 0x1F, 0x01, 0x19, 0x11, 0xC1, 0x63, 0x85, 0x61, 0x71};

// Variáveis globais
unsigned char contador = 0;  // Valor inicial da contagem
unsigned char sentido = 1;   // 1 = crescente, 0 = decrescente

void main(void) {
    // Configuração das portas
    TRISD = 0x00;       // PORTD como saída (conectado ao display de 7 segmentos)
    PORTAbits.RA0 = 1; // RB0 como entrada (botão)
    PORTD = 0x00;       // Inicializa display apagado

    // Habilita os resistores de pull-up internos
    OPTION_REGbits.nRBPU = 0;

    // Início do loop principal
    while (1) {
        // Verifica se o botão RB0 foi pressionado
        if (PORTBbits.RB0 == 0) { // Botão pressionado
            __delay_ms(20); // Debounce
            if (PORTBbits.RB0 == 0) { // Confirmação do botão pressionado
                sentido = !sentido; // Alterna o sentido
                while (PORTBbits.RB0 == 0); // Aguarda o botão ser solto
            }
        }

        // Atualiza o contador com base no sentido
        if (sentido) { // Contagem crescente
            contador++;
            if (contador > 15) {
                contador = 0; // Reinicia a contagem
            }
        } else { // Contagem decrescente
            if (contador == 0) {
                contador = 15; // Reinicia para o valor máximo
            } else {
                contador--;
            }
        }

        // Atualiza o display de 7 segmentos
        PORTD = segment[contador];
        __delay_ms(500); // Intervalo entre as atualizações
    }
}
