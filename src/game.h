#ifndef GAME_H
#define GAME_H

#include <map>
#include <vector>
#include <stack>
#include "piece.h"
#include "movemasks.h"
#include "move.h"

#define PIECES_PER_SIDE 16

class Game
{
    Piece board[64];
    Piece turn                              = Pieces::WHITE;
    Bitboard game_bitboard                  = 0ULL;
    Bitboard white_bitboard                 = 0ULL;
    Bitboard black_bitboard                 = 0ULL;

    Bitboard white_king_bitboard            = 0ULL;
    Bitboard black_king_bitboard            = 0ULL;

    bool has_white_king_moved               = false;
    bool has_white_queenside_rook_moved     = false;
    bool has_white_kingside_rook_moved      = false;
    bool has_black_king_moved               = false;
    bool has_black_queenside_rook_moved     = false;
    bool has_black_kingside_rook_moved      = false;


    bool can_castle_kingside(Bitboard attacked_squares);
    bool can_castle_queenside(Bitboard attacked_squares);

    // TODO bitboards for pieces, ie white_pawn_bitboard

    std::stack<Move> history;

    template<Piece color>
    void set_bitboard(Bitboard bitboard) {
        if (color == Pieces::WHITE) white_bitboard = bitboard;
        else black_bitboard = bitboard;
    }

    template<Piece color>
    Bitboard get_bitboard() {
        if (color == Pieces::WHITE) return white_bitboard;
        else return black_bitboard;
    }

    // Bitboard get_attacked_squares(Bitboard pieces);
    // Bitboard get_attacked_squares_ignore_king(Bitboard pieces);

public:
    int en_passant_count                    = 0;
    int captures                            = 0;
    int num_castles                         = 0;
    MoveMasks movemasks;

    bool draw                               = false;
    bool wcm                                = false;
    bool bcm                                = false;
    bool wc                                 = false;
    bool bc                                 = false;

    Game(std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
    Game(Game & g)                      = delete;
    void read_fen(std::string fen);
    void print();
    Bitboard get_game_bitboard();
    Bitboard get_white_bitboard();
    Bitboard get_black_bitboard();
    Piece * get_board();
    Piece get(int index);
    Piece get(int x, int y);

    Move get_last_move();
    bool is_history_empty();
    void pop_last_move();
    void push_to_history(Move move);
    std::stack<Move> get_history();

    std::vector<Move> get_moves_at_square(int sq);

    bool is_blacks_turn();
    bool is_whites_turn();
    void switch_turns();
    Piece get_turn();
    bool last_move_resulted_in_check();
    bool is_in_check();

    void get_moves(std::vector<Move> & moves);
    void move(Move & move);
    void undo();

private:
    static std::map<char, Piece> fen_to_piece;
};

#endif