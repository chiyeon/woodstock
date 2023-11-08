#ifndef MOVE_H
#define MOVE_H

#include "constants.h"

struct Moves {
  static constexpr int SHIFT_TO = 6;
  static constexpr int SHIFT_PIECE = 12;
  static constexpr int SHIFT_CAPTURED = 17;
  static constexpr int SHIFT_FLAGS = 22;

  static constexpr Flag NOTHING = 0b0;
  static constexpr Flag EN_PASSANT = 0b1;
  static constexpr Flag PROMOTION = 0b10;
  static constexpr Flag FIRST_MOVE =
      0b100; // not assigned until the move is made. used for undo()
  static constexpr Flag CASTLE = 0b1000;

  static constexpr Move FILTER_FROM = 0b111111; // num 0-63 for pos
  static constexpr Move FILTER_TO = 0b111111000000;
  static constexpr Move FILTER_PIECE =
      0b11111000000000000; // num 0-22 for pieces
  static constexpr Move FILTER_CAPTURED = 0b1111100000000000000000;
  static constexpr Move FILTER_FLAGS = 0b111110000000000000000000000;

  static inline Flag pop_flag(Flag &flags) {
    int trailing_zeroes = __builtin_ctz(flags);
    flags &= flags - 1;
    return 1 << trailing_zeroes;
  }

  static inline Move create(int from = 0, int to = 0, int piece = 0,
                            int captured = 0, Flag flags = 0) {
    return from | to << SHIFT_TO | piece << SHIFT_PIECE |
           captured << SHIFT_CAPTURED | flags << SHIFT_FLAGS;
  }

  static inline int get_from(const Move &move) { return move & FILTER_FROM; }

  static inline int get_to(const Move &move) {
    return (move & FILTER_TO) >> SHIFT_TO;
  }

  static inline Piece get_piece(const Move &move) {
    return (move & FILTER_PIECE) >> SHIFT_PIECE;
  }

  static inline Piece get_captured(const Move &move) {
    return (move & FILTER_CAPTURED) >> SHIFT_CAPTURED;
  }

  static inline Flag get_flags(const Move &move) {
    return (move & FILTER_FLAGS) >> SHIFT_FLAGS;
  }

  static inline void add_flag(Move &move, Flag flag) {
    move |= (flag << SHIFT_FLAGS);
  }
};

#endif