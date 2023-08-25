#ifndef ZOBRIST_H
#define ZOBRIST_H

#include <random>
#include <map>
#include "constants.h"
#include "piece.h"

struct TranspositionEntry
{
   Hash key;            // associated zobrist key
   int depth;           // depth ascertained at
   int eval;            // evaluation of board
   // best move todo

   TranspositionEntry()
      : key(0ULL)
      , depth(0)
      , eval(0)
   { }

   TranspositionEntry(Hash key, int depth, int eval)
      : key(key)
      , depth(depth)
      , eval(eval)
   { }
};

const int hashtable_size = 100000;

class ZobristHasher
{
   TranspositionEntry HashTable[hashtable_size];
   Hash ZobristTable[64][12];
   std::map<Piece, char> piece_to_index = {
      {Pieces::WHITE | Pieces::PAWN, 0},
      {Pieces::WHITE | Pieces::KNIGHT, 1},
      {Pieces::WHITE | Pieces::BISHOP, 2},
      {Pieces::WHITE | Pieces::ROOK, 3},
      {Pieces::WHITE | Pieces::QUEEN, 4},
      {Pieces::WHITE | Pieces::KING, 5},
      {Pieces::BLACK | Pieces::PAWN, 6},
      {Pieces::BLACK | Pieces::KNIGHT, 7},
      {Pieces::BLACK | Pieces::BISHOP, 8},
      {Pieces::BLACK | Pieces::ROOK, 9},
      {Pieces::BLACK | Pieces::QUEEN, 10},
      {Pieces::BLACK | Pieces::KING, 11},
   };

   Hash get_random_hash()
   {
      Hash u1, u2, u3, u4;
      u1 = (Hash)(rand() & 0xFFFF);
      u2 = (Hash)(rand() & 0xFFFF);
      u3 = (Hash)(rand() & 0xFFFF);
      u4 = (Hash)(rand() & 0xFFFF);

      return u1 | (u2 << 16) | (u3 << 32) | (u4 << 48);
   }

   void initialize_table()
   {
      for (int i = 0; i < 64; ++i) {
         for (int j = 0; j < 12; ++j) {
            ZobristTable[i][j] = get_random_hash();
         }
      }
   }

   Hash compute_zobrist_key(Piece * board)
   {
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
   ZobristHasher()
   {
      srand(time(NULL));

      TranspositionEntry e;
      printf("%d bytes\n", sizeof(e));
   }
   ZobristHasher(ZobristHasher & hasher) = delete;

   void store_entry(Piece * board, int depth, int eval)
   {
      Hash zobrist_key = compute_zobrist_key(board);
      int key = zobrist_key % hashtable_size;
      HashTable[key] = TranspositionEntry(zobrist_key, depth, eval);
   }

   TranspositionEntry get_entry(Piece * board)
   {
      Hash zobrist_key = compute_zobrist_key(board);
      int key = zobrist_key % hashtable_size;
      return HashTable[key];
   }
};

#endif