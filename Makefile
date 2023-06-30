clean:
	rm -rf -f build/

build: clean src/woodstock.cpp
	mkdir build
	emcc src/woodstock.cpp -o build/index.html

cbuild: clean src/woodstock.cpp
	mkdir build
	g++ src/woodstock.cpp -o build/woodstock.o

serve: build/
	lite-server

test: build/
	./build/woodstock.o
