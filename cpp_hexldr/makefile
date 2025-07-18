#CC= g++ -std=c++17 -Wno-deprecated -I ~/include -static -static-libgcc -static-libstdc++ 
CC= g++ -std=c++17 -I ~/include -static -static-libgcc -static-libstdc++ 
OBJS= ~/lib/lists.o ~/lib/ihex_line.o ~/lib/serlnk.o ~/lib/exec_call.o

all:  ~/lib/ihex_line.o hexldr ~/bin/hexldr hexldr_ns ~/bin/hexldr_ns tst_rtsdtr

hexldr: hexldr.cc
	$(CC) $(OBJS) -o hexldr hexldr.cc 

~/bin/hexldr: hexldr
	cp hexldr ~/bin

hexldr_ns: hexldr_ns.cc
	$(CC) $(OBJS) -o hexldr_ns hexldr_ns.cc 

~/bin/hexldr_ns: hexldr_ns
	cp hexldr_ns ~/bin

clean:
	rm -f hexldr ~/bin/hexldr hexldr_ns ~/bin/hexldr_ns tst_rtsdtr 

tst_rtsdtr: tst_rtsdtr.cc
	$(CC) -o tst_rtsdtr tst_rtsdtr.cc

