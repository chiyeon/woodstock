#include "history.h"
#include "move.h"
#include "game.h"

void History::add_to_repetition_table(LogElement log)
{
   Hash key = hasher.get_key(log.board);
   int table_index = key % HASHTABLE_SIZE;

   // find first empty space if our space is occupied
   while (RepetitionTable[table_index].key != key && RepetitionTable[table_index].count != 0) {
      table_index++; 

      if (table_index >= HASHTABLE_SIZE) table_index = 0;
   }

   RepetitionTable[table_index].count++;
   RepetitionTable[table_index].key = key;
   if (RepetitionTable[table_index].count >= 3) threefold_repetition = true;
}

void History::remove_from_repetition_table(LogElement log)
{
   Hash key = hasher.get_key(log.board);
   int table_index = key % HASHTABLE_SIZE;

   while (RepetitionTable[table_index].key != key) {
      table_index++;

      if (table_index >= HASHTABLE_SIZE) table_index = 0;
   }

   RepetitionTable[table_index].count--;
   if (RepetitionTable[table_index].count == 2) threefold_repetition = false;
}

void History::record(Move move, Piece board[64])
{
   LogElement log_el = LogElement(move, board);
   log.push_back(log_el);

   add_to_repetition_table(log_el);
}

Move History::get_last_move()
{
   return log.back().move;
}

Move History::pop_last_move()
{
   LogElement last_move = log.back();
   log.pop_back();

   remove_from_repetition_table(last_move);

   return last_move.move;
}

bool History::check_threefold_repetition(Piece board[64])
{
   return threefold_repetition;
}

bool History::is_empty()
{
   return log.empty();
}
