CC= g++ -std=c++17 -fmax-errors=3 -I ~/include

all: ~/lib/ihex_line.o ~/lib/fwclas.o test/runit.skl

~/lib/fwclas.o: fwclas.cc fwclas.h fwclas.mrg ~/lib/ihex_line.o
	$(CC) -c fwclas.cc
	cp fwclas.o ~/lib
	cp fwclas.h ~/include

fwclas.cc: fwclas.cld fwclas.mrg
	gencla fwclas

test/runit.skl: ~/lib/fwclas.o
	make -C test

~/lib/ihex_line.o: ihex_line.cc ihex_line.h
	g++ -c -I ~/include ihex_line.cc
	cp ihex_line.h ~/include
	mv ihex_line.o ~/lib

ihex_line.cc ihex_line.h: ihex_line.cld ihex_line.mrg
	gencla ihex_line

clean:
	rm -f fwclas.o ~/lib/fwclas.o ~/include/fwclas.h fwclas.cc
