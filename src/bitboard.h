#include <stdio.h>

// represent bitboards as 64 bit unsigned integers
using Bitboard = unsigned long;

/*
 * Bitboards is the tool class with 
 * static helper functions
 */
struct Bitboards
{
    static void print(Bitboard bitboard);   // prints bitboard to terminal

    static Bitboard get(int x, int y);              // returns with bit at location
    static Bitboard get_i(int i);
};

