#include <stdio.h>
#include <cctype>
#include <vector>
#include <stack>
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

    // this is actually broken if we arent playing on a standard board (more than 16 pieces per side)
    std::vector<int> white_pieces;
    std::vector<int> black_pieces;
    Bitboards::bitboard_to_positions(white_pieces, white_bitboard);
    Bitboards::bitboard_to_positions(black_pieces, black_bitboard);
    for (int i = 0; i < PIECES_PER_SIDE; ++i) {
        white_piece_positions[num_white_pieces++] = white_pieces[i];
        black_piece_positions[num_black_pieces++] = black_pieces[i];
    }
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
    printf("\n");
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

bool Game::in_check()
{
    Bitboard attacking_squares = 0;

    Bitboard axis_bitboard = is_whites_turn() ? black_bitboard : white_bitboard;
    Bitboard ally_bitboard = is_whites_turn() ? white_bitboard : black_bitboard;
    Bitboard king_position = 0;
    // Piece target_king = (Pieces::KING | (is_whites_turn() ? Pieces::WHITE : Pieces::BLACK));

    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            int index = y * 8 + x;
            if (board[index] == Pieces::EMPTY) continue;
            if (board[index] == Pieces::KING | (is_whites_turn() ? Pieces::WHITE : Pieces::BLACK)) {
                king_position = Bitboards::get_i(index);
                continue;
            } else if ((board[index] & Pieces::FILTER_COLOR) != turn) continue;

            Bitboard piece_moves = 0;
        switch (board[index] & Pieces::FILTER_PIECE) {
            case Pieces::PAWN:
                piece_moves = (Pieces::get_pawn_captures(x, y, *this) & axis_bitboard);
                break;
            case Pieces::KNIGHT:
                piece_moves = Pieces::get_knight_moves(x, y, *this);
                break;
            case Pieces::BISHOP:
                // TEMPORARY RAYCASTING METHOD
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
                break;
            case Pieces::ROOK:
                // temporary raycasting method
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
                break;
            case Pieces::QUEEN:
                // temporary raycasting method
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
                break;
            case Pieces::KING:
                piece_moves = Pieces::get_king_moves(x, y, *this);
                break;
        }
    
        attacking_squares |= piece_moves;
        }   
    }

    return attacking_squares & king_position;
}

void Game::get_moves(std::vector<Move> & moves)
{
    moves.reserve(Constants::MAX_CHESS_MOVES_PER_POSITION);

    /*
     * note: it is MARGINALLY faster to not constnatly call is_blacks_turn.
     * marginally enough for me not to care?
     */
    Bitboard not_game_bitboard = ~game_bitboard;
    Bitboard axis_bitboard = is_blacks_turn() ? white_bitboard : black_bitboard;
    Bitboard ally_bitboard = is_blacks_turn() ? black_bitboard : white_bitboard;
    Bitboard not_axis_bitboard = ~axis_bitboard;
    Bitboard not_ally_bitboard = ~ally_bitboard;
    int num_allied_pieces = is_blacks_turn() ? num_black_pieces : num_white_pieces;
    int * ally_piece_positions = is_blacks_turn() ? black_piece_positions : white_piece_positions;

    for (int j = 0; j < num_allied_pieces; ++j) {
        int pos = ally_piece_positions[j];
        int x = pos % 8;
        int y = pos / 8;
        Piece piece = board[pos];

        Bitboard piece_moves = 0;
        // for every piece, get moves based on type
        switch (piece & Pieces::FILTER_PIECE) {
            case Pieces::PAWN:
                piece_moves = Pieces::get_pawn_moves(x, y, *this) & not_game_bitboard;
                piece_moves |= (Pieces::get_pawn_captures(x, y, *this) & axis_bitboard);
                break;
            case Pieces::KNIGHT:
                piece_moves = Pieces::get_knight_moves(x, y, *this);
                break;
            case Pieces::BISHOP:
                // TEMPORARY RAYCASTING METHOD
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
                break;
            case Pieces::ROOK:
                // temporary raycasting method
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
                break;
            case Pieces::QUEEN:
                // temporary raycasting method
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
                break;
            case Pieces::KING:
                piece_moves = Pieces::get_king_moves(x, y, *this);
                break;
        }

        piece_moves &= not_ally_bitboard;

        std::vector<int> positions;
        Bitboards::bitboard_to_positions(positions, piece_moves);
        for (auto & target_pos : positions) {
            int captured = board[target_pos];
            Move potential_move(pos, target_pos, piece, captured);

            move(potential_move);
            if (!in_check()) {
                moves.push_back(potential_move);
            }
            undo();
        }
    }
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
        for (int i = 0; i < num_black_pieces; ++i) {
            if (black_piece_positions[i] == move.from) {
                black_piece_positions[i] = move.to;
                break;
            }
        }

        // captured enemy piece
        if (white_bitboard & to_bitboard) {
            white_bitboard ^= to_bitboard;

            // find our piece and replace it with the last piece in our list, effectively
            // removing it & shortening the size of our list.
            for (int i = 0; i < num_white_pieces; ++i) {
                if (white_piece_positions[i] == move.to) {
                    white_piece_positions[i] = white_piece_positions[--num_white_pieces];
                    break;
                }
            }
        }
    } else {
        white_bitboard ^= from_bitboard;
        white_bitboard |= to_bitboard;

        for (int i = 0; i < num_white_pieces; ++i) {
            if (white_piece_positions[i] == move.from) {
                white_piece_positions[i] = move.to;
                break;
            }
        }

        // captured enemy piece
        if (black_bitboard & to_bitboard) {
            black_bitboard ^= to_bitboard;
            for (int i = 0; i < num_black_pieces; ++i) {
                if (black_piece_positions[i] == move.to) {
                    black_piece_positions[i] = black_piece_positions[--num_black_pieces];
                    break;
                }
            }
        }
    }

    // move actual pieces
    board[move.from] = Pieces::EMPTY;
    board[move.to] = move.piece;

    // TODO special cases

    // flip sides
    turn = is_blacks_turn() ? Pieces::WHITE : Pieces::BLACK;

    history.push(move);
}

void Game::undo()
{
    Move last_move = history.top();
    history.pop();

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
        
        for (int i = 0; i < num_black_pieces; ++i) {
            if (black_piece_positions[i] == last_move.to) {
                black_piece_positions[i] = last_move.from;
                break;
            }
        }

        // captured enemy piece
        if (last_move.captured != 0) {
            white_bitboard |= to_bitboard;
            white_piece_positions[num_white_pieces++] = last_move.to;
        }
    } else {
        white_bitboard |= from_bitboard;
        white_bitboard ^= to_bitboard;

        for (int i = 0; i < num_white_pieces; ++i) {
            if (white_piece_positions[i] == last_move.to) {
                white_piece_positions[i] = last_move.from;
                break;
            }
        }

        // captured enemy piece
        if (last_move.captured != 0) {
            black_bitboard |= to_bitboard;
            black_piece_positions[num_black_pieces++] = last_move.to;
        }
    }

    // move actual pieces
    board[last_move.to] = last_move.captured == 0 ? Pieces::EMPTY : last_move.captured;
    board[last_move.from] = last_move.piece;

    // TODO special cases

    // flip sides
    turn = is_blacks_turn() ? Pieces::WHITE : Pieces::BLACK;
}