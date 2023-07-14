// represent bitboards as 64 bit unsigned integers
// it appears uint64_t == unsigned long long for emcc
using Bitboard = uint64_t;
using Piece = short;

#ifndef CONSTANTS_H
#define CONSTANTS_H

struct Constants
{
    const static int BOARD_SIZE = 8;
    const static int MAX_CHESS_MOVES_PER_POSITION = 218;
};

#endif