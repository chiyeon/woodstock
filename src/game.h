#ifndef GAME_H
#define GAME_H

#include <map>
#include <vector>
#include "piece.h"
#include "movemasks.h"
#include "move.h"

class Game
{
    Piece board[64];
    Piece turn = Pieces::WHITE;
    Bitboard game_bitboard = 0;         // updates everytime move() is used
    Bitboard white_bitboard = 0;
    Bitboard black_bitboard = 0;
    // TODO bitboards for pieces, ie white_pawn_bitboard

    std::vector<Move> history;

public:
    MoveMasks movemasks;

    Game(std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
    void read_fen(Piece * board, std::string fen);
    void print();
    Bitboard get_game_bitboard();
    Piece get(int index);
    Piece get(int x, int y);

    bool is_blacks_turn();
    bool is_whites_turn();
    Piece get_turn();

    std::vector<Move> get_moves();
    void move(Move move);
    void undo();

private:
    static std::map<char, Piece> fen_to_piece;
};

#endif