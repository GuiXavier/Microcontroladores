/*
 * File: main.c
 * Author: Trae AI
 * Description: PIC16F877A program that reads analog value from A3 when RB0 button is pressed,
 *              displays it on LCD, counts 3 seconds with Timer2, and sends 'ok' via UART
 */

#include <xc.h>
#include <stdio.h>

// Configuration bits
#pragma config FOSC = HS        // External High-Speed crystal
#pragma config WDTE = OFF       // Watchdog Timer disabled
#pragma config PWRTE = ON       // Power-up Timer enabled
#pragma config BOREN = ON       // Brown-out Reset enabled
#pragma config LVP = OFF        // Low Voltage Programming disabled
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection disabled
#pragma config WRT = OFF        // Flash Program Memory Write disabled
#pragma config CP = OFF         // Flash Program Memory Code Protection disabled

#define _XTAL_FREQ 20000000     // 20MHz crystal frequency

// LCD pin definitions (4-bit mode)
#define LCD_RS      RD2         // Register Select pin
#define LCD_EN      RD3         // Enable pin
#define LCD_D4      RD4         // Data pin 4
#define LCD_D5      RD5         // Data pin 5
#define LCD_D6      RD6         // Data pin 6
#define LCD_D7      RD7         // Data pin 7

// LCD commands
#define LCD_CLEAR           0x01
#define LCD_RETURN_HOME     0x02
#define LCD_ENTRY_MODE_SET  0x04
#define LCD_DISPLAY_CONTROL 0x08
#define LCD_FUNCTION_SET    0x20
#define LCD_SET_CGRAM_ADDR  0x40
#define LCD_SET_DDRAM_ADDR  0x80

// LCD flags for display entry mode
#define LCD_ENTRY_RIGHT     0x00
#define LCD_ENTRY_LEFT      0x02
#define LCD_ENTRY_SHIFT_INC 0x01
#define LCD_ENTRY_SHIFT_DEC 0x00

// LCD flags for display on/off control
#define LCD_DISPLAY_ON      0x04
#define LCD_DISPLAY_OFF     0x00
#define LCD_CURSOR_ON       0x02
#define LCD_CURSOR_OFF      0x00
#define LCD_BLINK_ON        0x01
#define LCD_BLINK_OFF       0x00

// LCD flags for function set
#define LCD_8BIT_MODE       0x10
#define LCD_4BIT_MODE       0x00
#define LCD_2LINE           0x08
#define LCD_1LINE           0x00
#define LCD_5x10DOTS        0x04
#define LCD_5x8DOTS         0x00

// LED pin definition
#define LED_PIN             RB1

// Global variables
 volatile unsigned int adc_value = 0;
 volatile unsigned char timer_count = 0;
 volatile unsigned char update_lcd = 0;
 volatile unsigned char timer_running = 0;

// Function prototypes
void initialize_system(void);
void initialize_lcd(void);
void lcd_send_cmd(unsigned char cmd);
void lcd_send_data(unsigned char data);
void lcd_send_string(const char *str);
void lcd_set_cursor(unsigned char row, unsigned char col);
void lcd_clear(void);
void initialize_adc(void);
unsigned int read_adc(unsigned char channel);
void initialize_uart(void);
void uart_send_char(unsigned char data);
void uart_send_string(const char *str);

// Interrupt Service Routine
void __interrupt() isr(void) {
    // External Interrupt (RB0/INT)
    if(INTCONbits.INTF) {
        // Read ADC value from channel 3 (AN3)
        adc_value = read_adc(3);
        update_lcd = 1;
        
        // Start the 3-second timer
        timer_count = 0;
        timer_running = 1;
        LED_PIN = 1;  // Turn on LED
        
        // Clear interrupt flag
        INTCONbits.INTF = 0;
    }
    
    // Timer2 Interrupt
    if(PIR1bits.TMR2IF) {
        timer_count++;
        
        // Check if 3 seconds have elapsed (depends on Timer2 configuration)
        // With prescaler 16, postscaler 15, and 20MHz crystal, Timer2 interrupt occurs every 12ms
        // So we need 250 interrupts to reach 3 seconds (250 * 12ms = 3000ms)
        if(timer_count >= 250 && timer_running) {
            timer_running = 0;
            LED_PIN = 0;  // Turn off LED
            
            // Send "ok" via UART
            uart_send_string("ok\r\n");
        }
        
        // Clear interrupt flag
        PIR1bits.TMR2IF = 0;
    }
}

int main(void) {
    char voltage_str[16];
    float voltage;
    
    // Initialize system components
    initialize_system();
    initialize_lcd();
    initialize_adc();
    initialize_uart();
    
    // Display initial message
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_send_string("Voltage: ");
    
    // Enable global interrupts
    INTCONbits.GIE = 1;
    
    while(1) {
        // Update LCD if new ADC value is available
        if(update_lcd) {
            // Convert ADC value to voltage (0-5V)
            voltage = (float)((float)adc_value * 5.0f) / 1023.0f;
            
            // Format voltage string with 2 decimal places
            sprintf(voltage_str, "%.2fV    ", voltage);
            
            // Display voltage on LCD
            lcd_set_cursor(0, 9);
            lcd_send_string(voltage_str);
            
            update_lcd = 0;
        }
        
        // Other tasks can be performed here
        __delay_ms(10);
    }
    
    return 0;
}

// Initialize system components
void initialize_system(void) {
    // Configure RB0 as input for external interrupt
    TRISBbits.TRISB0 = 1;  // RB0 as input
    
    // Configure RB1 as output for LED
    TRISBbits.TRISB1 = 0;  // RB1 as output
    LED_PIN = 0;           // Initially off
    
    // Configure PORTD for LCD
    TRISD = 0x00;          // All PORTD pins as output for LCD
    
    // Configure external interrupt on RB0
    INTCONbits.INTE = 1;   // Enable RB0/INT external interrupt
    INTCONbits.INTF = 0;   // Clear interrupt flag
    OPTION_REGbits.INTEDG = 1; // Interrupt on rising edge
    
    // Configure Timer2
    T2CON = 0x00;          // Stop Timer2, prescaler 1:1
    PR2 = 255;             // Timer2 period register
    T2CONbits.T2CKPS = 0b11; // Prescaler 1:16
    T2CONbits.TOUTPS = 0b1110; // Postscaler 1:15
    PIE1bits.TMR2IE = 1;   // Enable Timer2 interrupt
    T2CONbits.TMR2ON = 1;  // Start Timer2
    
    // Enable peripheral interrupts
    INTCONbits.PEIE = 1;
}

// Initialize LCD in 4-bit mode
void initialize_lcd(void) {
    // Wait for LCD to power up
    __delay_ms(50);
    
    // Initialize LCD in 4-bit mode
    // First, force it into 8-bit mode (in case it was in 4-bit mode)
    LCD_RS = 0;
    LCD_EN = 0;
    
    // Send 0x30 three times to ensure 8-bit mode
    LCD_D7 = 0; LCD_D6 = 0; LCD_D5 = 1; LCD_D4 = 1;
    LCD_EN = 1; __delay_us(1); LCD_EN = 0; __delay_ms(5);
    
    LCD_EN = 1; __delay_us(1); LCD_EN = 0; __delay_ms(5);
    
    LCD_EN = 1; __delay_us(1); LCD_EN = 0; __delay_ms(5);
    
    // Now set to 4-bit mode
    LCD_D7 = 0; LCD_D6 = 0; LCD_D5 = 1; LCD_D4 = 0;
    LCD_EN = 1; __delay_us(1); LCD_EN = 0; __delay_ms(5);
    
    // Function set: 4-bit mode, 2 lines, 5x8 dots
    lcd_send_cmd(LCD_FUNCTION_SET | LCD_4BIT_MODE | LCD_2LINE | LCD_5x8DOTS);
    
    // Display control: display on, cursor off, blink off
    lcd_send_cmd(LCD_DISPLAY_CONTROL | LCD_DISPLAY_ON | LCD_CURSOR_OFF | LCD_BLINK_OFF);
    
    // Entry mode set: increment cursor position, no display shift
    lcd_send_cmd(LCD_ENTRY_MODE_SET | LCD_ENTRY_LEFT | LCD_ENTRY_SHIFT_DEC);
    
    // Clear display
    lcd_clear();
}

// Send command to LCD
void lcd_send_cmd(unsigned char cmd) {
    LCD_RS = 0;  // Command mode
    
    // Send high nibble
    LCD_D7 = (cmd & 0x80) ? 1 : 0;
    LCD_D6 = (cmd & 0x40) ? 1 : 0;
    LCD_D5 = (cmd & 0x20) ? 1 : 0;
    LCD_D4 = (cmd & 0x10) ? 1 : 0;
    
    // Pulse enable pin
    LCD_EN = 1;
    __delay_us(1);
    LCD_EN = 0;
    __delay_us(100);
    
    // Send low nibble
    LCD_D7 = (cmd & 0x08) ? 1 : 0;
    LCD_D6 = (cmd & 0x04) ? 1 : 0;
    LCD_D5 = (cmd & 0x02) ? 1 : 0;
    LCD_D4 = (cmd & 0x01) ? 1 : 0;
    
    // Pulse enable pin
    LCD_EN = 1;
    __delay_us(1);
    LCD_EN = 0;
    __delay_us(100);
}

// Send data to LCD
void lcd_send_data(unsigned char data) {
    LCD_RS = 1;  // Data mode
    
    // Send high nibble
    LCD_D7 = (data & 0x80) ? 1 : 0;
    LCD_D6 = (data & 0x40) ? 1 : 0;
    LCD_D5 = (data & 0x20) ? 1 : 0;
    LCD_D4 = (data & 0x10) ? 1 : 0;
    
    // Pulse enable pin
    LCD_EN = 1;
    __delay_us(1);
    LCD_EN = 0;
    __delay_us(100);
    
    // Send low nibble
    LCD_D7 = (data & 0x08) ? 1 : 0;
    LCD_D6 = (data & 0x04) ? 1 : 0;
    LCD_D5 = (data & 0x02) ? 1 : 0;
    LCD_D4 = (data & 0x01) ? 1 : 0;
    
    // Pulse enable pin
    LCD_EN = 1;
    __delay_us(1);
    LCD_EN = 0;
    __delay_us(100);
}

// Send string to LCD
void lcd_send_string(const char *str) {
    unsigned char i = 0;
    while(str[i] != '\0') {
        lcd_send_data(str[i++]);
    }
}

// Set LCD cursor position
void lcd_set_cursor(unsigned char row, unsigned char col) {
    unsigned char address;
    
    // Calculate address based on row and column
    if(row == 0) {
        address = 0x80 + col;  // First row starts at 0x80
    } else {
        address = 0xC0 + col;  // Second row starts at 0xC0
    }
    
    lcd_send_cmd(address);
}

// Clear LCD display
void lcd_clear(void) {
    lcd_send_cmd(LCD_CLEAR);
    __delay_ms(2);  // Clear command needs longer delay
}

// Initialize ADC
void initialize_adc(void) {
    // Configure AN3 (RA3) as analog input
    TRISAbits.TRISA3 = 1;  // RA3 as input
    
    // Configure ADC
    ADCON1bits.ADFM = 1;    // Right justified result
    ADCON1bits.PCFG = 0b1100; // AN0-AN3 as analog, rest as digital
    
    ADCON0bits.ADCS = 0b10; // Fosc/32 as ADC clock
    ADCON0bits.CHS = 3;     // Select channel 3 (AN3)
    ADCON0bits.ADON = 1;    // Turn on ADC
}

// Read ADC value from specified channel
unsigned int read_adc(unsigned char channel) {
    // Select channel
    ADCON0bits.CHS = channel;
    
    // Wait for acquisition time
    __delay_us(20);
    
    // Start conversion
    ADCON0bits.GO_nDONE = 1;
    
    // Wait for conversion to complete
    while(ADCON0bits.GO_nDONE);
    
    // Return result (10-bit value)
    return ((unsigned int)(ADRESH << 8) + (unsigned int)ADRESL);
}

// Initialize UART
void initialize_uart(void) {
    // Configure RC6/TX and RC7/RX pins
    TRISCbits.TRISC6 = 0;    // RC6/TX as output
    TRISCbits.TRISC7 = 1;    // RC7/RX as input
    
    // Configure UART
    SPBRG = 129;             // 9600 baud rate with 20MHz crystal (BRGH=1)
    TXSTAbits.BRGH = 1;      // High speed
    TXSTAbits.SYNC = 0;      // Asynchronous mode
    RCSTAbits.SPEN = 1;      // Enable serial port
    TXSTAbits.TXEN = 1;      // Enable transmission
    RCSTAbits.CREN = 1;      // Enable reception
}

// Send a character via UART
void uart_send_char(unsigned char data) {
    // Wait until transmit buffer is empty
    while(!TXSTAbits.TRMT);
    
    // Send data
    TXREG = data;
}

// Send a string via UART
void uart_send_string(const char *str) {
    unsigned char i = 0;
    while(str[i] != '\0') {
        // Wait until transmit buffer is empty
        while(!TXSTAbits.TRMT);
        // Send data directly without calling uart_send_char
        TXREG = str[i++];
    }
}