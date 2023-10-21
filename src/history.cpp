#include "history.h"
#include "move.h"
#include "game.h"

void History::record(Move move, Piece board[64])
{
   log.push_back(LogElement(move, board));

   Hash key = hasher.get_key(board);
   hash_count[key]++;

   if (hash_count[key] >= 3) {
      threefold_repetition = true;
   }
}

Move History::get_last_move()
{
   return log.back().move;
}

Move History::pop_last_move()
{
   LogElement last_move = log.back();
   log.pop_back();

   Hash key = hasher.get_key(last_move.board);
   hash_count[key]--;

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
