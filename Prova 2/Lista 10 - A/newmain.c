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

// LCD (4 bits)
#define RS PORTEbits.RE0
#define EN PORTEbits.RE1
#define D4 PORTDbits.RD4
#define D5 PORTDbits.RD5
#define D6 PORTDbits.RD6
#define D7 PORTDbits.RD7

// Botão e LED
#define BOTAO   RB0
#define LED     RB1

// Variáveis globais
volatile unsigned char fAtualizaLCD = 0;   
volatile unsigned int contadorT2 = 0;
volatile unsigned char contando3s = 0;     

char textoLCD[16]; // Para mostrar valor no LCD



void configInterrupcoes();
void configTimer2();
void configUART();
void enviarChar(char c);
void enviarString(const char *s);

void lcd_inicializar();
void lcd_comando(unsigned char c);
void lcd_dado(unsigned char d);
void lcd_pos(unsigned char l, unsigned char c);
void lcd_texto(const char *s);

void adc_inicializar();
unsigned int adc_leitura();


//------------------------------------------------------------------------------
// Interrupções
void __interrupt() isrGeral(void)
{
    // Botão em RB0
    if (INTCONbits.INTF)
    {
        INTCONbits.INTF = 0;
        fAtualizaLCD = 1;
    }
    // Timer2
    if (PIR1bits.TMR2IF)
    {
        PIR1bits.TMR2IF = 0;
        if (contando3s)
        {
            contadorT2++;
            if (contadorT2 >= 312) // ~3s
            {
                contadorT2 = 0;
                contando3s = 0;
                T2CONbits.TMR2ON = 0;
                LED = 0; // Apaga LED
                enviarString("OK\r\n");
            }
        }
    }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Programa principal
void main()
{
    // I/Os
    TRISA = 0xFF; 
    TRISBbits.TRISB0 = 1; 
    TRISBbits.TRISB1 = 0; 
    TRISD = 0x00;
    TRISE = 0x00;

    lcd_inicializar();
    adc_inicializar();
    configUART();
    configTimer2();
    configInterrupcoes();

    lcd_pos(0, 0);
    lcd_texto("Tensao:");

    while(1)
    {
        if (fAtualizaLCD)
        {
            fAtualizaLCD = 0;

            unsigned int adc_val = adc_leitura();
            float tensao = (adc_val * 5.0f) / 1023.0f;

            lcd_pos(1, 0);
            sprintf(textoLCD, "V: %.2f V", tensao);
            lcd_texto(textoLCD);

            contadorT2 = 0;
            contando3s = 1;
            T2CONbits.TMR2ON = 1;
            LED = 1; // Liga LED
        }
    }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Configurações e periféricos
void adc_inicializar()
{
    ADCON0 = 0b10011001;  
    ADCON1 = 0b10000010;  
}

unsigned int adc_leitura()
{
    __delay_us(20);
    ADCON0bits.GO_nDONE = 1;
    while(ADCON0bits.GO_nDONE);
    return ((ADRESH << 8) + ADRESL);
}

void configInterrupcoes()
{
    INTCONbits.GIE  = 1;   
    INTCONbits.PEIE = 1;   
    INTCONbits.INTE = 1;   
    OPTION_REGbits.INTEDG = 1;
    INTCONbits.INTF = 0;   
}

void configTimer2()
{
    T2CONbits.T2CKPS = 0b11;  // prescaler=16
    T2CONbits.TOUTPS = 0b1011; // postscaler=12
    PR2 = 249; 
    TMR2 = 0;
    PIR1bits.TMR2IF = 0;
    PIE1bits.TMR2IE = 1;
}

void configUART()
{
    TRISCbits.TRISC6 = 1; 
    TRISCbits.TRISC7 = 1; 
    SPBRG = 129;
    TXSTAbits.BRGH = 1;
    TXSTAbits.SYNC = 0;
    RCSTAbits.SPEN = 1;
    RCSTAbits.RX9  = 0;
    RCSTAbits.CREN = 0;
    TXSTAbits.TXEN = 1;
}

void enviarChar(char c)
{
    while(!TXSTAbits.TRMT);
    TXREG = c;
}

void enviarString(const char *s)
{
    while(*s) enviarChar(*s++);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Funções de LCD
void lcd_inicializar()
{
    lcd_comando(0x33);
    lcd_comando(0x32);
    lcd_comando(0x28); 
    lcd_comando(0x0C); 
    lcd_comando(0x06); 
    lcd_comando(0x01); 
    __delay_ms(2);
}

void lcd_comando(unsigned char c)
{
    D4 = (c >> 4) & 0x01;
    D5 = (c >> 5) & 0x01;
    D6 = (c >> 6) & 0x01;
    D7 = (c >> 7) & 0x01;
    RS = 0;
    EN = 1; __delay_ms(2); EN = 0;

    D4 =  c       & 0x01;
    D5 = (c >> 1) & 0x01;
    D6 = (c >> 2) & 0x01;
    D7 = (c >> 3) & 0x01;
    EN = 1; __delay_ms(2); EN = 0;
}

void lcd_dado(unsigned char d)
{
    D4 = (d >> 4) & 0x01;
    D5 = (d >> 5) & 0x01;
    D6 = (d >> 6) & 0x01;
    D7 = (d >> 7) & 0x01;
    RS = 1;
    EN = 1; __delay_ms(2); EN = 0;

    D4 =  d       & 0x01;
    D5 = (d >> 1) & 0x01;
    D6 = (d >> 2) & 0x01;
    D7 = (d >> 3) & 0x01;
    EN = 1; __delay_ms(2); EN = 0;
}

void lcd_texto(const char *s)
{
    while(*s) lcd_dado(*s++);
}

void lcd_pos(unsigned char l, unsigned char c)
{
    unsigned char pos;
    pos = (l == 0) ? (0x80 + c) : (0xC0 + c);
    lcd_comando(pos);
}
//------------------------------------------------------------------------------