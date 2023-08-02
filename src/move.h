#ifndef MOVE_H
#define MOVE_H

#include "constants.h"

enum MoveFlag
{
    NOTHING,
    EN_PASSANT,
    PROMOTION,
    CASTLE_BOTH_ALLOWED,        // todo fix this :sob:
    CASTLE_KINGSIDE_ALLOWED,
    CASTLE_QUEENSIDE_ALLOWED,
    KING_FIRST_MOVE,
    KINGSIDE_ROOK_FIRST_MOVE,
    QUEENSIDE_ROOK_FIRST_MOVE,
    PROMOTION_QUEEN,
    PROMOTION_ROOK,
    PROMOTION_KNIGHT,
    PROMOTION_BISHOP,
};

struct Move
{
    int from;
    int to;
    Piece piece;
    Piece captured;
    MoveFlag flag;

    Move(int from, int to, Piece piece)
        : from(from)
        , to(to)
        , piece(piece)
        , captured(0)
        , flag(NOTHING)
    { }

    Move(int from, int to, Piece piece, Piece captured)
        : from(from)
        , to(to)
        , piece(piece)
        , captured(captured)
        , flag(NOTHING)
    { }

    Move(int from, int to, Piece piece, Piece captured, MoveFlag flag)
        : from(from)
        , to(to)
        , piece(piece)
        , captured(captured)
        , flag(flag)
    { }

    bool operator == (const Move & move)
    {
        return 
            move.from == from &&
            move.to == to &&
            move.piece == piece &&
            move.flag == flag;
    }
};

#endif