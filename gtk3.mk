targets += gt
objs    += val.o

gt: CFLAGS+=`pkg-config --cflags gtk+-3.0`
gt: LDLIBS+=`pkg-config --libs   gtk+-3.0`
gt: gt.c val.o
