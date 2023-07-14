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
    Bitboards::print(black_bitboard);
    Bitboards::print(white_bitboard);
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

    Bitboard axis_bitboard = is_whites_turn() ? black_bitboard : white_bitboard;
    Bitboard ally_bitboard = is_whites_turn() ? white_bitboard : black_bitboard;
    Bitboard not_axis_bitboard = ~axis_bitboard;
    Bitboard not_ally_bitboard = ~ally_bitboard;

    // get rid of this looping someday...
    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            int i = y * 8 + x;
            Piece piece = board[y * 8 + x];
            
            if (piece == 0 || (piece & Pieces::FILTER_COLOR) != turn) continue;

            // for every piece, get moves based on type
            switch (piece & Pieces::FILTER_PIECE) {
                case Pieces::PAWN:
                {
                    Bitboard piece_moves = Pieces::get_pawn_moves(x, y, *this);
                    piece_moves &= not_ally_bitboard;
                    piece_moves |= (Pieces::get_pawn_captures(x, y, *this) & axis_bitboard);

                    std::vector<int> positions = Bitboards::bitboard_to_positions(piece_moves);
                    for (auto pos : positions) {
                        int captured = board[pos];
                        moves.push_back(Move(i, pos, piece, captured));
                    }

                    break;
                }
                case Pieces::KNIGHT:
                {
                    Bitboard piece_moves = Pieces::get_knight_moves(x, y, *this);
                    piece_moves &= not_ally_bitboard;
                    std::vector<int> positions = Bitboards::bitboard_to_positions(piece_moves);
                    for (auto pos : positions) {
                        int captured = board[pos];
                        moves.push_back(Move(i, pos, piece, captured));
                    }
                    break;
                }
                case Pieces::BISHOP:
                {
                    // TEMPORARY RAYCASTING METHOD
                    Bitboard piece_moves = 0;

                    for (int i = 1; (x + i < 8) && (y + i < 8); ++i) {
                        Bitboard target_square = Bitboards::get(x + i, y + i);
                        piece_moves |= target_square;
                        if (game_bitboard & target_square) break;
                    }

                    for (int i = 1; (x + -i >= 0) && (y + i < 8); ++i) {
                        Bitboard target_square = Bitboards::get(x - i, y + i);
                        piece_moves |= target_square;
                        if (game_bitboard & target_square) break;
                    }

                    for (int i = 1; (x + i < 8) && (y + -i >= 0); ++i) {
                        Bitboard target_square = Bitboards::get(x + i, y - i);
                        piece_moves |= target_square;
                        if (game_bitboard & target_square) break;
                    }

                    for (int i = 1; (x + -i >= 0) && (y + -i >= 0); ++i) {
                        Bitboard target_square = Bitboards::get(x - i, y - i);
                        piece_moves |= target_square;
                        if (game_bitboard & target_square) break;
                    }

                    piece_moves &= not_ally_bitboard;

                    std::vector<int> positions = Bitboards::bitboard_to_positions(piece_moves);
                    for (auto pos : positions) {
                        int captured = board[pos];
                        moves.push_back(Move(i, pos, piece, captured));
                    }
                    // Bitboards::print(piece_moves);
                    break;
                }
                case Pieces::ROOK:
                {
                    // temporary raycasting method
                    Bitboard piece_moves = 0;

                    for (int i = x + 1; i < 8; ++i) {
                        Bitboard target_square = Bitboards::get(i, y);
                        // if there is a piece on target square, bye bye
                        piece_moves |= target_square;
                        if (game_bitboard & target_square) break;
                    }

                    for (int i = x - 1; i >= 0; --i) {
                        Bitboard target_square = Bitboards::get(i, y);
                        // if there is a piece on target square, bye bye
                        piece_moves |= target_square;
                        if (game_bitboard & target_square) break;
                    }

                    for (int i = y + 1; i < 8; ++i) {
                        Bitboard target_square = Bitboards::get(x, i);
                        // if there is a piece on target square, bye bye
                        piece_moves |= target_square;
                        if (game_bitboard & target_square) break;
                    }

                    for (int i = y - 1; i >= 0; --i) {
                        Bitboard target_square = Bitboards::get(x, i);
                        // if there is a piece on target square, bye bye
                        piece_moves |= target_square;
                        if (game_bitboard & target_square) break;
                    }

                    piece_moves &= not_ally_bitboard;

                    std::vector<int> positions = Bitboards::bitboard_to_positions(piece_moves);
                    for (auto pos : positions) {
                        int captured = board[pos];
                        moves.push_back(Move(i, pos, piece, captured));
                    }


                    break;
                }
                case Pieces::QUEEN:
                {
                    // temporary raycasting method
                    Bitboard piece_moves = 0;

                    for (int i = x + 1; i < 8; ++i) {
                        Bitboard target_square = Bitboards::get(i, y);
                        // if there is a piece on target square, bye bye
                        piece_moves |= target_square;
                        if (game_bitboard & target_square) break;
                    }

                    for (int i = x - 1; i >= 0; --i) {
                        Bitboard target_square = Bitboards::get(i, y);
                        // if there is a piece on target square, bye bye
                        piece_moves |= target_square;
                        if (game_bitboard & target_square) break;
                    }

                    for (int i = y + 1; i < 8; ++i) {
                        Bitboard target_square = Bitboards::get(x, i);
                        // if there is a piece on target square, bye bye
                        piece_moves |= target_square;
                        if (game_bitboard & target_square) break;
                    }

                    for (int i = y - 1; i >= 0; --i) {
                        Bitboard target_square = Bitboards::get(x, i);
                        // if there is a piece on target square, bye bye
                        piece_moves |= target_square;
                        if (game_bitboard & target_square) break;
                    }

                    for (int i = 1; (x + i < 8) && (y + i < 8); ++i) {
                        Bitboard target_square = Bitboards::get(x + i, y + i);
                        piece_moves |= target_square;
                        if (game_bitboard & target_square) break;
                    }

                    for (int i = 1; (x + -i >= 0) && (y + i < 8); ++i) {
                        Bitboard target_square = Bitboards::get(x - i, y + i);
                        piece_moves |= target_square;
                        if (game_bitboard & target_square) break;
                    }

                    for (int i = 1; (x + i < 8) && (y + -i >= 0); ++i) {
                        Bitboard target_square = Bitboards::get(x + i, y - i);
                        piece_moves |= target_square;
                        if (game_bitboard & target_square) break;
                    }

                    for (int i = 1; (x + -i >= 0) && (y + -i >= 0); ++i) {
                        Bitboard target_square = Bitboards::get(x - i, y - i);
                        piece_moves |= target_square;
                        if (game_bitboard & target_square) break;
                    }

                    piece_moves &= not_ally_bitboard;

                    std::vector<int> positions = Bitboards::bitboard_to_positions(piece_moves);
                    for (auto pos : positions) {
                        int captured = board[pos];
                        moves.push_back(Move(i, pos, piece, captured));
                    }
                    break;
                }
                case Pieces::KING:
                {
                    Bitboard piece_moves = Pieces::get_king_moves(x, y, *this);
                    piece_moves &= not_ally_bitboard;
                    std::vector<int> positions = Bitboards::bitboard_to_positions(piece_moves);
                    for (auto pos : positions) {
                        int captured = board[pos];
                        moves.push_back(Move(i, pos, piece, captured));
                    }
                    break;
                }
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

    // put piece back in from, only remove piece at to if there was nothing there
    // in the first place (we didn't capture)
    game_bitboard |= from_bitboard;
    if (last_move.captured == 0) game_bitboard ^= to_bitboard;

    if (!is_blacks_turn()) {
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