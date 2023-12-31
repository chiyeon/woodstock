#ifndef MOVEMASKS_H
#define MOVEMASKS_H

#include "bitboard.h"
#include "constants.h"
#include <map>

struct MagicEntry {
   Bitboard *ptr;
   Bitboard mask;
   Magic magic;
   int shift;
};

/*
 * moves = anything where moves are precalculated
 * mask = potential masks before magics, used to find blocking spaces
 */
class MoveMasks {
   /* precalculated moves for 'jumping' pieces */
   Bitboard knight_moves[64];
   Bitboard king_moves[64];

   /* storage of actual moves */
   Bitboard rook_moves[64][4096];
   Bitboard bishop_moves[64][512];

   MagicEntry rook_magic_table[64];
   MagicEntry bishop_magic_table[64];

   Magic get_random_magic();
   Magic get_random_small_magic();

   void initialize_magic_table();
   Magic find_magic(int pos, Piece piece_type);
   void find_all_magics();
   // void load_precalculated_magics();

   Bitboard add_one_with_masked_bits(Bitboard current, Bitboard mask);
   std::vector<Bitboard> get_all_blocker_combinations(Bitboard blocker);

   void calculate_knight_moves(Bitboard *moveset);
   void calculate_king_moves(Bitboard *moveset);

   void calculate_rook_masks();
   void calculate_bishop_masks();

 public:
   MoveMasks();
   MoveMasks(MoveMasks &t) = delete;
   Bitboard get_knight_move(int pos);
   Bitboard get_king_move(int pos);
   Bitboard get_bishop_move(Bitboard occupation, int pos);
   Bitboard get_rook_move(Bitboard occupation, int pos);

   void test();
};

#endif
