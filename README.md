# bootloader-pic16
pic16 assembly level bootloader and c++ hex loader 

All makefiles assumes the existence of ~/include, ~/lib and ~/bin. Files are assembled using gpasm, a part of gputils.
gputils can be found here: https://gputils.sourceforge.io/

This project is reflected on sourceforge under the same name.

cpp_hexldr is the general hex file loader for the PIC16[l]f15344,345. The loader makes use of the other programs and make will need to be run for each one. hexldr.cc allows scatterload, while hexldr_ns.cc does not allow scatterload so the hex file must have the pagesel, goto init as the first 2 enteries in block 0.

cpp_do_exec contains exec_call.cc used for calling OS functions from the hexldr program.

cpp_fwclass contains ihex_line.cc used for managing lines of intel hex data read/written by the hexldr.

cpp_serlnk contains serlnk.cc used for serial interface.

pic16f15344,345 are the assembler bootloaders. Once the pic is programmed with the stub hex file, hexldr can be used to program it with any hex files.

1-wire_interface contains schematics for 2 versions of the USB to serial interface used by the hexldr program.
