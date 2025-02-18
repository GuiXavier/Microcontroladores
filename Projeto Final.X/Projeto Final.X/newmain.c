#include <xc.h>
#include <stdio.h>

// Config bits (exemplo)
#pragma config FOSC = XT
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config CP = OFF

#define _XTAL_FREQ 2000000

// Definição (opcional) das linhas do teclado em PORTD
#define LINHA1 PORTDbits.RD0
#define LINHA2 PORTDbits.RD1
#define LINHA3 PORTDbits.RD2
#define LINHA4 PORTDbits.RD3

// Tamanho máximo da sequência de velocidades
#define MAX_SEQ 10

// Variáveis globais
unsigned char memoria_eeprom[MAX_SEQ];  // Simulação de EEPROM
unsigned int sequencia[MAX_SEQ];       // Armazena a sequência digitada
unsigned int indice = 0;              // Índice da sequência

// Protótipos
void configurar_teclado();
char ler_teclado();
void configurar_uart();
void uart_enviar_caractere(char c);
void uart_enviar_string(const char *str);
void uart_enviar_valor(unsigned int valor);
void escrever_memoria(unsigned int valor);
void exibir_sequencia();
void resetar_sequencia();
void configurar_pwm();
void ajustar_pwm(unsigned int velocidade);

void main() {
    configurar_teclado();
    configurar_uart();
    configurar_pwm();

    unsigned int valor_digitado = 0;

    while (1) {
        char tecla = ler_teclado();

        if (tecla != '\0') {
            __delay_ms(100); 
            while (ler_teclado() != '\0'); // Aguarda soltar a tecla
            __delay_ms(50);

            if (tecla >= '0' && tecla <= '9') {
                valor_digitado = (valor_digitado * 10) + (tecla - '0');
                uart_enviar_caractere(tecla);
            } 
            else if (tecla == 'F') {  // '#' confirma
                if (indice < MAX_SEQ) {
                    sequencia[indice] = valor_digitado;
                    escrever_memoria(valor_digitado);
                    ajustar_pwm(valor_digitado);
                    indice++;
                    uart_enviar_string("\r\nSalvo: ");
                    uart_enviar_valor(valor_digitado);
                }
                valor_digitado = 0;
            }
            else if (tecla == 'C') {  // 'C' exibe sequência
                exibir_sequencia();
                resetar_sequencia();
            }
        }
    }
}

// ------------------------
// Configuração e leitura do Teclado
// ------------------------
void configurar_teclado() {
    TRISB = 0x0F;   // RB0-RB3 como entrada
    TRISD = 0xF0;   // RD0-RD3 como saída
    PORTD = 0x00;   // Inicia com as linhas em 0
}

char ler_teclado() {
    const char matriz_teclas[4][4] = {
        {'0', '1', '2', '3'},
        {'4', '5', '6', '7'},
        {'8', '9', 'A', 'B'},
        {'C', 'D', 'E', 'F'} 
    };

    for (unsigned char linha = 0; linha < 4; linha++) {
        PORTD = ~(1 << linha);  // Ativa apenas uma das linhas
        __delay_ms(10);         // Pequeno delay para estabilizar

        if (!PORTBbits.RB0) return matriz_teclas[linha][0];
        if (!PORTBbits.RB1) return matriz_teclas[linha][1];
        if (!PORTBbits.RB2) return matriz_teclas[linha][2];
        if (!PORTBbits.RB3) return matriz_teclas[linha][3];

        __delay_ms(100);
    }
    return '\0';
}

// ------------------------
// Função para Exibir Sequência
// ------------------------
void exibir_sequencia() {
    uart_enviar_string("\r\nSequencia: ");
    if (indice == 0) {
        uart_enviar_string("Vazia\r\n");
        return;
    }
    for (unsigned char i = 0; i < indice; i++) {
        uart_enviar_valor(sequencia[i]);
        uart_enviar_caractere(' ');
    }
    uart_enviar_string("\r\n");
}

void resetar_sequencia() {
    indice = 0;
    for (unsigned char i = 0; i < MAX_SEQ; i++) {
        memoria_eeprom[i] = 0;
        sequencia[i] = 0;
    }
    ajustar_pwm(0);
    uart_enviar_string("Sequencia reiniciada.\r\n");
}

void escrever_memoria(unsigned int valor) {
    if (indice < MAX_SEQ) {
        memoria_eeprom[indice] = valor;
    }
}

// ------------------------
// Configuração e Ajuste do PWM (RC2)
// ------------------------
void configurar_pwm() {
    // RC2 como saída (CCP1)
    TRISC2 = 0;            
    CCP1CON = 0b00001100;  // CCP1 em modo PWM
    PR2 = 255;             
    CCPR1L = 0;            // Duty Cycle inicial
    T2CON = 0b00000100;    // Liga Timer2 (prescaler 1:1)
}

void ajustar_pwm(unsigned int velocidade) {
    if (velocidade > 100) velocidade = 100;
    CCPR1L = (velocidade * 255) / 100; 
}

// ------------------------
// Funções da UART
// ------------------------
void configurar_uart() {
    TXSTA = 0x24;  // TX habilitado, modo assíncrono, BRGH=1
    RCSTA = 0x90;  // UART habilitada
    SPBRG = 12;    // Baud ~9600 para Fosc = 2 MHz
}

void uart_enviar_caractere(char c) {
    while (!TXIF);
    TXREG = c;
}

void uart_enviar_string(const char *str) {
    while (*str) {
        uart_enviar_caractere(*str++);
    }
}

void uart_enviar_valor(unsigned int valor) {
    char buffer[10];
    sprintf(buffer, "%u ", valor);
    uart_enviar_string(buffer);
}
