#include <xc.h>
#define _XTAL_FREQ 20000000
// CONFIG
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = ON         // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3/PGM pin has PGM function; low-voltage programming enabled)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

#define RS PORTEbits.RE0
//#define RW PORTEbits.RE1
#define EN PORTEbits.RE1

#define c0 PORTCbits.RC0
#define c1 PORTCbits.RC1
#define c2 PORTCbits.RC2
#define c3 PORTCbits.RC3
#define b0 PORTBbits.RB0
#define b1 PORTBbits.RB1
#define b2 PORTBbits.RB2
#define b3 PORTBbits.RB3      // DEFININDO as portas do TECLADO numerico

void teclado();

unsigned char segment[]={0x03,0x9f,0x25,0x0d,0x99,0x49,0x41,0x1f,0x01,0x19,0x11,0xc1,0x63,0x85,0x61,0x71},i=0;

void lcd_data(unsigned char data)
{
    PORTD = data;
    RS = 1;
    //RW = 0;
    EN = 1;
    __delay_ms(5);
    EN = 0;
}

void lcd_command(unsigned char cmd)
{
    PORTD = cmd;
    RS = 0;
    //RW = 0;
    EN = 1;
    __delay_ms(5);
    EN = 0;
}

void lcd_string(const unsigned char *str,unsigned char num)  
{
    unsigned char i;
    for(i=0;i<num;i++)
    {
      lcd_data(str[i]);  
        
    }
}

void lcd_initialise()
{
    lcd_command(0x38);//Function Set 0011 1000 (D5=1) (DL=D4=1 - 8 bits) (D3=N=1 - 2 linhas) (D2=F=0 - 5x10 tipo da fonte)
    lcd_command(0x06);//Entry Mode Set 0000 0110 (D2=1) (D1=I/D=1 - Cursor move para direita) (D0=SH=0 mensagem se desloca) 
    lcd_command(0x0C);//Display ON/OFF 0000 1100 (D3=1) (D2=D=1 Liga Display) (D1=C=0 Desliga Cursor) (D0=B=0 Cursor n o pisca)
    lcd_command(0x01);//Display Clear 0000 0001
}

void main(void) 
{
    TRISE = 0x00;
    TRISD = 0x00;
    lcd_initialise();
    
    TRISA = 0x00;
    TRISC = 0x00;    // definindo as entradas e saidas das portas
    TRISB = 0xff;    // PORTB definido como entrada talvez esteja errado
    
    
    while(1)
    {
        teclado();
        PORTD = 0xff;
        
        
        lcd_command(0x80);//D7=1 D6 a D0 = 0 - 80h. Começa no primeiro digito
        lcd_string("NOME",4);   //O NUMERO REPRESENTA A QUANTIDADE DE LETRAS DA PALAVRA
        lcd_command(0xC0);
        lcd_string("MATRICULA",9);    // estudar como funcionar as funções para a tela LCD conforme o pressionamento das teclas
    }
    return;
}
void teclado()
{
    c0=0;c1=1;c2=1;c3=1;
    if(b0==0)
    {
        PORTD = segment[0];
        while(b0==0);
    }
    if(b1==0)
    {
        PORTD = segment[1];
        while(b1==0);
    }
    if(b2==0)
    {
        PORTD = segment[2];
        while(b2==0);
    }
    if(b3==0)
    {
        PORTD = segment[3];
        while(b3==0);
    }
    c0=1;c1=0;c2=1;c3=1;
    if(b0==0)
    {
        PORTD = segment[4];
        while(b0==0);
    }
    if(b1==0)
    {
        PORTD = segment[5];
        while(b1==0);
    }
    if(b2==0)
    {
        PORTD = segment[6];
        while(b2==0);
    }
    if(b3==0)
    {
        PORTD = segment[7];
        while(b3==0);
    }
    c0=1;c1=1;c2=0;c3=1;
    if(b0==0)
    {
        PORTD = segment[8];
        while(b0==0);
    }
    if(b1==0)
    {
        PORTD = segment[9];
        while(b1==0);
    }
    if(b2==0)
    {
        PORTD = segment[10];
        while(b2==0);
    }
    if(b3==0)
    {
        PORTD = segment[11];
        while(b3==0);
    }
     c0=1;c1=1;c2=1;c3=0;
    if(b0==0)
    {
        PORTD = segment[12];
        while(b0==0);
    }
    if(b1==0)
    {
        PORTD = segment[13];
        while(b1==0);
    }
    if(b2==0)
    {
        PORTD = segment[14];
        while(b2==0);
    }
    if(b3==0)
    {
        PORTD = segment[15];
        while(b3==0);
    }
}


