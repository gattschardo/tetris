targets += tt

tt: tt.c tetris.o val.o

test: tt
	./tt
