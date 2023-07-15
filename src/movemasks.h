#ifndef MOVEMASKS_H
#define MOVEMASKS_H

#include <map>
#include "constants.h"
#include "bitboard.h"

struct MagicEntry
{
    Bitboard mask;
    Magic magic;
};

/*
 * moves = anything where moves are precalculated
 * mask = potential masks before magics, used to find blocking spaces
 */
class MoveMasks
{
    Bitboard knight_moves[64];
    Bitboard queen_moves[64];
    Bitboard king_moves[64];

    Bitboard rook_masks[64];
    Bitboard bishop_masks[64];

    Bitboard rook_moves[64][4096];
    Bitboard bishop_moves[64][512];

    int rook_index_bits[64];
    int bishop_index_bits[64];

    MagicEntry rook_table[64];
    MagicEntry bishop_table[64];

    // const std::map<std::pair<MagicEntry, int>, int> ROOK_MAGIC;
    // const std::map<std::pair<MagicEntry, int>, int> BISHOP_MAGIC;

    Magic get_random_magic();
    Magic get_random_small_magic();
    Magic find_magics(int pos);
    
    void calculate_knight_moves(Bitboard * moveset);
    void calculate_king_moves(Bitboard * moveset);

    void calculate_rook_masks(Bitboard * moveset);
    void calculate_bishop_masks(Bitboard * moveset);
    
    void calculate_all_queen_moves(Bitboard * moveset);

public:
    MoveMasks();
    MoveMasks(MoveMasks & t) = delete;
    Bitboard * get_knight_moves();
    Bitboard * get_queen_moves();
    Bitboard * get_king_moves();
    Bitboard get_bishop_move(Bitboard occupation, int pos);
    Bitboard get_rook_move(Bitboard occupation, int pos);
};

#endif