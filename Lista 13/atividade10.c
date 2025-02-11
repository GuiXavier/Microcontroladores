//-----------------------------
// Configurações do PIC16F877
//-----------------------------
#include <xc.h>
#define _XTAL_FREQ 4000000   // Frequência do oscilador: 4MHz 

//-----------------------------
// Endereço do dispositivo I2C (conversor I2C->paralelo)
//-----------------------------
#define I2C_SLAVE_ADDR 0x00   // Exemplo: 0x27 

//-----------------------------
// Definições para o LCD (mapeamento do conversor)
//-----------------------------
#define LCD_RS 0x01  // Bit0
#define LCD_EN 0x02  // Bit1

//-----------------------------
// Protótipos das funções I2C
//-----------------------------
void I2C_Master_Init(const unsigned long c);
void I2C_Master_Start(void);
void I2C_Master_Restart(void);
void I2C_Master_Stop(void);
void I2C_Master_Write(unsigned char data);

//-----------------------------
// Protótipos das funções do LCD
//-----------------------------
void LCD_sendNibble(unsigned char nibble, unsigned char rs);
void LCD_sendByte(unsigned char byte, unsigned char rs);
void LCD_command(unsigned char cmd);
void LCD_data(unsigned char data);
void LCD_init(void);
void LCD_setCursor(unsigned char linha, unsigned char coluna);
void LCD_print(char *str);

//-----------------------------
// Função principal
//-----------------------------
void main(void) {
    // Inicializa o I2C com a velocidade desejada  100kHz
    I2C_Master_Init(100000);
    
    // Inicializa o LCD
    LCD_init();
    
    // Exibe "bruno" na primeira linha
    LCD_setCursor(1, 1);   // Linha 1, coluna 1
    LCD_print("bruno");
    
    // Exibe "2330776" na segunda linha
    LCD_setCursor(2, 1);   // Linha 2, coluna 1
    LCD_print("2330776");
    
    while(1){
        // Laço infinito (pode inserir outras tarefas)
    }
}

//-----------------------------------------------------
// Implementação das funções I2C no modo mestre
//-----------------------------------------------------
void I2C_Master_Init(const unsigned long c) {
    // Configura o módulo MSSP para modo mestre I2C:
    // - SSPM3:SSPM0 = 1000 (modo mestre, clock = Fosc/(4*(SSPADD+1)))
    // - SSPEN = 1 para habilitar o módulo
    SSPCON  = 0x28;     // 0010 1000: modo mestre I2C e habilita o módulo
    SSPCON2 = 0x00;
    SSPADD  = (_XTAL_FREQ/(4*c))-1; // Ajusta a velocidade (ex.: 100kHz)
    SSPSTAT = 0x00;     // Configura sem slew rate (para 100kHz)
}

void I2C_Master_Start(void) {
    SEN = 1;            // Inicia a condição de partida
    while(SEN);         // Aguarda a conclusão da condição de partida
}

void I2C_Master_Restart(void) {
    RSEN = 1;           // Condição de reinício
    while(RSEN);
}

void I2C_Master_Stop(void) {
    PEN = 1;            // Inicia a condição de parada
    while(PEN);         // Aguarda a conclusão da condição de parada
}

void I2C_Master_Write(unsigned char data) {
    SSPBUF = data;      // Carrega o dado no registrador de buffer
    while(BF);          // Aguarda até a transmissão ser concluída
    // Aguarda até que o módulo I2C esteja livre (verificando os bits de controle)
    while((SSPCON2 & 0x1F) || (SSPSTAT & 0x04));
}

//-----------------------------------------------------
// Funções para controle do LCD via I2C
//-----------------------------------------------------

// Envia um nibble (4 bits) para o LCD, junto com o bit RS
// O procedimento gera um pulso no sinal EN para acionar o latch
void LCD_sendNibble(unsigned char nibble, unsigned char rs) {
    unsigned char data;
    // Coloca os 4 bits do nibble nas posições 2 a 5 do byte
    data = (nibble & 0x0F) << 2;
    // Acrescenta o bit RS, se necessário
    if(rs)
        data |= LCD_RS;
    
    // Gera pulso de Enable: primeiro EN em nível alto, depois baixa
    // Envia via I2C
    // 1) Envia com EN = 1
    data |= LCD_EN;
    I2C_Master_Start();
    // O endereço do escravo é enviado com R/W = 0 (bit 0 em 0)
    I2C_Master_Write(I2C_SLAVE_ADDR << 1);
    I2C_Master_Write(data);
    I2C_Master_Stop();
    
    __delay_us(1);   // Pulso de enable (mínimo de 450ns; 1us é suficiente)
    
    // 2) Envia com EN = 0 (para "travar" os dados no LCD)
    data &= ~LCD_EN;
    I2C_Master_Start();
    I2C_Master_Write(I2C_SLAVE_ADDR << 1);
    I2C_Master_Write(data);
    I2C_Master_Stop();
    
    __delay_us(100);  // Aguarda tempo para o LCD processar (pode ajustar se necessário)
}

// Envia um byte completo (8 bits) para o LCD, enviando primeiro o nibble alto e depois o nibble baixo.
void LCD_sendByte(unsigned char byte, unsigned char rs) {
    LCD_sendNibble(byte >> 4, rs);   // Envia nibble alto
    LCD_sendNibble(byte & 0x0F, rs);   // Envia nibble baixo
}

// Envia um comando para o LCD (rs = 0)
void LCD_command(unsigned char cmd) {
    LCD_sendByte(cmd, 0);
    __delay_ms(2);  // Aguarda o comando ser processado
}

// Envia um dado (caractere) para o LCD (rs = 1)
void LCD_data(unsigned char data) {
    LCD_sendByte(data, 1);
    __delay_ms(2);
}

// Inicializa o LCD (seguindo o procedimento para modo 4-bits)
void LCD_init(void) {
    // Aguardar 15ms após ligar
    __delay_ms(20);
    
    // Inicialização: enviar o comando 0x03 (modo 8-bits) três vezes
    LCD_sendNibble(0x03, 0);
    __delay_ms(5);
    LCD_sendNibble(0x03, 0);
    __delay_us(100);
    LCD_sendNibble(0x03, 0);
    __delay_us(100);
    
    // Mudar para modo 4-bits (enviar 0x02)
    LCD_sendNibble(0x02, 0);
    __delay_us(100);
    
    // Agora o LCD já está em modo 4-bits; envia os comandos de função
    LCD_command(0x28);   // 0x28: Função set -> 4-bit, 2 linhas, fonte 5x8
    LCD_command(0x0C);   // 0x0C: Display ON, cursor OFF
    LCD_command(0x01);   // 0x01: Clear display
    __delay_ms(2);
    LCD_command(0x06);   // 0x06: Entry mode set (incrementa o cursor)
}

// Define a posição do cursor
// linha: 1 ou 2; coluna: 1 a 16
void LCD_setCursor(unsigned char linha, unsigned char coluna) {
    unsigned char pos;
    // Endereços: 0x80 para linha 1 e 0xC0 para linha 2
    if(linha == 1)
        pos = 0x80 + (coluna - 1);
    else
        pos = 0xC0 + (coluna - 1);
    LCD_command(pos);
}

// Envia uma string para o LCD
void LCD_print(char *str) {
    while(*str) {
        LCD_data(*str++);
    }
}