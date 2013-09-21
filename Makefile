CFLAGS  := -Wall -ansi

targets :=
objs    := val.o tetris.o

all:

variants := gtk3 sdl2
include $(patsubst %,%.mk,$(variants))

all: ${targets}


clean:
	${RM} ${targets} ${objs}
