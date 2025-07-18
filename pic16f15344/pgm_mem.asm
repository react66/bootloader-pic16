; pgm_mem.asm
; REA June 2019
; pic16f153x4
;*****************************************************

; writeable pgm mem starts at 800 hex ends fff
; ie rows 40-7f hex, 7 bits address the row
; NVMADRH bits 3..0 and NVMADRL bits 7..5
; for this, the low bits of NVMADRL don't matter but leave 0
; data write access is 32 14 bit words at a time
; data read is by fsr and is just bytes in the page.
; 

pgm_row_erase; row address low in comm1, hi in comm2 common area
  banksel NVMADRL
  movf comm1,w
  movwf NVMADRL
  movf comm2,w
  movwf NVMADRH
  bcf NVMCON1,NVMREGS ; pfm area
  bsf NVMCON1,FREE ; erase
  bsf NVMCON1,WREN ; enable writes
  call pgm_unlk_seq;
  bcf NVMCON1,WREN
  movlb 0 ; always
  return;

  ; unlock seq
pgm_unlk_seq; for any write ops
  banksel NVMADRL
  movlw 0x55
  movwf NVMCON2
  movlw 0xaa
  movwf NVMCON2
  bsf NVMCON1,WR ; set wr bit
  ; routine hangs here till done, also FREE bit set to 0 
  return;

pgm_row_write; Write a row, low in comm1, hi in comm2, data in bank 2
  movlw 0x01 ; set up FSR0 pointing to buffer
  movwf FSR0H ; bank 2
  ; check if this is address 0 and jam 318e,2e20
  ; for 345 jam 319e,2e20
  ; make sure we never clobber the PIC btldr
  movf comm1,w
  iorwf comm2,w
  btfss STATUS,Z
  bra pgm_row_cont
  movlw 0x20
  movwf FSR0L ; start of data 64 bytes
  movlw 0x31 ; overwrite first 4 bytes in bank 2 data:
  movwi FSR0++
  #ifdef __16F15344
  movlw 0x8e
  #else
  movlw 0x9e
  #endif
  movwi FSR0++
  movlw 0x2e
  movwi FSR0++
  movlw 0x20
  movwi FSR0++
pgm_row_cont;
  movlw 0x20 ; point FSR0L to start of data
  movwf FSR0L
  banksel NVMADRL
  movf comm1,w
  movwf NVMADRL
  movf comm2,w
  movwf NVMADRH
  bcf NVMCON1,NVMREGS ; pfm area
  bsf NVMCON1,WREN ; enable writes
  bsf NVMCON1,LWLO ; load only write latches
pgm_w_loop; load 64 bytes, we let the second byte be the low one
  moviw FSR0++
  movwf NVMDATH ; first data byte
  moviw FSR0++
  movwf NVMDATL ; second
  movf NVMADRL,w ; check
  xorlw 0x1F
  andlw 0x1F
  btfsc STATUS,Z ; last of 32 exit
  bra pgm_do_write ; exit when last latch ready to load
  call pgm_unlk_seq ; load a latch
  incf NVMADRL,f ; next location
  bra pgm_w_loop
pgm_do_write;
  bcf NVMCON1,LWLO ; finished latches
  call pgm_unlk_seq ; do the write
  bcf NVMCON1,WREN ; done
  movlb 0
  return;

pgm_row_read; read a row into bank 2
  banksel NVMADRL
  movf comm1,w
  movwf NVMADRL
  movf comm2,w
  movwf NVMADRH
  movlw 0x01 ; 
  movwf FSR0H ; bank 2
  movlw 0x20 ; start of data 64 bytes
  movwf FSR0L
  bcf NVMCON1,NVMREGS
pgm_rd_loop;
  BSF NVMCON1,RD
  movf NVMDATH,w ; do hi, lo
  movwi FSR0++
  movf NVMDATL,w
  movwi FSR0++
  incf NVMADRL,f
  movf NVMADRL,w
  andlw 0x1f ; these bits 0 when done
  btfss STATUS,Z ; exit
  bra pgm_rd_loop
  movlb 0
  return; 

  
