; Projeto 1 - Exemplo de programa em assembly
; Prof. Alessandro
; Descri��o: Sequ�ncia de LEDs controlada por bot�o

; ------------------- Configura��o dos FUSE BITS ------------------
PROCESSOR 16F877
#include <xc.inc> 

; CONFIG
CONFIG  FOSC = HS             ; Oscillator Selection bits (HS oscillator)
CONFIG  WDTE = OFF            ; Watchdog Timer Enable bit (WDT disabled)
CONFIG  PWRTE = OFF           ; Power-up Timer Enable bit (PWRT disabled)
CONFIG  CP = OFF              ; FLASH Program Memory Code Protection bits (Code protection off)
CONFIG  BOREN = OFF           ; Brown-out Reset Enable bit (BOR disabled)
CONFIG  LVP = OFF             ; Low Voltage In-Circuit Serial Programming Enable bit (RB3 is digital I/O)
CONFIG  CPD = OFF             ; Data EE Memory Code Protection (Code Protection off)
CONFIG  WRT = OFF             ; FLASH Program Memory Write Enable

; ------------------- Pagina��o da mem�ria (de dados) ----------------
bank0 MACRO 
    BCF STATUS, 5
    BCF STATUS, 6
    endm

bank1 MACRO  
    BSF STATUS, 5
    BCF STATUS, 6
    endm

; ------------------- Cria��o de vari�veis -------------------------
PSECT udata
    TEMP1: DS 1
    TEMP2: DS 1
    SEQUENCIA: DS 1        ; Vari�vel para controlar qual sequ�ncia ser� executada

; ------------------- Defini��o de entradas ------------------------
#define BOTAO  PORTB, 0

; ------------------- Defini��o de sa�das --------------------------
#define LED_ON_0  bsf PORTD, 0
#define LED_OFF_0 bcf PORTD, 0

#define LED_ON_1  bsf PORTD, 1
#define LED_OFF_1 bcf PORTD, 1

#define LED_ON_2  bsf PORTD, 2
#define LED_OFF_2 bcf PORTD, 2

#define LED_ON_3  bsf PORTD, 3
#define LED_OFF_3 bcf PORTD, 3

#define LED_ON_4  bsf PORTD, 4
#define LED_OFF_4 bcf PORTD, 4

#define LED_ON_5  bsf PORTD, 5
#define LED_OFF_5 bcf PORTD, 5

#define LED_ON_6  bsf PORTD, 6
#define LED_OFF_6 bcf PORTD, 6

#define LED_ON_7  bsf PORTD, 7
#define LED_OFF_7 bcf PORTD, 7

#define LED_OFF_TODOS  clrf PORTD

; ------------------- Vetor de reset -----------------
PSECT code, abs
ORG 0x00
goto INICIO

; ------------------- Vetor de interrup��es -----------------
PSECT code, abs
ORG 0x04
retfie

; ------------------- Subrotinas -----------------
DELAY:
    movlw   11111111B
    movwf   TEMP2
label:
    movlw   255
    movwf   TEMP1
    decfsz  TEMP1, F
    goto    $-1
    decfsz  TEMP2, F
    goto    label
    return

; ------------------- In�cio do programa -------------
INICIO:
    clrf PORTA
    clrf PORTB
    clrf PORTC
    clrf PORTD
    clrf PORTE
    clrf SEQUENCIA        ; Inicializa a vari�vel de sequ�ncia

; ------------------- Configura��es do microcontrolador -------------
    bank1
    movlw 0xFF
    movwf TRISA
    movlw 0xFF
    movwf TRISB
    movlw 0xFF
    movwf TRISC
    movlw 0x00
    movwf TRISD
    movlw 0xFF
    movwf TRISE
    bank0

; ------------------- Programa principal -------------
Loop:
    ; Verifica se o bot�o foi pressionado
BotaoApertado:
    btfsc BOTAO
    goto BotaoApertado     ; Aguarda at� o bot�o ser solto
    btfss SEQUENCIA, 0
    goto Sequencia1
    goto Sequencia2

Sequencia1:
    ; Sequ�ncia 1: LEDs pares e �mpares alternando
    LED_ON_0
    LED_ON_2
    LED_ON_4
    LED_ON_6
    LED_OFF_1
    LED_OFF_3
    LED_OFF_5
    LED_OFF_7
    call DELAY

    LED_ON_1
    LED_ON_3
    LED_ON_5
    LED_ON_7
    LED_OFF_0
    LED_OFF_2
    LED_OFF_4
    LED_OFF_6
    call DELAY

    LED_OFF_TODOS
    call DELAY

    ; Acende LEDs em ordem crescente at� todos acenderem
    LED_ON_0
    call DELAY
    LED_ON_1
    call DELAY
    LED_ON_2
    call DELAY
    LED_ON_3
    call DELAY
    LED_ON_4
    call DELAY
    LED_ON_5
    call DELAY
    LED_ON_6
    call DELAY
    LED_ON_7
    call DELAY

    LED_OFF_TODOS
    call DELAY

    ; Acende LEDs em ordem crescente novamente
    LED_ON_0
    call DELAY
    LED_ON_1
    call DELAY
    LED_ON_2
    call DELAY
    LED_ON_3
    call DELAY
    LED_ON_4
    call DELAY
    LED_ON_5
    call DELAY
    LED_ON_6
    call DELAY
    LED_ON_7
    call DELAY

    LED_OFF_TODOS
    call DELAY

    bsf SEQUENCIA, 0      ; Marca que a pr�xima � a segunda sequ�ncia
    goto Loop

Sequencia2:
    ; Sequ�ncia 2: LEDs D4 a D7, depois D0 a D3
    LED_ON_4
    LED_ON_5
    LED_ON_6
    LED_ON_7
    LED_OFF_0
    LED_OFF_1
    LED_OFF_2
    LED_OFF_3
    call DELAY

    LED_ON_0
    LED_ON_1
    LED_ON_2
    LED_ON_3
    LED_OFF_4
    LED_OFF_5
    LED_OFF_6
    LED_OFF_7
    call DELAY

    LED_OFF_TODOS
    call DELAY

    ; Acende LEDs em sequ�ncia at� D7
    LED_ON_0
    call DELAY
    LED_OFF_0
    LED_ON_1
    call DELAY
    LED_OFF_1
    LED_ON_2
    call DELAY
    LED_OFF_2
    LED_ON_3
    call DELAY
    LED_OFF_3
    LED_ON_4
    call DELAY
    LED_OFF_4
    LED_ON_5
    call DELAY
    LED_OFF_5
    LED_ON_6
    call DELAY
    LED_OFF_6
    LED_ON_7
    call DELAY
    LED_OFF_7

    ; Acende LEDs em ordem decrescente
    LED_ON_6
    call DELAY
    LED_OFF_6
    LED_ON_5
    call DELAY
    LED_OFF_5
    LED_ON_4
    call DELAY
    LED_OFF_4
    LED_ON_3
    call DELAY
    LED_OFF_3
    LED_ON_2
    call DELAY
    LED_OFF_2
    LED_ON_1
    call DELAY
    LED_OFF_1
    LED_ON_0
    call DELAY
    LED_OFF_0

    LED_OFF_TODOS
    bcf SEQUENCIA, 0      ; Marca que a pr�xima � a primeira sequ�ncia
    goto Loop

end
