targets += st

st: CFLAGS += -std=c99 `sdl2-config --cflags`
st: LDLIBS += `sdl2-config --libs`
st: st.c tetris.o val.o
