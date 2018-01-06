CFLAGS  := -Wall -ansi -g

targets :=
objs    := val.o tetris.o

all:

variants := gtk3 sdl2 sdl1 test
include $(patsubst %,%.mk,$(variants))

index.html:
	emcc -O2 s1.c tetris.c val.c -o s1.bc
	emcc s1.bc -O2 -s WASM=1 -o index.html

all: ${targets} index.html

run: index.html
	emrun --no_browser --port 8000 .

clean:
	${RM} ${targets} ${objs} index.html index.js index.wasm
