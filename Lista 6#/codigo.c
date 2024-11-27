#include <xc.h>
#define _XTAL_FREQ 20000000

// CONFIG
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = ON         // Low-Voltage Programming Enable bit
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits
#pragma config CP = OFF         // Flash Program Memory Code Protection bit

#define RS PORTEbits.RE0
#define EN PORTEbits.RE1
#define BOTAO PORTBbits.RB0

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

void lcd_string(const unsigned char *str) {
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

void main(void) {
    TRISE = 0x00;  // Configura PORT E como saída
    TRISD = 0x00;  // Configura PORT D como saída
    TRISB = 0x01;  // Configura RB0 como entrada

    lcd_initialise();

    unsigned char estado = 0;  

    while (1) {
        if (BOTAO == 1) {  
            debounce();

            if (BOTAO == 1) {  
                estado = (estado + 1) % 3;  
                lcd_command(0x01);  

                switch (estado) {
                    case 1:  
                        lcd_command(0x80);
                        lcd_string((const unsigned char *)"Paulo");
                        break;
                    case 2:  
                        lcd_command(0xC0);  
                        lcd_string((const unsigned char *)"a2095920");
                        break;
                    case 3:  
                        lcd_command(0x01);
                        break;
                }

                while (BOTAO == 1) {
                   
                }
            }
        }
    }
}
