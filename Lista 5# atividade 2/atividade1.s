; Projeto 1 - Exemplo de programa em assembly
; Prof. Alessandro
; Descri��o: Contador bin�rio controlado por bot�o - Exibe o valor nos LEDs do PORTD

; ------------------- Configura��o dos FUSE BITS ------------------
PROCESSOR 16F877
#include <xc.inc>

; Configura��o do hardware do microcontrolador
; CONFIG
CONFIG FOSC = HS             ; Oscillator Selection bits (HS oscillator)
CONFIG WDTE = OFF            ; Watchdog Timer Enable bit (WDT disabled)
CONFIG PWRTE = OFF           ; Power-up Timer Enable bit (PWRT disabled)
CONFIG CP = OFF              ; FLASH Program Memory Code Protection bits (Code protection off)
CONFIG BOREN = OFF           ; Brown-out Reset Enable bit (BOR disabled)
CONFIG LVP = OFF             ; Low Voltage In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
CONFIG CPD = OFF             ; Data EE Memory Code Protection (Code Protection off)
CONFIG WRT = OFF             ; FLASH Program Memory Write Enable (Unprotected program memory may not be written to by EECON control)

; ------------------- Pagina��o da mem�ria (de dados) ----------------
bank0 MACRO
    BCF STATUS, 5
    BCF STATUS, 6
    endm

bank1 MACRO
    BSF STATUS, 5
    BCF STATUS, 6
    endm

bank2 MACRO
    BCF STATUS, 5
    BSF STATUS, 6
    endm

; ------------------- Cria��o de vari�veis -------------------------
PSECT udata
    TEMP1: DS 1
    TEMP2: DS 1
    contador: DS 1
    botaoEstado: DS 1
    botaoUltimoEstado: DS 1

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
    movlw 11111111B
    movwf TEMP2
label:
    movlw 255
    movwf TEMP1
    decfsz TEMP1
    goto $-1
    decfsz TEMP2
    goto label
    return

DelayDebounce:
    movlw 0xFF              ; Atraso maior para debounce (ajustado para ser mais robusto)
    movwf TEMP1
DebounceLoop:
    decfsz TEMP1, F
    goto DebounceLoop
    return

; ------------------- In�cio do programa -------------
INICIO:
    clrf PORTA              ; Limpa os registradores de portas
    clrf PORTB
    clrf PORTC
    clrf PORTD
    clrf PORTE

    bank1                   ; Configura os pinos como entradas ou sa�das
    movlw 0xFF
    movwf TRISA             ; Configura PORTA como entrada
    movlw 0x01              ; Configura RB0 como entrada, os demais como sa�da
    movwf TRISB
    movlw 0x00              ; Configura todos os pinos de PORTD como sa�das
    movwf TRISD
    bank0

    clrf contador           ; Inicializa o contador com 0
    clrf botaoUltimoEstado  ; Inicializa o estado anterior do bot�o com 0

Loop:
    ; Verifica se o bot�o est� pressionado (n�vel l�gico 0)
    btfss PORTB, 0
    goto BotaoPressionado
    ; Caso contr�rio, o bot�o est� solto (n�vel l�gico 1)
    goto BotaoSolto

BotaoPressionado:
    movlw 1
    movwf botaoEstado       ; Marca que o bot�o foi pressionado
    goto Loop

BotaoSolto:
    movf botaoEstado, W
    btfsc STATUS, 2         ; Se botaoEstado for 0, o bot�o n�o foi pressionado antes
    goto Loop

    ; Se o bot�o foi pressionado anteriormente e agora est� solto, incrementa
    call DelayDebounce      ; Chama a sub-rotina de debounce
    incf contador, F        ; Incrementa o contador
    movf contador, W        ; Move o valor do contador para W
    movwf PORTD             ; Exibe o contador nos LEDs conectados ao PORTD
    clrf botaoEstado        ; Limpa o estado do bot�o
    goto Loop               ; Volta ao loop principal

end
