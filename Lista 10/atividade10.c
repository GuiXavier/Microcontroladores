#include <xc.h>

// Configura��es do microcontrolador
#pragma config FOSC = HS       // Oscilador de alta velocidade
#pragma config WDTE = OFF      // Watchdog Timer desativado
#pragma config PWRTE = OFF     // Power-up Timer desativado
#pragma config CP = OFF        // Prote��o de c�digo desativada

#define _XTAL_FREQ 20000000    // Frequ�ncia do oscilador (20 MHz)

// Vari�veis globais
volatile unsigned char estado_led_rd0 = 0; // LED controlado pelo Timer2
volatile unsigned char estado_led_rd1 = 0; // LED controlado por borda de subida em RB0
volatile unsigned char estado_led_rd2 = 0; // LED controlado por mudan�a de estado em RB7
volatile unsigned char estado_anterior_rb7 = 0; // Estado anterior de RB7
volatile unsigned int contador_timer2 = 0; // Contador do Timer2

// Fun��es para configurar perif�ricos
void configurar_pinos(void);
void configurar_timer2(void);
void configurar_interrupcoes(void);

// Rotina de interrup��o
void __interrupt() interrupcao(void) {
    // Interrup��o do Timer2
    if (PIR1bits.TMR2IF) {
        PIR1bits.TMR2IF = 0; // Limpa a flag do Timer2
        if (++contador_timer2 >= 156) { // Aproximadamente 1 segundo
            contador_timer2 = 0;
            estado_led_rd0 = ~estado_led_rd0; // Alterna estado do LED no RD0
            PORTDbits.RD0 = estado_led_rd0;
        }
    }

    // Interrup��o externa (borda de subida em RB0)
    if (INTCONbits.INTF) {
        INTCONbits.INTF = 0; // Limpa a flag de interrup��o externa
        estado_led_rd1 = ~estado_led_rd1; // Alterna estado do LED no RD1
        PORTDbits.RD1 = estado_led_rd1;
    }

    // Interrup��o de mudan�a de estado no PORTB (RB7)
    if (INTCONbits.RBIF) {
        INTCONbits.RBIF = 0; // Limpa a flag de interrup��o de mudan�a de estado
        if (PORTBbits.RB7 != estado_anterior_rb7) {
            estado_led_rd2 = ~estado_led_rd2; // Alterna estado do LED no RD2
            PORTDbits.RD2 = estado_led_rd2;
            estado_anterior_rb7 = PORTBbits.RB7; // Atualiza estado anterior de RB7
        }
    }
}

// Fun��o principal
void main(void) {
    configurar_pinos();
    configurar_timer2();
    configurar_interrupcoes();
    while (1) {
        // Loop principal vazio; funcionamento controlado pelas interrup��es
    }
}

// Configura os pinos de entrada e sa�da
void configurar_pinos(void) {
    TRISD = 0x00;               // Configura PORTD como sa�da
    TRISBbits.TRISB0 = 1;       // Configura RB0 como entrada
    TRISBbits.TRISB7 = 1;       // Configura RB7 como entrada
    PORTD = 0x00;               // Inicializa PORTD (todos os LEDs apagados)
    estado_anterior_rb7 = PORTBbits.RB7; // L� estado inicial de RB7
}

// Configura Timer2 para temporiza��o
void configurar_timer2(void) {
    PR2 = 250;                  // Define o per�odo do Timer2
    T2CONbits.T2CKPS = 0b11;    // Prescaler 1:16
    T2CONbits.TOUTPS = 0b1111;  // P�s-escaler 1:16
    T2CONbits.TMR2ON = 1;       // Habilita Timer2
    PIR1bits.TMR2IF = 0;        // Limpa a flag do Timer2
    PIE1bits.TMR2IE = 1;        // Habilita interrup��o do Timer2
}

// Configura interrup��es
void configurar_interrupcoes(void) {
    INTCONbits.GIE = 1;         // Habilita interrup��es globais
    INTCONbits.PEIE = 1;        // Habilita interrup��es perif�ricas
    OPTION_REGbits.INTEDG = 1;  // Configura borda de subida para RB0
    INTCONbits.INTE = 1;        // Habilita interrup��o externa em RB0
    INTCONbits.INTF = 0;        // Limpa a flag de interrup��o externa
    INTCONbits.RBIE = 1;        // Habilita interrup��o em mudan�a de estado no PORTB
    INTCONbits.RBIF = 0;        // Limpa a flag de interrup��o em PORTB
    OPTION_REGbits.nRBPU = 0;   // Habilita resistores pull-up no PORTB
}
