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

    static Bitboard get(int x, int y);
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
    static Bitboard board_to_bitboard(Piece * board, Piece filter, Piece accepted_value);

    static int bit_count(Bitboard bitboard);
    static void bitboard_to_positions(std::vector<int> & positions, Bitboard bitboard);
    static int bitboard_to_position(Bitboard bitboard);

    static int pop_lsb(Bitboard & bitboard);
    static int get_lsb(Bitboard bitboard);
    static bool contains_multiple_bits(Bitboard bitboard);
    
    // change these to static values
    static constexpr Bitboard A_FILE                                = 0b1000000010000000100000001000000010000000100000001000000010000000;
    static constexpr Bitboard H_FILE                                = 0b0000000100000001000000010000000100000001000000010000000100000001;
    static constexpr Bitboard AB_FILE                               = 0b1100000011000000110000001100000011000000110000001100000011000000;
    static constexpr Bitboard GH_FILE                               = 0b0000001100000011000000110000001100000011000000110000001100000011;
    static constexpr Bitboard NOT_A_FILE                            = 0b0111111101111111011111110111111101111111011111110111111101111111;
    static constexpr Bitboard NOT_H_FILE                            = 0b1111111011111110111111101111111011111110111111101111111011111110;
    static constexpr Bitboard NOT_AB_FILE                           = 0b0011111100111111001111110011111100111111001111110011111100111111;
    static constexpr Bitboard NOT_GH_FILE                           = 0b1111110011111100111111001111110011111100111111001111110011111100;
    static constexpr Bitboard ROW_1                                 = 0b11111111;
    static constexpr Bitboard ROW_8                                 = 0b1111111100000000000000000000000000000000000000000000000000000000;
    static constexpr Bitboard NOT_ROW_1                             = 0b1111111111111111111111111111111111111111111111111111111100000000;
    static constexpr Bitboard NOT_ROW_8                             = 0b0000000011111111111111111111111111111111111111111111111111111111;

    static constexpr Bitboard BLACK_KINGSIDE_CASTLE_SPACES          =      0b11000000000000000000000000000000000000000000000000000000000;
    static constexpr Bitboard BLACK_KINGSIDE_CASTLE_ATTACK_FREE     =     0b111000000000000000000000000000000000000000000000000000000000;
    static constexpr Bitboard BLACK_QUEENSIDE_CASTLE_SPACES         =  0b111000000000000000000000000000000000000000000000000000000000000;
    static constexpr Bitboard BLACK_QUEENSIDE_CASTLE_ATTACK_FREE    =   0b11100000000000000000000000000000000000000000000000000000000000;
    static constexpr Bitboard WHITE_KINGSIDE_CASTLE_SPACES          =     0b110;
    static constexpr Bitboard WHITE_KINGSIDE_CASTLE_ATTACK_FREE     =    0b1110;
    static constexpr Bitboard WHITE_QUEENSIDE_CASTLE_SPACES         = 0b1110000;
    static constexpr Bitboard WHITE_QUEENSIDE_CASTLE_ATTACK_FREE    =  0b111000;
};

#endif