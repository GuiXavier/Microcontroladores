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

unsigned char tecla_pressionada = 0;  // Tecla pressionada
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
    unsigned char tecla = 20;
    
  while(tecla == 20){  
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
  }
    return tecla;
}

void atualiza_lcd() {
    
     unsigned char tecla_pressionado = teclado();
    
    char buffer[16];
    sprintf(buffer, "%d", tecla_pressionado); 

    lcd_command(0x80);
    //lcd_string("Contador cliques");
    lcd_command(0xC0);
    lcd_string(buffer);
}
//void atualiza_lcd_com_for(unsigned char tecla) {
//    static unsigned char cursor_pos = 0x80; // Início da primeira linha do LCD.
//
//    if (cursor_pos < 0x90) { // Se ainda houver espaço na primeira linha.
//        for (unsigned char i = cursor_pos; i < 0x90; i++) {
//            lcd_command(i); // Move o cursor para a posição atual.
//            if (tecla < 10) {          // Teclas 0-9 (números).
//                lcd_data(tecla + '0'); // Converte o número em caractere ASCII.
//            } else {                   // Teclas 10-15 (A-F).
//                lcd_data(tecla - 10 + 'A'); // Converte o número em letra (A-F).
//            }
//            cursor_pos++; // Atualiza a posição do cursor.
//            break;        // Sai do loop após escrever uma tecla.
//        }
//    } else {
//        lcd_command(0x01);   // Limpa o display.
//        cursor_pos = 0x80;   // Reseta para o início da primeira linha.
//    }
//}


void main(void) {
    
    TRISE = 0x00;  
    TRISD = 0x00; 
    TRISC = 0x00;  
    TRISB = 0xFF; 
    
    lcd_initialise();
    atualiza_lcd();
    
    while (1) {
        
        unsigned char tecla = teclado();
        
        static unsigned char cursor_pos = 0x80; // Início da primeira linha do LCD.

        
        for(int i = cursor_pos; i < 31; i++ ){
            
            lcd_command(i); // Move o cursor para a posição atual.

            if(cursor_pos < 16){
            
              if (tecla < 10) {          // Teclas 0-9 (números).
                    lcd_data(tecla + '0'); // Converte o número em caractere ASCII.
              } else { 
                                          // Teclas 10-15 (A-F).
                    lcd_data(tecla - 10 + 'A'); // Converte o número em letra (A-F).
              }
              cursor_pos++; // Atualiza a posição do cursor.
                break;        // Sai do loop após escrever uma tecla.
                
            lcd_command(0xC0);    // para pular de linha 
            atualiza_lcd();
            
            }else{
                    lcd_command(0xC0);
                    cursor_pos = 0xC0;
            
            }
        
        }
        if (tecla != 0xFF) {  
            debounce();  
            while (teclado() != 0xFF);
        }
        
         lcd_initialise();

    while (1) {
        unsigned char tecla = teclado();
        atualiza_lcd();

        debounce();
        while (teclado() != 20); // Aguarda soltar a tecla
                }
    }
}
