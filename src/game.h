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
    Bitboard game_bitboard                  = 0;         // updates everytime move() is used
    Bitboard white_bitboard                 = 0;
    Bitboard black_bitboard                 = 0;

    Bitboard white_king_bitboard            = 0;
    Bitboard black_king_bitboard            = 0;

    Bitboard can_white_castle_kingside      = true;
    Bitboard can_white_castle_queenside     = true;
    Bitboard can_black_castle_kingside      = true;
    Bitboard can_black_castle_queenside     = true;

    // TODO bitboards for pieces, ie white_pawn_bitboard

    std::stack<Move> history;

public:
    int en_passant_count                    = 0;
    int captures                            = 0;
    int num_castles                         = 0;
    MoveMasks movemasks;

    Game(std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
    Game(Game & g)                          = delete;
    void read_fen(Piece * board, std::string fen);
    void print();
    Bitboard get_game_bitboard();
    Bitboard get_white_bitboard();
    Bitboard get_black_bitboard();
    Piece * get_board();
    Piece get(int index);
    Piece get(int x, int y);

    Move get_last_move();

    std::vector<Move> get_moves_at_square(int sq);

    bool is_blacks_turn();
    bool is_whites_turn();
    void switch_turns();
    Piece get_turn();
    bool in_check();
    bool is_in_check();

    void get_moves(std::vector<Move> & moves);
    void move(Move & move);
    void undo();

private:
    static std::map<char, Piece> fen_to_piece;
};

#endif