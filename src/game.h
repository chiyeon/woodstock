#ifndef GAME_H
#define GAME_H

#include <map>
#include <vector>
#include <stack>
#include "piece.h"
#include "movemasks.h"
#include "move.h"
#include "zobrist.h"

#define PIECES_PER_SIDE 16

class Game
{
public:
    Piece board[64];
    Piece turn                              = Pieces::WHITE;
    Piece not_turn                          = Pieces::BLACK;
    bool black_turn                         = false;
    Bitboard game_bb                        = 0ULL;
    Bitboard piece_bbs[23];                 // 23 from max "value" (BLACK | KING = 22)

    bool has_white_king_moved               = false;
    bool has_white_queenside_rook_moved     = false;
    bool has_white_kingside_rook_moved      = false;
    bool has_black_king_moved               = false;
    bool has_black_queenside_rook_moved     = false;
    bool has_black_kingside_rook_moved      = false;

    bool can_castle_kingside(Bitboard attacked_squares);
    bool can_castle_queenside(Bitboard attacked_squares);

    std::stack<Move> history;
public:
    int en_passant_count                    = 0;
    int captures                            = 0;
    int num_castles                         = 0;
    MoveMasks movemasks;
    // ZobristHasher hasher;

    // bool draw                               = false;
    // bool wcm                                = false;
    // bool bcm                                = false;
    // bool wc                                 = false;
    // bool bc                                 = false;

    Game(std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    Game(Game & g)                      = delete;
    void read_fen(std::string fen);
    void print();
    Bitboard get_game_bitboard();
    Bitboard get_piece_bb(Piece filter);
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
    Piece get_not_turn();
    bool last_move_resulted_in_check();
    bool is_in_check();

    bool king_in_check();
    bool no_moves_left();
    bool is_checkmated();

    void get_moves(std::vector<Move> & moves);
    void move(Move & move);
    void undo();

private:
    static std::map<char, Piece> fen_to_piece;
};

#endif
