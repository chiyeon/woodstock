CFILES=src/*.cpp
CMP_ARGS=-Wall -Wfatal-errors

clean:
	rm -rf -f build/

build: clean ${CFILES}
	mkdir build
	emcc ${CFILES} -o build/index.html

cbuild: clean ${CFILES}
	mkdir build
	g++ ${CMP_ARGS} ${CFILES} -o build/woodstock.o

serve: build/
	lite-server

test: build/
	./build/woodstock.o
