.PHONY: all clean

all: crt0.o

crt0.o: $(wildcard *.s *.sinc)
	cl65 -t nes -Oisr -c crt0.s

clean:
	rm -f *.o
