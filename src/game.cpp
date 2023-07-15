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
    int index = 63;
    for (int i = 0; i < fen_length; ++i) {
        // read number of spaces
        if (isdigit(fen[i])) {
            int num_spaces = int(fen[i] - '0');
            for (int j = 0; j < num_spaces; ++j) {
                board[index--] = Pieces::EMPTY;
            }
        } else if (fen[i] != '/') {
            board[index--] = Pieces::piece_from_name_short(fen[i]);
        }
    }

    // update our game bitboard representation
    game_bitboard = Bitboards::board_to_bitboard(board);
    white_bitboard = Bitboards::board_to_bitboard(board, Pieces::WHITE);
    black_bitboard = Bitboards::board_to_bitboard(board, Pieces::BLACK);

    for (int i = 0; i < 64; ++i) {
        if ((board[i] & Pieces::FILTER_PIECE) == Pieces::KING) {
            if ((board[i] & Pieces::FILTER_COLOR) == Pieces::BLACK) {
                black_king_bitboard = Bitboards::get_i(i);
            } else {
                white_king_bitboard = Bitboards::get_i(i);
            }
        }
    }
}

void Game::print()
{
    for (int y = 7; y >= 0; --y) {
        for (int x = 7; x >= 0; --x) {
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

    Bitboard axis_bitboard = is_whites_turn() ? white_bitboard : black_bitboard;
    // Bitboard ally_bitboard = is_whites_turn() ? white_bitboard : black_bitboard;
    Bitboard king_position;
    // std::vector<int> ally_positions;
    // Bitboards::bitboard_to_positions(ally_positions, ally_bitboard);
    // for (int i = 0; i < ally_positions.size(); ++i) {
    //     if ((ally_positions[i] & Pieces::FILTER_PIECE) == Pieces::KING) {
    //         king_position = Bitboards::get_i(i);
    //         break;
    //     }
    // }

    // why is it flipped? well, in this case, we are checking if the last move just made
    // (whether legal or not) was what put us into check. so while it may be blacks turn,
    // we are checking if white is in check because of the move we made
    Piece target = Pieces::KING | (is_blacks_turn() ? Pieces::WHITE : Pieces::BLACK);
    for (int i = 0; i < 64; ++i) {
        if (board[i] == target) {
            king_position = Bitboards::get_i(i);
            break;
        }
    }

    std::vector<int> axis_positions;
    Bitboards::bitboard_to_positions(axis_positions, axis_bitboard);

    // for (int x = 0; x < 8; ++x) {
    //     for (int y = 0; y < 8; ++y) {
    {
        for (auto & index : axis_positions) {
            //int index = y * 8 + x;
            // if (board[index] == Pieces::EMPTY || (board[index] & Pieces::FILTER_COLOR) == turn) continue;

            Bitboard piece_moves = 0;
        switch (board[index] & Pieces::FILTER_PIECE) {
            case Pieces::PAWN:
                piece_moves = (Pieces::get_pawn_captures(index, *this));
                break;
            case Pieces::KNIGHT:
                piece_moves = Pieces::get_knight_moves(index, *this);
                break;
            case Pieces::BISHOP:
            {
                int x = index % 8;
                int y = index / 8;
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
            }
            case Pieces::ROOK:
            {
                int x = index % 8;
                int y = index / 8;
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
            }
            case Pieces::QUEEN:
            {
                int x = index % 8;
                int y = index / 8;
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
            }
            case Pieces::KING:
                piece_moves = Pieces::get_king_moves(index, *this);
                break;
        }
    
        attacking_squares |= piece_moves;

        if (attacking_squares & king_position) return true;
        }   
    }

    return (attacking_squares & king_position);
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

    /*
     * note: converting these bitboards to positions
     * is JUST faster than storing an array of positions as ints it seems.
     * 
     * However, testing was only done with "simulatons" for checks, that is
     * move/undo was being used a lot.
     */
    std::vector<int> ally_piece_positions;
    Bitboards::bitboard_to_positions(ally_piece_positions, ally_bitboard);

    // for (int j = 0; j < ally_piece_positions.size(); ++j) {
    for (auto & pos : ally_piece_positions) {
        // int pos = ally_piece_positions[j];
        Piece piece = board[pos];

        Bitboard piece_moves = 0;
        // for every piece, get moves based on type
        switch (piece & Pieces::FILTER_PIECE) {
            case Pieces::PAWN:
                piece_moves = Pieces::get_pawn_moves(pos, *this) & not_game_bitboard;
                piece_moves |= (Pieces::get_pawn_captures(pos, *this) & axis_bitboard);
                break;
            case Pieces::KNIGHT:
                piece_moves = Pieces::get_knight_moves(pos, *this);
                break;
            case Pieces::BISHOP:
            {
                int x = pos % 8;
                int y = pos / 8;
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
            }
            case Pieces::ROOK:
            {
                int x = pos % 8;
                int y = pos / 8;
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
            }
            case Pieces::QUEEN:
            {
                int x = pos % 8;
                int y = pos / 8;
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
            }
            case Pieces::KING:
                piece_moves = Pieces::get_king_moves(pos, *this);
                break;
        }

        // skip any pieces with no moves anyway
        // if (piece_moves == 0) continue;

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

void Game::move(Move & move)
{
    // update game bitboards
    Bitboard from_bitboard = Bitboards::get_i(move.from);
    Bitboard to_bitboard = Bitboards::get_i(move.to);

    game_bitboard ^= from_bitboard;
    game_bitboard |= to_bitboard;

    if (is_blacks_turn()) {
        black_bitboard ^= from_bitboard;
        black_bitboard |= to_bitboard;

        // if ((move.piece & Pieces::FILTER_PIECE) == Pieces::KING) {
        //     black_king_bitboard = Bitboards::get_i(move.to);
        // }

        // captured enemy piece
        if (white_bitboard & to_bitboard) {
            white_bitboard ^= to_bitboard;
        }
    } else {
        white_bitboard ^= from_bitboard;
        white_bitboard |= to_bitboard;

        // if ((move.piece & Pieces::FILTER_PIECE) == Pieces::KING) {
        //     white_king_bitboard = Bitboards::get_i(move.to);
        // }

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

        // if ((last_move.piece & Pieces::FILTER_PIECE) == Pieces::KING) {
        //     black_king_bitboard = Bitboards::get_i(last_move.from);
        // }

        // captured enemy piece
        if (last_move.captured != 0) {
            white_bitboard |= to_bitboard;
        }
    } else {
        white_bitboard |= from_bitboard;
        white_bitboard ^= to_bitboard;

        // if ((last_move.piece & Pieces::FILTER_PIECE) == Pieces::KING) {
        //     white_king_bitboard = Bitboards::get_i(last_move.from);
        // }

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