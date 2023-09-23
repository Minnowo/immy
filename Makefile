

CFLAGS=-lImlib2 -lX11

SRC_FILES=doko.c window.c image.c options.c

all:

	cc ${SRC_FILES} ${CFLAGS}


clean:
	rm a.out