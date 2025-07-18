//REA 030521 exec call routine for Linux
//
#include "exec_call.h"

pair<string, int> exec_call(const char* cmd) {
    array<char, 128> buffer;
    string result;
    int return_code = -1;
    auto pclose_wrapper = [&return_code](FILE* cmd){ return_code = pclose(cmd); };
    { // scope is important, have to make sure the ptr goes out of scope first
    const unique_ptr<FILE, decltype(pclose_wrapper)> pipe(popen(cmd, "r"),
    pclose_wrapper);
    if (pipe) {
        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
            result += buffer.data();
        }
    }
    }
    return make_pair(result, return_code);
}


