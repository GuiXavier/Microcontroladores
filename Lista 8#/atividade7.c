#include <xc.h>
#include <stdio.h>
#define _XTAL_FREQ 20000000

// CONFIG
#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config BOREN = OFF
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

 
 

// Definições do LCD
#define RS PORTEbits.RE0
#define EN PORTEbits.RE1

void lcd_command(unsigned char cmd);
void lcd_data(unsigned char data);
void lcd_initialise();
void lcd_string(const char *str);
void adc_initialise();
unsigned int read_adc();

char buffer[16];

void main() {
    // Configurações de PORTS
    TRISE = 0x00;
    TRISD = 0x00;
    TRISA = 0xFF; // PORTA como entrada para ADC

    lcd_initialise();
    adc_initialise();

    while (1) {
        unsigned int adc_value = read_adc();         // Lê o ADC
        float voltage = adc_value * 5.0f / 1023.0f;  // Converte para tensão

        // Exibe valor bruto do ADC
        lcd_command(0x80);
        sprintf(buffer, "ADC: %04u", adc_value);
        lcd_string(buffer);

        // Exibe valor em tensão
        lcd_command(0xC0);
        sprintf(buffer, "V: %.2f V", voltage);
        lcd_string(buffer);

        __delay_ms(500);
    }
}

void adc_initialise() {
    ADCON1 = 0x06;  // Configura RA3 como analógico (AN3), os demais digitais
    ADCON0 = 0x0D;  // Seleciona AN3 e habilita ADC
}

unsigned int read_adc() {
    __delay_us(20);              // Tempo para estabilização
    ADCON0bits.GO_DONE = 1;      // Inicia conversão
    while (ADCON0bits.GO_DONE);  // Aguarda conclusão
    return ((unsigned int)ADRESH << 8) | ADRESL;
}

void lcd_command(unsigned char cmd) {
    PORTD = cmd;
    RS = 0;
    EN = 1;
    __delay_ms(5);
    EN = 0;
}

void lcd_data(unsigned char data) {
    PORTD = data;
    RS = 1;
    EN = 1;
    __delay_ms(5);
    EN = 0;
}

void lcd_initialise() {
    lcd_command(0x38);
    lcd_command(0x06);
    lcd_command(0x0C);
    lcd_command(0x01);
}

void lcd_string(const char *str) {
    while (*str) {
        lcd_data(*str++);
    }
}
