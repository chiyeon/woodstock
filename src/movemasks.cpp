#include "movemasks.h"
#include "bitboard.h"

void MoveMasks::calculate_all_rook_moves(Bitboard * moveset)
{
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            int i = x + y * 8;
            moveset[i] = 
                Bitboards::get_row(y) 
                ^ Bitboards::get_column(x);
        }
    }
}

void MoveMasks::calculate_all_bishop_moves(Bitboard * moveset)
{
    // for (int y = 0; y < 8; y++) {
    //     for (int x = 0; x < 8; x++) {
    //         int i = x + y * 8;
    //     }
    // }
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