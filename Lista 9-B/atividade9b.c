#include <xc.h>

// Configurações do microcontrolador
#pragma config FOSC = HS       // Oscilador de alta velocidade
#pragma config WDTE = OFF      // Watchdog Timer desativado
#pragma config PWRTE = OFF     // Power-up Timer desativado
#pragma config CP = OFF        // Proteção de código desativada

#define _XTAL_FREQ 20000000    // Frequência do oscilador (20 MHz)

// LCD
#define RS PORTEbits.RE0
#define EN PORTEbits.RE1

#define BOTAO_CONTAR PORTBbits.RB0
#define BOTAO_RESET PORTBbits.RB1

unsigned int contador = 0; // Armazena o número de pressões
char buffer[5];

// Funções do LCD
void lcd_command(unsigned char cmd) {
    PORTD = cmd;
    RS = 0;
    EN = 1;
    __delay_ms(2);
    EN = 0;
}

void lcd_data(unsigned char data) {
    PORTD = data;
    RS = 1;
    EN = 1;
    __delay_ms(2);
    EN = 0;
}

void lcd_initialise() {
    lcd_command(0x38);
    lcd_command(0x0C);
    lcd_command(0x06);
    lcd_command(0x01);
    __delay_ms(2);
}

void lcd_string(const char *str) {
    while (*str) {
        lcd_data(*str++);
    }
}

// Formata o número manualmente
void formata_contador(unsigned int contador) {
    buffer[0] = (contador / 1000) % 10 + '0';  // Milhar
    buffer[1] = (contador / 100) % 10 + '0';   // Centena
    buffer[2] = (contador / 10) % 10 + '0';    // Dezena
    buffer[3] = contador % 10 + '0';           // Unidade
    buffer[4] = '\0';                          // Final da string
}

// Atualiza o LCD
void exibir_contador_LCD() {
    lcd_command(0x80);
    lcd_string("Contador:");
    lcd_command(0xC0);
    formata_contador(contador);
    lcd_string(buffer);
}

// Tratamento de debounce
unsigned char debounce(unsigned char botao) {
    if (botao == 1) {
        __delay_ms(50);
        if (botao == 1) {
            return 1;
        }
    }
    return 0;
}

void main() {
    // Configuração de portas
    TRISD = 0x00;
    TRISE = 0x00;
    TRISB = 0xFF;

    lcd_initialise();
    exibir_contador_LCD();

    while (1) {
        // Incrementa o contador
        if (debounce(BOTAO_CONTAR)) {
            contador++;
            exibir_contador_LCD();
            while (BOTAO_CONTAR);
        }

        // Reseta o contador
        if (debounce(BOTAO_RESET)) {
            contador = 0;
            exibir_contador_LCD();
            while (BOTAO_RESET);
        }
    }
}
