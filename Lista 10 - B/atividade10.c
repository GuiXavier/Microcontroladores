#include <xc.h>

#pragma config FOSC = XT
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config CP = OFF
#pragma config BOREN = OFF
#pragma config LVP = OFF
#pragma config WRT = OFF

#define _XTAL_FREQ 4000000

#define MOTOR_PIN  PORTDbits.RD0
#define BTN_RB0    PORTBbits.RB0
#define BTN_RB1    PORTBbits.RB1

volatile unsigned char motor_ativo = 0;
volatile unsigned char tempo_alvo_overflows = 0;
volatile unsigned char contagem_overflows = 0;

void configurar_pinos(void);
void configurar_timer1(void);
void configurar_interrupcoes(void);

void __interrupt() isr(void)
{
    if (PIR1bits.TMR1IF)
    {
        PIR1bits.TMR1IF = 0;
        if (motor_ativo)
        {
            contagem_overflows++;
            if (contagem_overflows >= tempo_alvo_overflows)
            {
                MOTOR_PIN = 0;
                motor_ativo = 0;
            }
        }
    }

    if (INTCONbits.INTF)
    {
        INTCONbits.INTF = 0;
        if (!motor_ativo)
        {
            motor_ativo = 1;
            contagem_overflows = 0;
            tempo_alvo_overflows = 20;   // original era 19
            MOTOR_PIN = 1;
        }
    }
}

void main(void)
{
    configurar_pinos();
    configurar_timer1();
    configurar_interrupcoes();

    static unsigned char rb1_anterior = 1;

    while(1)
    {
        unsigned char rb1_atual = BTN_RB1;
        if (rb1_anterior == 1 && rb1_atual == 0)
        {
            if (!motor_ativo)
            {
                motor_ativo = 1;
                contagem_overflows = 0;
                tempo_alvo_overflows = 39;  // original era 38
                MOTOR_PIN = 1;
            }
        }
        rb1_anterior = rb1_atual;
    }
}

void configurar_pinos(void)
{
    TRISD = 0x00;
    PORTD = 0x00;
    TRISBbits.TRISB0 = 1;
    TRISBbits.TRISB1 = 1;
    MOTOR_PIN = 0;
}

void configurar_timer1(void)
{
    T1CONbits.TMR1CS = 0;
    T1CONbits.T1CKPS = 0b11;
    T1CONbits.TMR1ON = 1;
    TMR1H = 0;
    TMR1L = 0;
    PIR1bits.TMR1IF = 0;
    PIE1bits.TMR1IE = 1;
}

void configurar_interrupcoes(void)
{
    INTCONbits.GIE  = 1;
    INTCONbits.PEIE = 1;
    OPTION_REGbits.INTEDG = 0;
    INTCONbits.INTE = 1;
    INTCONbits.INTF = 0;
    OPTION_REGbits.nRBPU = 0;
}