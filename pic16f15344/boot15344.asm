; boot15344.asm bootloader for 15344 or 15345
;   1) protect word locs E20 to end of mem.
;
; pins rb5=rx1 = anb5, rb7=tx1 = anb7
; disable an for these 2 pins
;
 org 0 ;
 pagesel btldr
 goto btldr; bootloader should not overwrite this.

 org btldr_addr ; last 512 bytes approx.
btldr; always start here, no interrupts in btldr.
; ********************* debug code leds ds1-ds4 are c0-c3
; banksel ANSELC
; movlw 0xf0 ; low 4 bits
; movwf ANSELC
; banksel PORTC
; movwf TRISC
; movwf LATC
; *********************** end debug code
; tris 1 is input 0 is output
 #ifdef bt_a4
 banksel ANSELA
 bcf ANSELA,ANSA4 ; rx
 banksel PORTA
 BSF TRISA,TRISA4
 banksel WPUA
 BSF WPUA,WPUA4
 movlb 0
 btfsc PORTA,RA4
 goto appvec
brkwaita btfss PORTA,RA4 ; continue when hi
 bra brkwaita
 #else
 banksel ANSELB
 bcf ANSELB,ANSB5 ;rx
 banksel PORTB
 bsf TRISB,TRISB5 ; in
 banksel WPUB
 bsf WPUB,WPUB5 ; weak pullup on receive
 movlb 0
 btfsc PORTB,RB5
 goto appvec ; it was hi so go run the code
brkwait btfss PORTB,RB5 ; continue when hi
 bra brkwait
 #endif
; ok enter bootloader:
; init serial buffer
 clrf uart_status; used here in polling mode defines in macros and stub
; buffers in b1 per macros_etc. Initialize here for ttlser
 movlw 0x20
 movwf rx_wloc ; rx fifo (bank4 range 0x20-0x6f)
 movwf rx_rloc
 clrf rx_ct
 movlw 0xA0
 movwf tx_wloc ; tx fifo (bank5 range 0xA0-0xEF)
 movwf tx_rloc
 clrf tx_ct 
; init uart
 #ifdef bt_a4
 ; set uart1 input and output to RA4
 banksel RX1DTPPS
 movlw 0x04 ; rx1 from RA4
 movwf RX1DTPPS
 banksel RA4PPS
 movlw 0x0f
 movwf RA4PPS ; tx1 to RA4
 #else
 ; normal RB5
 banksel RB5PPS
 movlw 0x0f
 movwf RB5PPS ; tx1 to RB5 same as rx 
 #endif
 banksel RC1REG ;
 bsf TX1STA,BRGH ; TXEN is not on
 clrf SP1BRGH
; movlw 0x44 ; assuming 32 Mhz this is for 115200 bd
 ;n=  (32000000/115200)/4 -1 = 68 = 0x44
; movlw 0x03 ; for 2 Mbit
 movlw bd_set ; in assembly
 movwf SPBRG1
 movlw 0x08
 movwf BAUDCTL1
 movlw 0x90 ; spen,cren
 movwf RCSTA1
 movlb 0 ; bank 0 is normal
bootloop; only need to service serial port and run commands:
 call rxtx_poll; will loop until lf_flag is set or data is sent.
 btfsc uart_status,lf_flag ; uart_monitor expects lf_flag.
 call uart_monitor;
 bra bootloop; 

; end_boot15344;
