030121 REA:
041723 REA:

program hexldr

Operation: hexldr p|v <hex_file> <dev_code> <serial_port> 

This program uses the pic15344/5 bootloader to program a hex file in the device.
Mapping assumed:

 bootloader: 0xE20-0xFFF ('344) or 0x1E20-0x1FFF ('345) .
 info: 0xE00-0xE1D or 0x1E00-0x1E1D (appvec: 0xE1E,0xE1F or 0x1e1e,0x1e1f)
 Program code: 0x00-0xBFF or 0x000-0x1BFF

Location 0x00 is expected to contain two words: "pagesel init" and "goto init"
This is automatically copied to the appvec location by the hexldr. The 0x00 
location is overwritten by the bootloader when the first block is written,
it will always contain "pagesel btldr" and "goto btldr".

locations 0xE20 or 0x1E20 and above are protected by hexldr. 

hexldr does not verify. Program data can be read via the PIC bootloader
so verification is possible. It would be possible to brick the device
if the write to block 0 was interrupted but that is unlikely.

070623 REA:

Changed to std=c++17. will allow <filesystem> and type byte.

080123 REA:

adding b (blank) and r (read) commands, delete v command
we will verify by reading and comparing hex files.

r <hex_file> <dev_code> <serial_port> will read 0x0000 through 
  0x0E1F or 0x1E1F. locations 0,1 will have the contents of
  appvec (user init), appvec will be set to 3fff.

b <dev_code> <serial_port> will blank everything up to the 
  bootloader. It will preserve locations 0,1 bootloader vector.
  (rationale: programming does not erase unused blocks so
   reading may carry extra unused code, blanking a device 
   before writing will make it easier to verify)

032025 REA:

allowing scatterload. If there is no Block 1 with branch to
init then just load blocks as given. 

copied hexldr.cc to hexldr_ns.cc file for backup.

NOTE: hexldr_ns does not allow scatterload, the hex file must
have the pagesel,goto init as the first 2 entries in block 0.
This is to guard against accidents.


