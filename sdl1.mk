targets += s1

s1: CFLAGS += -std=c99 `sdl-config --cflags`
s1: LDLIBS += `sdl-config --libs`
s1: s1.c tetris.o val.o
