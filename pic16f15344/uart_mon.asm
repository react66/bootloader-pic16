; uart_mon.asm 
; REA PIC16LF153x4 target 
; ************************** UART ******************************************
 mainv_def um0
 mainv_def um1
 mainv_def um2
 mainv_def um3
 mainv_def um4
 mainv_def um5

; uses um0, FSR0
uart_enq; put w in the tx fifo bank 5
 movwf um0 ; scratch
 movlw 0x02
 movwf FSR0H
 movf tx_wloc,w
 movwf FSR0L
 movf um0,w
 movwi FSR0++
 incf tx_ct,f ; safe to inc here.
 movf FSR0L,w
 movwf tx_wloc
 xorlw 0xF0 ; wrap?
 btfss STATUS,Z
 return; no wrap
 movlw 0xA0 ; wrap in bank 5
 movwf tx_wloc;
 return;

; uses [FSR0]
uart_txnib; hex nibble mask, convert to char and output
 andlw 0x0F
 addlw 0xF6 ; -10
 btfsc STATUS,C ; if carry it was A or greater
 addlw 0x07 ; add difference to make it alpha
 addlw 0x3A ; add 3 to upper and add back in A
 bra uart_enq; done

; uses um1 [FSR0]
uart_b2nib; put 2 nibbles as hex digits
 movwf um1 ; save
 swapf um1,w
 call uart_txnib ; hi part
 movf um1,w
 goto uart_txnib ; low part

; uses [FSR0]
uart_nack_eol; send ? and eol
 bcf uart_status,uerr ; always clear
 movlw 0x3f
 call uart_enq
 bra uart_eol;
uart_ack_eol; send ack and eol
 movlw 0x2A ; asterisk for ack
 call uart_enq
uart_eol; put eol and enable TXIE
 movlw 0x0A ; EOL=lf
 call uart_enq
set_tx_active;
 bsf uart_status,tx_active; flag
 banksel RCSTA1
 bcf RCSTA,CREN ; turn off rx
 bsf TX1STA,TXEN ; turn on tx
 movlb 0
 return  

; uses um2 [FSR0]
uart_pop_rx; pop one char into w
 movlw 0x02
 movwf FSR0H
 movf rx_rloc,w
 movwf FSR0L
 moviw FSR0++ ; post increment 
 decf rx_ct,f;
 movwf um2; scratch save 
 movf FSR0L,w
 movwf rx_rloc ; assume ok then check
 xorlw 0x70 ; wrap?
 btfss STATUS,Z ;
 bra pop_ok; no wrap
 movlw 0x20 ; wrap
 movwf rx_rloc;
pop_ok:
 movf um2,w;
 return

; uses [FSR0]
rx_nib; receive one nibble hex
 call uart_pop_rx
 btfss WREG,6 ; if clear it is numeric
 goto rx_nib_numeric
 addlw 0x09 ; change 1 to A
rx_nib_numeric; just mask and return
 andlw 0x0F
 return; 

; uses um3 [FSR0]
rx_hex; receive 2 hex chars return hex digit in w
 call rx_nib ; get nybble in W
 movwf um3
 swapf um3,f
 call rx_nib
 iorwf um3,w
 return

; uses [FSR0]
uart_purge; abort and send nack
 movf rx_ct,w ; if zero return
 btfsc STATUS,Z
 bra uart_nack_eol
 call uart_pop_rx ; pop char
 bra uart_purge; next

; pop the command byte and get comm2, comm1 discard lf
get_comm2_1;
 call uart_pop_rx ; discard one command byte
 call rx_hex ;
 movwf comm2 ; hi
 call rx_hex ;
 movwf comm1 ; lo
 call uart_pop_rx; discard lf
 return;

gc2_chk; get_comm2_1 and check if valid (< btldr_addr)
 call get_comm2_1 ; hi,lo
 movlw btldr_addr_lo
 subwf comm1,w ; lo-btldr_addr_lo
 movlw btldr_addr_hi
 subwfb comm2,w ; hi-btldr_addr_hi
 ; if btldr_addr > comm2,1 then C=0 which means borrow 
 ; if C=1 the address in comm2,1 is bad
 btfsc STATUS,C
 bsf uart_status,uerr ; user error
 return;

uart_monitor; process the rcv queue
; normal processing
 bcf uart_status,lf_flag; service this command
 movlw 0x02 ; bank 4 0x20-0x6f rcv data
 movwf FSR0H
 movf rx_rloc,w
 movwf FSR0L
; NOW INDF0 points at command char

; uses FSR1 um4 [FSR0]
 movf INDF0,w ; look at char
 xorlw "j" ; jbbmmnn read locations ******************************
 btfss STATUS,Z
 bra uart_monitorK
 movlw 0x08 ; count must=8
 xorwf rx_ct,w
 btfss STATUS,Z
 bra uart_purge ;reject
 call uart_pop_rx ; discard one command byte
 call rx_hex ; load fsr1 with bb mm.
 movwf FSR1H
 call rx_hex ;
 movwf FSR1L
 call rx_hex ; buffer is x50 long, 3 chars per byte here 80/3=26 =1A max 
 movwf um4 ; nn
 call uart_pop_rx ; lf
 movlw 0x1A
 subwf um4,w ; ok if the top bit is 1 we are ok otherwise jam 1A
 btfsc WREG,7
 bra uart_j1 ; is ok
 movlw 0x1A
 movwf um4
uart_j1; fill output buffer
 moviw FSR1++; post inc puts in w
 call uart_b2nib
 movlw 0x20 ; space
 call uart_enq
 decfsz um4,f
 bra uart_j1; another byte
 bra uart_eol; lf and enable tx

uart_monitorK; K command echo whole line when rcvd lf
 movf INDF0,w ; look at char
 xorlw "K" ; K.... reply whole string
 btfss STATUS,Z
 bra uart_monitorQ ; 
 call uart_pop_rx ; get K
 call uart_enq
uk_loop; loop for rx_ct
 movf rx_ct,w
 btfsc STATUS,Z
 bra set_tx_active; exit
 call uart_pop_rx ; get char
 call uart_enq
 bra uk_loop;

uart_monitorQ; another
 movf INDF0,w ; look at char
 xorlw "Q" ; Quit RESET *****************************************
 btfss STATUS,Z
 bra uart_monitorR
 goto appvec; 
; reset ; end of the line... soft reset

uart_monitorR; read a line of memory into bank 2 buffer
 movf INDF0,w
 xorlw "R" ; Rhhll or REA
 btfss STATUS,Z
 bra uart_monitorm
 movlw 0x06 ; count exactly 6
 xorwf rx_ct,w
 btfss STATUS,Z
 bra UMR;
 btfss uart_status,k_unlocked ; check if ok command
 bra uart_purge ; abort
 call gc2_chk; rid of command byte and get 2, check valid
 btfsc uart_status,uerr
 bra uart_nack_eol;
 call pgm_row_read;
 bra uart_ack_eol;
UMR; Check for unlock seq REA
 movlw 0x04 
 xorwf rx_ct,w ; exactly 4
 btfss STATUS,Z
 bra uart_purge ; abort
 call uart_pop_rx; pop R
 call uart_pop_rx; E?
 xorlw 0x45
 btfss STATUS,Z
 bra uart_purge ; no, exit
 call uart_pop_rx; A?
 xorlw 0x41
 btfss STATUS,Z
 bra uart_purge; no, abort
 bsf uart_status,k_unlocked; ok to allow memory operations.
 call uart_pop_rx ;rid lf
 bra uart_ack_eol

; uses um5 FSR1 [FSR0]
uart_monitorm; another ; 
 movf INDF0,w ; look at char
 xorlw "m" ; mnnxx ... m + addr + 16 bytes to write a partial row in RAM
 ; note this writes to bank 2 and address is 20,30,40,50 for 4 sequences
 ; this is data to be written to program store with the u command
 btfss STATUS,Z
 bra uart_monitoru ; reject
 btfss uart_status,k_unlocked ; unlocked?
 bra uart_purge ; no, abort.
 movlw 0x24 ; count must = 36d = 24 hex
 xorwf rx_ct,w
 btfss STATUS,Z
 bra uart_purge ; reject
 call uart_pop_rx ; discard one command byte
 call rx_hex ;
 movwf FSR1L ; offset to start, trust it
 movlw 0x01 ; bank 2
 movwf FSR1H
 movlw 0x10 ; 16
 movwf um5 ; counter
uart_wb2: ; fill bank 2 bytes
 call rx_hex
 movwi FSR1++;
 decfsz um5,f
 bra uart_wb2 ; do 16
 call uart_pop_rx ; lf
 bra uart_ack_eol; signal done

uart_monitoru; program a row of memory
 movf INDF0,w ; look at char
 xorlw "u" ; uhhll .. NVMADRH, NVMADRL
 btfss STATUS,Z
 bra uart_monitorb ; reject
 btfss uart_status,k_unlocked ; unlocked?
 bra uart_purge ; no, abort.
 movlw 0x06 ; count must=6
 xorwf rx_ct,w
 btfss STATUS,Z
 bra uart_purge ; reject
 call gc2_chk; rid of command byte and get 2, check valid
 btfsc uart_status,uerr
 bra uart_nack_eol;
 call pgm_row_write;
 bra uart_ack_eol; signal done.

uart_monitorb; blank a row of memory
 movf INDF0,w ; look at char
 xorlw "b" ; bhhll .. NVMADRH, NVMADRL
 btfss STATUS,Z
 bra uart_monitori ; reject
 btfss uart_status,k_unlocked ; unlocked?
 bra uart_purge ; no, abort.
 movlw 0x06 ; count must=6
 xorwf rx_ct,w
 btfss STATUS,Z
 bra uart_purge ; reject
 call gc2_chk; rid of command byte and get 2, check valid
 btfsc uart_status,uerr
 bra uart_nack_eol;
 call pgm_row_erase;
 bra uart_ack_eol; signal done.

uart_monitori; i command
 movf INDF0,w ; look at char
 xorlw "i" ; 
 btfss STATUS,Z
 bra uart_purge ; reject
 movlw 0x02 ; count must=2
 xorwf rx_ct,w
 btfss STATUS,Z
 bra uart_purge ; reject
 call uart_pop_rx ; pop i
 call uart_pop_rx ; pop lf
 movlw devcd
 call uart_b2nib ; send it as 2 bytes
 movlw base_344_rev
 call uart_b2nib ; send it as 2 bytes
 bra uart_eol ; lf and enable tx

; ************************** END UART ******************************
