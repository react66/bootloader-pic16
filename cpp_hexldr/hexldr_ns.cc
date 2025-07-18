// program hexldr
// REA 100720 program to read in a hex file and program a PIC16F153XX
//   using bootloader.
#include "lists.h"
#include "ihex_line.h"
#include "serlnk.h"
#include "exec_call.h"
#include <unistd.h> // sleep function
#include <experimental/filesystem>

// send command to read a block
void readblock(serlnk &s1, int blkno) {
char workbuff[100]; // used for commands
strlnk* stmp;
int upos=blkno*32; // word position in memory for block
sprintf(&workbuff[0],"R%04x",upos);
stmp=new strlnk(workbuff); // create copy in strlnk
*stmp+="\n";
s1.lnk_putget(stmp,3); // 3 msec wait time
if (!((stmp->strlnk_len == 2) && (*stmp=="*\n"))) {
  cerr<<"R response error: >"<<*stmp<<"<\n";
  exit(1);
  }
delete(stmp);  
};

int main(int argc, char* argv[])
{
lists_register();
ifstream if0;
ofstream of0;

strlnk* parm[10]; // up to 10 parms from command line
for (int i=0;i<argc-1;i++)parm[i] = new strlnk(argv[i+1]);

if (argc!=5)  {
  cout<<"*******************************************************************\n";
  cout<<"*******************************************************************\n";
  cout<<"************ program hexldr R 1.0 for loading pic16f153XX\n";
  cout<<"Operation: hexldr p|r <hex_file> <dev_code> <serial_port> \n";
  cout<<"USAGE:\n";
  cout<<"   p|r: program or read\n";
  cout<<"   <hex_file>: hex file to program or create, eg. a12_ex.hex\n";
  cout<<"     hex files do not contain bootloader\n";
  cout<<"   <dev_code>:\n";
  cout<<        "pic16f15324/44: 13\n";
  cout<<        "pic16f15325/45: 1D\n";
  cout<<"   <serial_port>: eg. /dev/ttyUSB0 on linux\n";
  cout<<"                     \\\\.\\COM1 or some such on Windows\n";
  cout<<"*******************************************************************\n";
  cout<<"*******************************************************************\n";

exit(1);
}
// handle args:
strlnk* dev_code, *hex_file, *serial_port, *op, *stmp, *hexnum;
int stat;
char workbuff[100]; // used for commands

op=parm[0];
hex_file=parm[1]; 
dev_code=parm[2];
serial_port=parm[3];

// ********************************* set up serial link:
serlnk s1(serial_port,2000000); // create object
// check setserial /dev/ttyUSBX low_latency
// if this fails there is no port by that name so exit.
strlnk* setser_call=new strlnk("setserial ");
*setser_call+=(serial_port->str1);
*setser_call+=" low_latency";
const auto process_ret = exec_call(setser_call->str1);
if (process_ret.second != 0) {
 cerr<<"ERROR: setserial low_latency did not work!\n";
 return 1;
 }
delete(setser_call);
cout<<*serial_port<<" set to low_latency\n";
s1.lnk_break(); // assert RTS,dtr 0 (clr), break=0, rts=1, break=1
stat = s1.lnk_open(); // if this fails we exit
if (stat != 0) {
 cerr<<"ERROR: serial link open fail, status: "<<stat<<endl;
 exit(1);
 }
// get the ID for the processor bootloader
stmp=new strlnk("i\n");
stat=s1.lnk_putget(stmp,100); // wait x msec here

// failure here could be bad connection
if (stmp->strlnk_len != 5) {
 cerr<<"ID response error: length: "<<stmp->strlnk_len;
 cerr<<" field >"<<*stmp<<endl;
 exit(1);
 }
if (stmp->strpos(dev_code->str1) != 0) {
 cerr<<"Bad device code: >"<<*stmp<<endl;
 exit(1);
 }
cout<<"device/rev: "<<*stmp<<endl;
delete(stmp);

// unlock for read/write access.
stmp=new strlnk("REA\n");
stat=s1.lnk_putget(stmp,100);
if (stmp->strlnk_len != 2) {
  cerr<<"UNLOCK FAILED, return length: "<<stmp->strlnk_len<<"\n";
  exit(1);
  }
if ((stmp->strlnk_len != 2) || !(*stmp=="*\n")) { 
  int ik=stmp->strlnk_len;
  cerr<<"UNLOCK FAILED, return length: "<<ik;
  if (ik>0) cerr<<" REPLY: "<<*stmp;
  cerr<<"\n";
  exit(1);
  }
delete(stmp);
//************************* serial link is setup

// btldr at 0xe20 for 4K, 0x1e20 for 8k
int nwords; // space to program is total-btldr
if (*dev_code=="13") nwords=4096-0x1e0;
else if(*dev_code=="1D") nwords=8192-0x1e0;
else {
 cerr<<"ERROR dev_code is: >"<<*dev_code<<"<\n";
 exit(1);
 }
// ********************** create the memory array
const int nbytes=nwords*2; 
const int nblks=nwords/32;
cout<<"nwords: "<<nwords<<" nbytes: "<<nbytes<<" nblks: "<<nblks<<endl;
ushort mem[nbytes];
// init array to unprogrammed
for (int i=0; i<nwords; i++) { // 4096 words
 mem[2*i]=0x3f;
 mem[2*i+1]=0xff;
 }

int debug=0; // to dump data read in, etc debug stuff.
int progress_marks=1;

// ****************************** PROGRAM *************************
// ****************************************************************
// program?
if (*op=="p") {  // program
// read in the hex file
rlist* hex_in=new rlist(hex_file->str1);
// report number of lines and check first and last line start with :
// cout<<"HEX file number of lines: "<<hex_in->count<<"\n";
stmp = (strlnk*)(hex_in->head);
if (stmp->str1[0] != ':') {
 cout<<"error in hex file, first line: >"<<*stmp<<"<\n";
 exit(1);
 }
stmp = (strlnk*)(hex_in->tail);
if (stmp->str1[0] != ':') {
 cout<<"error in hex file, last line: >"<<*stmp<<"<\n";
 exit(1);
 }

rlist* il = new rlist(); // make a list of processed lines
stmp = (strlnk*)(hex_in->head);
while (stmp) {
 il->enq(new ihex_line(stmp));
 stmp = (strlnk*)(stmp->n);
}

// move into mem[]

int blk[nblks]; // nblks blocks of 32 words each in program area
for (int i=0; i<nblks; i++) blk[i]=0; // set to 1 if active
int eaddr=0; //extended address
ihex_line* ihl = (ihex_line*)(il->head);
int ihl_counter=0;
while (ihl) {
 if (ihl->rec_type != 0) {
  if (ihl->rec_type == 4) { // 2 byte extended address in data field
  eaddr=(ihl->data[0]) * 256 + ihl->data[1];
  }
  ihl = (ihex_line*)(ihl->n);
  ihl_counter++;
  continue;
  }
 if (eaddr > 0) {
  ihl = (ihex_line*)(ihl->n);
  ihl_counter++;
  continue; // this processor is small
  }
 int mempos = ihl->byte_loc; // byte location 0 to nbytes-1
 for (int i=0; i<(ihl->byte_len/2); i++) {
  if ( mempos >= nbytes ) {
    cout<<"bad memory location: "<<mempos<<"\n";
    cout<<"line: "<<ihl_counter<<"\n";
    exit(1);
    }
  blk[mempos/64]=1; // mark as active, start and endpoints
  mem[mempos]=ihl->data[i*2+1]; // reverse the order
  mem[mempos+1]=ihl->data[i*2];
  mempos+=2;
  }
 ihl = (ihex_line*)(ihl->n);
 ihl_counter++;
 }
 // BLK 0 must be there!!!
 if (blk[0]==0) {
  cerr<<" ERROR: hex file is missing block 0 data\n";
  exit(1);
  }
 // always set both last block to be programmed. 
 // firmware will overwrite loc 0, here we make sure appvec
 // is set for hex files that have no bootloader
 blk[nblks-1]=1;
 // if first 4 bytes= 31 8e||9e 2e 20 then the bootloader is in
 // the hex file already so don't change appvec.
 if ((mem[0]!=0x31) || !((mem[1]==0x8e) || (mem[1]==0x9e)) ||
     (mem[2]!=0x2e) || (mem[3]!=0x20)) {
    // fixup appvec, first 4 bytes are copied to appvec
    for (int i=0; i<4; i++) mem[i + nbytes - 4]=mem[i];
    }
////////////// firmware always sets loc 0: goto btldr

// debug: dump the 64 byte mem blocks in the hex file
if (debug==1) {
cout<<setfill('0');
for (int i=0; i<nblks; i++) {
 if (blk[i]==1) { // dump this block
 cout<<"\nblk: "<<dec<<i;
 cout<<hex<<" code words "<<setw(4)<<(i*32)<<" to ";
 cout<<setw(4)<<(i*32+31)<<"\n";
 // dump 4 lines of 8 words each
 for (int j=0; j<4; j++) {
  int startpos=i*64+j*16;
  for (int k=0; k<8; k++) { // setw only works once
   cout<<setw(2)<<mem[startpos+k*2]<<setw(2)<<mem[startpos+k*2+1]<<" ";
   }
  cout<<"\n";
 }
 }
}
cout<<"\n\n";
}
cout<<"\n";

// program all the blocks with data, skip unused blocks
for (int blkno=0;blkno<nblks;blkno++) {
 int upos=blkno*32; // word position in memory for block
 if (progress_marks) {
 if (blkno%64==0) cout<<endl;
 else if (blkno%16==0) cout<<" ";
 if (blk[blkno]==1)cout<<"1"; else cout<<"-";
 cout.flush();
 }
 if  (blk[blkno]==1) { // only work on used blocks
  // fill the buffer with 32 words, 16 bytes at a time offsets 20,30,40,50
  // mnnxxxx...<lf> command
  int startpos=blkno*64; // 64 bytes per block
  for (int jj=20;jj<60;jj+=10){ // 10 will be interpreted as hex 16
   sprintf(&workbuff[0],"m%2d",jj);
   for (int i=0;i<16;i++) {
     sprintf((char*)(&(workbuff[2*i+3])),"%02X",mem[startpos+i]);
     }
   stmp=new strlnk(workbuff);
   *stmp+="\n";
//  cout<<"\nblkno,jj,string: "<<blkno<<" "<<jj<<" "<<*stmp<<endl;
   stat=s1.lnk_putget(stmp,3);
   if (!((stmp->strlnk_len == 2) && (*stmp=="*\n"))) {
     cerr<<"\nresponse error: length: "<<stmp->strlnk_len;
     cerr<<" field >"<<*stmp<<"<"<<endl;
     cerr<<"blkno: "<<blkno<<endl;
     exit(1);
     }
   delete(stmp);
   startpos+=16;
  } //for 32 word block
  // blank the memory block
  sprintf(&workbuff[0],"b%04x",upos);
  stmp=new strlnk(workbuff);
  *stmp+="\n";
  stat=s1.lnk_putget(stmp,3);
  if (!((stmp->strlnk_len == 2) && (*stmp=="*\n"))) {
    cerr<<"b response error: length: "<<stmp->strlnk_len;
    cerr<<" field >"<<*stmp<<"<"<<endl;
    cerr<<"blkno: "<<blkno<<endl;
    exit(1);
    }
  delete(stmp);
  // now the u command to write to mem
  sprintf(&workbuff[0],"u%04x",upos);
  stmp=new strlnk(workbuff);
  *stmp+="\n";
  stat=s1.lnk_putget(stmp,3);
  if (!((stmp->strlnk_len == 2) && (*stmp=="*\n"))) {
    cerr<<"m response error: length: "<<stmp->strlnk_len;
    cerr<<" field >"<<*stmp<<endl;
    exit(1);
    }
  delete(stmp);
  } //if blk!=0
  } //blkno
 } //if (*op=="p"}

// ***************************************************************
// **************** READ *****************************************

if (*op=="r") { // read contents and create hex file
// ************* get the appvec to put in blk 0
// first read the block just before the bootloader=nblks-1
int blkno=nblks-1;
readblock(s1,blkno);
// now the end block is in pic memory.
// read the last 4 bytes using j command
strlnk* appvec = new strlnk("j015C04\n");
s1.lnk_putget(appvec,3); // 3 msec wait time
cout<<"\nappvec: "<<*appvec<<endl;
// now read in all the blocks from 0 to nblks-1
for (blkno=0;blkno<nblks;blkno++) {
 readblock(s1,blkno);
 // flip bytes and put in mem
 int byteloc=blkno*64; //32 words per block
 for (int picloc=32;picloc<(96);picloc+=16) { //4 lines
   stmp=new strlnk("j01");
   hexnum= new strlnk();
   hexnum->to_hex(picloc,2);
   *stmp+=*hexnum;
   delete(hexnum);
   *stmp+="10\n";
//   cout<<"stmp before: "<<*stmp<<endl;
   s1.lnk_putget(stmp,3);
// substitute appvec for first 4 bytes:
   if ((blkno==0) && (picloc==32)) {
     *appvec-="\n"; //hangover from serlnk
     stmp->s("31 8E 2E 20 ",appvec->str1,1);
   }
   //reverse order and put 8 words (16 bytes) in
   for (int spt=0;spt<48;spt+=6) {
    mem[byteloc++]=stmp->hex_to_uns(spt+3,2); // make lo-hi order
    mem[byteloc++]=stmp->hex_to_uns(spt,2);
    } //spt
   delete(stmp);
 } //picloc
} // for blkno
// now output the hex file

// first put it in ihex_line form
ihex_line* ihl;
rlist* ohex = new rlist();
int save_it=0;
for (int i=0; i<nbytes; i+=16) {
 ihl=new ihex_line();
 save_it=0; //default do not save
 ihl->byte_len=0x10;
 ihl->byte_loc=i;
 ihl->rec_type=0;
 for (int j=0;j<ihl->byte_len;j++) {
  ihl->data[j]=mem[j+i];
  if (((j%2 == 0) && (mem[j+i] != 0xff)) ||
      ((j%2 == 1) && (mem[j+i] != 0x3f))) save_it=1;
 }
 // trim any lines with ff3f...
 if (save_it) ohex->enq(ihl); else delete(ihl);
}
// open output file
cout<<"output file: "<<*hex_file<<endl;
of0.open(hex_file->str1,ios::out);
if (!of0) { 
cerr<<"Error: cannot open output file "<<*parm[0]<<"\n";
}
// now output hex file
ihl=(ihex_line*)(ohex->head);
strlnk* line;
of0<<":020000040000FA\n"; // first line of hex file
while (ihl) {
 ihl->gen_str(line);
 of0<<*line<<endl;
 delete(line);
 ihl=(ihex_line*)(ihl->n);
 }
of0<<":00000001FF\n";
of0.close();

} // if (*op=="r")

// flash the rts,dtr lines to reset the encoder and enter runtime

cout<<"\nDone. Sending RESET now:\n";
stmp=new strlnk("Q\n");
stat=s1.lnk_put(stmp);

usleep(10 * 1000); // sleep 10 msec so it gets out of bootloader

close(s1.lnk_num);

return 0;
}

