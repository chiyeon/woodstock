#ifndef ZOBRIST_H
#define ZOBRIST_H

#include "constants.h"
#include "piece.h"
#include <ctime>
#include <map>
#include <random>

struct TranspositionEntry {
   Hash key;    // associated zobrist key
   short depth; // depth ascertained at
   short eval;  // evaluation of board
   // uint32_t best_move;  // Move is 64 bits. we can save storage as we only
   // need to/from
   Move best_move;
   bool wcm, bcm, draw;

   TranspositionEntry()
      : key(0ULL), depth(0), eval(0), best_move(0), wcm(false), bcm(false),
        draw(false) {}

   TranspositionEntry(TranspositionEntry &te) = delete;

   TranspositionEntry(Hash key, int depth, int eval, Move best_move, bool wcm,
                      bool bcm, bool draw)
      : key(key), depth(depth), eval(eval)
        //, best_move(static_cast<uint32_t>(best_move))
        ,
        best_move(best_move), wcm(wcm), bcm(bcm), draw(draw) {}
};

const int hashtable_size = 1128889;
// const int hashtable_size = 112129;

class ZobristHasher {
   TranspositionEntry *HashTable;
   Hash ZobristTable[64][23]; // size 23, but only uses 12. 23 so we can pass in
                              // Piece as Piece instead of switching. not as
                              // space conservant, but faster
   Hash ZobristSwitchSides;
   Hash zobrist_key;

   Hash get_random_hash() {
      Hash u1, u2, u3, u4;
      u1 = (Hash)(rand() & 0xFFFF);
      u2 = (Hash)(rand() & 0xFFFF);
      u3 = (Hash)(rand() & 0xFFFF);
      u4 = (Hash)(rand() & 0xFFFF);

      return u1 | (u2 << 16) | (u3 << 32) | (u4 << 48);
   }

   void initialize_table() {
      for (int i = 0; i < 64; ++i) {
         for (int j = 0; j < 23; ++j) {
            ZobristTable[i][j] = get_random_hash();
         }
      }
      ZobristSwitchSides = get_random_hash();
   }

   Hash compute_zobrist_key(Piece *board) {
      Hash hash = 0ULL;

      for (int i = 0; i < 64; ++i) {
         Piece piece = board[i];
         if (piece != 0ULL) {
            hash ^= ZobristTable[i][piece];
         }
      }

      return hash;
   }

   // adjusts the current zobrist key using the position and piece given
   inline void apply_to_key(int pos, Piece piece) {
      zobrist_key ^= ZobristTable[pos][piece];
   }

 public:
   Hash get_zobrist_key() { return zobrist_key; }

   Hash calculate_zobrist_key(Piece *board) {
      return compute_zobrist_key(board);
   }

   ZobristHasher() : HashTable(new TranspositionEntry[hashtable_size]) {
      srand(time(NULL));
      initialize_table();
   }
   ZobristHasher(ZobristHasher &hasher) = delete;

   Hash get_from_zobrist_table(int pos, Piece piece) {
      return ZobristTable[pos][piece];
   }

   void store_entry(int depth, int eval, Move best_move, bool wcm, bool bcm,
                    bool draw) {
      HashTable[zobrist_key % hashtable_size] = TranspositionEntry(
         zobrist_key, depth, eval, best_move, wcm, bcm, draw);
   }

   TranspositionEntry &get_entry() {
      return HashTable[zobrist_key % hashtable_size];
   }

   void set_initial_key(Piece *board) {
      zobrist_key = compute_zobrist_key(board);
   }

   void update_key(Move move) {
      int from = Moves::get_from(move);
      int to = Moves::get_to(move);
      int piece = Moves::get_piece(move);
      int captured = Moves::get_captured(move);
      int flags = Moves::get_flags(move);
      bool is_blacks_turn = (piece & Pieces::FILTER_COLOR) == Pieces::BLACK;

      // get ou rnext that that is promotion, castle, or en passant
      flags &= ~Moves::FIRST_MOVE;

      switch (flags) {
      default: {
         apply_to_key(from, piece);
         apply_to_key(to, piece);

         if (captured != 0) {
            apply_to_key(to, captured);
         }

         break;
      }
      case Moves::EN_PASSANT: {
         int ep_square = is_blacks_turn ? to + 8 : to - 8;

         apply_to_key(from, piece);
         apply_to_key(to, piece);
         apply_to_key(ep_square, captured);
         break;
      }
      case Moves::CASTLE: {
         apply_to_key(from, piece);
         apply_to_key(to, piece);

         int rook_to, rook_from;
         Piece rook =
            Pieces::ROOK | (is_blacks_turn ? Pieces::BLACK : Pieces::WHITE);

         if (to > from) {
            rook_to = to - 1;
            rook_from = to + 2;
         } else {
            rook_to = to + 1;
            rook_from = to - 1;
         }

         apply_to_key(rook_from, rook);
         apply_to_key(rook_to, rook);
         break;
      }

      case Moves::PROMOTION: {
         Piece pawn = Pieces::PAWN | (piece & Pieces::FILTER_COLOR);
         apply_to_key(from, pawn);
         apply_to_key(to, piece);

         if (captured != 0) {
            apply_to_key(to, captured);
         }
         break;
      }
      }

      if (is_blacks_turn)
         zobrist_key ^= ZobristSwitchSides;
   }

   ~ZobristHasher() { delete[] HashTable; }
};

#endif
