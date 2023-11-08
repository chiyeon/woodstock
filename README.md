> **Woodstock is a work in progress! Expect issues.**

# About
This branch contains the woodstock chess engine and AI in C++, to be compiled to web assembly for running in the browser.

## Features
- Magic Bitboards for sliding pieces
- Minimaxing/Negamaxing/Negascouting with Alpha-Beta pruning
- Playable on Browser with WebAssembly
- Load from FEN string
- PGN Export
- Transposition Table with Zobrist Hashing (WIP)
- Iterative Deepening Search (WIP)
- UCI Compliant (WIP)

## Dependencies
- `make`
- `g++` or `clang`
- `clang-format` for linting
- `emscripten` for compiling to web assembly
- `python` for debug web page serving

## Make Commands
### General
- `clean`: Removes the `build/` folder
- `format`: Formats files based on LLVM style

### Testing
The following make command can take an optional argument `target`, which can be `web`, `g++`, or `clang`. The default is `web`.
- `debug`: Compiles and runs based `target`

Example with `target`:
```console
make debug target=clang
```

### Compilation
The following make command can take an optional argument `target`, which can be `web`, `g++`, or `clang`. The default is `web`.
- `build`: Builds `target` to `build/`.
   - Compiles with `emcc` for `web`
   - Compiles with `g++` for `g++`
   - Compiles with `clang` for `clang`

### Running
- `run`: Runs binary if it exists in `build/`
- `serve` Uses `python` to serve to `localhost:8000`
