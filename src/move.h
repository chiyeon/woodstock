#ifndef MOVE_H
#define MOVE_H

#include "constants.h"

struct Move
{
    Bitboard to;
    Bitboard from;
    Piece piece;
    Piece captured;

    Move(Bitboard to, Bitboard from, Piece piece)
        : to(to)
        , from(from)
        , piece(piece)
        , captured(0)
    { }

    Move(Bitboard to, Bitboard from, Piece piece, Piece captured)
        : to(to)
        , from(from)
        , piece(piece)
        , captured(captured)
    { }
};

#endif