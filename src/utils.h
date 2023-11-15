#ifndef UTILS_H
#define UTILS_H

#include "move.h"
#include "piece.h"
#include <sstream>
#include <string>

struct Utils {

   static int square_to_index(char *square) {
      int x, y;

      switch (square[0]) {
      case 'a':
         x = 7;
         break;
      case 'b':
         x = 6;
         break;
      case 'c':
         x = 5;
         break;
      case 'd':
         x = 4;
         break;
      case 'e':
         x = 3;
         break;
      case 'f':
         x = 2;
         break;
      case 'g':
         x = 1;
         break;
      case 'h':
         x = 0;
         break;
      }

      y = (int)(square[1]) - '1';

      return y * 8 + x;
   }

   static std::string index_to_square(int index) {
      int x = 7 - (index % 8);
      int y = index / 8;
      std::stringstream ss;
      ss << (char)('a' + x);
      ss << (char)('1' + y);
      return ss.str();
   }

   // converts move to algebreaic notation
   static std::string move_to_an(Move move, bool ambiguous_column = false,
                                 bool ambiguous_row = false) {
      std::stringstream ss;

      bool is_pawn =
         (Moves::get_piece(move) & Pieces::FILTER_PIECE) == Pieces::PAWN;
      bool promotion = (Moves::get_flags(move) & Moves::PROMOTION);
      bool capture = Moves::get_captured(move) != 0;
      bool was_ambiguous = ambiguous_column || ambiguous_row;

      if (!is_pawn && !promotion) {
         ss << Pieces::name_short(
            (Moves::get_piece(move) & Pieces::FILTER_PIECE) | Pieces::WHITE);
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
         ss << "="
            << Pieces::name_short(
                  (Moves::get_piece(move) & Pieces::FILTER_PIECE) |
                  Pieces::WHITE);
      }
      return ss.str();
   }

   // converst move to long algebraic notiation
   static std::string move_to_lan(Move move) {
      std::stringstream ss;

      bool promoting = Moves::get_flags(move) & Moves::PROMOTION;

      if ((Moves::get_piece(move) & Pieces::FILTER_PIECE) != Pieces::PAWN &&
          !promoting) {
         // always capitalize
         ss << Pieces::name_short(
            (Moves::get_piece(move) & Pieces::FILTER_PIECE) | Pieces::WHITE);
      }

      ss << index_to_square(Moves::get_from(move));

      if (Moves::get_captured(move) != 0) {
         ss << 'x';
      }

      ss << index_to_square(Moves::get_to(move));
      if (promoting) {
         ss << Pieces::name_short(
            (Moves::get_piece(move) & Pieces::FILTER_PIECE) | Pieces::WHITE);
      }
      return ss.str();
   }

   template <typename Game>
   static Move lan_to_move(std::string lan, Game & game) {
      int from = 0, to = 0;
      Piece piece, captured;
      Flag flags = 0;
      int piece_boost = 0;
      int capture_boost = 0;

      switch (lan[0]) {
         case 'b':
         case 'B':
         case 'n':
         case 'N':
         case 'r':
         case 'R':
         case 'q':
         case 'Q':
         case 'k':
         case 'K':
            piece_boost = 1;
            break;
      }

      for (auto & c : lan) {
         if (c == 'x') {
            capture_boost = 1;
            break;
         }
      }

      char from_sq[3] = {lan[piece_boost+0], lan[piece_boost+1]};
      char to_sq[3] = {lan[piece_boost+2+capture_boost], lan[piece_boost+3+capture_boost]};

      from += square_to_index(from_sq);
      to += square_to_index(to_sq);
      captured = game.get(to);
   
      switch (piece & Pieces::FILTER_PIECE) {
         case Pieces::KING:
            if (abs(from % 8 - to % 8) >= 2) {
               flags |= Moves::FIRST_MOVE | Moves::CASTLE;
            }

            if (game.can_castle_kingside() || game.can_castle_queenside()) {
               flags |= Moves::FIRST_MOVE;
            }
            break;
         case Pieces::ROOK:
            if (game.can_castle_kingside() || game.can_castle_queenside()) {
               flags |= Moves::FIRST_MOVE;
            }
            break;
         case Pieces::PAWN:
            if (to / 8 == 0 || to / 8 == 7) {
               flags |= Moves::PROMOTION;
            } else if (abs(from - to) >= 2) {
               flags |= Moves::EN_PASSANT;
            }
            break;
      }

      return Moves::create(from, to, piece, captured, flags);
   }
};

#endif
