#include "history.h"
#include "game.h"
#include "move.h"
#include <sstream>
#include <time.h>

void History::clear() {
   for (int i = 0; i < HASHTABLE_SIZE; ++i) {
      RepetitionTable[i].key = 0ULL;
      RepetitionTable[i].count = 0;
   }

   threefold_repetition = false;

   log.clear();
}

void History::record(Move move, Hash zobrist_key) {
   log.push_back(move);

   int table_index = zobrist_key % HASHTABLE_SIZE;

   // find first empty space if our space is occupied

   while (RepetitionTable[table_index].key != zobrist_key &&
          RepetitionTable[table_index].count != 0) {
      table_index = (table_index + 1) % HASHTABLE_SIZE;
   }

   RepetitionTable[table_index].count++;
   RepetitionTable[table_index].key = zobrist_key;
   if (RepetitionTable[table_index].count >= 3)
      threefold_repetition = true;
}

Move History::pop_last_move(Hash zobrist_key) {
   Move last_move = log.back();
   log.pop_back();

   int table_index = zobrist_key % HASHTABLE_SIZE;

   while (RepetitionTable[table_index].key != zobrist_key) {
      table_index = (table_index + 1) % HASHTABLE_SIZE;
   }

   RepetitionTable[table_index].count--;
   if (RepetitionTable[table_index].count == 2)
      threefold_repetition = false;
   return last_move;
}

Move History::get_last_move() { return log.back(); }

bool History::check_threefold_repetition() { return threefold_repetition; }

bool History::is_empty() { return log.empty(); }

std::string History::get_date() {
   time_t now = time(0);
   struct tm tstruct;
   tstruct = *localtime(&now);
   char buf[12];
   strftime(buf, sizeof(buf), "%Y.%m.%d", &tstruct);

   return buf;
}

std::string History::get_as_pgn(std::string name, bool was_black, int result,
                                std::string starting_fen) {
   Game g(starting_fen); // game to simulate
   std::string str_res;
   switch (result) {
   case 0:
      // draw
      str_res = "1/2-1/2";
      break;
   case 1:
      // wcm
      str_res = "1-0";
      break;
   case 2:
      // bcm
      str_res = "0-1";
      break;
   }

   // some defaults for now
   std::string out;
   std::stringstream ss(out);
   ss << "[Event \"Woodstock Test v" << WOODSTOCK_VERSION
      << "\"]\n"
         "[Site \"https://chess.benjiwong.com\"]\n"
         "[Date \""
      << get_date()
      << "\"]\n"
         "[Round \"1\"]\n"
         "[White \""
      << (was_black ? "Woodstock" : name)
      << "\"]\n"
         "[Black \""
      << (was_black ? name : "Woodstock")
      << "\"]\n"
         "[Result \""
      << str_res << "\"]\n\n";

   int log_size = log.size();
   for (int i = 0; i < log_size; i++) {
      if (i != 0)
         ss << " ";
      if (i % 2 == 0)
         ss << (i / 2 + 1) << ".";

      Move move = log[i];

      int flags = Moves::get_flags(move);
      if (flags & Moves::CASTLE) {
         if (Moves::get_from(move) < Moves::get_to(move))
            ss << "0-0-0";
         else
            ss << "0-0";
         g.move(move);
      } else {

         // check ambiguity
         // must check BEFORE we do the move!
         int ambiguity = g.is_square_ambiguous(Moves::get_to(move));
         ss << Utils::move_to_an(move, ambiguity == 1, ambiguity == 2);

         g.move(move);
         // promotions add '+' for some reason. block it.
         if (g.is_king_in_check() && !g.is_gameover() &&
             !(flags & Moves::PROMOTION))
            ss << "+";
      }
   }

   if (result > 0) {
      ss << "#";
   }

   ss << " ";

   ss << str_res;

   return ss.str();
}
