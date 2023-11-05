// #ifndef UTILS_H
// #define UTILS_H

// #include <string>
// #include <sstream>
// #include "move.h"
// #include "piece.h"

// int square_to_index(char * square) {
//     int x, y;

//     switch (square[0]) {
//         case 'a': x = 7;
//             break;
//         case 'b': x = 6;
//             break;
//         case 'c': x = 5;
//             break;
//         case 'd': x = 4;
//             break;
//         case 'e': x = 3;
//             break;
//         case 'f': x = 2;
//             break;
//         case 'g': x = 1;
//             break;
//         case 'h': x = 0;
//             break;
//     }

//     y = (int)(square[1]) - '1';

//     return y * 8 + x;
// }

// std::string index_to_square(int index)
// {
//    int x = 7 - (index % 8);
//    int y = index / 8;
//    std::stringstream ss;
//    ss << (char)('a' + x) << (char)('1' + y);
//    return ss.str();
// }

// // converts move to algebreaic notation
// std::string move_to_an(Move move) {
//     std::stringstream ss;

//     bool is_pawn = (Moves::get_piece(move) & Pieces::FILTER_PIECE) != Pieces::PAWN;
//     bool capture = Moves::get_captured(move) != 0;

//     if (!is_pawn) {
//         ss << Pieces::name_short(Moves::get_piece(move));
//     }

//     if (capture) {
//         if (is_pawn) {
//             // get column
//             ss << index_to_square(Moves::get_from(move) % 8);
//         }
//       ss << 'x';
//    }

//    ss << index_to_square(Moves::get_to(move));
//    return ss.str();
// }

// // converst move to long algebraic notiation
// std::string move_to_lan(Move move)
// {
//    std::stringstream ss;

//    if ((Moves::get_piece(move) & Pieces::FILTER_PIECE) != Pieces::PAWN) {
//       ss << Pieces::name_short(Moves::get_piece(move)); 
//    }

//    ss << index_to_square(Moves::get_from(move));

//    if (Moves::get_captured(move) != 0) {
//       ss << 'x';
//    }

//    ss << index_to_square(Moves::get_to(move));
//    return ss.str();
// }

// #endif