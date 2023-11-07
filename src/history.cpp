#include "history.h"
#include <sstream>
#include <time.h>
#include "move.h"
#include "game.h"

void History::clear()
{
   for (int i = 0; i < HASHTABLE_SIZE; ++i) {
      RepetitionTable[i].key = 0ULL;
      RepetitionTable[i].count = 0;
   }

   log.clear();
}

void History::add_to_repetition_table(LogElement & log)
{
   Hash key = hasher.get_key(log.board);
   int table_index = key % HASHTABLE_SIZE;

   // find first empty space if our space is occupied
   while (RepetitionTable[table_index].key != key && RepetitionTable[table_index].count != 0) {
      table_index = (table_index + 1) % HASHTABLE_SIZE;
   }

   RepetitionTable[table_index].count++;
   RepetitionTable[table_index].key = key;   
   if (RepetitionTable[table_index].count >= 3) threefold_repetition = true;
}

void History::remove_from_repetition_table(LogElement & log)
{
   Hash key = hasher.get_key(log.board);
   int table_index = key % HASHTABLE_SIZE;

   while (RepetitionTable[table_index].key != key) {
      table_index = (table_index + 1) % HASHTABLE_SIZE;
   }

   RepetitionTable[table_index].count--;
   if (RepetitionTable[table_index].count == 2) threefold_repetition = false;
}

void History::record(Move move, Piece * board)
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

bool History::check_threefold_repetition()
{
   return threefold_repetition;
}

bool History::is_empty()
{
   return log.empty();
}

std::string History::get_date()
{
   time_t now = time(0);
   struct tm tstruct;
   tstruct = *localtime(&now);
   char buf[12];
   strftime(buf, sizeof(buf), "%Y.%m.%d", &tstruct);

   return buf;
}

std::string History::get_as_pgn(std::string name, bool was_black, int result)
{
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
   ss << "[Event \"Woodstock Test v" << WOODSTOCK_VERSION << "\"]\n" \
         "[Site \"https://chess.benjiwong.com\"]\n" \
         "[Date \"" << get_date() << "\"]\n" \
         "[Round \"1\"]\n" \
         "[White \"" << (was_black ? "Woodstock" : name) << "\"]\n" \
         "[Black \"" << (was_black ? name : "Woodstock") << "\"]\n" \
         "[Result \"" << str_res << "\"]\n\n";
   
   int log_size = log.size();
   for (int i = 0; i < log_size; i += 2) {
      if (i != 0) ss << " ";
      ss << (i/2 + 1) << ".";
      if (Moves::get_flags(log[i].move) & Moves::CASTLE) {
         int from = Moves::get_from(log[i].move);
         int to = Moves::get_to(log[i].move);

         if (to > from) ss << "0-0-0";
         else ss << "0-0";
      } else {
         ss << Utils::move_to_an(log[i].move);
      }
      if ((i + 1) < log_size) {
         if (Moves::get_flags(log[i+1].move) & Moves::CASTLE) {
            int from = Moves::get_from(log[i+1].move);
            int to = Moves::get_to(log[i+1].move);

            if (to > from) ss << "0-0-0";
            else ss << "0-0";
         } else {
            ss << " " << Utils::move_to_an(log[i+1].move);
         }
      }
   }

   if (result > 0) {
      ss << "#";
   }

   ss << " ";

   ss << str_res;
      
   return ss.str();
}
