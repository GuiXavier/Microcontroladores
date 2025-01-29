#include <xc.h>

// Configura��es do microcontrolador
#pragma config FOSC = HS       // Oscilador de alta velocidade
#pragma config WDTE = OFF      // Watchdog Timer desativado
#pragma config PWRTE = OFF     // Power-up Timer desativado
#pragma config CP = OFF        // Prote��o de c�digo desativada

#define _XTAL_FREQ 16000000    // Frequ�ncia do oscilador (16 MHz)

// Inicializa o ADC para leitura de tens�o
void inicializar_ADC() {
    ADCON0 = 0b10011001;  // Habilita ADC, seleciona canal AN3 (RA3), FOSC/8
    ADCON1 = 0b10000010;  // Justificado � direita, Vref+ = VDD, Vref- = VSS
}

// Realiza a leitura do ADC no canal especificado
unsigned int realizar_leitura_ADC(unsigned char canal) {
    ADCON0 &= 0xC5;               // Limpa bits de sele��o de canal
    ADCON0 |= (canal << 3);       // Define o canal
    __delay_ms(2);                // Tempo de aquisi��o
    ADCON0bits.GO_DONE = 1;       // Inicia convers�o
    while (ADCON0bits.GO_DONE);   // Aguarda o t�rmino da convers�o
    return ((unsigned int) ADRESH << 8) | ADRESL; // Combina os 10 bits do resultado
}

// Configura PWM no pino RC2
void configurar_PWM() {
    CCP1CON = 0b00001100; // Configura CCP1 em modo PWM
    PR2 = 0xFF;           // Define o per�odo do PWM
    T2CON = 0b00000100;   // Habilita Timer2 com prescaler 1:1
    CCPR1L = 0x00;        // Inicializa Duty Cycle em 0
}

// Ajusta o duty cycle do PWM com valor do ADC
void ajustar_duty_PWM(unsigned int valor) {
    valor &= 0x03FF;                 // Garante que valor esteja entre 0-1023
    CCPR1L = (unsigned char)(valor >> 2);  // Carrega os 8 bits mais significativos
    CCP1CONbits.CCP1X = (valor >> 1) & 1;  // Ajusta o bit menos significativo
    CCP1CONbits.CCP1Y = valor & 1;         // Ajusta o segundo bit menos significativo
}

void main() {
    unsigned int leitura_ADC, duty_PWM;

    // Configura��es iniciais de entradas e sa�das
    TRISA = 0xFF;           // Configura PORTA como entrada (RA3 para ADC)
    TRISCbits.TRISC2 = 0;   // Configura RC2 como sa�da para PWM

    inicializar_ADC();      // Inicializa o ADC
    configurar_PWM();       // Configura o PWM

    while (1) {
        leitura_ADC = realizar_leitura_ADC(3); // L� o ADC no canal AN3 (RA3)
        duty_PWM = leitura_ADC & 0x03FF;       // Garante que esteja entre 0-1023
        ajustar_duty_PWM(duty_PWM);           // Atualiza o PWM com o novo Duty Cycle
        __delay_ms(5);                        // Adiciona um pequeno atraso
    }
}
