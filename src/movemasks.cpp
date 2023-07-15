#include "movemasks.h"
#include "bitboard.h"
#include "constants.h"
#include <random>
// #include <time.h>
#include <vector>

void MoveMasks::calculate_knight_moves(Bitboard * moveset)
{
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            int i = x + y * 8;

            Bitboard start = Bitboards::get(x, y);
            Bitboard movement = 0;

            movement |= (start >> 17)   & Bitboards::NOT_A_FILE;
            movement |= (start >> 15)   & Bitboards::NOT_H_FILE;
            movement |= (start >> 10)   & Bitboards::NOT_AB_FILE;
            movement |= (start >> 6)    & Bitboards::NOT_GH_FILE;
            movement |= (start << 17)   & Bitboards::NOT_H_FILE;
            movement |= (start << 15)   & Bitboards::NOT_A_FILE;
            movement |= (start << 10)   & Bitboards::NOT_GH_FILE;
            movement |= (start << 6)    & Bitboards::NOT_AB_FILE;

            moveset[i] = movement;
        }
    }
}

/*
 * calculates bitboards for all possible rook positions. as it is 
 * used for blocking, does NOT include the last square in any particular direction
 */
void MoveMasks::calculate_rook_masks(Bitboard * moveset)
{
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            int i = x + y * 8;
            moveset[i]
                = Bitboards::get_row(y) 
                ^ Bitboards::get_column(x);
            
            if (x != 0) moveset[i] &= ~Bitboards::get_column(0);
            if (x != 7) moveset[i] &= ~Bitboards::get_column(7);
            if (y != 0) moveset[i] &= ~Bitboards::get_row(0);
            if (y != 7) moveset[i] &= ~Bitboards::get_row(7);
        }
    }
}

void MoveMasks::calculate_bishop_masks(Bitboard * moveset)
{
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            int i = x + y * 8;

            moveset[i]
                = Bitboards::get_diagonal_downwards_right(x, y)
                | Bitboards::get_diagonal_downwards_left(x, y)
                | Bitboards::get_diagonal_upwards_right(x, y)
                ^ Bitboards::get_diagonal_upwards_left(x, y);
        }
    }
}

void MoveMasks::calculate_all_queen_moves(Bitboard * moveset)
{
}

void MoveMasks::calculate_king_moves(Bitboard * moveset)
{
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            int i = x + y * 8;
            Bitboard movement = 0;
            Bitboard start = Bitboards::get(x, y);

            movement |= (start >> 8);                           // south
            movement |= (start >> 9) & Bitboards::NOT_A_FILE;   // south east
            movement |= (start >> 7) & Bitboards::NOT_H_FILE;   // south west
            movement |= (start >> 1) & Bitboards::NOT_A_FILE;   // east
            movement |= (start << 1) & Bitboards::NOT_H_FILE;   // west
            movement |= (start << 7) & Bitboards::NOT_A_FILE;   // north east
            movement |= (start << 9) & Bitboards::NOT_H_FILE;   // north west
            movement |= (start << 8);                           // north

            moveset[i] = movement;
        }
    }
}

MoveMasks::MoveMasks()
    : rook_index_bits{
        12, 11, 11, 11, 11, 11, 11, 12,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        12, 11, 11, 11, 11, 11, 11, 12
    }
    , bishop_index_bits{
        6, 5, 5, 5, 5, 5, 5, 6,
        5, 5, 5, 5, 5, 5, 5, 5,
        5, 5, 7, 7, 7, 7, 5, 5,
        5, 5, 7, 9, 9, 7, 5, 5,
        5, 5, 7, 9, 9, 7, 5, 5,
        5, 5, 7, 7, 7, 7, 5, 5,
        5, 5, 5, 5, 5, 5, 5, 5,
        6, 5, 5, 5, 5, 5, 5, 6
    }
{
    // srand(time(NULL));

    calculate_knight_moves(knight_moves);
    // calculate_rook_masks(rook_masks);
    // calculate_bishop_masks(bishop_masks);
    //calculate_all_queen_moves(queen_moves);     // calculate after bishop and rook...
    calculate_king_moves(king_moves);
}

Bitboard * MoveMasks::get_knight_moves()
{
    return knight_moves;
}

Bitboard * MoveMasks::get_queen_moves()
{
    return {};
}

Bitboard * MoveMasks::get_king_moves()
{
    return king_moves;
}

Magic MoveMasks::get_random_magic()
{
    Magic u1, u2, u3, u4;
    u1 = (Magic)(rand() & 0xFFFF);
    u2 = (Magic)(rand() & 0xFFFF);
    u3 = (Magic)(rand() & 0xFFFF);
    u4 = (Magic)(rand() & 0xFFFF);

    return u1 | (u2 << 16) | (u3 << 32) | (u4 << 48);
}

Magic MoveMasks::get_random_small_magic()
{
    // 3x & to get smaller numbers / less bits
    return get_random_magic() & get_random_magic() & get_random_magic();
}

Magic MoveMasks::find_magics(int pos)
{
    /* psuedocode: 
     * 
     * find the mask, then calculate all possible blocker combinations
     * for that mask
     * 
     * then for each blocker combination calculate the proper positions with raycasting
     * then, use that to determine the magics by brute forcing
     *
     * */
}

Bitboard MoveMasks::get_bishop_move(Bitboard occupation, int pos)
{
    occupation &= bishop_table[pos].mask;
    occupation += bishop_table[pos].magic;
    occupation >> 64 - 9;
    return bishop_moves[pos][occupation];
}