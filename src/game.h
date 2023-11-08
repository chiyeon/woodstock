#ifndef GAME_H
#define GAME_H

#include "history.h"
#include "move.h"
#include "movemasks.h"
#include "piece.h"
#include "zobrist.h"
#include <map>
#include <stack>
#include <vector>

#define PIECES_PER_SIDE 16

class Game {
public:
  Piece board[64];
  Piece turn = Pieces::WHITE;
  Piece not_turn = Pieces::BLACK;
  bool black_turn = false;
  Bitboard game_bb = 0ULL;
  Bitboard piece_bbs[23]; // 23 from max "value" (BLACK | KING = 22)

  bool has_white_king_moved = false;
  bool has_white_queenside_rook_moved = false;
  bool has_white_kingside_rook_moved = false;
  bool has_black_king_moved = false;
  bool has_black_queenside_rook_moved = false;
  bool has_black_kingside_rook_moved = false;

  bool can_castle_kingside(Bitboard attacked_squares);
  bool can_castle_queenside(Bitboard attacked_squares);

  History history;

public:
  void reset();
  MoveMasks movemasks;
  // ZobristHasher hasher;

  bool draw = false;
  bool wcm = false; // white checkmate = white has checkmated black
  bool bcm = false; // black checkmate = black has checkmated white
  // bool wc                                 = false;
  // bool bc                                 = false;

  Game(std::string fen =
           "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  Game(Game &g) = delete;
  std::string get_pgn(std::string name, bool was_black);
  void read_fen(std::string fen =
                    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  void print();
  Bitboard get_game_bitboard();
  Bitboard get_piece_bb(Piece filter);
  Piece *get_board();
  Piece get(int index);
  Piece get(int x, int y);

  History &get_history();
  bool is_history_empty();
  Move get_last_move();

  std::vector<Move> get_moves_at_square(int sq);

  bool is_blacks_turn();
  bool is_whites_turn();
  void switch_turns();
  Piece get_turn();
  Piece get_not_turn();
  bool last_move_resulted_in_check();
  bool is_in_check();

  bool is_king_in_check();
  bool no_moves_left();
  bool is_gameover();

  int is_square_ambiguous(int index);

  void get_moves(std::vector<Move> &moves);
  void move(Move &move, bool verbose = false);
  void undo(bool verbose = false);

private:
  static std::map<char, Piece> fen_to_piece;
};

#endif
