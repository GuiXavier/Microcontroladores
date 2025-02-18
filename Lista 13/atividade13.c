#include <xc.h>
#include <stdio.h>

#define _XTAL_FREQ 4000000  // Frequência do oscilador: 4MHz
#define I2C_SLAVE_ADDR 0x27 // Endereço do PCF8574 (alterar se necessário)

// Definição dos bits do PCF8574 para controle do LCD
#define LCD_RS 0x01  // RS ? P0 do PCF8574
#define LCD_RW 0x02  // RW ? P1 do PCF8574
#define LCD_EN 0x04  // EN ? P2 do PCF8574
#define LCD_BACKLIGHT 0x08  // Ativar luz de fundo (P3 do PCF8574)

//=======================================
// Protótipos das funções I2C
//=======================================
void I2C_Master_Init(unsigned long c);
void I2C_Master_Start(void);
void I2C_Master_Stop(void);
void I2C_Master_Write(unsigned char data);
void I2C_scan(void);

//=======================================
// Protótipos das funções do LCD
//=======================================
void LCD_sendNibble(unsigned char nibble, unsigned char rs);
void LCD_sendByte(unsigned char byte, unsigned char rs);
void LCD_command(unsigned char cmd);
void LCD_data(unsigned char data);
void LCD_init(void);
void LCD_setCursor(unsigned char linha, unsigned char coluna);
void LCD_print(const char *str);
void LCD_backlight(unsigned char state);

//=======================================
// Protótipos das funções UART
//=======================================
void UART_Init(void);
void UART_PrintChar(char c);
void UART_Print(const char *str);
void UART_PrintInt(unsigned int valor);

//=======================================
// Função principal
//=======================================
void main(void) {
    UART_Init();
    I2C_Master_Init(100000);  // Inicializa I2C a 100 kHz
    I2C_scan();  // Verifica se o PCF8574 está respondendo no barramento

    LCD_init();  // Inicializa o LCD via I2C
    LCD_backlight(1);  // Ativa luz de fundo

    LCD_setCursor(1, 1);
    LCD_print("Joao Pedro");  // Nome

    LCD_setCursor(2, 1);
    LCD_print("2095882");  // RA

    while(1);
}

//=======================================
// Implementação das funções I2C
//=======================================
void I2C_Master_Init(unsigned long c) {
    SSPCON  = 0x28;
    SSPCON2 = 0x00;
    SSPADD  = (unsigned char)((_XTAL_FREQ / (4 * c)) - 1);
    SSPSTAT = 0x00;
}

void I2C_Master_Start(void) {
    SEN = 1;  
    while(SEN);
}

void I2C_Master_Stop(void) {
    PEN = 1;  
    while(PEN);
}

void I2C_Master_Write(unsigned char data) {
    SSPBUF = data;
    while (BF);
    if (SSPCON2bits.ACKSTAT) {
        I2C_Master_Stop();
    }
}

//=======================================
// Escaneia o barramento I2C
//=======================================
void I2C_scan(void) {
    unsigned char address;
    UART_Print("\r\nIniciando escaneamento I2C...\r\n");

    for (address = 0x20; address < 0x40; address++) {
        I2C_Master_Start();
        I2C_Master_Write((unsigned char)(address << 1));

        if (!SSPCON2bits.ACKSTAT) {  // Se recebeu ACK, encontrou um dispositivo
            I2C_Master_Stop();
            UART_Print("? Dispositivo encontrado no endereco: 0x");
            UART_PrintInt(address);
            UART_Print("\r\n");
            return;
        }
        I2C_Master_Stop();
    }

    UART_Print("? Nenhum dispositivo I2C encontrado!\r\n");
}

//=======================================
// Implementação do LCD
//=======================================
void LCD_sendNibble(unsigned char nibble, unsigned char rs) {
    unsigned char data = 0;

    // Envia os bits de dados para os pinos corretos do PCF8574
    if (nibble & 0x01) data |= (1 << 4);  // D4 ? P4 no PCF8574
    if (nibble & 0x02) data |= (1 << 5);  // D5 ? P5 no PCF8574
    if (nibble & 0x04) data |= (1 << 6);  // D6 ? P6 no PCF8574
    if (nibble & 0x08) data |= (1 << 7);  // D7 ? P7 no PCF8574

    // Configura RS e RW
    if (rs) data |= LCD_RS;  
    data &= ~LCD_RW;  // RW = 0 para sempre escrever

    data |= LCD_EN;  // Pulso de enable = 1
    data |= LCD_BACKLIGHT; // Mantém o backlight ligado
    I2C_Master_Start();
    I2C_Master_Write(I2C_SLAVE_ADDR << 1);
    I2C_Master_Write(data);
    I2C_Master_Stop();
    __delay_us(50);

    data &= ~LCD_EN;  // Pulso de enable = 0
    I2C_Master_Start();
    I2C_Master_Write(I2C_SLAVE_ADDR << 1);
    I2C_Master_Write(data);
    I2C_Master_Stop();
    __delay_us(50);
}

void LCD_sendByte(unsigned char byte, unsigned char rs) {
    LCD_sendNibble(byte >> 4, rs);
    LCD_sendNibble(byte & 0x0F, rs);
}

void LCD_command(unsigned char cmd) {
    LCD_sendByte(cmd, 0);
    __delay_ms(2);
}

void LCD_data(unsigned char data) {
    LCD_sendByte(data, 1);
    __delay_ms(2);
}

void LCD_init(void) {
    __delay_ms(30);
    LCD_sendNibble(0x03, 0);
    __delay_ms(10);
    LCD_sendNibble(0x03, 0);
    __delay_us(200);
    LCD_sendNibble(0x03, 0);
    __delay_us(200);
    
    LCD_sendNibble(0x02, 0);
    __delay_ms(10);

    LCD_command(0x28);
    LCD_command(0x0C);
    LCD_command(0x01);
    __delay_ms(10);
    LCD_command(0x06);
}

void LCD_setCursor(unsigned char linha, unsigned char coluna) {
    unsigned char pos = (linha == 1) ? (0x80 + coluna - 1) : (0xC0 + coluna - 1);
    LCD_command(pos);
}

void LCD_print(const char *str) {
    while (*str) {
        LCD_data(*str++);
    }
}

void LCD_backlight(unsigned char state) {
    unsigned char data = state ? LCD_BACKLIGHT : 0x00;
    I2C_Master_Start();
    I2C_Master_Write(I2C_SLAVE_ADDR << 1);
    I2C_Master_Write(data);
    I2C_Master_Stop();
}

//=======================================
// Implementação da UART para debug
//=======================================
void UART_Init(void) {
    TXSTA = 0x24;  
    RCSTA = 0x90;  
    SPBRG = 25;    
}

void UART_PrintChar(char c) {
    while (!TXIF);
    TXREG = c;
}

void UART_Print(const char *str) {
    while (*str) {
        UART_PrintChar(*str++);
    }
}

void UART_PrintInt(unsigned int valor) {
    char buffer[10];
    sprintf(buffer, "%u", valor);
    UART_Print(buffer);
}
