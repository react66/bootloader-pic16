CC= g++ -std=c++17 -I ~/include 
OBJS= ~/lib/lists.o ~/lib/serlnk.o 

all: ~/lib/serlnk.o

~/lib/serlnk.o: serlnk.cc serlnk.h
	${CC} -c serlnk.cc
	mv serlnk.o ~/lib
	cp serlnk.h ~/include

