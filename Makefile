CFILES=src/*.cpp
CMP_ARGS=-Wall -Wfatal-errors -std=c++20 -fassociative-math -ffast-math -flto -fomit-frame-pointer

G++_ARGS=-Ofast -march=native
EMCC_ARGS=-Os -s TOTAL_STACK=4MB -s ASSERTIONS=2

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
