CFILES=src/*.cpp
CMP_ARGS=-Wall -Wfatal-errors -std=c++20 -lstdc++ -flto -foptimize-sibling-calls -fomit-frame-pointer

G++_ARGS=-O3 -march=native
EMCC_ARGS=-O3 -s STACK_SIZE=128MB -s ASSERTIONS=2 --shell-file src/web/index.html -s MODULARIZE -s WASM=1 -s EXPORT_NAME="WoodstockModule" -s EXPORTED_RUNTIME_METHODS='["ccall", "stringToUTF8"]' -s EXPORTED_FUNCTIONS='["_malloc", "_free"]' -s TOTAL_MEMORY=512MB -s ALLOW_MEMORY_GROWTH

clean:
	rm -rf -f build/

build_web: clean ${CFILES}
	mkdir build
	emcc ${CMP_ARGS} ${EMCC_ARGS} ${CFILES} -o build/index.html
	cp -r src/web/public/* build/

build_binary: clean ${CFILES}
	mkdir build
	g++ ${CMP_ARGS} ${G++_ARGS} ${CFILES} -o build/woodstock.o

build_clang_binary: clean ${CFILES}
	mkdir build
	clang ${CMP_ARGS} ${G++_ARGS} ${CFILES} -o build/woodstock.o

serve: build/
	python -m http.server --directory ./build

run_binary: build/
	./build/woodstock.o

debug: build_binary run_binary
debug_clang: build_clang_binary run_binary
web: build_web serve
