# Woodstock Web-Assembly Re-write
This branch contains the rewrite of the woodstock chess engine and AI in C++, to be compiled to web assembly for running in the browser.

# Dependencies
Naturally, a C++ compiler like `g++` will be necessary for testing, as well as `make` for ease of use. Woodstock uses `emscripten` to compiled the source to web assembly, so that will also be essential. For serving the web page during testing, Woodstock makes use of `lite-server` from `npm`, but any kind of local web server will do.