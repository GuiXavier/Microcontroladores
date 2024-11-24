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

// Variáveis globais
unsigned int contador = 0;

// Função para envio de dados ao LCD
void lcd_data(unsigned char data) {
    PORTD = data;
    RS = 1;
    EN = 1;
    __delay_ms(5);
    EN = 0;
}

// Função para envio de comandos ao LCD
void lcd_command(unsigned char cmd) {
    PORTD = cmd;
    RS = 0;
    EN = 1;
    __delay_ms(5);
    EN = 0;
}

// Função para exibir uma string no LCD
void lcd_string(const char *str) {
    while (*str) {
        lcd_data(*str++);
    }
}

// Inicialização do LCD
void lcd_initialise() {
    lcd_command(0x38);  // Configuração do modo de 8 bits
    lcd_command(0x06);  // Cursor incrementa à direita
    lcd_command(0x0C);  // Display ON, Cursor OFF
    lcd_command(0x01);  // Limpa o display
}

// Leitura do teclado matricial
char teclado_matricial() {
    PORTB = 0x0E;  // Primeira linha ativa
    if (PORTBbits.RB4 == 0) return '1';
    if (PORTBbits.RB5 == 0) return '2';
    if (PORTBbits.RB6 == 0) return '3';
    if (PORTBbits.RB7 == 0) return 'A';

    PORTB = 0x0D;  // Segunda linha ativa
    if (PORTBbits.RB4 == 0) return '4';
    if (PORTBbits.RB5 == 0) return '5';
    if (PORTBbits.RB6 == 0) return '6';
    if (PORTBbits.RB7 == 0) return 'B';

    PORTB = 0x0B;  // Terceira linha ativa
    if (PORTBbits.RB4 == 0) return '7';
    if (PORTBbits.RB5 == 0) return '8';
    if (PORTBbits.RB6 == 0) return '9';
    if (PORTBbits.RB7 == 0) return 'C';

    PORTB = 0x07;  // Quarta linha ativa
    if (PORTBbits.RB4 == 0) return '*';
    if (PORTBbits.RB5 == 0) return '0';
    if (PORTBbits.RB6 == 0) return '#';
    if (PORTBbits.RB7 == 0) return 'D';

    return 0;  // Nenhuma tecla pressionada
}

void main(void) {
    TRISE = 0x00;  // Configurações do LCD
    TRISD = 0x00;
    TRISB = 0xF0;  // Linhas do teclado como entrada, colunas como saída
    PORTB = 0x0F;  // Colunas inicialmente em nível alto

    lcd_initialise();

    char buffer[10];  // Buffer para armazenar o contador formatado
    char tecla;

    while (1) {
        tecla = teclado_matricial();  // Lê a tecla pressionada

        if (tecla) {
            __delay_ms(200);  // Anti-bounce
            if (tecla == '0') {
                contador = 0;  // Zera o contador
            } else {
                contador++;    // Incrementa o contador
            }

            lcd_command(0x80);       // Posiciona o cursor na linha 1
            sprintf(buffer, "%04d", contador);  // Formata o contador com 4 dígitos
            lcd_string("Contador:");
            lcd_command(0xC0);       // Posiciona o cursor na linha 2
            lcd_string(buffer);      // Exibe o contador
        }
    }
}
