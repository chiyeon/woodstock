#include <stdio.h>
#include <cctype>
#include <vector>
#include "game.h"
#include "piece.h"
#include "move.h"

Game::Game(std::string fen)
{
    read_fen(board, fen);
}

void Game::read_fen(Piece * board, std::string fen)
{
    int fen_length = fen.size();
    int index = 0;
    for (int i = 0; i < fen_length; ++i) {
        // read number of spaces
        if (isdigit(fen[i])) {
            int num_spaces = int(fen[i] - '0');
            for (int j = 0; j < num_spaces; ++j) {
                board[index] = Pieces::EMPTY;
                index++;
            }
        } else if (fen[i] != '/') {
            board[index] = Pieces::piece_from_name_short(fen[i]);
            index++;
        }
    }

    // update our game bitboard representation
    game_bitboard = Bitboards::board_to_bitboard(board);
    white_bitboard = Bitboards::board_to_bitboard(board, Pieces::WHITE);
    black_bitboard = Bitboards::board_to_bitboard(board, Pieces::BLACK);
}

void Game::print()
{
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            Piece piece = board[x + y * 8];
            printf("%c ", piece == Pieces::EMPTY 
                ? ((y % 2 + x) % 2 == 0 ? ',' : '.') 
                : Pieces::name_short(piece)
            );
        }
        printf("\n");
    }
}

Bitboard Game::get_game_bitboard()
{
    return game_bitboard;
}

Piece Game::get(int index)
{
    return board[index];
}

Piece Game::get(int x, int y)
{
    return board[y * 8 + x];
}

bool Game::is_blacks_turn()
{
    return turn == Pieces::BLACK;
}

bool Game::is_whites_turn()
{
    return turn == Pieces::WHITE;
}

Piece Game::get_turn()
{
    return turn;
}

std::vector<Move> Game::get_moves()
{
    std::vector<Move> moves;

    // get rid of this looping someday...
    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            Piece piece = board[y * 8 + x];
            
            if (piece == 0 || (piece & Pieces::FILTER_COLOR) != turn) continue;

            // for every piece, get moves based on type
            switch (piece & Pieces::FILTER_PIECE) {
                case Pieces::PAWN:
                {
                    Bitboard piece_moves = Pieces::get_pawn_moves(x, y, *this);
                    piece_moves &= ~(is_blacks_turn() ? black_bitboard : white_bitboard);
                    std::vector<int> positions = Bitboards::bitboard_to_positions(piece_moves);
                    for (auto pos : positions) {
                        moves.push_back(Move(y * 8 + x, pos, piece));
                    }
                    break;
                }
                case Pieces::KNIGHT:
                {
                    Bitboard piece_moves = Pieces::get_knight_moves(x, y, *this);
                    piece_moves &= ~(is_blacks_turn() ? black_bitboard : white_bitboard);
                    std::vector<int> positions = Bitboards::bitboard_to_positions(piece_moves);
                    for (auto pos : positions) {
                        moves.push_back(Move(y * 8 + x, pos, piece));
                    }
                    break;
                }
                case Pieces::BISHOP:
                {
                    Bitboard piece_moves = Pieces::get_bishop_moves(x, y, *this);

                    // get collisions
                    Bitboard collisions = piece_moves & game_bitboard;
                    std::vector<int> collision_positions = Bitboards::bitboard_to_positions(collisions);
                    for (auto pos : collision_positions) {
                        int col_x = pos % 8;
                        int col_y = pos / 8;

                        if (col_x > x && col_y > y) {
                            piece_moves &= ~Bitboards::get_diagonal_downwards_right(x, y);
                        } else if (col_x < x && col_y > y){
                            piece_moves &= ~Bitboards::get_diagonal_downwards_left(x, y);
                        } else if (col_x < x && col_y < y){
                            piece_moves &= ~Bitboards::get_diagonal_upwards_left(x, y);
                        } else if (col_x > x && col_y < y){
                            piece_moves &= ~Bitboards::get_diagonal_upwards_right(x, y);
                        }
                    }

                    std::vector<int> positions = Bitboards::bitboard_to_positions(piece_moves);
                    for (auto pos : positions) {
                        moves.push_back(Move(y * 8 + x, pos, piece));
                    }
                    break;
                }
                case Pieces::ROOK:
                {
                    Bitboard piece_moves = Pieces::get_rook_moves(x, y, *this);

                    // get collisions
                    Bitboard collisions = piece_moves & game_bitboard;
                    std::vector<int> collision_positions = Bitboards::bitboard_to_positions(collisions);
                    for (auto pos : collision_positions) {
                        int col_x = pos % 8;
                        int col_y = pos / 8;

                        if (col_x > x) {
                            piece_moves &= ~Bitboards::get_row_segment(y, col_x + 1, 7);
                        } else if (col_x < x){
                            piece_moves &= ~Bitboards::get_row_segment(y, 0, col_x - 1);
                        } else if (col_y > y){
                            piece_moves &= ~Bitboards::get_column_segment(x, col_y + 1, 7);
                        } else if (col_y < y){
                            piece_moves &= ~Bitboards::get_column_segment(x, 0, col_y - 1);
                        }
                    }

                    std::vector<int> positions = Bitboards::bitboard_to_positions(piece_moves);
                    for (auto pos : positions) {
                        moves.push_back(Move(y * 8 + x, pos, piece));
                    }

                    break;
                }
                // TODO others
            }
        }
    }

    return moves;
}

void Game::move(Move move)
{
    // update game bitboards
    Bitboard from_bitboard = Bitboards::get_i(move.from);
    Bitboard to_bitboard = Bitboards::get_i(move.to);

    game_bitboard ^= from_bitboard;
    game_bitboard |= to_bitboard;

    if (is_blacks_turn()) {
        black_bitboard ^= from_bitboard;
        black_bitboard |= to_bitboard;

        // captured enemy piece
        if (white_bitboard & to_bitboard) {
            white_bitboard ^= to_bitboard;
        }
    } else {
        white_bitboard ^= from_bitboard;
        white_bitboard |= to_bitboard;

        // captured enemy piece
        if (black_bitboard & to_bitboard) {
            black_bitboard ^= to_bitboard;
        }
    }

    // move actual pieces
    board[move.from] = Pieces::EMPTY;
    board[move.to] = move.piece;

    // TODO special cases

    // flip sides
    turn = is_blacks_turn() ? Pieces::WHITE : Pieces::BLACK;

    history.push_back(move);
}

void Game::undo()
{
    Move last_move = history.back();
    history.pop_back();

    // update game bitboards
    Bitboard from_bitboard = Bitboards::get_i(last_move.from);
    Bitboard to_bitboard = Bitboards::get_i(last_move.to);

    game_bitboard |= from_bitboard;
    game_bitboard ^= to_bitboard;

    if (is_blacks_turn()) {
        black_bitboard |= from_bitboard;
        black_bitboard ^= to_bitboard;

        // captured enemy piece
        if (last_move.captured != 0) {
            white_bitboard |= to_bitboard;
        }
    } else {
        white_bitboard |= from_bitboard;
        white_bitboard ^= to_bitboard;

        // captured enemy piece
        if (last_move.captured != 0) {
            black_bitboard |= to_bitboard;
        }
    }

    // move actual pieces
    board[last_move.to] = last_move.captured == 0 ? Pieces::EMPTY : last_move.captured;
    board[last_move.from] = last_move.piece;

    // TODO special cases

    // flip sides
    turn = is_blacks_turn() ? Pieces::WHITE : Pieces::BLACK;

}