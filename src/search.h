#ifndef SEARCH_H
#define SEARCH_H

#include "game.h"
#include "zobrist.h"

class Search
{
   // friend struct MoveComparator;

   constexpr static int piece_values[7] = {
      0,
      10,
      30,
      32,
      45,
      90,
      2000
   };

   constexpr static int MVV_LVA_table[7][8] = {
      {0, 0, 0, 0, 0, 0, },
      {0, 15, 14, 13, 12, 11, 10}, // victim P, attacker None, P, N, B, R, Q, K
      {0, 25, 24, 23, 22, 21, 20}, // victim N, attacker None, P, N, B, R, Q, K
      {0, 35, 34, 33, 32, 31, 30}, // victim B, attacker None, P, N, B, R, Q, K
      {0, 45, 44, 43, 42, 41, 40}, // victim R, attacker None, P, N, B, R, Q, K
      {0, 55, 54, 53, 52, 51, 50}, // victim Q, attacker None, P, N, B, R, Q, K
      {0, 0, 0, 0, 0, 0, 0},       // victim K, attacker None, P, N, B, R, Q, K
   };

   constexpr static float black_pawn_table[64] = {
      0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
      5.0,  6.0,  5.0,  5.0,  5.0,  6.0,  5.0,  5.0,
      1.0,  1.0,  2.0,  3.0,  3.0,  2.0,  1.0,  1.0,
      0.5,  0.5,  1.0,  2.5,  2.5,  1.0,  0.5,  0.5,
      0.0,  0.0,  0.0,  2.0,  2.0,  0.0,  0.0,  0.0,
      0.5, -0.5, -1.0,  0.0,  0.0, -1.0, -0.5,  0.5,
      -2,  1.0, -1.0,  -3.0, -2.0,  1.0,  1.0,  -1,
      0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0
   };

   constexpr static float black_knight_table[64] = {
      -5.0, -4.0, -3.0, -3.0, -3.0, -3.0, -4.0, -5.0,
      -4.0, -2.0,  0.0,  0.0,  0.0,  0.0, -2.0, -4.0,
      -3.0,  0.0,  1.0,  1.5,  1.5,  1.0,  0.0, -3.0,
      -3.0,  0.5,  1.5,  2.0,  2.0,  1.5,  0.5, -3.0,
      -3.0,  0.0,  1.5,  2.0,  2.0,  1.5,  0.0, -3.0,
      -3.0,  0.5,  1.0,  1.5,  1.5,  1.0,  0.5, -3.0,
      -4.0, -2.0,  0.0,  0.5,  0.5,  0.0, -2.0, -4.0,
      -5.0, -4.0, -3.0, -3.0, -3.0, -3.0, -4.0, -5.0
   };

   constexpr static float black_bishop_table[64] = {
      -2.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -2.0,
      -1.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -1.0,
      -1.0,  0.0,  0.5,  1.0,  1.0,  0.5,  0.0, -1.0,
      -1.0,  0.5,  0.5,  1.0,  1.0,  0.5,  0.5, -1.0,
      -1.0,  0.0,  1.0,  1.0,  1.0,  1.0,  0.0, -1.0,
      -1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0, -1.0,
      -1.0,  0.5,  0.0,  0.0,  0.0,  0.0,  0.5, -1.0,
      -2.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -2.0
   };

   constexpr static float black_rook_table[64] = {
      0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.5,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  0.5,
      -0.5,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -0.5,
      -0.5,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -0.5,
      -0.5,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -0.5,
      -0.5,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -0.5,
      -0.5,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -0.5,
       0.0,   0.0, 0.0,  0.5,  0.5,  0.0,  0.0,  0.0
   };

   constexpr static float black_queen_table[64] = {
      -2.0, -1.0, -1.0, -0.5, -0.5, -1.0, -1.0, -2.0,
      -1.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -1.0,
      -1.0,  0.0,  0.5,  0.5,  0.5,  0.5,  0.0, -1.0,
      -0.5,  0.0,  0.5,  0.5,  0.5,  0.5,  0.0, -0.5,
       0.0,  0.0,  0.5,  0.5,  0.5,  0.5,  0.0, -0.5,
      -1.0,  0.5,  0.5,  0.5,  0.5,  0.5,  0.0, -1.0,
      -1.0,  0.0,  0.5,  0.0,  0.0,  0.0,  0.0, -1.0,
      -2.0, -1.0, -1.0, -0.5, -0.5, -1.0, -1.0, -2.0
   };

   constexpr static float black_king_table[64] = {
      -3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0,
      -3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0,
      -3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0,
      -3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0,
      -2.0, -3.0, -3.0, -4.0, -4.0, -3.0, -3.0, -2.0,
      -1.0, -2.0, -2.0, -2.0, -2.0, -2.0, -2.0, -1.0,
       2.0,  2.0,  0.0,  0.0,  0.0,  0.0,  2.0,  2.0,
       2.0,  3.0,  1.0,  0.0,  0.0,  1.0,  3.0,  2.0 
   };

   constexpr static float white_pawn_table[64] = {
      0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
      -2,  1.0, -1.0,  -3.0, -2.0,  1.0,  1.0,  -1,
      0.5, -0.5, -1.0,  0.0,  0.0, -1.0, -0.5,  0.5,
      0.0,  0.0,  0.0,  2.0,  2.0,  0.0,  0.0,  0.0,
      0.5,  0.5,  1.0,  2.5,  2.5,  1.0,  0.5,  0.5,
      1.0,  1.0,  2.0,  3.0,  3.0,  2.0,  1.0,  1.0,
      5.0,  6.0,  5.0,  5.0,  5.0,  6.0,  5.0,  5.0,
      0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
   };

   constexpr static float white_knight_table[64] = {
      -5.0, -4.0, -3.0, -3.0, -3.0, -3.0, -4.0, -5.0,
      -4.0, -2.0,  0.0,  0.5,  0.5,  0.0, -2.0, -4.0,
      -3.0,  0.5,  1.0,  1.5,  1.5,  1.0,  0.5, -3.0,
      -3.0,  0.0,  1.5,  2.0,  2.0,  1.5,  0.0, -3.0,
      -3.0,  0.5,  1.5,  2.0,  2.0,  1.5,  0.5, -3.0,
      -3.0,  0.0,  1.0,  1.5,  1.5,  1.0,  0.0, -3.0,
      -4.0, -2.0,  0.0,  0.0,  0.0,  0.0, -2.0, -4.0,
      -5.0, -4.0, -3.0, -3.0, -3.0, -3.0, -4.0, -5.0,
   };

   constexpr static float white_bishop_table[64] = {
      -2.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -2.0,
      -1.0,  0.5,  0.0,  0.0,  0.0,  0.0,  0.5, -1.0,
      -1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0, -1.0,
      -1.0,  0.0,  1.0,  1.0,  1.0,  1.0,  0.0, -1.0,
      -1.0,  0.5,  0.5,  1.0,  1.0,  0.5,  0.5, -1.0,
      -1.0,  0.0,  0.5,  1.0,  1.0,  0.5,  0.0, -1.0,
      -1.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -1.0,
      -2.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -2.0,
   };

   constexpr static float white_rook_table[64] = {
       0.0,   0.0, 0.0,  0.5,  0.5,  0.0,  0.0,  0.0,
      -0.5,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -0.5,
      -0.5,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -0.5,
      -0.5,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -0.5,
      -0.5,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -0.5,
      -0.5,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -0.5,
       0.5,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  0.5,
      0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
   };

   constexpr static float white_queen_table[64] = {
      -2.0, -1.0, -1.0, -0.5, -0.5, -1.0, -1.0, -2.0,
      -1.0,  0.0,  0.5,  0.0,  0.0,  0.0,  0.0, -1.0,
      -1.0,  0.5,  0.5,  0.5,  0.5,  0.5,  0.0, -1.0,
       0.0,  0.0,  0.5,  0.5,  0.5,  0.5,  0.0, -0.5,
      -0.5,  0.0,  0.5,  0.5,  0.5,  0.5,  0.0, -0.5,
      -1.0,  0.0,  0.5,  0.5,  0.5,  0.5,  0.0, -1.0,
      -1.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -1.0,
      -2.0, -1.0, -1.0, -0.5, -0.5, -1.0, -1.0, -2.0,
   };

   constexpr static float white_king_table[64] = {
       2.0,  3.0,  1.0,  0.0,  0.0,  1.0,  3.0,  2.0,
       2.0,  2.0,  0.0,  0.0,  0.0,  0.0,  2.0,  2.0,
      -1.0, -2.0, -2.0, -2.0, -2.0, -2.0, -2.0, -1.0,
      -2.0, -3.0, -3.0, -4.0, -4.0, -3.0, -3.0, -2.0,
      -3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0,
      -3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0,
      -3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0,
      -3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0,
   };

   Game & game;
   ZobristHasher hasher;
   // int alphabeta(int depth, int alpha, int beta, bool maximizing_player);
   int alphabeta(int depth, int alpha, int beta, bool maximizing_player = false);
   int negascout(int depth, int alpha, int beta);
   int negamax(int depth, int alpha, int beta);

   std::vector<int> get_move_scores(const std::vector<Move> & moves);
   void swap(std::vector<int> & vec, int i, int j);

public:
   Search(Game & game);
   float evaluate_position(Game & game);
   Move get_best_move(int depth);
   int num_positions_evaluated = 0;

};

// struct MoveComparator
// {
//    bool operator()(const Move & lm, const Move & rm)
//    {
//       return Search::MVV_LVA_table[
//          (lm.captured & Pieces::FILTER_PIECE)
//       ][
//          (lm.piece & Pieces::FILTER_PIECE)
//       ] < Search::MVV_LVA_table[
//          (rm.captured & Pieces::FILTER_PIECE)
//       ][
//          (rm.piece & Pieces::FILTER_PIECE)
//       ];
//    } 
// };

#endif