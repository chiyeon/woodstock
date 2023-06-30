CFILES=src/bitboard.cpp src/woodstock.cpp

clean:
	rm -rf -f build/

build: clean ${CFILES}
	mkdir build
	emcc ${CFILES} -o build/index.html

cbuild: clean ${CFILES}
	mkdir build
	g++ -Wall ${CFILES} -o build/woodstock.o

serve: build/
	lite-server

test: build/
	./build/woodstock.o
