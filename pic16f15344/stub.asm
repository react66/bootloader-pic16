; FILE stub.asm
; REA 9/3/2020
; 052021 HFINT32 no PLL
;
 page
;
; 
; turn off the banksel warnings/messages
 ERRORLEVEL -302
 ERRORLEVEL -306
 
; This encoder was using HFINTPLL, now HFINT32 (may 20 '21)
 __CONFIG _CONFIG1, _RSTOSC_HFINT32 & _CLKOUTEN_OFF & _CSWEN_OFF & _FCMEN_OFF & _FEXTOSC_OFF
 __CONFIG _CONFIG2, _MCLRE_ON & _PWRTE_OFF & _LPBOREN_ON & _BOREN_ON & _BORV_LO & _ZCD_OFF & _PPS1WAY_OFF & _STVREN_ON 
 __CONFIG _CONFIG3, _WDTE_OFF 
 __CONFIG _CONFIG4, _BBEN_OFF & _SAFEN_OFF & _WRTAPP_OFF & _WRTB_OFF & _WRTC_ON & _WRTSAF_OFF & _LVP_ON
 __CONFIG _CONFIG5, _CP_OFF
 #ifdef __16F15344
 #include "p16f15344.inc"
 #else
  #ifdef __16F15345
  #include "p16f15345.inc"
  #else
  #error "[stub.asm] BAD PROCESSOR NAME"
  #endif
 #endif
   
 #include "macros_etc.inc"
; these are only in the bootloader:
 mainv_def ugt0
 mainv_def ugt1
 mainv_def ugt2
 mainv_def ugt3

; constants in macros_etc
; top of memory is 0xfff = 4k-1 or 0x1fff (for pic16f15345)
; btldr_addr is at 0xE20 or 0x1E20
; appvec_loc is expected 0xE20-2 = 0xE1E or 0x1E1E (345)
;******** The host interface must do the following:
; do not write over the four bytes at 0x00 these
; are copied into the 1st block before programming it.
; save the 4 bytes at loc 0 in the user image these
; are copied to appvec_loc when that block is
; programmed. If there is no hex record for that
; block, a block of 7f's is used with the 4 bytes
; that have been saved.


; ********* included in bootloader: ************
 #include "boot15344.asm";
 #include "rxtx_poll.asm";
 #include "pgm_mem.asm";
 #include "uart_mon.asm";

 org appvec_loc ; appvec position is set in macros_etc at btldr-4
appvec;
 pagesel btldr
 goto btldr ; entry point will be overwritten

 END ; stub_end
