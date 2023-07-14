CFILES=src/*.cpp
CMP_ARGS=-Wall -Wfatal-errors -std=c++20 -march=native -fassociative-math -ffast-math -flto -fomit-frame-pointer

G++_ARGS=-Ofast
EMCC_ARGS=-O0

clean:
	rm -rf -f build/

build: clean ${CFILES}
	mkdir build
	emcc ${CMP_ARGS} ${EMCC_ARGS} ${CFILES} -o build/index.html

cbuild: clean ${CFILES}
	mkdir build
	g++ ${CMP_ARGS} ${G++_ARGS} ${CFILES} -o build/woodstock.o

serve: build/
	lite-server

test: build/
	./build/woodstock.o
