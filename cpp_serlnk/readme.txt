REA 080723

serlnk class is serial interface used in hexldr.
It can set break and get into the bootloader.

eg. 
serlnk s1(serial_port,2000000); // create object

Low latency setting:

strlnk* setser_call=new strlnk("setserial ");
*setser_call+=(serial_port->str1);
*setser_call+=" low_latency";
const auto process_ret = exec_call(setser_call->str1);
if (process_ret.second != 0) {
 cerr<<"ERROR: setserial low_latency did not work!\n";
 return 1;
 }

Opening:

s1.lnk_break(); // assert RTS,dtr 0 (clr), break=0, rts=1, break=1
stat = s1.lnk_open(); // if this fails we exit
if (stat != 0) {
 cerr<<"ERROR: serial link open fail, status: "<<stat<<endl;
 exit(1);
 }
// get the ID for the processor bootloader
stmp=new strlnk("i\n");
stat=s1.lnk_putget(stmp,100);
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

Notice we explicitly add \n in the string for this, it is
added because the put routine does not append \n. It is
not normally needed because the string has a null termination.

