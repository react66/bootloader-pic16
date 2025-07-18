CC= g++ -Wno-deprecated  -fmax-errors=3 -I ~/include ~/lib/lists.o ~/lib/fwclas.o 

all: fwtest runit.skl

fwtest: fwtest.cc ~/lib/lists.o ~/lib/fwclas.o 
	$(CC) -g -o fwtest fwtest.cc

runit.skl: fwtest seq00C0.hex
	fwtest seq00C0.hex seq00C0.pwt
	touch runit.skl

clean:
	rm -f fwtest runit.skl
