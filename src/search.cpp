#include "search.h"
#include "bitboard.h"
#include <algorithm>

Search::Search(Game & game)
   : game(game)
{ }

void Search::swap(std::vector<int> & vec, int i, int j)
{
   // vec[i] += vec[j];
   // vec[j] = vec[i] - vec[j];
   // vec[i] -= vec[j];
   int tmp = vec[i];
   vec[i] = vec[j];
   vec[j] = tmp;
}

float Search::evaluate_position(Game & game, std::vector<Move> & moves)
{
   float white_score = 0, black_score = 0;

   if (moves.size() == 0) {
      // no moves available
      if (game.is_in_check()) {
         // checkmate
         return (game.is_blacks_turn() ? 1 : -1) * 999999999;
      } else {
         // draw
         return 0;
      }
   }

   std::vector<int> wp_positions;
   std::vector<int> bp_positions;

   Bitboards::bitboard_to_positions(wp_positions, game.get_white_bitboard());
   Bitboards::bitboard_to_positions(bp_positions, game.get_black_bitboard());

   for (auto & pos : wp_positions) {
      int piece_type = (game.get(pos) & Pieces::FILTER_PIECE);

      white_score += piece_values[piece_type];

      switch (piece_type) {
         case Pieces::PAWN:
            white_score += white_pawn_table[pos];
            break;
         case Pieces::KNIGHT:
            white_score += white_knight_table[pos];
            break;
         case Pieces::BISHOP:
            white_score += white_bishop_table[pos];
            break;
         case Pieces::ROOK:
            white_score += white_rook_table[pos];
            break;
         case Pieces::QUEEN:
            white_score += white_queen_table[pos];
            break;
         case Pieces::KING:
            white_score += white_king_table[pos];
            break;
      }
   }

   for (auto & pos : bp_positions) {
      int piece_type = (game.get(pos) & Pieces::FILTER_PIECE);

      black_score += piece_values[piece_type];

      switch (piece_type) {
         case Pieces::PAWN:
            black_score += black_pawn_table[pos];
            break;
         case Pieces::KNIGHT:
            black_score += black_knight_table[pos];
            break;
         case Pieces::BISHOP:
            black_score += black_bishop_table[pos];
            break;
         case Pieces::ROOK:
            black_score += black_rook_table[pos];
            break;
         case Pieces::QUEEN:
            black_score += black_queen_table[pos];
            break;
         case Pieces::KING:
            black_score += black_king_table[pos];
            break;
      }
   }

   // mobility


   return white_score - black_score;
}

std::vector<int> Search::get_move_scores(const std::vector<Move> & moves)
{
   std::vector<int> scored_moves;
   scored_moves.reserve(moves.size());

   for (auto & move : moves)
   {
      scored_moves.push_back(MVV_LVA_table[(move.captured & Pieces::FILTER_PIECE)][(move.piece & Pieces::FILTER_PIECE)]);
   }
   
   return scored_moves;
}

Move Search::get_best_move(int depth)
{
   num_positions_evaluated = 0;

   int best_move_eval = -9999999;
   Move best_move(0, 0, 0);

   std::vector<Move> moves;
   game.get_moves(moves);

   for (auto & move : moves)
   {
      game.move(move);

      // int eval = (game.is_blacks_turn() ? 1 : -1) * alphabeta(depth - 1, -100000000, 100000000);
      int eval = negascout(depth - 1, -10000, 10000);
      // int eval = alphabeta(depth - 1, -10000, 10000);

      game.undo();

      if (eval >= best_move_eval) {
         best_move = move;
         best_move_eval = eval;
      }
   }

   // printf("Found best move at depth %d with %d positions\n", depth, num_positions_evaluated);
   // num_positions_evaluated = 0;

   return best_move;
}

int Search::negascout(int depth, int alpha, int beta)
{
   num_positions_evaluated++;

   std::vector<Move> moves;
   game.get_moves(moves);
   if (depth == 0) return evaluate_position(game, moves);

   int b = beta;
   
   for (int i = 0; i < moves.size(); ++i) {
      Move move = moves[i];
      game.move(move);

      int eval = -negascout(depth - 1, -b, -alpha);

      if ((eval > alpha) && (eval < beta) && (i > 0))
         eval = -negascout(depth - 1, -beta, -alpha);

      alpha = std::max(alpha, eval);
      if (alpha >= beta) 
         return alpha;
      b = alpha + 1;

      game.undo();
   }
   return alpha;
}

int Search::alphabeta(int depth, int alpha, int beta)
{
   num_positions_evaluated++;
   int best_move_eval = -999999;

   std::vector<Move> moves;
   game.get_moves(moves);

   if (depth == 0) return evaluate_position(game, moves);
   
   std::vector<int> move_scores = get_move_scores(moves);
   int num_moves = moves.size();

   for (int i = 0 ; i < num_moves; ++i) 
   {
      // percolate priority moves to the top
      for (int j = i; j < num_moves; ++j)
      {
         if (move_scores[j] > move_scores[i]) {
            std::swap(move_scores[i], move_scores[j]);
            std::swap(moves[i], moves[j]);
         }
      }

      game.move(moves[i]);
      int eval = -alphabeta(depth - 1, -beta, -alpha);
      game.undo();

      if (eval >= beta)
      {
         return eval;
      }
      if (eval > best_move_eval)
      {
         best_move_eval = eval;
         if (eval > alpha) {
            alpha = eval;
         }
      }

   }
   return best_move_eval;
}

// int Search::alphabeta(int depth, int alpha, int beta, bool maximizing_player)
// {
//    num_positions_evaluated++;

//    std::vector<Move> moves;
//    game.get_moves(moves);
//    if (depth == 0) return -evaluate_position(game, moves);
//    int num_moves = moves.size();
//    std::vector<int> move_scores = get_move_scores(moves);

//    // std::sort(moves.begin(), moves.end(), MoveComparator());

//    if (maximizing_player) {
//       int best_move_eval = -999999;

//       for (int i = 0; i < num_moves; ++i)
//       {
//          for (int j = i; j < num_moves; ++j) {
//             // if (move_scores[j] > move_scores[i]) swap(move_scores, i, j);
//             if (move_scores[j] > move_scores[i]) {
//                std::swap(move_scores[i], move_scores[j]);
//                std::swap(moves[i], moves[j]);
//             }
//          }

//          Move move = moves[i];

//          game.move(move);
//          best_move_eval = std::max(best_move_eval, alphabeta(depth - 1, alpha, beta, false));
//          game.undo();

//          alpha = std::max(alpha, best_move_eval);
//          if (best_move_eval >= beta) break;
//       }

//       return best_move_eval;
//    } else {
//       int best_move_eval = 999999;

//       for (int i = 0; i < num_moves; ++i)
//       {
//          for (int j = i; j < num_moves; ++j) {
//             // if (move_scores[j] > move_scores[i]) swap(move_scores, i, j);
//             if (move_scores[j] > move_scores[i]) {
//                std::swap(move_scores[i], move_scores[j]);
//                std::swap(moves[i], moves[j]);
//             }
//          }

//          Move move = moves[i];
//          game.move(move);
//          best_move_eval = std::min(best_move_eval, alphabeta(depth - 1, alpha, beta, true));
//          game.undo();
 
//          beta = std::min(beta, best_move_eval);
//          if (best_move_eval <= alpha) break;
//       }

//       return best_move_eval;
//    }
// }