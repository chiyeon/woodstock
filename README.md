> **Woodstock is a work in progress! Expect issues.**

# About
This branch contains the woodstock chess engine and AI in C++, to be compiled to web assembly for running in the browser.

### Dependencies
- `make`
- `g++` or `clang`
- `emscripten` for compiling to web assembly
- `python` for debug web page serving

### Features
- Magic Bitboards for sliding pieces
- Minimaxing/Negamaxing/Negascouting with Alpha-Beta pruning
- Playable on Browser with WebAssembly
- Load from FEN string
- PGN Export
- Transposition Table with Zobrist Hashing (WIP)
- Iterative Deepening Search (WIP)
- UCI Compliant (WIP)
