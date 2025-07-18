#!/bin/bash
# using gpasm to assemble stub.asm
# case sensitivity off, w level 0, quiet q+
# chkerr keyword filename
function chkerr() {
 if grep -i  $1 $2
 then
   echo "$1 , see $2 file"
   sleep 8
   exit 1
 fi
 }
function grep_vars() {
echo "*********************** isr_vars bank 0 *************" > grepvars.txt
grep -i "equ isr_vars" stub.lst >> grepvars.txt
echo "*********************** main_vars bank 0 ************" >> grepvars.txt
grep -i "equ main_vars" stub.lst >> grepvars.txt
echo "*********************** main_b1_vars bank 1 *********" >> grepvars.txt
grep -i "equ main_b1_vars" stub.lst >> grepvars.txt
echo "*********************** main_comm_vars all banks ****" >> grepvars.txt
grep -i "equ main_comm_vars" stub.lst >> grepvars.txt
echo "******************** END OF FILE ********************" >> grepvars.txt
}
# bdset 0x03 for 2 mbit, 0x44 for 115200
bd_set=0x03
sw_rev=02

gpasm -w1 -pPIC16F15344 -Dbd_set=$bd_set -Ddevcd=0x13 -Dsw_rev=$sw_rev stub.asm
if [ $? -ne 0 ]
 then
 echo "error exit"
 exit 1
 fi 
echo "finished dev 0x13 (PIC16F15344) mpasmx for Revision $sw_rev "
if [ -e amt14_base.err ]
then
 chkerr ERROR stub.err
 chkerr WARN stub.err 
 chkerr DEFAULT stub.err 
 fi
rm -f *.err
rm -f *.o
rm -f *.cod
grep_vars
mv grepvars.txt lst_files/stub344_vars_r${sw_rev}.txt
mv stub.hex hex_files/stub344_r${sw_rev}.hex
mv stub.lst lst_files/stub344_r${sw_rev}.lst
echo "moved and renamed the hex and lst files"

gpasm -w1 -pPIC16F15344 -Dbd_set=$bd_set -Ddevcd=0x13 -Dsw_rev=$sw_rev -Dbt_a4 stub.asm
if [ $? -ne 0 ]
 then
 echo "error exit"
 exit 1
 fi 
echo "finished dev 0x13 (PIC16F15344) mpasmx for Revision $sw_rev bt_a4"
if [ -e amt14_base.err ]
then
 chkerr ERROR stub.err
 chkerr WARN stub.err 
 chkerr DEFAULT stub.err 
 fi
rm -f *.err
rm -f *.o
rm -f *.cod
grep_vars
mv grepvars.txt lst_files/stub344_vars_r${sw_rev}_a4.txt
mv stub.hex hex_files/stub344_r${sw_rev}_a4.hex
mv stub.lst lst_files/stub344_r${sw_rev}_a4.lst
echo "moved and renamed the hex and lst files"

gpasm -w1 -pPIC16F15345 -Dbd_set=$bd_set -Ddevcd=0x1D -Dsw_rev=$sw_rev stub.asm
if [ $? -ne 0 ]
 then
 echo "error exit"
 exit 1
 fi 
echo "finished dev 0x1D (PIC16F15345) gpasm for Revision $sw_rev "
if [ -e amt14_base.err ]
then
 chkerr ERROR stub.err
 chkerr WARN stub.err 
 chkerr DEFAULT stub.err 
 fi
rm -f *.err
rm -f *.o
rm -f *.cod
grep_vars
mv grepvars.txt lst_files/stub345_vars_r${sw_rev}.txt
mv stub.hex hex_files/stub345_r${sw_rev}.hex
mv stub.lst lst_files/stub345_r${sw_rev}.lst
echo "moved and renamed the hex and lst files"


sleep 2
