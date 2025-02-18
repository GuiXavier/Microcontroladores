/*
 * Arquivo: main.c
 * Compilador: XC8 (Microchip)
 * Dispositivo: PIC16F877A
 * 
 * 
 * 
 */

#include <xc.h>
#include <stdint.h>

// CONFIGURAÇÃO DO HARDWARE
#pragma config FOSC  = HS
#pragma config WDTE  = OFF
#pragma config PWRTE = ON
#pragma config BOREN = ON
#pragma config LVP   = OFF
#pragma config CPD   = OFF
#pragma config WRT   = OFF
#pragma config CP    = OFF

#define _XTAL_FREQ 20000000UL   // Frequência do clock

// DEFINIÇÕES DO LCD
#define LCD_RS     RE0
#define LCD_EN     RE1
#define LCD_D4     RD4
#define LCD_D5     RD5
#define LCD_D6     RD6
#define LCD_D7     RD7

#define LCD_RS_DIR TRISE0
#define LCD_EN_DIR TRISE1
#define LCD_D4_DIR TRISD4
#define LCD_D5_DIR TRISD5
#define LCD_D6_DIR TRISD6
#define LCD_D7_DIR TRISD7

// DEFINIÇÕES DO TECLADO 4x4
//#define KEYPAD_ROW0   RC0
//#define KEYPAD_ROW1   RC1
//#define KEYPAD_ROW2   RC2
//#define KEYPAD_ROW3   RC3
//
//#define KEYPAD_COL0   RB0
//#define KEYPAD_COL1   RB1
//#define KEYPAD_COL2   RB2
//#define KEYPAD_COL3   RB3

#define KEYPAD_ROW0   PORTCbits.RC0
#define KEYPAD_ROW1   PORTCbits.RC1
#define KEYPAD_ROW2   PORTCbits.RC2
#define KEYPAD_ROW3   PORTCbits.RC3

#define KEYPAD_COL0   PORTBbits.RB0
#define KEYPAD_COL1   PORTBbits.RB1
#define KEYPAD_COL2   PORTBbits.RB2
#define KEYPAD_COL3   PORTBbits.RB3



#define EEPROM_ADDR 0x00

// PROTÓTIPOS
void LCD_Init(void);
void LCD_Cmd(unsigned char cmd);
void LCD_Char(char data);
void LCD_String(const char *str);
void LCD_Clear(void);
void LCD_SetCursor(uint8_t row, uint8_t col);

void Keypad_Init(void);
char Keypad_GetChar(void);

void EEPROM_Write(uint8_t address, uint8_t data);
uint8_t EEPROM_Read(uint8_t address);

// FUNÇÃO PRINCIPAL
void main(void)
{
    char key_pressed;
    char previous_key = 0;  
    char eeprom_value;

    LCD_Init();
    Keypad_Init();

    while(1)
    {
        LCD_SetCursor(1, 1);
        LCD_String("Dig. Tecla");

        key_pressed = Keypad_GetChar();
        LCD_SetCursor(2, 1);
        LCD_Char(key_pressed);

        if (key_pressed == 'F')
        {
            if (previous_key != 0)
            {
                EEPROM_Write(EEPROM_ADDR, (uint8_t)previous_key);
                __delay_ms(10);
            }
            LCD_Clear();
            continue;
        }

        if (key_pressed == 'E')
        {
            eeprom_value = EEPROM_Read(EEPROM_ADDR);
            LCD_Clear();
            LCD_SetCursor(1, 1);
            LCD_String("Tecla Dig.");
            LCD_SetCursor(2, 1);
            LCD_Char(eeprom_value);
            (void)Keypad_GetChar();
//            LCD_Clear();
//            continue;
            
            if(key_pressed != 'E')
            {       
                    LCD_Clear();
                    continue;
            }    
            
        }

        previous_key = key_pressed;
    }
}

// IMPLEMENTAÇÃO DO LCD
void LCD_Init(void)
{
    LCD_RS_DIR = LCD_EN_DIR = LCD_D4_DIR = LCD_D5_DIR = LCD_D6_DIR = LCD_D7_DIR = 0;

    __delay_ms(20); 

    LCD_Cmd(0x03);
    __delay_ms(5);
    LCD_Cmd(0x03);
    __delay_us(200);
    LCD_Cmd(0x03);
    __delay_us(200);

    LCD_Cmd(0x02);
    LCD_Cmd(0x28);
    LCD_Cmd(0x0C);
    LCD_Cmd(0x06);
    LCD_Cmd(0x01);
    __delay_ms(2);
}

void LCD_Cmd(unsigned char cmd)
{
    LCD_RS = 0;
    PORTD = (PORTD & 0x0F) | (cmd & 0xF0);
    LCD_EN = 1; __delay_us(10); LCD_EN = 0;

    PORTD = (PORTD & 0x0F) | ((cmd << 4) & 0xF0);
    LCD_EN = 1; __delay_us(10); LCD_EN = 0;

    __delay_ms(2);
}

void LCD_Char(char data)
{
    LCD_RS = 1;
    PORTD = (PORTD & 0x0F) | (data & 0xF0);
    LCD_EN = 1; __delay_us(10); LCD_EN = 0;

    PORTD = (PORTD & 0x0F) | ((data << 4) & 0xF0);
    LCD_EN = 1; __delay_us(10); LCD_EN = 0;

    __delay_ms(2);
}

void LCD_String(const char *str)
{
    while(*str) LCD_Char(*str++);
}

void LCD_Clear(void)
{
    LCD_Cmd(0x01);
    __delay_ms(2);
}

void LCD_SetCursor(uint8_t row, uint8_t col)
{
    uint8_t address = (row == 1) ? 0x80 + (col - 1) : 0xC0 + (col - 1);
    LCD_Cmd(address);
}

// IMPLEMENTAÇÃO DO TECLADO
static const char keypad_map[4][4] = 
{
    {'0','1','2','3'},
    {'4','5','6','7'},
    {'8','9','A','B'},
    {'C','D','E','F'}
};

void Keypad_Init(void)
{
    TRISC = 0xF0;  // Linhas como saída, colunas como entrada
    OPTION_REGbits.nRBPU = 0; 
}

char Keypad_GetChar(void)
{
    while(1)
    {
        for(uint8_t row = 0; row < 4; row++)
        {
            PORTC = ~(1 << row);
            __delay_us(50);

            if(KEYPAD_COL0 == 0) { while(KEYPAD_COL0==0); return keypad_map[row][0]; }
            if(KEYPAD_COL1 == 0) { while(KEYPAD_COL1==0); return keypad_map[row][1]; }
            if(KEYPAD_COL2 == 0) { while(KEYPAD_COL2==0); return keypad_map[row][2]; }
            if(KEYPAD_COL3 == 0) { while(KEYPAD_COL3==0); return keypad_map[row][3]; }
        }
    }
}

// IMPLEMENTAÇÃO DA EEPROM
void EEPROM_Write(uint8_t address, uint8_t data)
{
   
    EEADR = address; 
    EEDATA = data;   

    EECON1bits.EEPGD = 0; 
    EECON1bits.WREN  = 1; 

    INTCONbits.GIE = 0;  
    EECON2 = 0x55;   
    EECON2 = 0xAA;   
    EECON1bits.WR = 1;   
    __nop();             

    EECON1bits.WREN = 0; 
    INTCONbits.GIE = 1;  
}

uint8_t EEPROM_Read(uint8_t address)
{
    EEADR = address;
    EECON1bits.EEPGD = 0; 
    EECON1bits.RD = 1;    
    __nop();
    return EEDATA;
}