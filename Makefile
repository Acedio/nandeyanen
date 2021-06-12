all: nes

nes: nes.o opcode.o
	gcc $^ -o $@

nes.o: nes.c
	gcc -c $^

%.o: %.c %.h
	gcc -c $<

clean:
	rm nes
	rm *.o
