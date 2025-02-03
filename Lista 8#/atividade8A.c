#include <xc.h>
#include <stdio.h>
#define _XTAL_FREQ 20000000

// CONFIGURAÇÕES
#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config BOREN = OFF
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

// Definições para o LCD
#define RS PORTEbits.RE0
#define EN PORTEbits.RE1

void lcd_command(unsigned char cmd);
void lcd_data(unsigned char data);
void lcd_initialise();
void lcd_string(const char *str);
void adc_initialise();
unsigned int read_adc(unsigned char canal);

// Buffer para o texto no LCD
char buffer[16];

void main() {
    // Configurações de PORTS
    TRISE = 0x00;  // PORT E como saída (Controle do LCD)
    TRISD = 0x00;  // PORT D como saída (Dados do LCD)
    TRISA = 0xFF;  // PORT A como entrada (ADC)

    // Inicializa LCD e ADC
    lcd_initialise();
    adc_initialise();

    lcd_command(0x80);  // Cursor na linha superior
    lcd_string("Tensao:");

    while (1) {
        // Lê o valor do ADC no canal AN3 (RA3)
        unsigned int adc_value = read_adc(3);
        float voltage = (adc_value * 5.0f) / 1023.0f;

        // Mostra o valor da tensão no LCD
        lcd_command(0xC0);  // Move o cursor para a segunda linha
        sprintf(buffer, "V: %.2f V", voltage);
        lcd_string(buffer);

        __delay_ms(500);  // Atualiza a cada 500ms
    }
}

// Configura o ADC
void adc_initialise() {
    ADCON0 = 0b10011001;  // Habilita ADC, seleciona AN3, frequência Fosc/8
    ADCON1 = 0b10000010;  // AN3 analógico, Vref+ e Vref- no Vdd/Vss
}

// Lê o ADC em um canal específico
unsigned int read_adc(unsigned char canal) {
    // Seleciona o canal
    ADCON0 &= 0xC5;                // Zera bits de seleção de canal
    ADCON0 |= (canal << 3);        // Configura o canal desejado
    __delay_us(20);                // Tempo para estabilizar
    ADCON0bits.GO_nDONE = 1;       // Inicia a conversão
    while (ADCON0bits.GO_nDONE);   // Aguarda a conclusão
    return ((ADRESH << 8) + ADRESL);  // Combina ADRESH e ADRESL
}

// Inicializa o LCD
void lcd_initialise() {
    lcd_command(0x38);  // Configura o LCD para modo 8 bits, 2 linhas
    lcd_command(0x0C);  // Liga o display, cursor desligado
    lcd_command(0x06);  // Incrementa o cursor
    lcd_command(0x01);  // Limpa o display
    __delay_ms(2);
}

// Envia um comando para o LCD
void lcd_command(unsigned char cmd) {
    PORTD = cmd;
    RS = 0;
    EN = 1;
    __delay_ms(2);
    EN = 0;
}

// Envia um caractere para o LCD
void lcd_data(unsigned char data) {
    PORTD = data;
    RS = 1;
    EN = 1;
    __delay_ms(2);
    EN = 0;
}

// Mostra uma string no LCD
void lcd_string(const char *str) {
    while (*str) {
        lcd_data(*str++);
    }
}
