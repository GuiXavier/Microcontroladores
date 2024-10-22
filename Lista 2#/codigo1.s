
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
    
#define BOTAO1  PORTA, 0
#define BOTAO2  PORTA, 1
#define BOTAO3  PORTA, 2
#define BOTAO4  PORTA, 3    


; ------------------- Definição de saídas -----------------
#define LED			PORTD, 7
#define LED_ON		bsf PORTD, 7
#define LED_OFF 	bcf PORTD, 7
    
; TALVEZ OS DE BAIXO TENHA QUE ALTERAR AS VARIAVEIS    ADICIONADO
    
#define LED1			PORTB, 0  
#define LED1_ON		bsf	PORTB, 0      
#define LED1_OFF	bcf	PORTB, 0 ; bit-clear-file 
    
#define LED2			PORTB, 1  
#define LED2_ON		bsf	PORTB, 1      
#define LED2_OFF	bcf	PORTB, 1
    
#define LED3			PORTB, 2  
#define LED3_ON		bsf	PORTB, 2      
#define LED3_OFF	bcf	PORTB, 2    

#define LED4			PORTB, 3  
#define LED4_ON		bsf	PORTB, 3      
#define LED4_OFF	bcf	PORTB, 3    
    
; ------------------- Vetor de reset -----------------

PSECT code, abs
ORG	0x00		;Define o endereço inicial de processamento
goto INICIO	 

; ------------------- Vetor de interrupções -----------------
;PSECT intvector, global, class=CODE, delta=2
;ORG	0x04		;Define o endereço inicial do tratamento de interrupções
;retfie	
PSECT code, abs
ORG	0x04		;Define o endereço inicial de processamento
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
movlw	0000000B ; ficam em alta impedância e não correm o risco
movwf	TRISB	; de queimar por algum motivo
movlw	0xFF
movwf	TRISC
movlw	0x01
movwf	TRISD
movlw	0xFF
movwf	TRISE
;bcf 	TRISB, 7
movlw   7		; colocar com 7 coloca no acumulador movlw 
movwf   ADCON1		; funcao movwf move o 7 para o ADCON1 
    
    
bank0				;RETORNA PARA O BANCO 0.

; ------------------- Programa principal -------------

loop:
    btfsc BOTAO
    goto loop1
    LED_ON
    call DELAY
    LED_OFF
    call DELAY
    goto loop

loop1:    
    btfsc BOTAO1
    goto loop2
    LED1_ON
    call DELAY
    LED1_OFF
    call DELAY
    goto loop1
    
loop2:
    btfsc BOTAO2
    goto loop3
    LED2_ON
    call DELAY
    LED2_OFF
    call DELAY
    goto loop2
    
loop3:  
    btfsc BOTAO3
    goto loop4
    LED3_ON
    call DELAY
    LED3_OFF
    call DELAY 
    goto loop3
    
loop4:   
    btfsc BOTAO4
    goto loop
    LED4_ON
    call DELAY
    LED4_OFF
    call DELAY
    goto loop4
    

   
; parte adicionada (talvez errada)    
    
    

 end  
   


