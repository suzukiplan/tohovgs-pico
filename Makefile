all:
	pio run -t upload

build:
	pio run

format: 
	make execute-format FILENAME=./src/app.cpp
        
execute-format:
	clang-format -style=file < ${FILENAME} > ${FILENAME}.bak
	cat ${FILENAME}.bak > ${FILENAME}
	rm ${FILENAME}.bak
