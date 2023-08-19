ROMS = src/rom_small_font.c\
	src/rom_songlist.c\
	src/rom_bgm.c\
	src/rom_k8x12S_jisx0201.c\
	src/rom_k8x12S_jisx0208.c

all: roms
	pio run -t upload

build: roms
	pio run

roms: tools/bmp2img/bmp2img tools/varext/varext tools/bin2var/bin2var tools/sldmak/sldmak
	make rom/songlist_utf8.dat
	tools/sldmak/sldmak rom/songlist_utf8.dat >BGM.mak
	make -f BGM.mak all
	make rom/songlist_sjis.dat
	make rom/songlist.bin
	make include/roms.hpp

tools/bmp2img/bmp2img: tools/bmp2img/bmp2img.cpp
	cd tools/bmp2img && make

tools/bin2var/bin2var: tools/bin2var/bin2var.cpp
	cd tools/bin2var && make

tools/varext/varext: tools/varext/varext.cpp
	cd tools/varext && make

tools/sljcnv/sljcnv: tools/sljcnv/sljcnv.cpp
	cd tools/sljcnv && make

tools/sldcnv/sldcnv: tools/sldcnv/sldcnv.cpp
	cd tools/sldcnv && make

tools/sldmak/sldmak: tools/sldmak/sldmak.cpp
	cd tools/sldmak && make

src/rom_small_font.c: rom/small_font.bmp
	tools/bmp2img/bmp2img -t 4x8 rom/small_font.bmp >src/rom_small_font.c

src/rom_k8x12S_jisx0201.c: rom/k8x12S_jisx0201.bmp
	tools/bmp2img/bmp2img -t 4x12 -m rom/k8x12S_jisx0201.bmp >src/rom_k8x12S_jisx0201.c

src/rom_k8x12S_jisx0208.c: rom/k8x12S_jisx0208.bmp
	tools/bmp2img/bmp2img -t 8x12 -m rom/k8x12S_jisx0208.bmp >src/rom_k8x12S_jisx0208.c

rom/songlist_utf8.dat: rom/songlist.json tools/sljcnv/sljcnv
	tools/sljcnv/sljcnv rom/songlist.json >rom/songlist_utf8.dat

rom/songlist_sjis.dat: rom/songlist_utf8.dat
	iconv -f utf8 -t sjis <rom/songlist_utf8.dat >rom/songlist_sjis.dat

rom/songlist.bin: rom/songlist_sjis.dat tools/sldcnv/sldcnv
	tools/sldcnv/sldcnv rom/songlist_sjis.dat rom/songlist.bin

src/rom_songlist.c: rom/songlist.bin
	tools/bin2var/bin2var rom/songlist.bin >src/rom_songlist.c

src/rom_bgm.c: rom/bgm.dat
	tools/bin2var/bin2var rom/bgm.dat >src/rom_bgm.c

include/roms.hpp: ${ROMS}
	tools/varext/varext ${ROMS} >include/roms.hpp

format: 
	make execute-format FILENAME=./src/app.cpp
        
execute-format:
	clang-format -style=file < ${FILENAME} > ${FILENAME}.bak
	cat ${FILENAME}.bak > ${FILENAME}
	rm ${FILENAME}.bak
