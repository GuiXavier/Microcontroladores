#include <xc.h>
#define _XTAL_FREQ 20000000
// CONFIG
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = OFF         // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3/PGM pin has PGM function; low-voltage programming enabled)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

#define c0 PORTCbits.RC0
#define c1 PORTCbits.RC1
#define c2 PORTCbits.RC2
#define c3 PORTCbits.RC3
#define b0 PORTBbits.RB0
#define b1 PORTBbits.RB1
#define b2 PORTBbits.RB2
#define b3 PORTBbits.RB3

void teclado();

unsigned char segment[]={0x03,0x9f,0x25,0x0d,0x99,0x49,0x41,0x1f,0x01,0x19,0x11,0xc1,0x63,0x85,0x61,0x71},i=0;
void main(void) 
{
    TRISD = 0x00;
    TRISA = 0x00;
    TRISC = 0x00;
    TRISB = 0xff;
    PORTAbits.RA5 = 0;
    
    while(1)
    {
    teclado();
    PORTD = 0xff;
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