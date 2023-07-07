#ifndef BITBOARD_H
#define BITBOARD_H

#include <stdio.h>
#include <stdint.h>
#include "constants.h"

/*
 * Bitboards is the tool class with 
 * static helper functions
 */
struct Bitboards
{
    static void print(Bitboard bitboard);   // prints bitboard to terminal

    static Bitboard get(int x, int y);              // returns with bit at location
    static Bitboard get_i(int i);

    static Bitboard get_row(int y);
    static Bitboard get_row_segment(int y, int x1, int x2);
    static Bitboard get_column(int x);
    static Bitboard get_column_segment(int x, int y1, int y2);

    static Bitboard get_diagonal_downwards_right(int x, int y);     // returns bitboard from (x, y) (not inclusive)
    static Bitboard get_diagonal_downwards_left(int x, int y);      // in target direction
    static Bitboard get_diagonal_upwards_right(int x, int y);
    static Bitboard get_diagonal_upwards_left(int x, int y);

    static Bitboard board_to_bitboard(Piece * board);
    static int bit_count(Bitboard bitboard);
    static int * bitboard_to_positions(Bitboard bitboard);

    const static Bitboard diagonal_downwards_right_starters[];           // we can store the 4 diff diag dirs into arrays, fastest
    const static Bitboard diagonal_downwards_left_starters[];            // way of calculating them
    const static Bitboard diagonal_upwards_right_starters[];
    const static Bitboard diagonal_upwards_left_starters[];
};

#endif