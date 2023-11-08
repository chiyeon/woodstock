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
   Hash ZobristTable[64][12];
   Hash ZobristSwitchSides;
   //int i,j; // these literally fix everything..
   Hash zobrist_key;
   std::map<Piece, char> piece_to_index = {
      {Pieces::WHITE | Pieces::PAWN, 0},   {Pieces::WHITE | Pieces::KNIGHT, 1},
      {Pieces::WHITE | Pieces::BISHOP, 2}, {Pieces::WHITE | Pieces::ROOK, 3},
      {Pieces::WHITE | Pieces::QUEEN, 4},  {Pieces::WHITE | Pieces::KING, 5},
      {Pieces::BLACK | Pieces::PAWN, 6},   {Pieces::BLACK | Pieces::KNIGHT, 7},
      {Pieces::BLACK | Pieces::BISHOP, 8}, {Pieces::BLACK | Pieces::ROOK, 9},
      {Pieces::BLACK | Pieces::QUEEN, 10}, {Pieces::BLACK | Pieces::KING, 11},
   };

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
         for (int j = 0; j < 12; ++j) {
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
            int piece_index = piece_to_index[piece];
            hash ^= ZobristTable[i][piece_index];
         }
      }

      return hash;
   }

 public:
   Hash get_zobrist_key() {
      return zobrist_key;
   }

   Hash calculate_zobrist_key(Piece * board) {
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

   void store_entry(int depth, int eval, Move best_move, bool wcm, bool bcm, bool draw) {
      HashTable[zobrist_key % hashtable_size] = TranspositionEntry(zobrist_key, depth, eval, best_move, wcm, bcm, draw);
   }

   /*
   void store_entry(Piece *board, int depth, int eval, Move best_move, bool wcm,
                    bool bcm, bool draw) {
      Hash zobrist_key = compute_zobrist_key(board);
      int key = zobrist_key % hashtable_size;
      HashTable[key] = TranspositionEntry(zobrist_key, depth, eval, best_move,
                                          wcm, bcm, draw);
   }

   TranspositionEntry & get_entry(Piece *board) {
      Hash zobrist_key = compute_zobrist_key(board);
      int key = zobrist_key % hashtable_size;
      return HashTable[key];
   }*/

   TranspositionEntry & get_entry() {
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
         default:
         {
            zobrist_key ^= ZobristTable[from][piece];
            zobrist_key ^= ZobristTable[to][piece];

            if (captured != 0) {
               zobrist_key ^= ZobristTable[to][captured];
            }

            break;
         }
         case Moves::EN_PASSANT:
         {
            int ep_square = is_blacks_turn ? to + 8 : to - 8;

            zobrist_key ^= ZobristTable[from][piece];
            zobrist_key ^= ZobristTable[to][piece];

            zobrist_key ^= ZobristTable[ep_square][captured];
            break;
         }
         case Moves::CASTLE:
         {
            zobrist_key ^= ZobristTable[from][piece];
            zobrist_key ^= ZobristTable[to][piece];

            int rook_to, rook_from;
            Piece rook = Pieces::ROOK | (is_blacks_turn ? Pieces::BLACK : Pieces::WHITE);

            if (to > from) {
               rook_to = to - 1;
               rook_from = to + 2;
            } else {
               rook_to = to + 1;
               rook_from = to - 1;
            }

            zobrist_key ^= ZobristTable[rook_from][rook];
            zobrist_key ^= ZobristTable[rook_to][rook];
            break;
         }

         case Moves::PROMOTION:
         {
            Piece pawn = Pieces::PAWN | (piece & Pieces::FILTER_COLOR);
            zobrist_key ^= ZobristTable[from][pawn];
            zobrist_key ^= ZobristTable[to][piece];

            if (captured != 0) {
               zobrist_key ^= ZobristTable[to][captured];
            }
            break;
         }
      }

      if (is_blacks_turn) zobrist_key ^= ZobristSwitchSides;
      
   }

   ~ZobristHasher() { delete[] HashTable; }
};

#endif
