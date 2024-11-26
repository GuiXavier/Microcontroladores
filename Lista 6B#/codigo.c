#include <xc.h>
#define _XTAL_FREQ 20000000

// CONFIG
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF       // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = OFF         // Low-Voltage Programming Enable bit
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits
#pragma config CP = OFF         // Flash Program Memory Code Protection bit

#define RS PORTEbits.RE0
#define EN PORTEbits.RE1
#define c0 PORTCbits.RC0
#define c1 PORTCbits.RC1
#define c2 PORTCbits.RC2
#define c3 PORTCbits.RC3
#define b0 PORTBbits.RB0
#define b1 PORTBbits.RB1
#define b2 PORTBbits.RB2
#define b3 PORTBbits.RB3

unsigned int contador = 0;  // Contador de cliques
unsigned char tecla_anterior = 0xFF;

void lcd_data(unsigned char data) {
    PORTD = data;
    RS = 1;
    EN = 1;
    __delay_ms(5);
    EN = 0;
}

void lcd_command(unsigned char cmd) {
    PORTD = cmd;
    RS = 0;
    EN = 1;
    __delay_ms(5);
    EN = 0;
}

void lcd_string(const char *str) {
    while (*str) {
        lcd_data(*str++);
    }
}

void lcd_initialise() {
    lcd_command(0x38);  // Function Set
    lcd_command(0x06);  // Entry Mode Set
    lcd_command(0x0C);  // Display ON/OFF
    lcd_command(0x01);  // Display Clear
}

void debounce() {
    __delay_ms(50);  // Delay for debounce
}

unsigned char teclado() {
    unsigned char tecla = 0xFF;

    c0 = 0; c1 = 1; c2 = 1; c3 = 1;
    if (b0 == 0) tecla = 0;
    if (b1 == 0) tecla = 1;
    if (b2 == 0) tecla = 2;
    if (b3 == 0) tecla = 3;

    c0 = 1; c1 = 0; c2 = 1; c3 = 1;
    if (b0 == 0) tecla = 4;
    if (b1 == 0) tecla = 5;
    if (b2 == 0) tecla = 6;
    if (b3 == 0) tecla = 7;

    c0 = 1; c1 = 1; c2 = 0; c3 = 1;
    if (b0 == 0) tecla = 8;
    if (b1 == 0) tecla = 9;
    if (b2 == 0) tecla = 10;
    if (b3 == 0) tecla = 11;

    c0 = 1; c1 = 1; c2 = 1; c3 = 0;
    if (b0 == 0) tecla = 12;
    if (b1 == 0) tecla = 13;
    if (b2 == 0) tecla = 14;
    if (b3 == 0) tecla = 15;

    return tecla;
}

void atualiza_lcd() {
    char buffer[16];
    unsigned int temp = contador;
    int pos = 14;

    // Construir o número alinhado à direita
    buffer[15] = '\0';  // Final da string
    while (pos >= 2) {
        buffer[pos--] = (temp % 10) + '0';
        temp /= 10;
    }

    // Preencher espaço até N:
    while (pos >= 2) {
        buffer[pos--] = ' ';
    }

    // Adicionar prefixo N:
    buffer[0] = 'N';
    buffer[1] = ':';

    lcd_command(0x80);  // Cursor na primeira linha
    lcd_string("Contador cliques");
    lcd_command(0xC0);  // Cursor na segunda linha
    lcd_string(buffer);
}

void main(void) {
    TRISE = 0x00;  // Configura PORT E como saída
    TRISD = 0x00;  // Configura PORT D como saída
    TRISC = 0x00;  // Configura PORT C como saída
    TRISB = 0xFF;  // Configura PORT B como entrada
    lcd_initialise();

    atualiza_lcd();  // Exibe a mensagem inicial no LCD

    while (1) {
        unsigned char tecla = teclado();  // Captura a tecla pressionada

        if (tecla != 0xFF) {  // Se alguma tecla foi pressionada
            debounce();  // Aguarda para debounce

            if (tecla == tecla_anterior) {
                contador++;  // Incrementa o contador para a mesma tecla
            } else {
                contador = 1;  // Reinicia o contador para uma nova tecla
                tecla_anterior = tecla;  // Atualiza a tecla anterior
            }

            atualiza_lcd();  // Atualiza o display LCD

            // Aguarda a tecla ser solta
            while (teclado() != 0xFF);
        }
    }
}
