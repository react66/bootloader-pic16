This bootloader uses 1 pin HDX operation at
   2 Mbits for loading the image.

The assembly proceeds with stub.asm which includes the other files.

Notes:
 top of memory is 0xfff = 4k-1 for 15344.
 bootloader is at 0x1000 - 0x1e0 = 0xE20
  appvec_loc is 0xE20-2 = 0xE1E
******** hexldr must do the following:
 2 words at loc 0 in the user image are the init location
 normally. If the bootloader is present in the image the
 2 words will be: 
" pagesel btldr" (movlp 0x01)  (318e)
" goto btldr" (2e20)
==> if this is true, the values are untouched because the 
    bootloader is present and the init location is already
    copied to location 0x1e1
==> Otherwise the 318e, 2e20 commands overwrite the goto
    init in location 0 and the original commands in location
    0 are copied to appvec_loc 

 NOTE the 2 words at location 0 of hex file are expected to 
 be pagesel init, goto init, a valid hex file must have
 that at the beginning or it will not work. hexldr should
 check that something reasonable is there.

Connections to the PIC (3 pins):

MCLR: RA3 is the PIC MCLR, connect to RTS or DTR.
GND:
RTX: receive and transmit HDX signal, RB5 on PIC16F15344 is default
     RX for uart1.

REA 042821:
 Q command was not working right, the terminal doesn't run after.
 substituting 
 goto appvec; should start application.

REA 103023:

R02 is the same as R01, just the ifdefs are added for '345.
Deleting R01 hex files.

