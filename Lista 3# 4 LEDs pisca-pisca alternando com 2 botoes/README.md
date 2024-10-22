# 5 BOTÕES E 5 LEDs Piscando

### A partir do programa do item anterior, 1, escreva um programa em Assembly que
quando um botão, ou chave, é pressionado um led distinto piscará.
Deverá ser entregue o arquivos “.s” no moodle e a demostração do circuito funcionando
no simulIDE.

``` assembly
; Projeto 1 - Exemplo de programa em assembly
; Prof. Alessandro
; Descrição: Exemplo de projeto em assembly - Para piscar um LED

; ------------------- Configuração dos FUSE BITS ------------------
PROCESSOR 16F877
#include <xc.inc> ; Arquivo da Microchip que contém os endereços e nomes dos registradores

    ;Configuração do 'hardware' do microcontrolador
; CONFIG
  CONFIG  FOSC = HS             ; Oscillator Selection bits (HS oscillator)
  CONFIG  WDTE = OFF            ; Watchdog Timer Enable bit (WDT disabled)
  CONFIG  PWRTE = OFF           ; Power-up Timer Enable bit (PWRT disabled)
  CONFIG  CP = OFF              ; FLASH Program Memory Code Protection bits (Code protection off)
  CONFIG  BOREN = OFF           ; Brown-out Reset Enable bit (BOR disabled)
  CONFIG  LVP = OFF             ; Low Voltage In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
  CONFIG  CPD = OFF             ; Data EE Memory Code Protection (Code Protection off)
  CONFIG  WRT = OFF             ; FLASH Program Memory Write Enable (Unprotected program memory may not be written to by EECON control)

// config statements should precede project file includes.

;#include <xc.inc>


;cblock 0x20 ; O endereço 0x20 é a partir de onde eu posso adicionar variáveis na memória de dados
	;TEMP
;endc

; ------------------ Arquivos incluídos no projeto -----------------

; ------------------- Paginação da memória (de dados) ----------------
bank0 MACRO 
    BCF STATUS, 5 ;
    BCF STATUS, 6 ;
    endm

bank1 MACRO  
    BSF STATUS, 5 ;
    BCF STATUS, 6 ;
    endm

bank2 MACRO  
    BCF STATUS, 5 ;
    BSF STATUS, 6 ;
    endm
; ------------------- Criação de variáveis -------------------------
PSECT udata
 TEMP1:DS 1
 TEMP2:DS 1
    
; ------------------- Definição de entradas ---------------
#define BOTAO	PORTD, 0

; ------------------- Definição de saídas -----------------
#define LED		PORTD, 7
#define LED_ON		bsf PORTD, 7
#define LED_OFF 	bcf PORTD, 7


; ------------------- Vetor de reset -----------------

PSECT code, abs
ORG	0x00		;Define o endereço inicial de processamento
goto INICIO	 

; ------------------- Vetor de interrupções -----------------
PSECT code, abs
ORG	0x04		;Define o endereço inicial do tratamento de interrupções
retfie


; ------------------- Subrotinas -----------------

DELAY:
	movlw  	11111111B			;inicia a contagem em 10
	movwf  	TEMP2
label:  movlw  	255			;inicia a contagem em 10
	movwf  	TEMP1			;precisamos de uma variável auxiliar que recebe 100
	decfsz	TEMP1			;diminui o valor de TEMP, se 0 pula uma linha. O código fica travado nesta linha até terminar
	goto    $-1
        decfsz	TEMP2
	goto    label
return

; ------------------- Início do programa -------------
;PSECT loopPrincipal, delta=2, abs
;ORG	0x0A	
INICIO:
clrf	PORTA	; Limpa os registradores de portas para garantir
clrf	PORTB	; que não há informações remanescentes
clrf	PORTC
clrf	PORTD
clrf	PORTE

; ------------------- Configurações do microcontrolador -------------
bank1		;ALTERA PARA O BANCO 1.
movlw	0xFF	; Coloca todos os pinos como entrada (menos o RB7) H`FF` -> W -> TRISA
movwf	TRISA	; Assim, os pinos que não iremos utilizar
movlw	0000001B ; ficam em alta impedância e não correm o risco
movwf	TRISB	; de queimar por algum motivo
movlw	0xFF
movwf	TRISC
movlw	0x01
movwf	TRISD
movlw	0xFF
movwf	TRISE
;bcf 	TRISB, 7

bank0				;RETORNA PARA O BANCO 0.

; ------------------- Programa principal -------------

    Loop:

    BTFSC BOTAO
    LED_OFF
    BTFSS BOTAO
    LED_ON
    goto Loop
end
```