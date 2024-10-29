
; Projeto 1 - Exemplo de programa em assembly
; Prof. Alessandro
; Descri��o: Exemplo de projeto em assembly - Para piscar um LED

; ------------------- Configura��o dos FUSE BITS ------------------
PROCESSOR 16F877
#include <xc.inc> ; Arquivo da Microchip que cont�m os endere�os e nomes dos registradores

    ;Configura��o do 'hardware' do microcontrolador
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


;cblock 0x20 ; O endere�o 0x20 � a partir de onde eu posso adicionar vari�veis na mem�ria de dados
	;TEMP
;endc

; ------------------ Arquivos inclu�dos no projeto -----------------

; ------------------- Pagina��o da mem�ria (de dados) ----------------
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
; ------------------- Cria��o de vari�veis -------------------------
PSECT udata
 TEMP1:DS 1
 TEMP2:DS 1
    
; ------------------- Defini��o de entradas ---------------   
#define BOTAO			PORTA, 0
#define BOTAO1			PORTA, 1
; ------------------- Defini��o de sa�das -----------------

#define LED			PORTD, 4  
#define LED_ON		bsf	PORTD, 4      
#define LED_OFF		bcf	PORTD, 4 ; bit-clear-file 
    
#define LED1			PORTD, 5  
#define LED1_ON		bsf	PORTD, 5      
#define LED1_OFF	bcf	PORTD, 5
    
#define LED2			PORTB, 6  
#define LED2_ON		bsf	PORTB, 6      
#define LED2_OFF	bcf	PORTB, 6    

#define LED3			PORTB, 7  
#define LED3_ON		bsf	PORTB, 7      
#define LED3_OFF	bcf	PORTB, 7    
                       

	
; ------------------- Vetor de reset -----------------

PSECT code, abs
ORG	0x00		;Define o endere�o inicial de processamento
goto INICIO	 

; ------------------- Vetor de interrup��es -----------------
;PSECT intvector, global, class=CODE, delta=2
;ORG	0x04		;Define o endere�o inicial do tratamento de interrup��es
;retfie	
PSECT code, abs
ORG	0x04		;Define o endere�o inicial de processamento
retfie	

; ------------------- Subrotinas -----------------

DELAY:
	movlw  	11111111B			;inicia a contagem em 10
	movwf  	TEMP2
label:  movlw  	255			;inicia a contagem em 10
	movwf  	TEMP1			;precisamos de uma vari�vel auxiliar que recebe 100
	decfsz	TEMP1			;diminui o valor de TEMP, se 0 pula uma linha. O c�digo fica travado nesta linha at� terminar
	goto    $-1
        decfsz	TEMP2
	goto    label
return

; ------------------- In�cio do programa -------------
;PSECT loopPrincipal, delta=2, abs
;ORG	0x0A	
INICIO:
clrf	PORTA	; Limpa os registradores de portas para garantir
clrf	PORTB	; que n�o h� informa��es remanescentes
clrf	PORTC
clrf	PORTD
clrf	PORTE
    
    

; ------------------- Configura��es do microcontrolador -------------
bank1		;ALTERA PARA O BANCO 1.
movlw	0xFF	; Coloca todos os pinos como entrada (menos o RB7) H`FF` -> W -> TRISA
movwf	TRISA	; Assim, os pinos que n�o iremos utilizar
movlw	0000000B ; ficam em alta imped�ncia e n�o correm o risco
movwf	TRISB	; de queimar por algum motivo
movlw	0xFF
movwf	TRISC
movlw	0x00
movwf	TRISD
movlw	0xFF
movwf	TRISE
;bcf 	TRISB, 7
movlw   7		; colocar com 7 coloca no acumulador movlw 
movwf   ADCON1		; funcao movwf move o 7 para o ADCON1     definindo a PORTA como entrada
    
    
bank0				;RETORNA PARA O BANCO 0.

; ------------------- Programa principal -------------

movlw 1
movwf PORTD

; ----- ADICIONADO     
movlw 1
movwf PORTB

loop:
	btfsc BOTAO 
	goto direita    
	LED_ON
	call DELAY
	LED_OFF
	goto loop
	    
direita:
    
	btfsc BOTAO1
	goto loop
	LED2_ON
	call DELAY
	LED2_OFF
	goto direita
end
    
 
   


