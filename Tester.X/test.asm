				list P=PIC12F752, r=dec
	#include "p12f752.inc"
	__config (_CP_OFF & _WDTE_OFF & _PWRTE_ON & _FOSC0_INT & _MCLRE_OFF)
	errorlevel -302


;Hardware:
;RA2: Motor PWM
MOTOR_PWM_TRISA equ TRISA2
MOTOR_PWM_LATA equ LATA2
;RA3 : input, RX
RX_PORTA equ RA3
RX_TRISA equ TRISA3
RX_IOCAN equ IOCAN3
RX_IOCAF equ IOCAF3
;HLTMR1 : baud rate bit timer, RX
;TMR2 : baud rate bit timer, TX
 
 
;Registers (Common RAM:
;byte:W_TEMP (holding register for W during interrupt)
W_TEMP equ 0x70
;byte:STATUS_TEMP (holding register for Status during interrupt)
STATUS_TEMP equ 0x71
;byte:UART_RecvShiftCounter (count the number of shift operations for the RX operation)
UART_RecvShiftCounter equ 0x72
;byte:UART_RecvShiftRegister (holding register for the shifted input)
UART_RecvShiftRegister equ 0x73
;byte:UART_RecvDataRegister (holding register for completely received data)
UART_RecvDataRegister equ 0x74
;bit: UART_RecvStarting (track whether or not we are beginning a receive operation)
UART_RecvStarting equ 0x75
;bit: UART_RecvFramingErr (track any framing errors during receive operations)
UART_RecvFramingErr equ 0x76
;bit: UART_RecvDataReady (flag for a completed receive operation)
UART_RecvDataReady equ 0x77
;byte: RX_HoldingReg (holding register for recently received data)
RX_HoldingReg equ 0x7D


;Constants
;bit: LSB (least significant bit of a byte)
LSB equ 0
;bit: MSB (most significant bit of a byte)
MSB equ 7
;byte: FULL_BIT_TIME (one bit time)
FULL_BIT_TIME equ 11
;byte: HALF_BIT_TIME (half of one bit time
HALF_BIT_TIME equ FULL_BIT_TIME/2
;bit: HLTCKPS1 (upper bit of the HLTMR1 prescaler)
HLTCKPS1 equ 0
;byte: UART_RecvMaxShiftCount (maximum number of shifts on receive operations)
UART_RecvMaxShiftCount equ 9
;TIMER_2_PRESCALE: prescale selector for Timer 2
TIMER_2_PRESCALE equ 0x00
;PWM_PERIOD: period for pwm operations (period = (PWM_PERIOD + 1)*4*TOSC*TIMER_2_PRESCALE)
PWM_PERIOD equ 24
;UPPER_HI_TIME: upper 8 bits of the 10 bit hi time
UPPER_HI_TIME equ 20
;LOWER_HI_TIME: lower 2 bits of the 10 bit lo time
LOWER_HI_TIME equ 0
;PWM_MODE: Bits for selecting PWM mode
PWM_MODE equ B'00001100'
;T2CON_T2CKPS_M: byte for masking off all but the T2CKPS
T2CON_T2CKPS_M equ B'11111100'
 
OSCCON_M = B'11001111'
OSC_FREQ = B'00110000'
 

	ORG 0
	GOTO SETUP
	ORG 4
	GOTO ISR

	ORG 5
SETUP
	BANKSEL OSCCON
	MOVLW OSCCON_M
	ANDWF OSCCON, W
	IORLW OSC_FREQ
	MOVWF OSCCON
	
;	select porta bank
	BANKSEL PORTA
;	clear porta
	CLRF PORTA
;	select lata bank
	BANKSEL LATA
;	clear lata
	CLRF LATA
;	select ansel bank
	BANKSEL ANSELA
;	clear ansel
	CLRF ANSELA
; //Configure RX port line as input, TX port line as output//
;	select trisa bank
	BANKSEL TRISA
;	set RX as input
	BSF TRISA, RX_TRISA
;	set motor PWM as output
	BCF TRISA, MOTOR_PWM_TRISA
;	select PR2 bank
	BANKSEL PR2
;	load period time into PR2
	MOVLW PWM_PERIOD
	MOVWF PR2
;	select T2CON bank
	BANKSEL T2CON
;	Adjust prescaler
	MOVLW T2CON_T2CKPS_M
	ANDWF T2CON, W
	IORLW TIMER_2_PRESCALE
	MOVWF T2CON
;	select CCPR1L bank
	BANKSEL CCPR1L
;	load upper byte of hi time
	MOVLW UPPER_HI_TIME
	MOVWF CCPR1L
;	select CCP1CON
	BANKSEL CCP1CON
;	load in lower bits of hi time and PWM mode
	MOVLW LOWER_HI_TIME
	IORLW PWM_MODE
	MOVWF CCP1CON
; 	clear UART_RecvFramingErr
	CLRF UART_RecvFramingErr
; 	clear UART_RecvDataReady
	CLRF UART_RecvDataReady
;	clear UART_RecvShiftRegister
	CLRF UART_RecvShiftRegister
;	clear UART_RecvDataRegister
	CLRF UART_RecvDataRegister
; 	program IOC Negative on RX, clear any pending int and enable interrupt
;	select iocaf bank
	BANKSEL IOCAF
;	clear iocaf
	CLRF IOCAF
;	select iocan bank
	BANKSEL IOCAN
;	enable falling edge interrupts on Rx
	BSF IOCAN, RX_IOCAN
;	select pir1 bank
	BANKSEL PIR1
;	clear hltmr1if
	BCF PIR1, HLTMR1IF
;	clear the Timer 2 flag
	BCF PIR1, TMR2IF
;	select pie1 bank
	BANKSEL PIE1
;	enable interrupts on hltmr1
	BSF PIE1, HLTMR1IE	
;	select intcon bank
	BANKSEL INTCON
;	enable interrupt on change interrupts
	BSF INTCON, IOCIE
;	enable peripheral interrupts
	BSF INTCON, PEIE
;	enable interrupts globally
	BSF INTCON, GIE
;	go to main
	GOTO MAIN
	
	
MAIN
;	if the receive flag is high
	BTFSC UART_RecvDataReady, LSB
;		run the receive routine
	CALL RX_COMMON_ROUTINE
;	loop
	GOTO MAIN

	
ISR
; //push//
; 	copy w to a temporary register
	MOVWF W_TEMP
; 	swap status into W
	SWAPF STATUS, W
; 	copy status into a temporary register
	MOVWF STATUS_TEMP


; //if IOCIF set
;	select INTCON bank
	BANKSEL INTCON
;	if IOCIF is set
	BTFSC INTCON, IOCIF
	CALL IOC_ISR


; //if HLTMR1 interrupt is active
;	select PIR1 bank
	BANKSEL PIR1
;	if the HLTMR1IF bit is set
	BTFSC PIR1, HLTMR1IF
;	go to RX ISR
	CALL RX_ISR
	

; //pop//
;	Swap the temprary status register into W
	SWAPF STATUS_TEMP, W
;	copy W to status
	MOVWF STATUS
;	Swap the temporary W register back to itself
	SWAPF W_TEMP, F
;	Swap the temporary W register to W
	SWAPF W_TEMP, W
;	return from interrupt
	RETFIE
 

IOC_ISR
; //if RX interrupt is active//
;	select IOCAF bank
	BANKSEL IOCAF
;	if RX interrupt is set
	BTFSC IOCAF, RX_IOCAF
;		go to RX IOC ISR
	CALL RX_IOC_ISR
	RETURN


RX_IOC_ISR
; //program HLTRM1 to fire interrupt after 1/2 bit time and enable
;	select hltmr1 bank
	BANKSEL HLTMR1
;	clear hltmr1
	CLRF HLTMR1
;	select hltpr1 bank
	BANKSEL HLTPR1
;	write HALF_BIT_TIME
	MOVLW HALF_BIT_TIME
	MOVWF HLTPR1
;	select hlt1con0 bank
	BANKSEL HLT1CON0
;	write hlt1con0 prescaler to h1ckps <1:0>; choose prescaler of 16 (1x)
	BSF HLT1CON0, H1CKPS1
;	set hlt1con0 h1on to turn on timer
	BSF HLT1CON0, H1ON
;	set UART_RecvStarting to 1
	BSF UART_RecvStarting, LSB
;	set UART_RecvShiftCounter to maximum number of shifts
	MOVLW UART_RecvMaxShiftCount
	MOVWF UART_RecvShiftCounter
; //clear IOCAF bit associated with RX//
;	select IOCAF bank
	BANKSEL IOCAF
;	clear Rx flag
	BCF IOCAF, RX_IOCAF
;	clear other interrupt flag
	BCF INTCON, IOCIF
; //disable IOC on RX//
;	select IOCAN bank
	BANKSEL IOCAN
;	clear RX IOCAN
	BCF IOCAN, RX_IOCAN
	
;;	initialize blinker
;	CALL INITIATE_BLINKER
	
;	Return
	RETURN


RX_ISR
; //if UART_RecvStarting set (doing start bit)//
	BTFSS UART_RecvStarting, LSB
	GOTO RX_DATA
;	  //clear UART_RecvStarting//
	CLRF UART_RecvStarting
;		if RX is low (we got good start bit)
	BANKSEL PORTA
	BTFSC PORTA, RX_PORTA
	GOTO RX_BAD_START
;		  //program HLTRM1 to fire interrupt after 1 bit time//
;			select HLTMR1 bank
	BANKSEL HLTMR1
;;			clear HLTMR1
;	CLRF HLTMR1
;			select HLTPR1 bank
	BANKSEL HLTPR1
;			write FULL_BIT_TIME
	MOVLW FULL_BIT_TIME
	MOVWF HLTPR1
;			go to end of if
	GOTO RX_ISR_END
RX_BAD_START
;	  //else ( RX is high, so bad start bit)//
;		  //program IOC Negative on RX and enable interrupt//
;			select HLT1CON0 bank
	BANKSEL HLT1CON0
;			clear HLT1CON0 H1ON <3>
	BCF HLT1CON0, H1ON
;			select IOCAN bank
	BANKSEL IOCAN
;			enable RX IOCAN
	BSF IOCAN, RX_IOCAN
;		 	go to end of if
	GOTO RX_ISR_END
RX_DATA
; //else (Doing data bit)//
;		decrement UART_RecvShiftCounter
;		if UART_RecvShiftCounter not 0
	DECFSZ UART_RecvShiftCounter
;			go to RX shift in
	GOTO RX_SHIFT_IN
;		 else
;			copy UART_RecvShiftRegister to UART_RecvDataRegister
	MOVF UART_RecvShiftRegister, W
	MOVWF UART_RecvDataRegister
;			if RX is low (bad stop bit)
	BANKSEL PORTA
	BTFSS PORTA, RX_PORTA
;				set UART_RecvFramingErr
	BSF UART_RecvFramingErr, LSB
;			set UART_RecvDataReady to 1
	BSF UART_RecvDataReady, LSB
;		  //disable HLTMR1 interrupt (just turn the timer off?)//
;			select HLT1CON0 bank
	BANKSEL HLT1CON0
;			clear HLT1CON0 H1ON <3>
	BCF HLT1CON0, H1ON
;		  //program IOC Negative on RX and enable interrupt//
;			select IOCAN bank
	BANKSEL IOCAN
;			enable RX IOCAN
	BSF IOCAN, RX_IOCAN
;		 	go to end of if	
	GOTO RX_ISR_END
RX_SHIFT_IN
;			 shift UART_RecvShiftRegister 1 position right
	RRF UART_RecvShiftRegister
;			  //Copy state of RX to MSB of UART_RecvShiftRegister//
;				select PORTA bank
	BANKSEL PORTA
;				if RX is set
	BTFSC PORTA, RX_PORTA
;				set the MSB
	BSF UART_RecvShiftRegister, MSB
;				else is RX is clear
	BTFSS PORTA, RX_PORTA
;				clear the MSB
	BCF UART_RecvShiftRegister, MSB
	
	
RX_ISR_END
; //clear HLTMR1IF//
;	select PIR1 bank
	BANKSEL PIR1
;	clear HLTMR1IF
	BCF PIR1, HLTMR1IF
; 	Return
	RETURN 


RX_COMMON_ROUTINE
;	clear the receive flag
	BCF UART_RecvDataReady, LSB
;	move the receive data into W
	MOVF UART_RecvDataRegister, W
;	copy W into receive routine holding register
	MOVWF RX_HoldingReg
	BANKSEL CCPR1L
;	load upper byte of hi time
	MOVWF CCPR1L
	BANKSEL T2CON
;	turn on Timer 2
	BSF T2CON, TMR2ON
;	return
	RETURN
	
	
 	END

