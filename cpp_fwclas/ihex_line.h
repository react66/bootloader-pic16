
// ihex_line.h

#ifndef ihex_line_h 
#define ihex_line_h

#include "lists.h"

const int cnum_ihex_line = 4001;


typedef class ihex_line *Pihex_line;
class ihex_line:public vbc_link
{
public:
ushort byte_len;
int byte_loc;
ushort rec_type;
ushort data[16];
ushort cksum;
long cnum(){return cnum_ihex_line ;}
ihex_line():vbc_link(){}
~ihex_line();
vbc_link* newobj(istream&);
ostream& pwrite(ostream&);
ostream& pdump(ostream&);

ihex_line(strlnk*); //constructor from strlnk (hex file line)
ihex_line(strlnk*,int); //constructor from strlnk (int=1 to ignore cksum)
void gen_str(strlnk* &); //create strlnk from ihex_line for hex file
ihex_line(ushort,int,ushort,ushort*); //constructor from array
};

void ihex_line_register();

#endif
