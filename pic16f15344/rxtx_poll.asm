; rxtx_poll.asm 
; REA 090720
; receive and transmit by polling for bootloader
;
rxtx_poll;
 ; if uart_status,tx_active is set we send data
 btfss uart_status,tx_active ; was set by uart_mon.
 bra rx_cont ; check receive
 BANKSEL PIR3
 btfss PIR3,TX1IF ;if set the tx buffer is empty
 bra rxtx_ret; set bank 0 and return
 movlb 0 ; normal
 decf tx_ct,f ; sending one
 btfsc STATUS,Z ; if zero reset tx_active
 bcf uart_status,tx_active;
 movf tx_rloc,w
 movwf FSR1L
 movlw 0x02 ; bank 5
 movwf FSR1H
 moviw FSR1++
 banksel TX1REG
 movwf TX1REG ; sent
 movlb 0
 movf FSR1L,w ; save and check
 movwf tx_rloc
 xorlw 0xF0 ; wrap?
 btfss STATUS,Z
 bra tx_nowrap ; not yet
 movlw 0xA0 ; yes wrap
 movwf tx_rloc;
tx_nowrap;
; wait on tx then turn on RX
 btfsc uart_status,tx_active ;=0 if last char
 bra rxtx_ret
 banksel TX1STA ; tx, rx regs for uart1
rxtx_wait 
 btfss TX1STA,TRMT ; 1 when done
 bra rxtx_wait
; switch over to receiver
 bcf TX1STA,TXEN ; turn off transmitter
 bsf RCSTA1,CREN ; enable receive
rxtx_ret;
 movlb 0
 return;

 ; rx must clear possible OERR because it will stop reception
rx_cont;
 BANKSEL PIR3 ; all the PIR regs are here.
 btfss PIR3,RC1IF ; receive data?
 bra rxtx_ret ; no receive flag so return
 BANKSEL RC1STA ; all the uart 1 regs here
 movf RC1REG,w ; get the data
 movlb 0 
 movwf rx_byte ; save for now
 movf rx_wloc,w
 movwf FSR1L
 movlw 0x02 ; bank 4
 movwf FSR1H
 movf rx_byte,w
 movwi FSR1++ ; post inc
 ; set flag if lf
 xorlw 0x0A ; lf?
 btfsc STATUS,Z
 bsf uart_status,lf_flag; yes, set flag
 incf rx_ct,f ; counter++
 movf FSR1L,w ; save wloc then check wrap
 movwf rx_wloc
 xorlw 0x70 ; wrap?
 btfss STATUS,Z
 bra rxtx_ret ; nowrap, done for now
 movlw 0x20 ; wrap
 movwf rx_wloc; 
 BANKSEL RC1STA ; quick check for overflow and exit
 btfss RC1STA,OERR
 bra rxtx_ret;
; overflow so clear it
clr_ovf;
 bcf RCSTA1,CREN ; stop continuous receive and clear fifo
 movf RCREG1,w 
 movf RCREG1,w 
 movf RCREG1,w 
 bsf RCSTA1,CREN ; restart continuous receive
 bra rxtx_ret; 


