#include "movemasks.h"
#include "bitboard.h"
#include "constants.h"
#include "piece.h"
#include <strings.h>
#include <random>
// #include <time.h>
#include <vector>

void MoveMasks::calculate_knight_moves(Bitboard * moveset)
{
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            int i = x + y * 8;

            Bitboard start = Bitboards::get(x, y);
            Bitboard movement = 0;

            movement |= (start >> 17)   & Bitboards::NOT_A_FILE;
            movement |= (start >> 15)   & Bitboards::NOT_H_FILE;
            movement |= (start >> 10)   & Bitboards::NOT_AB_FILE;
            movement |= (start >> 6)    & Bitboards::NOT_GH_FILE;
            movement |= (start << 17)   & Bitboards::NOT_H_FILE;
            movement |= (start << 15)   & Bitboards::NOT_A_FILE;
            movement |= (start << 10)   & Bitboards::NOT_GH_FILE;
            movement |= (start << 6)    & Bitboards::NOT_AB_FILE;

            moveset[i] = movement;
        }
    }
}

/*
 * calculates bitboards for all possible rook positions. as it is 
 * used for blocking, does NOT include the last square in any particular direction
 */
void MoveMasks::calculate_rook_masks()
{
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            int i = x + y * 8;
            rook_magic_table[i].mask
                = Bitboards::get_row(y) 
                ^ Bitboards::get_column(x);
            
            if (x != 0) rook_magic_table[i].mask &= ~Bitboards::get_column(0);
            if (x != 7) rook_magic_table[i].mask &= ~Bitboards::get_column(7);
            if (y != 0) rook_magic_table[i].mask &= ~Bitboards::get_row(0);
            if (y != 7) rook_magic_table[i].mask &= ~Bitboards::get_row(7);
        }
    }
}

void MoveMasks::calculate_bishop_masks()
{
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            int i = x + y * 8;

            bishop_magic_table[i].mask
                = Bitboards::get_diagonal_downwards_right(x, y)
                | Bitboards::get_diagonal_downwards_left(x, y)
                | Bitboards::get_diagonal_upwards_right(x, y)
                ^ Bitboards::get_diagonal_upwards_left(x, y);
        }
    }
}

void MoveMasks::calculate_king_moves(Bitboard * moveset)
{
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            int i = x + y * 8;
            Bitboard movement = 0;
            Bitboard start = Bitboards::get(x, y);

            movement |= (start >> 8);                           // south
            movement |= (start >> 9) & Bitboards::NOT_A_FILE;   // south east
            movement |= (start >> 7) & Bitboards::NOT_H_FILE;   // south west
            movement |= (start >> 1) & Bitboards::NOT_A_FILE;   // east
            movement |= (start << 1) & Bitboards::NOT_H_FILE;   // west
            movement |= (start << 7) & Bitboards::NOT_A_FILE;   // north east
            movement |= (start << 9) & Bitboards::NOT_H_FILE;   // north west
            movement |= (start << 8);                           // north

            moveset[i] = movement;
        }
    }
}

MoveMasks::MoveMasks() {
    srand(time(NULL));

    populate_magic_shift_bits();      // starts iniitalization of magic tables

    calculate_knight_moves(knight_moves);
    calculate_rook_masks();
    calculate_bishop_masks();
    //calculate_all_queen_moves(queen_moves);     // calculate after bishop and rook...
    calculate_king_moves(king_moves);

    find_all_magics();
}

void MoveMasks::populate_magic_shift_bits()
{
    int rook_shift_bits[64] = {
        12, 11, 11, 11, 11, 11, 11, 12,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        12, 11, 11, 11, 11, 11, 11, 12
    };

    int bishop_shift_bits[64] = {
        6, 5, 5, 5, 5, 5, 5, 6,
        5, 5, 5, 5, 5, 5, 5, 5,
        5, 5, 7, 7, 7, 7, 5, 5,
        5, 5, 7, 9, 9, 7, 5, 5,
        5, 5, 7, 9, 9, 7, 5, 5,
        5, 5, 7, 7, 7, 7, 5, 5,
        5, 5, 5, 5, 5, 5, 5, 5,
        6, 5, 5, 5, 5, 5, 5, 6
    };

    for (int i = 0; i < 64; ++i) {
        rook_magic_table[i] = MagicEntry{0ULL, 0ULL, rook_shift_bits[i]};
        bishop_magic_table[i] = MagicEntry{0ULL, 0ULL, bishop_shift_bits[i]};
    }
}

Bitboard * MoveMasks::get_knight_moves()
{
    return knight_moves;
}

Bitboard * MoveMasks::get_queen_moves()
{
    return {};
}

Bitboard * MoveMasks::get_king_moves()
{
    return king_moves;
}

Magic MoveMasks::get_random_magic()
{
    Magic u1, u2, u3, u4;
    u1 = (Magic)(rand() & 0xFFFF);
    u2 = (Magic)(rand() & 0xFFFF);
    u3 = (Magic)(rand() & 0xFFFF);
    u4 = (Magic)(rand() & 0xFFFF);

    return u1 | (u2 << 16) | (u3 << 32) | (u4 << 48);
}

Magic MoveMasks::get_random_small_magic()
{
    // 3x & to get smaller numbers / less bits
    return get_random_magic() & get_random_magic() & get_random_magic();
}

// "adds one" to current while ONLY using the masked bits. for example:
/*
 * if our mask is           100111001
 * then 1 would be          000000001
 * but 2 would instead be   000001000
 * following the mask
 */
Bitboard MoveMasks::add_one_with_masked_bits(Bitboard current, Bitboard mask)
{
    // get index of least significant bit of the mask
    int lsb_pos = ffsll(mask) - 1;
    Bitboard lsb = Bitboards::get_i(lsb_pos);

    // if the bit is 0, just set it to one. otherwise we have to wrap the bits
    if ((current & lsb) == 0) {
        return current | lsb;
    } else {
        return add_one_with_masked_bits(current ^ lsb, mask ^ lsb);
    }
}

// given a bishop/rook blocker mask, return all possible combinations of blockers that can be in that mask
std::vector<Bitboard> MoveMasks::get_all_blocker_combinations(Bitboard blocker)
{
    std::vector<Bitboard> blockers_combinations;
    // todo reserve
    blockers_combinations.push_back(blocker);       // add fully blocked as one
    
    Bitboard count_board = 0ULL;    // this will increment like binary does each iteration
    while (blocker != count_board)
    {
        // add to our list of blockers
        blockers_combinations.push_back(count_board);
        // increment our count_board
        count_board = add_one_with_masked_bits(count_board, blocker);
    }

    return blockers_combinations;
}

Magic MoveMasks::find_magics(int pos, Piece piece_type)
{
    /* psuedocode: 
     * 
     * find the mask, then calculate all possible blocker combinations
     * for that mask
     * 
     * then for each blocker combination calculate the proper positions with raycasting
     * then, use that to determine the magics by brute forcing
     *
     * defining attributes of a magic: 
     * - is the same for each square
     * - can (with the help of index bits) map
     *   blocker combinations to small indexes with no collisions
     * */

    // printf("Finding magics for %s\n", piece_type == Pieces::BISHOP ? "bishops" : "rooks");

    Bitboard start_position = Bitboards::get_i(pos);
    Bitboard mask = (piece_type == Pieces::BISHOP) ? bishop_magic_table[pos].mask : rook_magic_table[pos].mask;
    std::vector<Bitboard> blocker_combinations = get_all_blocker_combinations(mask);
    int num_moves = blocker_combinations.size();
    int num_magic_moves = piece_type == Pieces::BISHOP ? 512 : 4096;
    std::vector<Bitboard> legal_moves;
    std::vector<Bitboard> magic_moves(num_magic_moves, 0);
    int index_bits = piece_type == Pieces::BISHOP ? bishop_magic_table[pos].shift : rook_magic_table[pos].shift;

    // printf("There are %d possible blocker boards.\n", num_moves);

    legal_moves.reserve(num_moves);

    // calculate all legal moves
    for (int i = 0; i < num_moves; ++i)
    {
        Bitboard movement = 0;

        if (piece_type == Pieces::BISHOP) {
            // calculacte bishop stuff
            int x = pos % 8;
            int y = pos / 8;
            for (int i = 1; (x + i < 8) && (y + i < 8); ++i) {
                Bitboard target_square = Bitboards::get(x + i, y + i);
                movement |= target_square;
                if (blocker_combinations[i] & target_square) break;
            }

            for (int i = 1; (x + -i >= 0) && (y + i < 8); ++i) {
                Bitboard target_square = Bitboards::get(x - i, y + i);
                movement |= target_square;
                if (blocker_combinations[i] & target_square) break;
            }

            for (int i = 1; (x + i < 8) && (y + -i >= 0); ++i) {
                Bitboard target_square = Bitboards::get(x + i, y - i);
                movement |= target_square;
                if (blocker_combinations[i] & target_square) break;
            }

            for (int i = 1; (x + -i >= 0) && (y + -i >= 0); ++i) {
                Bitboard target_square = Bitboards::get(x - i, y - i);
                movement |= target_square;
                if (blocker_combinations[i] & target_square) break;
            }
        } else {
            // calculate rook stuff
            int x = pos % 8;
            int y = pos / 8;
            for (int i = x + 1; i < 8; ++i) {
                Bitboard target_square = Bitboards::get(i, y);
                // if there is a piece on target square, bye bye
                movement |= target_square;
                if (blocker_combinations[i] & target_square) break;
            }

            for (int i = x - 1; i >= 0; --i) {
                Bitboard target_square = Bitboards::get(i, y);
                movement |= target_square;
                if (blocker_combinations[i] & target_square) break;
            }

            for (int i = y + 1; i < 8; ++i) {
                Bitboard target_square = Bitboards::get(x, i);
                movement |= target_square;
                if (blocker_combinations[i] & target_square) break;
            }

            for (int i = y - 1; i >= 0; --i) {
                Bitboard target_square = Bitboards::get(x, i);
                movement |= target_square;
                if (blocker_combinations[i] & target_square) break;
            }
        }
    }

    // once all legal moves are calculated, test random magics till found
    int num_tries = 100000000;

    for (int i = 0; i < num_tries; ++i) {
        Magic magic = get_random_small_magic();
        fill(magic_moves.begin(), magic_moves.end(), 0ULL);    // reset magic moves
        for (int j = 0; j < num_moves; ++j) {
            Bitboard key = (blocker_combinations[j] * magic) >> (64 - index_bits);
            if (key >= num_magic_moves) goto next_magic;
            if (magic_moves[key] == 0) {
                magic_moves[key] = legal_moves[j];
            } else if (magic_moves[key] != legal_moves[j]) {
                // not a good magic if there is a collision that ISNT the same legal move
                goto next_magic;
            }
        }

        // printf("Magic fonud?!??!\n");
        if (piece_type == Pieces::BISHOP) {
            for (int j = 0; j < magic_moves.size(); ++j) {
                bishop_moves[pos][j] = magic_moves[j];
            }
        } else {
            for (int j = 0; j < magic_moves.size(); ++j) {
                rook_moves[pos][j] = magic_moves[j];
            }
        }
        return magic;
        next_magic:;
    }

    printf("Magic not found in %d tries.\n", num_tries);
    return 0;
}

Bitboard MoveMasks::get_bishop_move(Bitboard occupation, int pos)
{
    occupation &= bishop_magic_table[pos].mask;
    occupation *= bishop_magic_table[pos].magic;
    occupation >>= (64 - rook_magic_table[pos].shift);
    return bishop_moves[pos][occupation];
}

Bitboard MoveMasks::get_rook_move(Bitboard occupation, int pos)
{
    occupation &= rook_magic_table[pos].mask;
    occupation *= rook_magic_table[pos].magic;
    occupation >>= (64 - rook_magic_table[pos].shift);
    return rook_moves[pos][occupation];
}

void MoveMasks::find_all_magics()
{
    for (int i = 0; i < 64; ++i) {
        rook_magic_table[i].magic = find_magics(i, Pieces::ROOK);
    }

    for (int i = 0; i < 64; ++i) {
        bishop_magic_table[i].magic = find_magics(i, Pieces::BISHOP);
    }

    Bitboards::print(get_rook_move(Bitboards::get_i(20), 20));
    Bitboards::print(rook_moves[23][2341]);
}