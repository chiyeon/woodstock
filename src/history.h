#ifndef HISTORY_H
#define HISTORY_H

#include "constants.h"
#include "utils.h"
#include "zobrist.h"
#include <map>
#include <vector>

#define HASHTABLE_SIZE 16381 // uses first 14 bits

struct RepetitionTableEntry {
   Hash key;
   int count;

   RepetitionTableEntry(Hash _key) : key(_key), count(0) {}

   RepetitionTableEntry() : key(0ULL), count(0) {}
};

class History {
   RepetitionTableEntry *RepetitionTable;

   std::vector<Move> log;
   std::map<Hash, int> hash_count;
   bool threefold_repetition = false;
   ZobristHasher hasher;

   std::string get_date();

 public:
   void clear();
   void record(Move move, Hash zobrist_hash);
   Move get_last_move();
   Move pop_last_move(Hash zobrist_hash);
   bool check_threefold_repetition();
   bool is_empty();
   bool at_least_2_moves() { return log.size() >= 2; }

   std::string get_as_pgn(std::string name, bool was_black, int result,
                          std::string starting_fen);

   History(History &h) = delete;
   History() : RepetitionTable(new RepetitionTableEntry[HASHTABLE_SIZE]) {
      clear();
   }
   ~History() { delete[] RepetitionTable; }
};

#endif
