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

// EEPROM
#define EEPROM_ENDERECO 0x00

// Variáveis globais
char sequencia[10] = "";
unsigned char posicao = 0;
unsigned char velocidade_atual = 0;

// Funções de EEPROM
void eeprom_write(unsigned char endereco, unsigned char dado);
unsigned char eeprom_read(unsigned char endereco);

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

    velocidade_atual = eeprom_read(EEPROM_ENDERECO);
    if (velocidade_atual > 10) velocidade_atual = 0;

    set_PWM(velocidade_atual);
    enviarVelocidadeAtualUART();

    while (1) {
        unsigned char tecla = ler_teclado();

        if (tecla != 0xFF) {
            if (posicao == 0) {
                eeprom_write(EEPROM_ENDERECO, tecla);
            }
            sequencia[posicao++] = tecla + '0';  
            sequencia[posicao] = '\0';  

            enviarSequenciaUART();

            set_PWM(tecla);
            enviarVelocidadeAtualUART();
        }
    }
}

void eeprom_write(unsigned char endereco, unsigned char dado) {
    EEADR = endereco;
    EEDATA = dado;
    EECON1bits.EEPGD = 0;
    EECON1bits.WREN = 1;
    INTCONbits.GIE = 0;
    EECON2 = 0x55;
    EECON2 = 0xAA;
    EECON1bits.WR = 1;
    INTCONbits.GIE = 1;
    EECON1bits.WREN = 0;
}

unsigned char eeprom_read(unsigned char endereco) {
    EEADR = endereco;
    EECON1bits.EEPGD = 0;
    EECON1bits.RD = 1;
    return EEDATA;
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
    while (*s) enviarChar(*s++);
}

void enviarSequenciaUART() {
    enviarString("Velocidades digitadas: ");
    enviarString(sequencia);
    enviarString("\r\n");
}

void enviarVelocidadeAtualUART() {
    char buffer[20];
    sprintf(buffer, "Velocidade atual: %d\r\n", velocidade_atual);
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