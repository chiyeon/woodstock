#include <algorithm>
#include <vector>
#include "bitboard.h"
#include "constants.h"
#include "piece.h"

void Bitboards::print(Bitboard bitboard)
{
    printf("Representation of %llu\n", bitboard);
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            printf("%llu ", (
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
    return 0b1000000000000000000000000000000000000000000000000000000000000000 >> (y * Constants::BOARD_SIZE + x);
}

Bitboard Bitboards::get_i(int i)
{
    return 0b1000000000000000000000000000000000000000000000000000000000000000 >> (i);
}

Bitboard Bitboards::get_row(int y)
{
    return 0b1111111100000000000000000000000000000000000000000000000000000000 >> (y * Constants::BOARD_SIZE);
}

Bitboard Bitboards::get_row_segment(int y, int x1, int x2)
{
    Bitboard row = get_row(y);
    row = row >> (7 - (x2 - x1));               // truncate row
    row = row << ((7 - x2) + (7 - y) * 8);      // move to position
    return row;
}

Bitboard Bitboards::get_column(int x)
{
    return 0b1000000010000000100000001000000010000000100000001000000010000000 >> (x);
}

Bitboard Bitboards::get_column_segment(int x, int y1, int y2)
{
    Bitboard column = get_column(x);
    column = column >> (7 - (y2 - y1) * 8);     // truncate row
    column = column << ((y1 - y2) * 8);          // move to position
    return column;
}

Bitboard Bitboards::get_diagonal_downwards_right(int x, int y)
{
    return diagonal_downwards_right_starters[x] >> (x + y * 8);
}

Bitboard Bitboards::get_diagonal_downwards_left(int x, int y)
{
    return diagonal_downwards_left_starters[(7 - x)] >> ((x + 1) + (y - 1) * 8);
}

Bitboard Bitboards::get_diagonal_upwards_right(int x, int y)
{
    return diagonal_upwards_right_starters[std::max(7 - y, x)] << ((7 - y) * 8) >> x;
}

Bitboard Bitboards::get_diagonal_upwards_left(int x, int y)
{
    return diagonal_upwards_left_starters[std::max(7 - y, 7 - x)] << ((7 - x) + (7 - y) * 8);
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
    int count = 0;
    Bitboard filter = get_i(0);
    for (int i = 0; i < 64; ++i) {
        if (((bitboard << i) & filter) != 0) ++count;
    }
    return count;
}

std::vector<int> Bitboards::bitboard_to_positions(Bitboard bitboard)
{
    std::vector<int> positions;
    
    Bitboard filter = get_i(0);

    for (int i = 0; i < 64; ++i) {
        // check if piece is in spot
        if ((filter & (bitboard << i)) != 0) {
            positions.push_back(i);
        }
    }

    return positions;
}

/*
 * these were primarily created for fast bishop diagonal
 * calculations... the speed (while it is there) isn't necessary
 * anymore. but its already here so *shrugs*
 */
const Bitboard Bitboards::diagonal_downwards_right_starters[] = {
    0b0000000001000000001000000001000000001000000001000000001000000001,
    0b0000000001000000001000000001000000001000000001000000001000000000,
    0b0000000001000000001000000001000000001000000001000000000000000000,
    0b0000000001000000001000000001000000001000000000000000000000000000,
    0b0000000001000000001000000001000000000000000000000000000000000000,
    0b0000000001000000001000000000000000000000000000000000000000000000,
    0b0000000001000000000000000000000000000000000000000000000000000000,
    0,
};

const Bitboard Bitboards::diagonal_downwards_left_starters[] = {
    0b0000000000000010000001000000100000010000001000000100000010000000,
    0b0000000000000010000001000000100000010000001000000100000000000000,
    0b0000000000000010000001000000100000010000001000000000000000000000,
    0b0000000000000010000001000000100000010000000000000000000000000000,
    0b0000000000000010000001000000100000000000000000000000000000000000,
    0b0000000000000010000001000000000000000000000000000000000000000000,
    0b0000000000000010000000000000000000000000000000000000000000000000,
    0,
};

const Bitboard Bitboards::diagonal_upwards_right_starters[] = {
    0b0000000100000010000001000000100000010000001000000100000000000000,
    0b0000000000000010000001000000100000010000001000000100000000000000,
    0b0000000000000000000001000000100000010000001000000100000000000000,
    0b0000000000000000000000000000100000010000001000000100000000000000,
    0b0000000000000000000000000000000000010000001000000100000000000000,
    0b0000000000000000000000000000000000000000001000000100000000000000,
    0b0000000000000000000000000000000000000000000000000100000000000000,
    0,
};

const Bitboard Bitboards::diagonal_upwards_left_starters[] = {
    0b1000000001000000001000000001000000001000000001000000001000000000,
    0b0000000001000000001000000001000000001000000001000000001000000000,
    0b0000000000000000001000000001000000001000000001000000001000000000,
    0b0000000000000000000000000001000000001000000001000000001000000000,
    0b0000000000000000000000000000000000001000000001000000001000000000,
    0b0000000000000000000000000000000000000000000001000000001000000000,
    0b0000000000000000000000000000000000000000000000000000001000000000,
    0,
};

const Bitboard Bitboards::A_FILE = get_column(0);
const Bitboard Bitboards::H_FILE = get_column(7);
const Bitboard Bitboards::AB_FILE = get_column(0) | get_column(1);
const Bitboard Bitboards::GH_FILE = get_column(7) | get_column(6);
const Bitboard Bitboards::NOT_A_FILE = ~get_column(0);
const Bitboard Bitboards::NOT_H_FILE = ~get_column(7);
const Bitboard Bitboards::NOT_AB_FILE = ~(get_column(0) | get_column(1));
const Bitboard Bitboards::NOT_GH_FILE = ~(get_column(7) | get_column(6));
const Bitboard Bitboards::ROW_1 = get_row(0);
const Bitboard Bitboards::ROW_8 = get_row(7);
const Bitboard Bitboards::NOT_ROW_1 = ~get_row(0);
const Bitboard Bitboards::NOT_ROW_8 = ~get_row(7);