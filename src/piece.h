#ifndef PIECE_H
#define PIECE_H

#include <string>
#include <map>
#include "constants.h"
class Game;


struct Pieces
{
    /*
     * Piece & Color definitions
     */
    static constexpr Piece EMPTY    = 0b00000;

    static constexpr Piece PAWN     = 0b00001;
    static constexpr Piece KNIGHT   = 0b00010;
    static constexpr Piece BISHOP   = 0b00011;
    static constexpr Piece ROOK     = 0b00100;
    static constexpr Piece QUEEN    = 0b00101;
    static constexpr Piece KING     = 0b00110;

    static constexpr Piece WHITE    = 0b01000;
    static constexpr Piece BLACK    = 0b10000;

    static constexpr Piece WP       = WHITE | PAWN;
    static constexpr Piece WN       = WHITE | KNIGHT;
    static constexpr Piece WB       = WHITE | BISHOP;
    static constexpr Piece WR       = WHITE | ROOK;
    static constexpr Piece WQ       = WHITE | QUEEN;
    static constexpr Piece WK       = WHITE | KING;
    static constexpr Piece BP       = BLACK | PAWN;
    static constexpr Piece BN       = BLACK | KNIGHT;
    static constexpr Piece BB       = BLACK | BISHOP;
    static constexpr Piece BR       = BLACK | ROOK;
    static constexpr Piece BQ       = BLACK | QUEEN;
    static constexpr Piece BK       = BLACK | KING;

    static constexpr Piece FILTER_COLOR = 0b11000;
    static constexpr Piece FILTER_PIECE = 0b00111;
    static constexpr Piece NO_FILTER    = 0b11111;

    static short get_color(Piece piece);
    static Piece get_piece(Piece piece);
    // static std::string name_full(Piece piece);
    static char name_short(Piece piece);
    static Piece piece_from_name_short(char name);

    template <bool is_black> static Bitboard get_pawn_forward(Bitboard occ);
    template <bool is_black> static Bitboard get_pawn_forward_two(Bitboard occ);
    template <bool is_black> static Bitboard get_pawn_captures(Bitboard occ);


    /*
     * pseudo-moves for individual pieces
     */
    
    static Bitboard get_pawn_moves(int pos, Game & game);
    static Bitboard get_pawn_captures(int pos, Game & game);
    static Bitboard get_knight_moves(int pos, Game & game);
    static Bitboard get_bishop_moves(int pos, Game & game);
    static Bitboard get_pseudolegal_bishop_moves(int pos, Game & game);
    static Bitboard get_rook_moves(int pos, Game & game);
    static Bitboard get_pseudolegal_rook_moves(int pos, Game & game);
    static Bitboard get_queen_moves(int pos, Game & game);
    static Bitboard get_pseudolegal_queen_moves(int pos, Game & game);
    static Bitboard get_queen_moves_using_blocker(int pos, Bitboard blocker, Game & game);
    static Bitboard get_king_moves(int pos, Game & game);

private:
    // static std::map<Piece, std::string> piece_to_name_full;
    static std::map<Piece, char> piece_to_name_short;
    static std::map<char, Piece> name_short_to_piece;
};

#endif