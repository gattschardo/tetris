CFLAGS+=-ansi `pkg-config --cflags gtk+-3.0`
LDLIBS+=`pkg-config --libs   gtk+-3.0`

targets:=tetris

all: ${targets}

tetris: tetris.c val.o

clean:
	${RM} ${targets}
