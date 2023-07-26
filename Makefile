CFILES=src/*.cpp
CMP_ARGS=-Wall -Wfatal-errors -std=c++2a -flto -fassociative-math -ffast-math -fomit-frame-pointer -Wunused-variable -Wsign-compare

G++_ARGS=-O3 -march=native
EMCC_ARGS=-Os -s TOTAL_STACK=4MB -s ASSERTIONS=2 --shell-file src/web/index.html -s MODULARIZE -s WASM=1 -s EXPORT_NAME="WoodstockModule" -s EXPORTED_RUNTIME_METHODS='["ccall"]'

clean:
	rm -rf -f build/

build: clean ${CFILES}
	mkdir build
	emcc ${CMP_ARGS} ${EMCC_ARGS} ${CFILES} -o build/index.html
	cp -r src/web/public/* build/

cbuild: clean ${CFILES}
	mkdir build
	g++ ${CMP_ARGS} ${G++_ARGS} ${CFILES} -o build/woodstock.o

serve: build/
	lite-server

test: build/
	./build/woodstock.o
