#include "history.h"
#include "move.h"
#include "game.h"

void History::record(Move move, Piece board[64])
{
   log.push_back(LogElement(move, board));
}

Move History::get_last_move()
{
   return log.back().move;
}

Move History::pop_last_move()
{
   Move last_move = log.back().move;
   log.pop_back();
   return last_move;
}

bool History::check_threefold_repetition(Piece board[64])
{
   // simulate move to determine board state
   int repetition_count = 0;

   for (auto & el : log)
   {
      if (el.board == board) {
         repetition_count++;
      }
   }

   return repetition_count >= 2;
}

bool History::is_empty()
{
   return log.empty();
}