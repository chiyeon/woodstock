#ifndef HISTORY_H
#define HISTORY_H

#include "constants.h"
#include "zobrist.h"
#include <vector>
#include <map>

// move made, and the board AFTER it was made
struct LogElement
{
   Move move;
   Piece board[64];

   LogElement(Move _move, Piece _board[64])
      : move(_move)
   {
      for (int i = 0; i < 64; ++i) {
         board[i] = _board[i];
      }
   }
};

class History
{
   std::vector<LogElement> log;
   std::map<Hash, int> hash_count;
   bool threefold_repetition = false;
   ZobristHasher hasher;
public:
   void record(Move move, Piece board[64]);
   Move get_last_move();
   Move pop_last_move();
   bool check_threefold_repetition(Piece board[64]);
   bool is_empty();
};

#endif
