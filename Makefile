

CFLAGS=-lImlib2 -lX11

SRC_FILES=doko.c window.c image.c options.c util.c
OUT_FILE=doko

all:

	cc ${SRC_FILES} -o${OUT_FILE} ${CFLAGS}

clean:
	rm ${OUT_FILE} 