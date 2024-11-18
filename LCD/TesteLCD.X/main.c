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
    while(1)
    {
        lcd_command(0x80);//D7=1 D6 a D0 = 0 - 80h. Começa no primeiro digito
        lcd_string("Paulo",5);
        lcd_command(0xC0);
        lcd_string("a2095920",8);
    }
    return;
}
