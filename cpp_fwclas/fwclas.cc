
// fwclas.cc

#include "fwclas.h"



//    START OF CLASS: hexim     *******************

vbc_link* hexim::newobj(istream& rst)
{
hexim* il = new hexim();
il->name= (strlnk*)read_obj(rst);
int j;
for (int i=0; i<16384; i++){
  rst>>hex>>j; // assume int compatible
  il->data[i]=(ushort)j;
}
return il;
}


ostream& hexim::pwrite(ostream& wst)
{
wst<<" "<<cnum();
if (name)name->pwrite(wst);
else wst<<" 0";
wst<<hex<<" ";
int lcount=0;
for (int i=0; i<16384; i++){
  wst<<data[i]<<" ";
  lcount++;
  if (lcount==16){lcount=0;wst<<" \\\n";};
}
return wst;
}

ostream& hexim::pdump(ostream& wst)
{
vbc_link::ofs+=1;
vbc_link::indent(wst);
wst<<"hexim cnum= "<<cnum()<<"\n";
vbc_link::indent(wst);
wst<<"name:";
if (name)name->pdump(wst);
else wst<<" 0";
wst<<"\n";
vbc_link::indent(wst);
wst<<"data:";
wst<<data<<"\n";
vbc_link::ofs-=1;
return wst;
}

hexim::~hexim()
{
if (name) delete(name);
}


hexim::hexim(char* ihexname):vbc_link(){
 name=0;
 for (auto i:data) data[i]=0;
 rlist* hexf=new rlist(ihexname);
 strlnk* lineis=(strlnk*)(hexf->head);
 int data_ptr;
 strlnk* val,*loc;
 while (lineis) {
  val=lineis->extract(7,8); // record type
  loc=lineis->extract(3,6); // hex byte loc
  data_ptr=loc->hex_to_uns(4);
  delete(loc);
  if (*val == "00"){ // get the data
   delete(val);
   while((lineis->strlnk_len)>8){ // comes in 2 byte groups
    val=lineis->extract(3,4);
    data[data_ptr+1]=val->hex_to_uns(2);
    delete(val);
    val=lineis->extract(3,4);
    data[data_ptr]=val->hex_to_uns(2);
    delete(val);
    data_ptr+=2;
   }}
   else delete(val);
  lineis=(strlnk*)(lineis->n);
  }
 }



//    END OF CLASS: hexim     *******************

void fwclas_register()
{
if (!(obj_reg)) lists_register();
obj_reg->enq(new hexim(),obj_reg->work);
}

