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
unsigned char segment[] = {0x03,0x9f,0x25,0x0d,0x99,0x49,0x41,0x1f,0x01,0x19,0x11,0xc1,0x63,0x85,0x61,0x71},i=0;

// VARIÁVEIS GLOBAIS
unsigned char contador = 0;  // Armazena o valor atual (0-15)
unsigned char sentido = 0;   // Define o sentido da contagem (1 = crescente, 0 = decrescente)


void main(void) {
    // CONFIGURAÇÃO DAS PORTAS
    TRISD = 0x00;       // PORTD como saída (conectado ao display de 7 segmentos)
    TRISB = 0xFF;       // PORTB como entrada (botão em RB0)
   //ORTD = 0x00;       // Inicializa o display apagado
    TRISA = 0x00;
    contador = 0;
    PORTAbits.RA5 = 0;
   //PTION_REGbits.nRBPU = 0; // Habilita os resistores de pull-up internos para PORTB

    // LOOP PRINCIPAL
    
    while(1){
if(PORTBbits.RB0 == 0){
//        // DETECÇÃO DO BOTÃO EM RB0
        if(PORTBbits.RB0 == 0)
       {
            sentido = !sentido;
        }    

        // CONTROLE DA CONTAGEM
        if (sentido == 1){// Contagem crescente
            contador++;
           if (contador > 15)  // Reinicia a contagem ao atingir o limite
                contador = 0;            
            PORTD = segment[contador]; // Mostra o valor no display de 7 segmentos
            __delay_ms(500); // Intervalo para visibilidade da contagem
        }else{
            contador--;
            if(contador == 0)
                contador = 15;
            PORTD = segment[contador];
            __delay_ms(500);
        }

       
//       if(contador == 16){
//           contador = 0;
//       }
//       if(contador == 0){
//           contador = 15;
//       }
    
    
}
}
}