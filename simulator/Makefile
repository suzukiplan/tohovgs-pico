CFLAGS = -O2 -I./src -I../src -I../include -I/usr/include/SDL2 -c
CPP = g++ -std=c++11 ${CFLAGS}
CC = gcc ${CFLAGS}

OBJECTS = vgssdk_sdl2.o\
	lz4.o\
	rom_bgm.o\
	rom_k8x12S_jisx0201.o\
	rom_k8x12S_jisx0208.o\
	rom_small_font.o\
	rom_songlist.o\
	vgstone.o\
	app.o

HEADERS = ../src/vgssdk.h\
	../src/vgsdecv.hpp\
	../src/lz4.h\
	../src/roms.hpp

all:
	cd .. && make roms
	make ${OBJECTS}
	@echo TODO: link objects

clean:
	rm -f *.o
	rm -f app

vgssdk_sdl2.o: src/vgssdk_sdl2.cpp ${HEADERS}
	${CPP} src/vgssdk_sdl2.cpp

lz4.o: ../src/lz4.c ${HEADERS}
	${CC} ../src/lz4.c

rom_bgm.o: ../src/rom_bgm.c ${HEADERS}
	${CC} ../src/rom_bgm.c

rom_k8x12S_jisx0201.o: ../src/rom_k8x12S_jisx0201.c ${HEADERS}
	${CC} ../src/rom_k8x12S_jisx0201.c

rom_k8x12S_jisx0208.o: ../src/rom_k8x12S_jisx0208.c ${HEADERS}
	${CC} ../src/rom_k8x12S_jisx0208.c

rom_small_font.o: ../src/rom_small_font.c ${HEADERS}
	${CC} ../src/rom_small_font.c

rom_songlist.o: ../src/rom_songlist.c ${HEADERS}
	${CC} ../src/rom_songlist.c

vgstone.o: ../src/vgstone.c ${HEADERS}
	${CC} ../src/vgstone.c

app.o: ../src/app.cpp ${HEADERS}
	${CPP} ../src/app.cpp