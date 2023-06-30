#ifndef PIECE_H
#define PIECE_H

#include <string>
#include <map>

using Piece = short;

struct Pieces
{
    /*
     * Piece & Color definitions
     */
    const static Piece EMPTY    = 0b00000;

    const static Piece PAWN     = 0b00001;
    const static Piece KNIGHT   = 0b00010;
    const static Piece BISHOP   = 0b00011;
    const static Piece ROOK     = 0b00100;
    const static Piece QUEEN    = 0b00101;
    const static Piece KING     = 0b00110;

    const static Piece WHITE    = 0b01000;
    const static Piece BLACK    = 0b10000;

    const static Piece FILTER_COLOR = 0b11000;
    const static Piece FILTER_PIECE = 0b00111;

    static short get_color(Piece piece);
    static Piece get_piece(Piece piece);
    // static std::string name_full(Piece piece);
    static char name_short(Piece piece);
    static Piece piece_from_name_short(char name);

    /*
     * pseudo-moves for individual pieces
     */
    
    static Bitboard get_pawn_moves(int x, int y, bool is_black);
    static Bitboard get_knight_moves(int x, int y);
    static Bitboard get_bishop_moves(int x, int y);
    static Bitboard get_rook_moves(int x, int y);
    static Bitboard get_queen_moves(int x, int y);
    static Bitboard get_king_moves(int x, int y, bool is_black);

private:
    // static std::map<Piece, std::string> piece_to_name_full;
    static std::map<Piece, char> piece_to_name_short;
    static std::map<char, Piece> name_short_to_piece;
};

#endif