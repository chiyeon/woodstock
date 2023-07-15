#ifndef MOVEMASKS_H
#define MOVEMASKS_H

#include <map>
#include "constants.h"
#include "bitboard.h"

struct MagicEntry
{
    Bitboard mask;
    Magic magic;
    int shift;
};

/*
 * moves = anything where moves are precalculated
 * mask = potential masks before magics, used to find blocking spaces
 */
class MoveMasks
{
    /* precalculated moves for 'jumping' pieces */
    Bitboard knight_moves[64];
    Bitboard king_moves[64];

    /* storage of actual moves */
    Bitboard rook_moves[64][4096];
    Bitboard bishop_moves[64][512];

    MagicEntry rook_magic_table[64];
    MagicEntry bishop_magic_table[64];

    // const std::map<std::pair<MagicEntry, int>, int> ROOK_MAGIC;
    // const std::map<std::pair<MagicEntry, int>, int> BISHOP_MAGIC;

    Magic get_random_magic();
    Magic get_random_small_magic();

    void populate_magic_shift_bits();
    Magic find_magics(int pos, Piece piece_type);
    void find_all_magics();

    Bitboard add_one_with_masked_bits(Bitboard current, Bitboard mask);
    std::vector<Bitboard> get_all_blocker_combinations(Bitboard blocker);
    
    void calculate_knight_moves(Bitboard * moveset);
    void calculate_king_moves(Bitboard * moveset);

    void calculate_rook_masks();
    void calculate_bishop_masks();

public:
    MoveMasks();
    MoveMasks(MoveMasks & t) = delete;
    Bitboard * get_knight_moves();
    Bitboard * get_queen_moves();
    Bitboard * get_king_moves();
    Bitboard get_bishop_move(Bitboard occupation, int pos);
    Bitboard get_rook_move(Bitboard occupation, int pos);

    void test();
};

#endif