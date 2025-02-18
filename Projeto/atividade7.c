#include <xc.h>
#include <stdio.h>
#include <string.h>

#define _XTAL_FREQ 20000000

// CONFIGURAÇÕES DO MICROCONTROLADOR
#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config BOREN = OFF
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

// Definições do teclado matricial
#define c0 PORTDbits.RD0
#define c1 PORTDbits.RD1
#define c2 PORTDbits.RD2
#define c3 PORTDbits.RD3

#define b0 PORTBbits.RB0
#define b1 PORTBbits.RB1
#define b2 PORTBbits.RB2
#define b3 PORTBbits.RB3

// Variáveis globais
char sequencia[10] = "";
unsigned char posicao = 0;
unsigned char velocidade_atual = 0;

// Funções PWM
void config_PWM();
void set_PWM(unsigned char velocidade);

// Funções UART
void config_UART();
void enviarChar(char c);
void enviarString(const char *s);
void enviarSequenciaUART();
void enviarVelocidadeAtualUART();

// Função do teclado matricial
unsigned char ler_teclado();

void main(void) {
    TRISD = 0xF0;
    TRISB = 0xFF;
    TRISCbits.TRISC2 = 0;

    config_PWM();
    config_UART();

    memset(sequencia, 0, sizeof(sequencia)); // Limpa buffer antes de iniciar

    while (1) {
        unsigned char tecla = ler_teclado();

        if (tecla != 0xFF) {
            if (posicao < sizeof(sequencia) - 1) {
                sequencia[posicao++] = tecla + '0';  
                sequencia[posicao] = '\0';  
            }
            enviarSequenciaUART();

            set_PWM(tecla);
            enviarVelocidadeAtualUART();
        }
    }
}

void config_PWM() {
    PR2 = 255;
    CCPR1L = 0;
    CCP1CON = 0b00001100;
    T2CON = 0b00000100;
}

void set_PWM(unsigned char velocidade) {
    unsigned int duty_cycle = velocidade * 25;
    CCPR1L = (unsigned char)(duty_cycle >> 2);
    CCP1CON = (CCP1CON & 0xCF) | ((duty_cycle & 0x03) << 4);
}

void config_UART() {
    TRISCbits.TRISC6 = 1; 
    TRISCbits.TRISC7 = 1; 
    SPBRG = 129;
    TXSTAbits.BRGH = 1;
    TXSTAbits.SYNC = 0;
    RCSTAbits.SPEN = 1;
    RCSTAbits.RX9 = 0;
    RCSTAbits.CREN = 0;
    TXSTAbits.TXEN = 1;
}

void enviarChar(char c) {
    while (!TXSTAbits.TRMT);
    TXREG = c;
}

void enviarString(const char *s) {
    while (*s) {
        enviarChar(*s);
        __delay_ms(1); // Pequeno delay para evitar corrupção de caracteres
        s++;
    }
}

void enviarSequenciaUART() {
    char buffer[30];
    snprintf(buffer, sizeof(buffer), "Seq: %s\r\n", sequencia);
    enviarString(buffer);
}

void enviarVelocidadeAtualUART() {
    char buffer[20];
    if (posicao > 0) {
        snprintf(buffer, sizeof(buffer), "Velocidade atual: %c\r\n", sequencia[posicao - 1]);
    } else {
        snprintf(buffer, sizeof(buffer), "Velocidade atual: 0\r\n");
    }
    enviarString(buffer);
}

unsigned char ler_teclado() {
    unsigned char tecla = 0xFF;

    c0 = 0; c1 = 1; c2 = 1; c3 = 1;
    if (!b0) tecla = 0;
    if (!b1) tecla = 1;
    if (!b2) tecla = 2;
    if (!b3) tecla = 3;

    c0 = 1; c1 = 0; c2 = 1; c3 = 1;
    if (!b0) tecla = 4;
    if (!b1) tecla = 5;
    if (!b2) tecla = 6;
    if (!b3) tecla = 7;

    c0 = 1; c1 = 1; c2 = 0; c3 = 1;
    if (!b0) tecla = 8;
    if (!b1) tecla = 9;

    return tecla;
}