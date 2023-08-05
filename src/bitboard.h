#ifndef BITBOARD_H
#define BITBOARD_H

#include <stdio.h>
#include <stdint.h>
#include <vector>
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

    static Bitboard get_between(int x1, int y1, int x2, int y2);
    static Bitboard get_between(int x1, int y1, int x2, int y2, Piece piece);

    static Bitboard board_to_bitboard(Piece * board);
    static Bitboard board_to_bitboard(Piece * board, Piece color);
    static int bit_count(Bitboard bitboard);
    static void bitboard_to_positions(std::vector<int> & positions, Bitboard bitboard);
    static int bitboard_to_position(Bitboard bitboard);

    static int pop_lsb(Bitboard & bitboard);
    static bool contains_multiple_bits(Bitboard bitboard);
    
    const static Bitboard A_FILE;
    const static Bitboard H_FILE;
    const static Bitboard AB_FILE;
    const static Bitboard GH_FILE;
    const static Bitboard NOT_A_FILE;
    const static Bitboard NOT_H_FILE;
    const static Bitboard NOT_AB_FILE;
    const static Bitboard NOT_GH_FILE;
    const static Bitboard ROW_1;
    const static Bitboard ROW_8;
    const static Bitboard NOT_ROW_1;
    const static Bitboard NOT_ROW_8;

    const static Bitboard BLACK_KINGSIDE_CASTLE_SPACES          =     0b11000000000000000000000000000000000000000000000000000000000;
    const static Bitboard BLACK_KINGSIDE_CASTLE_ATTACK_FREE     =    0b111000000000000000000000000000000000000000000000000000000000;
    const static Bitboard BLACK_QUEENSIDE_CASTLE_SPACES         = 0b111000000000000000000000000000000000000000000000000000000000000;
    const static Bitboard BLACK_QUEENSIDE_CASTLE_ATTACK_FREE    =  0b11100000000000000000000000000000000000000000000000000000000000;
    const static Bitboard WHITE_KINGSIDE_CASTLE_SPACES          =     0b110;
    const static Bitboard WHITE_KINGSIDE_CASTLE_ATTACK_FREE     =    0b1110;
    const static Bitboard WHITE_QUEENSIDE_CASTLE_SPACES         = 0b1110000;
    const static Bitboard WHITE_QUEENSIDE_CASTLE_ATTACK_FREE    =  0b111000;

private:
    static int get_most_significant_bit_position(Bitboard bitboard);
};

#endif