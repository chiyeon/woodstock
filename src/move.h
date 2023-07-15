#ifndef MOVE_H
#define MOVE_H

#include "constants.h"

enum MoveFlag
{
    PAWN_DOUBLE,
    EN_PASSANT,
    PROMOTION,
    CASTLE,
};

struct Move
{
    int from;
    int to;
    Piece piece;
    Piece captured;
    MoveFlag * flags;

    Move(int from, int to, Piece piece)
        : from(from)
        , to(to)
        , piece(piece)
        , captured(0)
    { }

    Move(int from, int to, Piece piece, Piece captured)
        : from(from)
        , to(to)
        , piece(piece)
        , captured(captured)
    { }
};

#endif