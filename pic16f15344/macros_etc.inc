; REA Sept 2020 bootloader and base code for PIC16f15344 
; REA added PIC16F15345, updates Jan 2023
; define start locations for shared or scratch variables
; or defines for includes
; NOTE: FSR0, FSR1 are saved automatically in interrupts
;       so main can use them, they are not dedicated.
; save BSR, WREG, 
; bank 2 64 bytes for pgm_mem it is 32 words r/w
; banks 0-5 have 80 bytes ea starting at offset x20
; bank 6 has 16 bytes starting at offset x20
;    Allocations:
; bank 4 rx, bank 5 tx

base_344_devcode equ devcd
base_344_rev equ sw_rev
; flash is 4096 words 2 pages '344, 8192 4 pages '345.
 #ifdef __16F15344
 #define end_flash .4096 ; 0x1000
 #else
  #ifdef __16F15345
  #define end_flash .8192 ; 0x2000
  #else
   #error "[macros_etc.inc] BAD PROCESSOR NAME"
  #endif
 #endif

 #define btldr_size .512 ; 
 #define btldr_addr (end_flash - btldr_size + .32)
 #define btldr_addr_lo (btldr_addr % .256)
 #define btldr_addr_hi (btldr_addr / .256)
; reserve 2 words for pagesel and goto appvector
 #define appvec_loc (btldr_addr - .2)

; no interrupts in bootloader!
; bank 0 variables 
mainv_def macro varname
varname equ main_vars
main_vars set main_vars + 1
 endm
mainv_start equ 0x30 ; can go to 0x06f in bank 0 access bank
main_vars set mainv_start ; start of kernel vars


 ; bank 1 variables
mainv_defb1 macro varname
varname equ main_b1_vars
main_b1_vars set main_b1_vars + 1
 endm
mainv_startb1 equ 0x20 ; can go to 0x6f using BSR set to 1
main_b1_vars set mainv_startb1 ; start of bank 1 kernel vars

; locations 70 to 7f are common all banks
mainv_defcomm macro varname
varname equ main_comm_vars
main_comm_vars set main_comm_vars + 1
 endm
mainv_startcomm equ 0x70 ; can go to 0x7f
main_comm_vars set mainv_startcomm ; start of common kernel vars


 mainv_defcomm comm1; four scratch vbls
 mainv_defcomm comm2; 
 mainv_defcomm comm3; 
 mainv_defcomm comm4; 

; *********************** UART ***************************************
; bank 4 rx, bank 5 tx
 mainv_def uart_status ;
 #define lf_flag 0; lf received, set by ISR clear by uart_mon
 #define tx_active 7
 #define k_unlocked 6
 #define uerr 5 ; received error

 ; RX FIFO is bank 4 lfsr hi is 0x02
 mainv_def rx_byte ; working register for ISR
 mainv_def rx_wloc ; bank 4 0x20-0x6f ; bank 4 so lfsr high is 0x02
 mainv_def rx_rloc ; read loc owned by Kernel
 mainv_def rx_ct ; count owned by ISR but Kernel can decrement

 ; TX FIFO is bank 5 lfsr hi is 0x02
 mainv_def tx_wloc ; write loc 0xA0-0xEf owned by Kernel
 mainv_def tx_rloc ; read loc owned by ISR
 mainv_def tx_ct ; count owned by ISR but Kernel can increment

 ; scratch
 mainv_def utmp1;
 mainv_def utmp2;
 mainv_def utmp3;
 mainv_def utmp4;
 mainv_def utmp5;

; *********************** END UART ***********************************

; *********************** pgm_mem ************************************
; this is bank 2, lfsr hi is 0x01
; locs 20-5f 64 bytes of data for pgm_mem
; ********************** end pgm_mem *****************************


; *********************** END RUNTIME DATA *****************************

