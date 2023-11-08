 #ifndef UTILS_H
 #define UTILS_H

 #include <string>
 #include <sstream>
 #include "move.h"
 #include "piece.h"

struct Utils {

 static int square_to_index(char * square) {
     int x, y;

     switch (square[0]) {
         case 'a': x = 7;
             break;
         case 'b': x = 6;
             break;
         case 'c': x = 5;
             break;
         case 'd': x = 4;
             break;
         case 'e': x = 3;
             break;
         case 'f': x = 2;
             break;
         case 'g': x = 1;
             break;
         case 'h': x = 0;
             break;
     }

     y = (int)(square[1]) - '1';

     return y * 8 + x;
 }

 static std::string index_to_square(int index)
 {
    int x = 7 - (index % 8);
    int y = index / 8;
    std::stringstream ss;
    ss << (char)('a' + x);
    ss << (char)('1' + y);
    return ss.str();
 }

 // converts move to algebreaic notation
 static std::string move_to_an(Move move, bool ambiguous_column = false, bool ambiguous_row = false) {
     std::stringstream ss;

     bool is_pawn = (Moves::get_piece(move) & Pieces::FILTER_PIECE) == Pieces::PAWN;
     bool promotion = (Moves::get_flags(move) & Moves::PROMOTION);
     bool capture = Moves::get_captured(move) != 0;
     bool was_ambiguous = ambiguous_column || ambiguous_row;

     if (!is_pawn && !promotion) {
         ss << Pieces::name_short((Moves::get_piece(move) & Pieces::FILTER_PIECE) | Pieces::WHITE); 
     }

      if (ambiguous_row) {
         ss << (char)('a' + (7 - (Moves::get_from(move) % 8)));
      } else if (ambiguous_column) {
         ss << (char)('1' + (Moves::get_from(move) / 8));
      }

     if (capture) {
         if ((is_pawn && !was_ambiguous) || (promotion && !was_ambiguous)) {
             // additionally print the column
             ss << (char)('a' + (7 - (Moves::get_from(move) % 8)));
         } 
       ss << 'x';
    }

    ss << index_to_square(Moves::get_to(move));

    if (promotion) {
      ss << "=" << Pieces::name_short((Moves::get_piece(move) & Pieces::FILTER_PIECE) | Pieces::WHITE); 
    }
    return ss.str();
 }

 // converst move to long algebraic notiation
 static std::string move_to_lan(Move move)
 {
    std::stringstream ss;

    if ((Moves::get_piece(move) & Pieces::FILTER_PIECE) != Pieces::PAWN) {
       // always capitalize
       ss << Pieces::name_short((Moves::get_piece(move) & Pieces::FILTER_PIECE) | Pieces::WHITE); 
    }

    ss << index_to_square(Moves::get_from(move));

    if (Moves::get_captured(move) != 0) {
       ss << 'x';
    }

    ss << index_to_square(Moves::get_to(move));
    return ss.str();
 }
};

 #endif
