#include <xc.h>
#include <stdio.h>

#pragma config FOSC = XT
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config CP = OFF
#define _XTAL_FREQ 2000000

// Definição dos pinos do teclado matricial
#define COLUNAS PORTB
#define LINHA1 PORTCbits.RC0
#define LINHA2 PORTCbits.RC1
#define LINHA3 PORTCbits.RC2
#define LINHA4 PORTCbits.RC3


// Protótipos das funções
void configurar_teclado();
char ler_teclado();
void configurar_uart();
void uart_enviar_caractere(char c);
void uart_enviar_string(const char *str);
void uart_enviar_valor(unsigned int valor);

void main() {
    configurar_teclado();
    configurar_uart();

    while (1) {
        char tecla = ler_teclado();  // Lê a tecla pressionada

        if (tecla != '\0') {  // Se uma tecla foi pressionada
            __delay_ms(100);  // Delay inicial para debounce

            while (ler_teclado() != '\0');  // ESPERA até que a tecla seja solta
            __delay_ms(50);  // Delay extra de segurança após a soltura

            if (tecla >= '0' && tecla <= 'F') {  // Se for um número de 0 a 9
                uart_enviar_caractere(tecla);  // Envia o número ao ser digitado
            } 
            
        }
    }
}

// ------------------------
// Funções do Teclado Matricial
// ------------------------
void configurar_teclado() {
    TRISB = 0x0F;  // RB0-RB3 como entrada (colunas)
    TRISC = 0xF0;  // RC0-RC3 como saída (linhas)
    PORTC = 0x00;  // Inicia com linhas em 0
}

char ler_teclado() {
    const char matriz_teclas[4][4] = {
        {'0', '1', '2', '3'},
        {'4', '5', '6', '7'},
        {'8', '9', 'A', 'B'},
        {'C', 'D', 'E', 'F'}  
    };

    for (unsigned char linha = 0; linha < 4; linha++) {
        PORTC = ~(1 << linha);  // Ativa apenas uma linha por vez

        if (!PORTBbits.RB0) return matriz_teclas[linha][0];
        if (!PORTBbits.RB1) return matriz_teclas[linha][1];
        if (!PORTBbits.RB2) return matriz_teclas[linha][2];
        if (!PORTBbits.RB3) return matriz_teclas[linha][3];

        __delay_ms(100);  // Pequeno delay para evitar debounce
    }
    return '\0';  // Retorna vazio se nenhuma tecla for pressionada
}

// ------------------------
// Funções da UART
// ------------------------
void configurar_uart() {
    TXSTA = 0x24;  // TX habilitado, modo assíncrono, BRGH=1
    RCSTA = 0x90;  // UART habilitada
    SPBRG = 12;    // Baud rate ~9600 para Fosc = 2 MHz
}

void uart_enviar_caractere(char c) {
    while (!TXIF);
    TXREG = c;
}

void uart_enviar_string(const char *str) {
    while (*str) {
        uart_enviar_caractere(*str++);
    }
}

void uart_enviar_valor(unsigned int valor) {
    char buffer[10];
    sprintf(buffer, "%u ", valor);
    uart_enviar_string(buffer);
}
