CFLAGS  := -Wall -ansi

targets :=
objs    :=

all:

variants := gtk3 sdl2
include $(patsubst %,%.mk,$(variants))

all: ${targets}


clean:
	${RM} ${targets} ${objs}
