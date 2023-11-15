> **Woodstock is a work in progress! Expect issues.**

[You can play against Woodstock here.](https://chess.benjiwong.com)

# About
This branch contains the woodstock chess engine and AI in C++, to be compiled to web assembly for running in the browser.

Currently, Woodstock searches 

## Features
- Magic Bitboards for sliding pieces
- Playable as White/Black
- Minimaxing/Negamaxing/Negascouting with Alpha-Beta pruning
- Playable on Browser with WebAssembly
- Interactable Chessboard
- Load from FEN string
- PGN Export
- Different Mid/Endgame evaluations (WIP)
- Transposition Table with Zobrist Hashing (WIP)
- Iterative Deepening Search (WIP)
- Follows all Chess Rules (Only missing insufficient material & 50 move)
- UCI Compliant (WIP)
- Opening Book (TODO)

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
