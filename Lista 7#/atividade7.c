#include <xc.h>
#include <stdio.h> // Biblioteca necessária para sprintf
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

unsigned char cursor_pos = 0x80; // Posição inicial do cursor no LCD

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
    __delay_ms(50);  // Delay para debounce
}

unsigned char teclado() {
    unsigned char tecla = 0xFF; // Valor inicial para nenhuma tecla

    // Verifica a primeira coluna
    c0 = 0; c1 = 1; c2 = 1; c3 = 1;
    if (b0 == 0) tecla = 0;
    if (b1 == 0) tecla = 1;
    if (b2 == 0) tecla = 2;
    if (b3 == 0) tecla = 3;

    // Verifica a segunda coluna
    c0 = 1; c1 = 0; c2 = 1; c3 = 1;
    if (b0 == 0) tecla = 4;
    if (b1 == 0) tecla = 5;
    if (b2 == 0) tecla = 6;
    if (b3 == 0) tecla = 7;

    // Verifica a terceira coluna
    c0 = 1; c1 = 1; c2 = 0; c3 = 1;
    if (b0 == 0) tecla = 8;
    if (b1 == 0) tecla = 9;
    if (b2 == 0) tecla = 10;
    if (b3 == 0) tecla = 11;

    // Verifica a quarta coluna
    c0 = 1; c1 = 1; c2 = 1; c3 = 0;
    if (b0 == 0) tecla = 12;
    if (b1 == 0) tecla = 13;
    if (b2 == 0) tecla = 14;
    if (b3 == 0) tecla = 15;

    return tecla;
}

void atualiza_lcd(unsigned char tecla) {
    if (cursor_pos == 0x90) { // Se preencheu as 16 casas da segunda linha
        lcd_command(0x01);   // Limpa o display
        cursor_pos = 0x80;   // Reseta para o início da primeira linha
    }

    lcd_command(cursor_pos); // Define a posição do cursor no LCD

    if (tecla < 10) { // Teclas 0-9 (números)
        lcd_data(tecla + '0'); // Converte o número em caractere ASCII
    } else { // Teclas 10-15 (A-F)
        lcd_data(tecla - 10 + 'A'); // Converte o número em letra (A-F)
    }

    cursor_pos++; // Move o cursor para a próxima posição

    if (cursor_pos == 0x90) { // Caso atinja o final da primeira linha
        cursor_pos = 0xC0;    // Muda para o início da segunda linha
    }
}

void main(void) {
    TRISE = 0x00;  // Configura PORT E como saída
    TRISD = 0x00;  // Configura PORT D como saída
    TRISC = 0x00;  // Configura PORT C como saída
    TRISB = 0xFF;  // Configura PORT B como entrada

    lcd_initialise();

    while (1) {
        unsigned char tecla = teclado();

        if (tecla != 0xFF) { // Se alguma tecla foi pressionada
            debounce();      // Evita múltiplos cliques devido ao bounce
            atualiza_lcd(tecla); // Atualiza o LCD com a tecla pressionada

            while (teclado() != 0xFF); // Aguarda a tecla ser solta
        }
    }
}
