
// ihex_line.cc

#include "ihex_line.h"



//    START OF CLASS: ihex_line     *******************

vbc_link* ihex_line::newobj(istream& rst)
{
ihex_line* il = new ihex_line();
rst>>il->byte_len;
rst>>il->byte_loc;
rst>>il->rec_type;
int j;
for (int i=0; i<16; i++){
  rst>>hex>>j; // assume int compatible
  il->data[i]=(ushort)j;
}
rst>>il->cksum;
return il;
}


ostream& ihex_line::pwrite(ostream& wst)
{
wst<<" "<<cnum();
wst<<" "<<byte_len;
wst<<" "<<byte_loc;
wst<<" "<<rec_type;
wst<<hex<<" ";
int lcount=0;
for (int i=0; i<16; i++){
  wst<<data[i]<<" ";
  lcount++;
  if (lcount==16){lcount=0;wst<<" \\\n";};
}
wst<<" "<<cksum;
return wst;
}

ostream& ihex_line::pdump(ostream& wst)
{
vbc_link::ofs+=1;
vbc_link::indent(wst);
wst<<"ihex_line cnum= "<<cnum()<<"\n";
vbc_link::indent(wst);
wst<<"byte_len:";
wst<<byte_len<<"\n";
vbc_link::indent(wst);
wst<<"byte_loc:";
wst<<byte_loc<<"\n";
vbc_link::indent(wst);
wst<<"rec_type:";
wst<<rec_type<<"\n";
vbc_link::indent(wst);
wst<<"data:";
wst<<data<<"\n";
vbc_link::indent(wst);
wst<<"cksum:";
wst<<cksum<<"\n";
vbc_link::ofs-=1;
return wst;
}

ihex_line::~ihex_line()
{
}


ihex_line::ihex_line(strlnk* si):vbc_link(){
  ushort icksum=0;
  byte_len=si->hex_to_uns(1,2);
  icksum+=byte_len;
  byte_loc=si->hex_to_uns(3,4);
  icksum+=byte_loc%256;
  icksum+=byte_loc/256;
  rec_type=si->hex_to_uns(7,2);
  icksum+=rec_type;
  for(int i=0;i<byte_len;i++) {
    data[i]=(si->hex_to_uns((9+i*2),2));
    icksum+=data[i];
   }
  icksum=-icksum & 0xff;
  cksum=si->hex_to_uns(si->strlnk_len-2,2);
  if (cksum!=icksum) {
   cout<<hex<<"icksum: "<<icksum<<" cksum: "<<cksum<<"\n";
   cerr<<"CHECKSUM ERROR, input line:\n";
   cerr<<*si<<"\n";
   exit(1);
  }
}

ihex_line::ihex_line(strlnk* si,int ignore):vbc_link(){
  ushort icksum=0;
  byte_len=si->hex_to_uns(1,2);
  icksum+=byte_len;
  byte_loc=si->hex_to_uns(3,4);
  icksum+=byte_loc%256;
  icksum+=byte_loc/256;
  rec_type=si->hex_to_uns(7,2);
  icksum+=rec_type;
  for(int i=0;i<byte_len;i++) {
    data[i]=(si->hex_to_uns((9+i*2),2));
    icksum+=data[i];
   }
  icksum=-icksum & 0xff;
  cksum=si->hex_to_uns(si->strlnk_len-2,2);
  if ((cksum!=icksum)&&(ignore!=1)) {
   cerr<<"CHECKSUM ERROR, input line:\n";
   cout<<hex<<"computed: "<<icksum<<" observed: "<<cksum<<"\n";
   cerr<<*si<<"\n";
   exit(1);
  }
}

ihex_line::ihex_line(ushort len,int loc,ushort rec_type,
ushort *dat):vbc_link(){
byte_len=len;
cksum=byte_len;
byte_loc=loc;
cksum+=byte_loc/256;
cksum+=byte_loc%256;
for (int i=0;i<byte_len;i++){
data[i]=dat[i];
cksum+=data[i];
}
cksum=-cksum & 0xff;
}

void ihex_line::gen_str(strlnk* &line) {
 line=new strlnk(":10"); // length
 int cksum=16;
 strlnk* a=new strlnk();
 a->to_hex(byte_loc,4);
 *line+=*a;
 cksum+=byte_loc/256;
 cksum+=byte_loc%256;
 *line+="00";
 for (int i=0;i<byte_len;i++) {
  cksum+=data[i];
  a->to_hex(data[i],2);
  *line+=*a;
  }
 a->to_hex(-cksum,2); //checksum
 *line+=*a;
 delete(a);
}



//    END OF CLASS: ihex_line     *******************

void ihex_line_register()
{
if (!(obj_reg)) lists_register();
obj_reg->enq(new ihex_line(),obj_reg->work);
}

