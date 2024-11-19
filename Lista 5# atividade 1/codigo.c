#include <xc.h>
#define _XTAL_FREQ 20000000 // Define a frequ�ncia do oscilador em 20 MHz (necess�rio para os atrasos)

// CONFIGURA��ES DO MICROCONTROLADOR
#pragma config FOSC = HS        // Oscilador HS (alta velocidade)
#pragma config WDTE = OFF       // Watchdog Timer desativado
#pragma config PWRTE = OFF      // Power-up Timer desativado
#pragma config BOREN = OFF      // Brown-out Reset desativado
#pragma config LVP = OFF        // Programa��o em baixa tens�o desativada
#pragma config CPD = OFF        // Prote��o do c�digo na EEPROM desativada
#pragma config WRT = OFF        // Escrita na mem�ria Flash desativada
#pragma config CP = OFF         // Prote��o do c�digo na mem�ria Flash desativada

// TABELA PARA O DISPLAY DE 7 SEGMENTOS (0-9 e A-F em hexadecimal)
unsigned char segment[] = {0x03,0x9f,0x25,0x0d,0x99,0x49,0x41,0x1f,0x01,0x19,0x11,0xc1,0x63,0x85,0x61,0x71},i=0;

// VARI�VEIS GLOBAIS
unsigned char contador = 0;  // Armazena o valor atual (0-15)
unsigned char sentido = 0;   // Define o sentido da contagem (1 = crescente, 0 = decrescente)


void main(void) {
    // CONFIGURA��O DAS PORTAS
    TRISD = 0x00;       // PORTD como sa�da (conectado ao display de 7 segmentos)
    TRISB = 0xFF;       // PORTB como entrada (bot�o em RB0)
   //ORTD = 0x00;       // Inicializa o display apagado
    TRISA = 0x00;
    contador = 0;
    PORTAbits.RA5 = 0;
   //PTION_REGbits.nRBPU = 0; // Habilita os resistores de pull-up internos para PORTB

    // LOOP PRINCIPAL
    
    while(1){
if(PORTBbits.RB0 == 0){
//        // DETEC��O DO BOT�O EM RB0
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