#include <xc.h>
#define _XTAL_FREQ 20000000 // Define a frequência do oscilador em 20 MHz (necessário para os atrasos)

// CONFIGURAÇÕES DO MICROCONTROLADOR
#pragma config FOSC = HS        // Oscilador HS (alta velocidade)
#pragma config WDTE = OFF       // Watchdog Timer desativado
#pragma config PWRTE = OFF      // Power-up Timer desativado
#pragma config BOREN = OFF      // Brown-out Reset desativado
#pragma config LVP = OFF        // Programação em baixa tensão desativada
#pragma config CPD = OFF        // Proteção do código na EEPROM desativada
#pragma config WRT = OFF        // Escrita na memória Flash desativada
#pragma config CP = OFF         // Proteção do código na memória Flash desativada

// TABELA PARA O DISPLAY DE 7 SEGMENTOS (0-9 e A-F em hexadecimal)
unsigned char segment[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07,0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71}, i=0;

// VARIÁVEIS GLOBAIS
unsigned char contador = 0;  // Armazena o valor atual (0-15)
unsigned char sentido = 1;   // Define o sentido da contagem (1 = crescente, 0 = decrescente)

void verificaBotao(){  
    if(PORTBbits.RB0 == 0)
    {
        //__delay_ms(20);
        return 1;
    
    }else{
        
        return 0;    
        
    }
 }


void main(void) {
    // CONFIGURAÇÃO DAS PORTAS
    TRISD = 0x00;       // PORTD como saída (conectado ao display de 7 segmentos)
    TRISB = 0xFF;       // PORTB como entrada (botão em RB0)
    PORTD = 0x00;       // Inicializa o display apagado

    OPTION_REGbits.nRBPU = 0; // Habilita os resistores de pull-up internos para PORTB


    // LOOP PRINCIPAL
    while(verificaBotao()){
        // DETECÇÃO DO BOTÃO EM RB0
        if(PORTBbits.RB0 == 0)
        {
            sentido = !sentido;
        }    

        // CONTROLE DA CONTAGEM
        if (sentido) { // Contagem crescente
            contador++;
            if (contador > 15) { // Reinicia a contagem ao atingir o limite
                contador = 0;
            }
        } else { // Contagem decrescente
            if (contador == 0) { // Reinicia para o valor máximo ao atingir 0
                contador = 15;
            } else {
                contador--;
            }
        }

        // ATUALIZA O DISPLAY
        PORTD = segment[contador]; // Mostra o valor no display de 7 segmentos
        __delay_ms(500); // Intervalo para visibilidade da contagem
    }
}
