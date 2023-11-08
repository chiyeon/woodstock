CFILES=src/*.cpp
CMP_ARGS=-Wall -Wunused-function -Wfatal-errors -std=c++20 -lstdc++ -flto -foptimize-sibling-calls -fomit-frame-pointer

G++_ARGS=-O3 -march=native
EMCC_ARGS=-O3 -s STACK_SIZE=128MB -s ASSERTIONS=2 --shell-file src/web/index.html -s MODULARIZE -s WASM=1 -s EXPORT_NAME="WoodstockModule" -s EXPORTED_RUNTIME_METHODS='["ccall", "stringToUTF8", "UTF8ToString"]' -s EXPORTED_FUNCTIONS='["_malloc", "_free"]' -s TOTAL_MEMORY=512MB -s ALLOW_MEMORY_GROWTH

target=web

web_build=		emcc ${CMP_ARGS} ${EMCC_ARGS} ${CFILES} -o build/index.html; cp -r src/web/public/* build/
g++_build=		g++ ${CMP_ARGS} ${G++_ARGS} ${CFILES} -o build/woodstock.o
clang_build=	clang ${CMP_ARGS} ${G++_ARGS} ${CFILES} -o build/woodstock.o

web_run=			serve
g++_run=			run
clang_run=		run

clean:
	rm -rf -f build/

format:
	clang-format -i src/*

build: clean ${CFILES}
	mkdir build
	${${target}_build}

run: build/
	./build/woodstock.o

serve: build/
	python -m http.server --directory ./build

debug: build ${${target}_run}
