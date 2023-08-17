ROMS = src/rom_small_font.c\
	src/rom_k8x12S_jisx0201.c\
	src/rom_k8x12S_jisx0208.c

all: roms
	pio run -t upload

build: roms
	pio run

roms: tools/bmp2img/bmp2img tools/varext/varext
	make include/roms.hpp

tools/bmp2img/bmp2img: tools/bmp2img/bmp2img.cpp
	cd tools/bmp2img && make

tools/varext/varext: tools/varext/varext.cpp
	cd tools/varext && make

src/rom_small_font.c: rom/small_font.bmp
	tools/bmp2img/bmp2img -t 4x8 rom/small_font.bmp >src/rom_small_font.c

src/rom_k8x12S_jisx0201.c: rom/k8x12S_jisx0201.bmp
	tools/bmp2img/bmp2img -t 4x12 -m rom/k8x12S_jisx0201.bmp >src/rom_k8x12S_jisx0201.c

src/rom_k8x12S_jisx0208.c: rom/k8x12S_jisx0208.bmp
	tools/bmp2img/bmp2img -t 8x12 -m rom/k8x12S_jisx0208.bmp >src/rom_k8x12S_jisx0208.c

include/roms.hpp: ${ROMS}
	tools/varext/varext ${ROMS} >include/roms.hpp

format: 
	make execute-format FILENAME=./src/app.cpp
        
execute-format:
	clang-format -style=file < ${FILENAME} > ${FILENAME}.bak
	cat ${FILENAME}.bak > ${FILENAME}
	rm ${FILENAME}.bak
