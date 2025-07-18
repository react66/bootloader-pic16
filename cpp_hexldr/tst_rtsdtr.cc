// tst_rtsdtr.cc
// see how to control the lines
#include <stdio.h>
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()
#include <sys/ioctl.h> //ioctl() call defenitions
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
using namespace std;

string exec(const char* cmd) {
    array<char, 128> buffer;
    string result;
    shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
            result += buffer.data();
    }
    return result;

}int main()
{
   // list the avail ports
   cout<<"getting port list:\n";
   string theports= exec("ls /dev/ttyUS*");
   cout<<theports;
   int fd;
   fd = open("/dev/ttyUSB0",O_RDWR | O_NOCTTY );//Open Serial Port
   int DTR_flag;
   DTR_flag = TIOCM_DTR;
   cout<<"setting DTR, press char to reset it:\n";
   ioctl(fd,TIOCMBIS,&DTR_flag);//Set RTS pin
   getchar();
   cout<<"setting break, press cr\n";
   ioctl(fd,TIOCSBRK);
   getchar();
   cout<<"clearing DTR, press char to exit\n";
   ioctl(fd,TIOCMBIC,&DTR_flag);//Clear RTS pin
   getchar();
   cout<<"clearing break, press cr\n";
   ioctl(fd,TIOCCBRK);
   getchar();
   close(fd);
   return 0;
}
