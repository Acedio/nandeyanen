all: nes

nes: nes.o opcode.o ines.o examine.o cpu.o memory.h mapper.o
	gcc -g $^ -o $@

nes.o: nes.c
	gcc -c $^

%.o: %.c %.h
	gcc -c $<

test: nes
	./nes nestest.nes

clean:
	rm nes
	rm *.o
