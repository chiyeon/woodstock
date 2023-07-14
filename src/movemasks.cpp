#include "movemasks.h"
#include "bitboard.h"
#include <vector>

void MoveMasks::calculate_all_knight_moves(Bitboard * moveset)
{
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            int i = x + y * 8;

            Bitboard start = Bitboards::get(x, y);
            Bitboard movement = 0;

            movement |= (start >> 17)   & Bitboards::NOT_H_FILE;
            movement |= (start >> 15)   & Bitboards::NOT_A_FILE;
            movement |= (start >> 10)   & Bitboards::NOT_GH_FILE;
            movement |= (start >> 6)    & Bitboards::NOT_AB_FILE;
            movement |= (start << 17)   & Bitboards::NOT_A_FILE;
            movement |= (start << 15)   & Bitboards::NOT_H_FILE;
            movement |= (start << 10)   & Bitboards::NOT_AB_FILE;
            movement |= (start << 6)    & Bitboards::NOT_GH_FILE;

            moveset[i] = movement;
        }
    }
}

/*
 * the "naive" approach: using raycasting to 
 * generate all possible rook moves, accounting for all possible board
 * combinations & blocking
 */
void MoveMasks::generate_rook_moves_at_square_raycasting(int square, Bitboard * moveset)
{

}

/*
 * calculates bitboards for all possible rook positions. as it is 
 * used for blocking, does NOT include the last square in any particular direction
 */
void MoveMasks::calculate_all_rook_moves(Bitboard * moveset)
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

// first we need our "ground truth": get a database of all legal moves by [piece location] and [blockers]
// given a board moveset, we can claculate
Bitboard * MoveMasks::get_rook_blocker_moves(Bitboard moveset_mask)
{
    // int bit_count = Bitboards::bit_count(moveset_mask);
    // int num_moves = 1 << bit_count;
    // Bitboard blocker_moves = new Bitboard[num_moves];

    // for (int i = 0; i < num_moves; ++i) {
    //     for (int j = 0; j < bit_count; ++j) {
    //         Bitboard bit = (i >> j) & 1;
    //         blocker_moves[i] = bit << 
    //     }
    // }

    // for (int y = 0; y < 8; ++y) {
    //     for (int x = 0; x < 8; ++x) {
    //         int i = x + y * 8;
            
    //     }
    // }
}

void MoveMasks::calculate_all_bishop_moves(Bitboard * moveset)
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
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            int i = x + y * 8;
            moveset[i] = rook_moves[i] ^ bishop_moves[i];
        }
    }
}

void MoveMasks::calculate_all_king_moves(Bitboard * moveset)
{

    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            int i = x + y * 8;
            Bitboard movement = 0;
            Bitboard start = Bitboards::get(x, y);

            // cases for top mobility
            if (start & Bitboards::NOT_ROW_1) {
                movement |= (start << 8);
                movement |= (start << 9) & Bitboards::NOT_H_FILE;
                movement |= (start << 7) & Bitboards::NOT_A_FILE;
            }

            if (start & Bitboards::NOT_ROW_8) {
                movement |= (start >> 8);
                movement |= (start >> 9) & Bitboards::NOT_A_FILE;
                movement |= (start >> 7) & Bitboards::NOT_H_FILE;
            }

            movement |= (start >> 1) & Bitboards::NOT_A_FILE;
            movement |= (start << 1) & Bitboards::NOT_H_FILE;

            moveset[i] = movement;
        }
    }
}

MoveMasks::MoveMasks()
{
    calculate_all_knight_moves(knight_moves);
    calculate_all_rook_moves(rook_moves);
    calculate_all_bishop_moves(bishop_moves);
    calculate_all_queen_moves(queen_moves);     // calculate after bishop and rook...
    calculate_all_king_moves(king_moves);
}

Bitboard * MoveMasks::get_rook_moves()
{
    return rook_moves;
}

Bitboard * MoveMasks::get_bishop_moves()
{
    return bishop_moves;
}

Bitboard * MoveMasks::get_knight_moves()
{
    return knight_moves;
}

Bitboard * MoveMasks::get_queen_moves()
{
    return queen_moves;
}

Bitboard * MoveMasks::get_king_moves()
{
    return king_moves;
}