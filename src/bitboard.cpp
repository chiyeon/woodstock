#include <algorithm>
#include <vector>
#include <cmath>
#include <strings.h>

#include "bitboard.h"
#include "constants.h"
#include "piece.h"

void Bitboards::print(Bitboard bitboard)
{
    printf("Representation of %llu\n", bitboard);
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            printf("%d ", (
                bitboard
                >> ((Constants::BOARD_SIZE * Constants::BOARD_SIZE - 1) - (x + y * Constants::BOARD_SIZE))
                & 1
            ));
        }
        printf("\n");
    }
}

Bitboard Bitboards::get(int x, int y)
{
    return 1ULL << (y * Constants::BOARD_SIZE + x);
}

Bitboard Bitboards::get_i(int i)
{
    return 1ULL << (i);
}

Bitboard Bitboards::get_row(int y)
{
    return 0b11111111ULL << (y * Constants::BOARD_SIZE);
}

Bitboard Bitboards::get_row_segment(int y, int x1, int x2)
{
    Bitboard row = get_row(0);
    row >>= (7 - (x2 - x1));                // truncate row
    row <<= (x2 - (x2 - x1));               // put it back
    row <<= y * 8;                          // proper y position
    return row;
}

Bitboard Bitboards::get_column(int x)
{
    return 0b0000000100000001000000010000000100000001000000010000000100000001ULL << (x);
}

Bitboard Bitboards::get_column_segment(int x, int y1, int y2)
{
    Bitboard column = get_column(x);
    column >>= (7 - (y2 - y1)) * 8;             // truncate column
    column <<= (y2 - (y2 - y1)) * 8;
    return column;
}

Bitboard Bitboards::get_diagonal_downwards_right(int x, int y)
{
    Bitboard output = 0ULL;
    for (int i = 0; i < x; i++) {
        output = (output >> 9) | (0b0000000001000000000000000000000000000000000000000000000000000000);
    }
    return (output >> ((7 - x) + (7 - y) * 8));
}

Bitboard Bitboards::get_diagonal_downwards_left(int x, int y)
{
    Bitboard output = 0ULL;
    for (int i = 0; i < (7 - x); i++) {
        output = (output >> 7) | (0b000000100000000000000000000000000000000000000000000000000000000);
    }
    return (output >> ((7 - x) + (7 - y) * 8));
}

Bitboard Bitboards::get_diagonal_upwards_right(int x, int y)
{
    Bitboard output = 0ULL;
    for (int i = 0; i < std::min(7 - y, x); i++) {
        output = (output << 7) | (0b100000000000000);
    }   //monkas
    output >>= (7 - x);
    output <<= (y * 8);
    return output;
}

Bitboard Bitboards::get_diagonal_upwards_left(int x, int y)
{
    Bitboard output = 0ULL;
    for (int i = 0; i < std::min(7 - x, 7 - y); i++) {
        output = (output << 9) | (0b1000000000);
    }

    return output << (x + y * 8);
}

Bitboard Bitboards::get_between(int x1, int y1, int x2, int y2)
{
    if (x1 > x2) {
        if (y1 > y2) {
            // south east
            return Bitboards::get_diagonal_downwards_right(x1, y1) ^ Bitboards::get_diagonal_downwards_right(x2, y2);
        } else if (y1 < y2) {
            // north east
            return Bitboards::get_diagonal_upwards_right(x1, y1) ^ Bitboards::get_diagonal_upwards_right(x2, y2);
        } else {
            // east
            return Bitboards::get_row_segment(y2, x2, x1 - 1);
        }
    } else if (x1 < x2) {
        if (y1 > y2) {
            // south west
            return Bitboards::get_diagonal_downwards_left(x1, y1) ^ Bitboards::get_diagonal_downwards_left(x2, y2);
        } else if (y1 < y2) {
            // north west
            return Bitboards::get_diagonal_upwards_left(x1, y1) ^ Bitboards::get_diagonal_upwards_left(x2, y2);
        } else {
            // west
            return Bitboards::get_row_segment(y2, x1 + 1, x2);
        }
    } else {
        if (y1 > y2) {
            // south
            return Bitboards::get_column_segment(x1, y2, y1 - 1);
        } else {
            // north
            return Bitboards::get_column_segment(x1, y1 + 1, y2);
        }
    }

    return 0ULL;
}

Bitboard Bitboards::get_between(int x1, int y1, int x2, int y2, Piece piece)
{
    if (x1 > x2) {
        if (y1 > y2) {
            // south east
            if (
                (piece & Pieces::FILTER_PIECE) == Pieces::BISHOP
                || (piece & Pieces::FILTER_PIECE) == Pieces::QUEEN
            ) 
                return Bitboards::get_diagonal_downwards_right(x1, y1) ^ Bitboards::get_diagonal_downwards_right(x2, y2);
        } else if (y1 < y2) {
            // north east
            if (
                (piece & Pieces::FILTER_PIECE) == Pieces::BISHOP
                || (piece & Pieces::FILTER_PIECE) == Pieces::QUEEN
            ) 
                return Bitboards::get_diagonal_upwards_right(x1, y1) ^ Bitboards::get_diagonal_upwards_right(x2, y2);
        } else {
            // east
            if (
                (piece & Pieces::FILTER_PIECE) == Pieces::ROOK
                || (piece & Pieces::FILTER_PIECE) == Pieces::QUEEN
            ) 
                return Bitboards::get_row_segment(y2, x2, x1 - 1);
        }
    } else if (x1 < x2) {
        if (y1 > y2) {
            // south west
            if (
                (piece & Pieces::FILTER_PIECE) == Pieces::BISHOP
                || (piece & Pieces::FILTER_PIECE) == Pieces::QUEEN
            ) 
                return Bitboards::get_diagonal_downwards_left(x1, y1) ^ Bitboards::get_diagonal_downwards_left(x2, y2);
        } else if (y1 < y2) {
            // north west
            if (
                (piece & Pieces::FILTER_PIECE) == Pieces::BISHOP
                || (piece & Pieces::FILTER_PIECE) == Pieces::QUEEN
            ) 
                return Bitboards::get_diagonal_upwards_left(x1, y1) ^ Bitboards::get_diagonal_upwards_left(x2, y2);
        } else {
            // west
            if (
                (piece & Pieces::FILTER_PIECE) == Pieces::ROOK
                || (piece & Pieces::FILTER_PIECE) == Pieces::QUEEN
            ) 
                return Bitboards::get_row_segment(y2, x1 + 1, x2);
        }
    } else {
        if (y1 > y2) {
            // south
            if (
                (piece & Pieces::FILTER_PIECE) == Pieces::ROOK
                || (piece & Pieces::FILTER_PIECE) == Pieces::QUEEN
            ) 
                return Bitboards::get_column_segment(x1, y2, y1 - 1);
        } else {
            // north
            if (
                (piece & Pieces::FILTER_PIECE) == Pieces::ROOK
                || (piece & Pieces::FILTER_PIECE) == Pieces::QUEEN
            ) 
                return Bitboards::get_column_segment(x1, y1 + 1, y2);
        }
    }

    return 0ULL;
}

Bitboard Bitboards::board_to_bitboard(Piece * board)
{
    Bitboard bitboard = 0;
    for (int i = 0; i < 64; ++i) {
        if (board[i] != 0) {
            bitboard |= get_i(i);
        }
    }
    return bitboard;
}

Bitboard Bitboards::board_to_bitboard(Piece * board, Piece color)
{
    Bitboard bitboard = 0;
    for (int i = 0; i < 64; ++i) {
        if (board[i] != 0 && ((board[i] & Pieces::FILTER_COLOR) == color)) {
            bitboard |= get_i(i);
        }
    }
    return bitboard;
}

int Bitboards::bit_count(Bitboard bitboard)
{
    return __builtin_popcountll(bitboard); // a bit slower than just looping?
}

void Bitboards::bitboard_to_positions(std::vector<int> & positions, Bitboard bitboard)
{
    // positions.reserve(bit_count(bitboard));
    positions.reserve(64);
    
    while (bitboard != 0) {
        positions.push_back(__builtin_ctzll(bitboard));
        bitboard &= bitboard - 1;
    }
}

int Bitboards::bitboard_to_position(Bitboard bitboard)
{
    return ffsll(bitboard);
}

int Bitboards::pop_lsb(Bitboard & bitboard)
{
    // int lsb_pos = ffsll(bitboard);
    // bitboard &= bitboard - 1;
    // return lsb_pos - 1;
    // these are about the same, this one seems to be better
    int lsb_pos = __builtin_ctzll(bitboard);
    bitboard &= bitboard - 1;
    return lsb_pos;
}

bool Bitboards::contains_multiple_bits(Bitboard bitboard)
{
    return bitboard & (bitboard - 1);
}