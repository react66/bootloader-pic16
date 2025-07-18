
// fwclas.h

#ifndef fwclas_h 
#define fwclas_h

#include "lists.h"

#include <gzstream.h>

const int cnum_hexim = 401;


typedef class hexim *Phexim;
class hexim:public vbc_link
{
public:
strlnk *name;
ushort data[16384];
long cnum(){return cnum_hexim ;}
hexim():vbc_link(){
name=0;}
~hexim();
vbc_link* newobj(istream&);
ostream& pwrite(ostream&);
ostream& pdump(ostream&);

hexim(char*); //read a hex file into data
};

void fwclas_register();

#endif
