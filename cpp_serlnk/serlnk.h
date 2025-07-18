// serlnk.h

#ifndef serlnk_h
#define serlnk_h

#include "lists.h"


typedef class serlnk *Pserlnk;
class serlnk:public vbc_link
{
public:
strlnk* lnk_name;
int lnk_num;
int lnk_speed;
int lnk_state; //0=down, 1=name set, 2=rts/dtr and break done, 3=up

serlnk():vbc_link(){}
serlnk(strlnk*,int); // constructor from strlnk, lnk_speed
~serlnk();
int lnk_break(); //send break no parms required
int lnk_rtsdtr_flash(); //reset the link with no break asserted
int lnk_open(); //open and set parms
int lnk_close(); //close and delete lnk_name
int lnk_putget(strlnk*, int); //prompt/return, sleep*1000
int lnk_put(strlnk*); //write string
int lnk_get(strlnk*,int); //as putget but null ptr
};

#endif
