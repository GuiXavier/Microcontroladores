#include <xc.h>
#include <stdio.h>

#define _XTAL_FREQ 20000000

// CONFIGURA��ES
#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config BOREN = OFF
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

// Defini��es para o LCD no MODO 4 BITS
#define RS PORTEbits.RE0
#define EN PORTEbits.RE1
#define D4 PORTDbits.RD4
#define D5 PORTDbits.RD5
#define D6 PORTDbits.RD6
#define D7 PORTDbits.RD7

// Bot�o para acionar a leitura do ADC
#define BOTAO RB0

// Vari�vel global para controle de atualiza��o
volatile unsigned char atualizar_LCD = 0;

// Prot�tipos das fun��es
void lcd_command(unsigned char cmd);
void lcd_data(unsigned char data);
void lcd_initialise();
void lcd_string(const char *str);
void lcd_set_cursor(unsigned char row, unsigned char col);
void adc_initialise();
unsigned int read_adc();
void configurar_interrupcoes();

char buffer[16]; // Buffer para exibir os valores

// Interrup��o
void __interrupt() isr(void) {
    if (INTCONbits.INTF) {  // Interrup��o externa (RB0 pressionado)
        INTCONbits.INTF = 0; // Limpa a flag de interrup��o
        atualizar_LCD = 1;   // Sinaliza para atualizar a tens�o no LCD
    }
}

void main() {
    // Configura��es de PORTS
    TRISE = 0x00;  // PORT E como sa�da (Controle do LCD)
    TRISD = 0x00;  // PORT D como sa�da (Dados do LCD)
    TRISA = 0xFF;  // PORT A como entrada (ADC)
    TRISBbits.TRISB0 = 1;  // RB0 como entrada

    // Inicializa LCD e ADC
    lcd_initialise();
    adc_initialise();
    configurar_interrupcoes();

    lcd_set_cursor(0, 0);
    lcd_string("Tensao:");

    while (1) {
        if (atualizar_LCD) {
            atualizar_LCD = 0; // Reset flag

            unsigned int adc_value = read_adc(); // L� o ADC no A3
            float voltage = (adc_value * 5.0f) / 1023.0f; // Converte ADC para Volts

            // Exibe o valor no LCD
            lcd_set_cursor(1, 0);
            sprintf(buffer, "V: %.2f V", voltage);
            lcd_string(buffer);
        }
    }
}

// Configura o ADC
void adc_initialise() {
    ADCON0 = 0b10011001;  // Habilita ADC, seleciona AN3, Fosc/8
    ADCON1 = 0b10000010;  // AN3 anal�gico, Vref+ e Vref- no Vdd/Vss
}

// L� o ADC
unsigned int read_adc() {
    __delay_us(20);             // Tempo para estabilizar
    ADCON0bits.GO_nDONE = 1;    // Inicia a convers�o
    while (ADCON0bits.GO_nDONE); // Aguarda a convers�o
    return ((ADRESH << 8) + ADRESL);  // Retorna os 10 bits do ADC
}

// Configura interrup��o no RB0
void configurar_interrupcoes() {
    INTCONbits.GIE  = 1;  // Habilita interrup��es globais
    INTCONbits.INTE = 1;  // Habilita interrup��o externa (RB0)
    OPTION_REGbits.INTEDG = 1; // Interrup��o na borda de subida
    INTCONbits.INTF = 0;  // Limpa flag de interrup��o
}

// Inicializa o LCD (Modo 4 bits)
void lcd_initialise() {
    lcd_command(0x33);  // Sequ�ncia de inicializa��o do LCD em 4 bits
    lcd_command(0x32);
    lcd_command(0x28);  // Modo 4 bits, 2 linhas
    lcd_command(0x0C);  // Display ON, Cursor OFF
    lcd_command(0x06);  // Incrementa o cursor
    lcd_command(0x01);  // Limpa o display
    __delay_ms(2);
}

// Envia comando para o LCD (Modo 4 bits)
void lcd_command(unsigned char cmd) {
    D4 = (cmd >> 4) & 0x01;
    D5 = (cmd >> 5) & 0x01;
    D6 = (cmd >> 6) & 0x01;
    D7 = (cmd >> 7) & 0x01;
    RS = 0;
    EN = 1;
    __delay_ms(2);
    EN = 0;

    D4 = cmd & 0x01;
    D5 = (cmd >> 1) & 0x01;
    D6 = (cmd >> 2) & 0x01;
    D7 = (cmd >> 3) & 0x01;
    EN = 1;
    __delay_ms(2);
    EN = 0;
}

// Envia dados para o LCD (Modo 4 bits)
void lcd_data(unsigned char data) {
    D4 = (data >> 4) & 0x01;
    D5 = (data >> 5) & 0x01;
    D6 = (data >> 6) & 0x01;
    D7 = (data >> 7) & 0x01;
    RS = 1;
    EN = 1;
    __delay_ms(2);
    EN = 0;

    D4 = data & 0x01;
    D5 = (data >> 1) & 0x01;
    D6 = (data >> 2) & 0x01;
    D7 = (data >> 3) & 0x01;
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

// Posiciona o cursor no LCD (linha, coluna)
void lcd_set_cursor(unsigned char row, unsigned char col) {
    unsigned char pos;
    if (row == 0)
        pos = 0x80 + col;
    else
        pos = 0xC0 + col;
    lcd_command(pos);
}
