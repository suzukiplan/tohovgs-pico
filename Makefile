ROMS = src/rom_small_font.c\
	src/rom_songlist.c\
	src/rom_bgm.c\
	src/rom_button_loop1.c\
	src/rom_button_infinity.c\
	src/rom_button_volume.c\
	src/rom_button_swipe_on.c\
	src/rom_button_swipe_off.c\
	src/rom_button_sequencial.c\
	src/rom_button_random.c\
	src/rom_icon_pause.c\
	src/rom_icon_play.c\
	src/rom_scroll_bar.c\
	src/rom_scroll_end.c\
	src/rom_k8x12S_jisx0201.c\
	src/rom_k8x12S_jisx0208.c

all: roms
	make all-`uname`

all-Darwin:
	pio run -t upload

all-Linux:
	pio run -t upload --upload-port=/dev/ttyACM0

build: roms
	pio run

clean:
	rm -f ./src/rom_*.c
	rm -f ./rom/bgm.dat
	rm -f ./rom/songlist_sjis.dat
	rm -f ./rom/songlist_utf8.dat
	rm -f ./rom/songlist.bin
	rm -f ./rom/*.bgm
	rm -f ./BGM.mak
	rm -f ./tools/bin2var/bin2var
	rm -f ./tools/bmp2img/bmp2img
	rm -f ./tools/bplay/bplay
	rm -f ./tools/sldcnv/sldcnv
	rm -f ./tools/sldmak/sldmak
	rm -f ./tools/sljcnv/sljcnv
	rm -f ./tools/varext/varext
	rm -f ./tools/vgsftv/vgsftv
	rm -f ./tools/vgslz4/vgslz4
	rm -f ./tools/vgsmml/vgsmml

roms:
	cd tools/bmp2img && make
	cd tools/bin2var && make
	cd tools/varext && make
	cd tools/sljcnv && make
	cd tools/sldcnv && make
	cd tools/sldmak && make
	cd tools/vgsmml && make
	cd tools/vgsftv && make
	cd tools/vgslz4 && make
	make rom/songlist_utf8.dat
	tools/sldmak/sldmak rom/songlist_utf8.dat >BGM.mak
	make -f BGM.mak all
	make rom/songlist_sjis.dat
	make rom/songlist.bin
	make src/roms.hpp

src/rom_small_font.c: rom/small_font.bmp
	tools/bmp2img/bmp2img -t 4x8 rom/small_font.bmp >src/rom_small_font.c

src/rom_k8x12S_jisx0201.c: rom/k8x12S_jisx0201.bmp
	tools/bmp2img/bmp2img -t 4x12 -m rom/k8x12S_jisx0201.bmp >src/rom_k8x12S_jisx0201.c

src/rom_k8x12S_jisx0208.c: rom/k8x12S_jisx0208.bmp
	tools/bmp2img/bmp2img -t 8x12 -m rom/k8x12S_jisx0208.bmp >src/rom_k8x12S_jisx0208.c

src/rom_button_loop1.c: rom/button_loop1.bmp
	tools/bmp2img/bmp2img -t 32x24 rom/button_loop1.bmp >src/rom_button_loop1.c

src/rom_button_infinity.c: rom/button_infinity.bmp
	tools/bmp2img/bmp2img -t 32x24 rom/button_infinity.bmp >src/rom_button_infinity.c

src/rom_button_volume.c: rom/button_volume.bmp
	tools/bmp2img/bmp2img -t 32x24 rom/button_volume.bmp >src/rom_button_volume.c

src/rom_button_swipe_on.c: rom/button_swipe_on.bmp
	tools/bmp2img/bmp2img -t 32x24 rom/button_swipe_on.bmp >src/rom_button_swipe_on.c

src/rom_button_swipe_off.c: rom/button_swipe_off.bmp
	tools/bmp2img/bmp2img -t 32x24 rom/button_swipe_off.bmp >src/rom_button_swipe_off.c

src/rom_button_sequencial.c: rom/button_sequencial.bmp
	tools/bmp2img/bmp2img -t 32x24 rom/button_sequencial.bmp >src/rom_button_sequencial.c

src/rom_button_random.c: rom/button_random.bmp
	tools/bmp2img/bmp2img -t 32x24 rom/button_random.bmp >src/rom_button_random.c

src/rom_icon_pause.c: rom/icon_pause.bmp
	tools/bmp2img/bmp2img -t 16x16 rom/icon_pause.bmp >src/rom_icon_pause.c

src/rom_icon_play.c: rom/icon_play.bmp
	tools/bmp2img/bmp2img -t 16x16 rom/icon_play.bmp >src/rom_icon_play.c

src/rom_scroll_bar.c: rom/scroll_bar.bmp
	tools/bmp2img/bmp2img -t 8x24 rom/scroll_bar.bmp >src/rom_scroll_bar.c

src/rom_scroll_end.c: rom/scroll_end.bmp
	tools/bmp2img/bmp2img -t 8x8 rom/scroll_end.bmp >src/rom_scroll_end.c

rom/songlist_utf8.dat: rom/songlist.json
	tools/sljcnv/sljcnv rom/songlist.json >rom/songlist_utf8.dat

rom/songlist_sjis.dat: rom/songlist_utf8.dat
	iconv -f utf8 -t sjis <rom/songlist_utf8.dat >rom/songlist_sjis.dat

rom/songlist.bin: rom/songlist_sjis.dat
	tools/sldcnv/sldcnv rom/songlist_sjis.dat rom/songlist.bin

src/rom_songlist.c: rom/songlist.bin
	tools/bin2var/bin2var rom/songlist.bin >src/rom_songlist.c

src/rom_bgm.c: rom/bgm.dat
	tools/bin2var/bin2var rom/bgm.dat >src/rom_bgm.c

src/roms.hpp: ${ROMS}
	tools/varext/varext ${ROMS} >src/roms.hpp

format: 
	make execute-format FILENAME=./src/app.cpp
	make execute-format FILENAME=./src/vgstone.c
	make execute-format FILENAME=./src/vgsdecv.hpp
	make execute-format FILENAME=./src/model.h
	make execute-format FILENAME=./src/vgssdk_pico.cpp
	make execute-format FILENAME=./src/vgssdk.h
	make execute-format FILENAME=./simulator/src/vgssdk_sdl2.cpp

execute-format:
	clang-format -style=file < ${FILENAME} > ${FILENAME}.bak
	cat ${FILENAME}.bak > ${FILENAME}
	rm ${FILENAME}.bak
