CC= g++ -Wno-deprecated -I ~/include ~/lib/lists.o ~/lib/exec_call.o

all: ~/lib/exec_call.o ~/bin/do_exec

~/bin/do_exec: do_exec.cc ~/lib/lists.o
	$(CC) -o do_exec do_exec.cc
	cp do_exec ~/bin

~/lib/exec_call.o: exec_call.cc exec_call.h
	g++ -Wno-deprecated -g -c -I ~/include exec_call.cc
	mv exec_call.o ~/lib
	cp exec_call.h ~/include

clean:
	rm -f do_exec ~/bin/do_exec
	rm -f ~/lib/exec_call.o
	rm -f ~/include/exec_call.h
