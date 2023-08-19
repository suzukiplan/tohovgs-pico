all: vgsmml

vgsmml: src/vgsmml.c src/vgsmml.h src/cli.c ../../src/miniz.c
	clang -o vgsmml -DMINIZ_NO_STDIO -DMINIZ_NO_TIME -I./src -I../../include src/vgsmml.c src/cli.c ../../src/miniz.c
