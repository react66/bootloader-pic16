// serlnk.cc

#include "serlnk.h"

// C library headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// c++ (c is limits.h)
#include <climits>

// Linux headers
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()
#include <sys/ioctl.h>

int serlnk::lnk_close(){
 close(lnk_num);
// delete(lnk_name); 
 return 0;
 }
 
serlnk::serlnk(strlnk* si,int speed):vbc_link(){
 lnk_name = new strlnk(si->str1); // just a copy
 lnk_num = 0;
 lnk_speed=speed;
 lnk_state=1;//name set, not open yet
 }

serlnk::~serlnk(){
 close(lnk_num);
 delete(lnk_name);
 }

int serlnk::lnk_break(){ 
 if (lnk_state!=1) return 1;
// assert dtr,rts,break, wait 20 msec,deassert dtr,rts, wait 20 msec, ~break
 lnk_num = open(lnk_name->str1, O_RDWR);
  if (lnk_num < 0) {
  cerr<<"ERROR "<<errno<<" from Open: "<<*lnk_name<<"  NOT FOUND\n"; exit(1); }
 int dtrrts_flag;
 dtrrts_flag= TIOCM_DTR | TIOCM_RTS;
 ioctl(lnk_num,TIOCMBIS,&dtrrts_flag); // set dtr,rts low
 ioctl(lnk_num,TIOCSBRK); //assert break
 usleep(2000); // 2 msec
 ioctl(lnk_num,TIOCMBIC,&dtrrts_flag); // clr dtr,rts low
 usleep(2000); // 2 msec
 ioctl(lnk_num,TIOCCBRK); // ~break
 lnk_state=2; // name set not open yet
// close(lnk_num);
 return 0;
}

int serlnk::lnk_rtsdtr_flash(){ 
 if (lnk_state!=1) return 1;
// assert dtr,rts,break, wait 20 msec,deassert dtr,rts, wait 20 msec, ~break
 lnk_num = open(lnk_name->str1, O_RDWR);
  if (lnk_num < 0) {
  cerr<<"ERROR "<<errno<<" from Open: "<<*lnk_name<<"  NOT FOUND\n"; exit(1); }
 int dtrrts_flag;
 dtrrts_flag= TIOCM_DTR | TIOCM_RTS;
 ioctl(lnk_num,TIOCMBIS,&dtrrts_flag); // set dtr,rts low
// ioctl(lnk_num,TIOCSBRK); //assert break
 usleep(2000); // 2 msec
 ioctl(lnk_num,TIOCMBIC,&dtrrts_flag); // clr dtr,rts low
 usleep(2000); // 2 msec
// ioctl(lnk_num,TIOCCBRK); // ~break
 lnk_state=2; // name set not open yet
// close(lnk_num);
 return 0;
}

int serlnk::lnk_open(){
 if (lnk_state==1) {
 lnk_num = open(lnk_name->str1, O_RDWR);
  if (lnk_num < 0) {
  cerr<<"ERROR "<<errno<<" from Open: "<<*lnk_name<<"  NOT FOUND\n"; exit(1); }
  }
 fcntl(lnk_num,F_SETFL,FNDELAY); // use timer and non-blocking
 struct termios atty;
 memset(&atty, 0, sizeof atty);
 if(tcgetattr(lnk_num, &atty) != 0) {
  cerr<<"ERROR "<<errno<<" from tcgetattr\n";
  exit(1);}
 atty.c_cflag=0x1cb2;
 atty.c_lflag=2;
 atty.c_iflag=0;
 atty.c_oflag = 0;
/*
VMIN = 0, VTIME = 0: No blocking, return immediately
VMIN > 0, VTIME = 0: blocking wait for this many bytes
VMIN = 0, VTIME > 0: blocking wait this time or when any byte
VMIN > 0, VTIME > 0: block until first byte then this many or timeout 
parms are ushort, deci-seconds vtime.
*/
 atty.c_cc[VTIME] = 0; // non-blocking
 atty.c_cc[VMIN] = 0;
 cfsetspeed(&atty, lnk_speed);
// cfsetospeed(&atty, 2000000);
 if (tcsetattr(lnk_num, TCSANOW, &atty) != 0) {
   cerr<<hex<<"ERROR "<<errno<<" from tcsetattr\n";
   exit(1);}
 tcflush(lnk_num,TCIOFLUSH);
 usleep(2000); // stabilize 4 msec
 lnk_state=3; // up I think.
 return 0;
 }

int serlnk::lnk_putget(strlnk* prompt, int use_timer=0){
 char read_buf [1024];
 int num_bytes;
 int read_lim=1023; // need room for termin.
 if (lnk_state!=3) return 1;
 write(lnk_num,prompt->str1, prompt->strlnk_len); 
 if (use_timer>0) usleep(use_timer * 1000);
 num_bytes = read(lnk_num, &read_buf, read_lim);
 if (!(num_bytes > 0)){ 
  cerr<<"ERROR READING (system string): "<<strerror(errno)<<"\n";
  cerr<<"No response on serial port\n";
  exit(1); 
  }
 read_buf[num_bytes]='\0'; // string term is null.
 delete(prompt);
 prompt = new strlnk(read_buf);
 return 0;
 }

int serlnk::lnk_put(strlnk* prompt){
 if (lnk_state!=3) return 1;
 write(lnk_num,prompt->str1, prompt->strlnk_len); 
 return 0;
 }

// prompt is a pointer that is null here
int serlnk::lnk_get(strlnk* prompt, int use_timer=0){
 char read_buf [1024];
 int num_bytes;
 int read_lim=1023; // need room for termin.
 if (lnk_state!=3) return 1;
 if (use_timer) usleep(use_timer * 1000);
 num_bytes = read(lnk_num, &read_buf, read_lim);
 if (!(num_bytes > 0)){ 
  cerr<<"ERROR READING (system string): "<<strerror(errno)<<"\n";
  cerr<<"No response on serial port\n";
  exit(1); 
  }
 read_buf[num_bytes]='\0'; // string term is null.
 prompt = new strlnk(read_buf);
 return 0;
 }

 
 

