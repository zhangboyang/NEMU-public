CC=gcc
CFLAGS=-O2
calc:calc.y calc.l
	bison -d calc.y
	flex -o calc.lex.c calc.l
	$(CC) $(CFLAGS) -o $@ calc.tab.c calc.lex.c -lfl

nemu:calc.y calc.l
	bison -d calc.y
	flex -o calc.lex.c calc.l
	$(CC) $(CFLAGS) -DCALC_NEMU -c -o calc.nemu.tab.o calc.tab.c
	$(CC) $(CFLAGS) -DCALC_NEMU -c -o calc.nemu.lex.o calc.lex.c
	
calc.lex: calc.y calc.l
	bison -d calc.y
	flex -o calc.lex.c calc.l
	$(CC) $(CFLAGS) -D CALC_LEX -o $@ calc.lex.c -lfl

clean:
	-rm -rf calc.lex.c calc.tab.h calc.tab.c calc calc.lex calc.nemu.tab.o calc.nemu.lex.o
