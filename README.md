# Exercicios microcontroladores, UTFPR-TD

# Arquivo teste

# Títulos (Headers)

# Título Grande 
## Título Médio
### Título Pequeno

# Texto em Negrito e Itálico

**Este texto é negrito**
*Este texto é itálico*
***Este texto é negrito e itálico***

# Listas

- Item 1
- Item 2
  - Subitem 1
  - Subitem 2

# Listas ordenadas: Use números seguidos por um ponto para listas ordenadas.

1. Primeiro item
2. Segundo item
   1. Subitem
   2. Subitem

# Links: Para adicionar links, use a seguinte sintaxe:

[Texto do link](URL)

[GitHub](https://github.com)

# Imagens

![Texto alternativo](URL da imagem)

![Logo do GitHub](https://github.githubassets.com/images/modules/logos_page/GitHub-Mark.png)

# Tabelas: Você pode criar tabelas usando | para separar colunas e - para criar a linha do cabeçalho.

| Cabeçalho 1 | Cabeçalho 2 |
| ----------- | ----------- |
| Celula 1    | Celula 2    |
| Celula 3    | Celula 4    |

# Checkboxes (Caixas de Seleção)
# Para criar uma lista de tarefas com caixas de seleção, use - [ ] para itens desmarcados e - [x] para itens marcados.

- [x] Tarefa 1
- [ ] Tarefa 2
- [ ] Tarefa 3


# Exemplo

# Projeto Microcontroladores

Este é um projeto para aprendizado de **microcontroladores**. 

## Funcionalidades
- [x] Criar programa básico
- [ ] Implementar comunicação serial
- [ ] Melhorar eficiência do código

## Como Usar

```bash
git clone https://github.com/GuiXavier/Microcontroladores.git

Blocos de Código: Para mostrar blocos de código, use três crases (```) no início e no final:
# Projeto Microcontrolador - Exemplo Blink LED

Este projeto é um exemplo básico de como piscar um LED utilizando o microcontrolador **PIC16F877A**.

## Código em C para o PIC16F877A

```c
// Arquivo: blink_led.c

#include <xc.h>

// Configurações do PIC (fuses)
#pragma config FOSC = HS        // Oscilador de alta velocidade
#pragma config WDTE = OFF       // Watchdog Timer desligado
#pragma config PWRTE = OFF      // Power-up Timer desligado
#pragma config BOREN = ON       // Brown-out Reset ligado
#pragma config LVP = OFF        // Programação com baixa voltagem desligada
#pragma config CPD = OFF        // Proteção de dados de memória desligada
#pragma config WRT = OFF        // Proteção de código escrita desligada
#pragma config CP = OFF         // Proteção de código desligada

#define _XTAL_FREQ 20000000  // Frequência de 20MHz (ajustar de acordo com o oscilador)

void main(void) {
    // Configura o pino RB0 como saída
    TRISB0 = 0;  // TRISB0 = 0 -> Saída

    while(1) {
        // Acende o LED (RB0 = 1)
        RB0 = 1; 
        __delay_ms(500);  // Aguarda 500 ms

        // Apaga o LED (RB0 = 0)
        RB0 = 0;  
        __delay_ms(500);  // Aguarda 500 ms
    }
}