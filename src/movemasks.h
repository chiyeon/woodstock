#ifndef MOVEMASKS_H
#define MOVEMASKS_H

#include "bitboard.h"

class MoveMasks
{
    Bitboard knight_moves[64];
    Bitboard rook_moves[64];
    Bitboard bishop_moves[64];

    void generate_rook_moves_at_square_raycasting(int square, Bitboard * moveset);
    
    void calculate_all_knight_moves(Bitboard * moveset);

    void calculate_all_rook_moves(Bitboard * moveset);
    Bitboard * get_rook_blocker_moves(Bitboard moveset_mask);

    void calculate_all_bishop_moves(Bitboard * moveset);

public:
    MoveMasks();
    Bitboard * get_knight_moves();
    Bitboard * get_rook_moves();
    Bitboard * get_bishop_moves();
};

#endif