#include "search.h"
#include "bitboard.h"
#include <algorithm>
#include <limits.h>

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

float Search::evaluate_position(Game & game)
{
   float white_score = 0, black_score = 0;

   if (game.wcm) return INT_MAX;
   else if (game.bcm) return -INT_MAX;
   else if (game.draw) return 0;

   Bitboard white_bitboard = game.get_piece_bb(Pieces::WHITE);
   while (white_bitboard != 0ULL) {
      int pos = Bitboards::pop_lsb(white_bitboard);
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

   Bitboard black_bitboard = game.get_piece_bb(Pieces::BLACK);
   while (black_bitboard != 0ULL) {
      int pos = Bitboards::pop_lsb(black_bitboard);
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
      scored_moves.push_back(MVV_LVA_table[(Moves::get_captured(move) & Pieces::FILTER_PIECE)][(Moves::get_piece(move) & Pieces::FILTER_PIECE)]);
   }
   
   return scored_moves;
}

void Search::start_search_timer()
{
   search_start_time = time(NULL);
}

Move Search::get_best_move_iterative_deepening(int & depth)
{
   Move best_move = 0;
   Move prev_best_move = 0;

   search_start_time = time(NULL);
   int current_depth;
   for (current_depth = 1; current_depth < max_search_depth && !out_of_time(); ++current_depth)
   {
      prev_best_move = best_move;
      best_move = get_best_move(current_depth); 
   }

   depth = current_depth;

   return completed_search ? best_move : prev_best_move;
   return best_move;
}

bool Search::out_of_time()
{
   time_t now = time(NULL);
   return difftime(now, search_start_time) > max_search_duration;
}

Move Search::get_best_move(int depth)
{
   completed_search = true;
   num_positions_evaluated = 0;

   int best_move_eval = -INT_MAX, alpha = -INT_MAX, beta = INT_MAX;
   Move best_move = 0;

   TranspositionEntry entry = hasher.get_entry(game.get_board());
   // if (entry.key != 0ULL && entry.depth >= depth) {
   //    printf("recalled");
   //    return entry.best_move;
   // }

   std::vector<Move> moves;
   game.get_moves(moves);
   std::vector<int> move_scores = get_move_scores(moves);
   int num_moves = moves.size();
   //best_move = moves[0]; // default best move

   for (int i = 0; i < num_moves; ++i) {
      for (int j = i; j < num_moves; ++j) {
         if (move_scores[j] > move_scores[i]) {
            std::swap(move_scores[i], move_scores[j]);
            std::swap(moves[i], moves[j]);
         }
      }

      Move move = moves[i];
      game.move(move);
      int eval = -negamax(depth - 1, -beta, -alpha);
      game.undo();

      if (eval >= beta) {
         if (eval > best_move_eval) {
            best_move = move;
            best_move_eval = eval;
         }
      } else {
         if (eval > best_move_eval) {
            best_move = move;
            best_move_eval = eval;

            alpha = std::max(alpha, best_move_eval);
         }
      }
   }
   /*

   for (auto & move : moves)
   {
      game.move(move);

      // false b/c we already did the first depth!
      // int eval = alphabeta(depth - 1, alpha, beta, false);
      // int eval = -negamax(depth - 1, -beta, -alpha);
      int eval = -negascout(depth - 1, -beta, -alpha);

      game.undo();

      if (eval > best_move_eval) {
         best_move = move;
         best_move_eval = eval;

         alpha = std::max(alpha, best_move_eval);
      }
   } */

   // store into transposition table
   hasher.store_entry(game.get_board(), depth, 0, best_move);

   return best_move;
}

int Search::negamax(int depth, int alpha, int beta)
{
   num_positions_evaluated++;
   //if (depth == 0) return (game.is_blacks_turn() ? -1 : 1) * evaluate_position(game);
   if (depth == 0 || game.is_gameover()) return (game.is_blacks_turn() ? -1 : 1) * evaluate_position(game);

   std::vector<Move> moves;
   game.get_moves(moves);
   std::vector<int> move_scores = get_move_scores(moves);
   int num_moves = moves.size();

   int max = -INT_MAX, eval = 0;
   for (int i = 0; i < num_moves; ++i)
   {
      for (int j = i; j < num_moves; ++j) {
         if (move_scores[j] > move_scores[i]) {
            std::swap(move_scores[i], move_scores[j]);
            std::swap(moves[i], moves[j]);
         }
      }

      game.move(moves[i]);
      eval = -negamax(depth - 1, -beta, -alpha);
      game.undo();

      if (eval >= beta) return eval;
      if (eval > max) {
         max = eval;
         if (eval > alpha) alpha = eval;
      }
   }

   return max;
}

int Search::negascout(int depth, int alpha, int beta)
{
   num_positions_evaluated++;
   if (depth == 0 || game.is_gameover()) return (game.is_blacks_turn() ? -1 : 1) * evaluate_position(game);
   
   std::vector<Move> moves;
   game.get_moves(moves);

   //std::vector<Move> moves;
   //game.get_moves(moves);
   std::vector<int> move_scores = get_move_scores(moves);
   int num_moves = moves.size();

   int b = beta, eval = 0;
   
   for (int i = 0; i < num_moves; ++i) {
      for (int j = i; j < num_moves; ++j) {
         if (move_scores[j] > move_scores[i]) {
            std::swap(move_scores[i], move_scores[j]);
            std::swap(moves[i], moves[j]);
         }
      }

      Move move = moves[i];
      game.move(move);

      eval = -negascout(depth - 1, -b, -alpha);

      if ((eval > alpha) && (eval < beta) && (i > 0))
            eval = -negascout(depth - 1, -beta, -alpha);

      alpha = std::max(alpha, eval);

      game.undo();
      
      if (alpha >= beta)
         return alpha;
      b = alpha + 1;
   }
   return alpha;
}

int Search::alphabeta(int depth, int alpha, int beta, bool maximizing_player)
{
   num_positions_evaluated++;

   if (!completed_search || out_of_time()) {
      completed_search = false;
      return -1;
   }
   
   if (depth == 0) {
      int eval = evaluate_position(game);
      return (game.is_blacks_turn() ? 1 : -1) * eval;
   }
   std::vector<Move> moves;
   game.get_moves(moves);
   int num_moves = moves.size();
   std::vector<int> move_scores = get_move_scores(moves);

   if (maximizing_player) {
      int best_move_eval = -INT_MAX;

      for (int i = 0; i < num_moves; ++i)
      {
         for (int j = i; j < num_moves; ++j) {
            if (move_scores[j] > move_scores[i]) {
               std::swap(move_scores[i], move_scores[j]);
               std::swap(moves[i], moves[j]);
            }
         }

         Move move = moves[i];

         game.move(move);
         best_move_eval = std::max(best_move_eval, alphabeta(depth - 1, alpha, beta, false));
         game.undo();

         alpha = std::max(alpha, best_move_eval);
         if (best_move_eval >= beta) break;
      }

      return best_move_eval;
   } else {
      int best_move_eval = INT_MAX;

      for (int i = 0; i < num_moves; ++i)
      {
         for (int j = i; j < num_moves; ++j) {
            if (move_scores[j] > move_scores[i]) {
               std::swap(move_scores[i], move_scores[j]);
               std::swap(moves[i], moves[j]);
            }
         }

         Move move = moves[i];
         game.move(move);
         best_move_eval = std::min(best_move_eval, alphabeta(depth - 1, alpha, beta, true));
         game.undo();
 
         beta = std::min(beta, best_move_eval);
         if (best_move_eval <= alpha) break;
      }

      return best_move_eval;
   }
}
