#include "movemasks.h"
#include "bitboard.h"
#include <vector>

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
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
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

void MoveMasks::

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
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            int i = x + y * 8;

            moveset[i]
                = Bitboards::get_diagonal_downwards_right(x, y)
                | Bitboards::get_diagonal_downwards_left(x, y)
                | Bitboards::get_diagonal_upwards_right(x, y)
                ^ Bitboards::get_diagonal_upwards_left(x, y);
        }
    }
}

MoveMasks::MoveMasks()
{
    calculate_all_rook_moves(rook_moves);
    calculate_all_bishop_moves(bishop_moves);
}

Bitboard * MoveMasks::get_rook_moves()
{
    return rook_moves;
}

Bitboard * MoveMasks::get_bishop_moves()
{
    return bishop_moves;
}