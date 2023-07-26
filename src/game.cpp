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
    char ranks[8] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
    int files[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    int last_move_from = -1;

    if (!history.empty()) {
        last_move_from = history.top().from;
    }

    for (int y = 7; y >= 0; --y) {
        printf("%d | ", files[y]);
        for (int x = 7; x >= 0; --x) {
            Piece piece = board[x + y * 8];
            printf("%c ", piece == Pieces::EMPTY 
                ? ((y * 8 + x != last_move_from) ? ((y % 2 + x) % 2 == 0 ? ',' : '.') : '_')
                : Pieces::name_short(piece)
            );
        }
        printf("\n");
    }
    printf("    ---------------\n    ");
    for (int i = 0; i < 8; ++i) {
        printf("%c ", ranks[i]);
    }
    printf("\n");
    printf("    %c%c - %c%c\n",
        can_white_castle_kingside ? 'K' : 'x',
        can_white_castle_queenside ? 'Q' : 'x',
        can_black_castle_kingside ? 'k' : 'x',
        can_black_castle_queenside ? 'q' : 'x'
    );
    
    printf("\n");

    // printf("Game bitboard:\n");
    // Bitboards::print(game_bitboard);
    // printf("White bitboard:\n");
    // Bitboards::print(white_bitboard);
    // printf("Black bitboard:\n");
    // Bitboards::print(black_bitboard);
    // printf("\n\n\n");
}

Bitboard Game::get_game_bitboard()
{
    return game_bitboard;
}

Bitboard Game::get_white_bitboard()
{
    return white_bitboard;
}

Bitboard Game::get_black_bitboard()
{
    return black_bitboard;
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

void Game::switch_turns()
{
    turn = is_blacks_turn() ? Pieces::WHITE : Pieces::BLACK;
}

Piece Game::get_turn()
{
    return turn;
}

bool Game::in_check()
{
    Bitboard axis_bitboard = is_whites_turn() ? white_bitboard : black_bitboard;

    // for debugging
    if (axis_bitboard == 0ULL) return false;

    Bitboard king_position = 0ULL;
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

    // if kind doesnt exist (testing) just return false
    if (king_position == 0ULL) return false;

    std::vector<int> axis_positions;
    Bitboards::bitboard_to_positions(axis_positions, axis_bitboard);

    for (auto & index : axis_positions) {
        Bitboard piece_moves = 0;
        switch (board[index] & Pieces::FILTER_PIECE) {
            case Pieces::PAWN:
                piece_moves = (Pieces::get_pawn_captures(index, *this));
                break;
            case Pieces::KNIGHT:
                piece_moves = Pieces::get_knight_moves(index, *this);
                break;
            case Pieces::BISHOP:
                piece_moves = Pieces::get_bishop_moves(index, *this);
                break;
            case Pieces::ROOK:
                piece_moves = Pieces::get_rook_moves(index, *this);
                break;
            case Pieces::QUEEN:
                piece_moves = Pieces::get_queen_moves(index, *this);
                break;
            case Pieces::KING:
                piece_moves = Pieces::get_king_moves(index, *this);
                break;
        }

        if (piece_moves & king_position) return true;
    }

    return false;
}

// the REAL ONE
bool Game::is_in_check()
{
    Bitboard axis_bitboard = is_whites_turn() ? black_bitboard : white_bitboard;

    // for debugging
    if (axis_bitboard == 0ULL) return false;

    Bitboard king_position = 0ULL;
    Piece target = Pieces::KING | (is_blacks_turn() ? Pieces::BLACK : Pieces::WHITE);
    for (int i = 0; i < 64; ++i) {
        if (board[i] == target) {
            king_position = Bitboards::get_i(i);
            break;
        }
    }

    // if kind doesnt exist (testing) just return false
    if (king_position == 0ULL) return false;

    std::vector<int> axis_positions;
    Bitboards::bitboard_to_positions(axis_positions, axis_bitboard);

    for (auto & index : axis_positions) {
        Bitboard piece_moves = 0;
        switch (board[index] & Pieces::FILTER_PIECE) {
            case Pieces::PAWN:
                piece_moves = (Pieces::get_pawn_captures(index, *this));
                break;
            case Pieces::KNIGHT:
                piece_moves = Pieces::get_knight_moves(index, *this);
                break;
            case Pieces::BISHOP:
                piece_moves = Pieces::get_bishop_moves(index, *this);
                break;
            case Pieces::ROOK:
                piece_moves = Pieces::get_rook_moves(index, *this);
                break;
            case Pieces::QUEEN:
                piece_moves = Pieces::get_queen_moves(index, *this);
                break;
            case Pieces::KING:
                piece_moves = Pieces::get_king_moves(index, *this);
                break;
        }

        if (piece_moves & king_position) return true;
    }

    return false;
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

    Bitboard attacked_squares = 0ULL;

    // TODO switch this to use recorded individual pieces
    Bitboard king_position;
    Piece target = Pieces::KING | (is_blacks_turn() ? Pieces::BLACK : Pieces::WHITE);
    for (int i = 0; i < 64; ++i) {
        if (board[i] == target) {
            king_position = Bitboards::get_i(i);
            break;
        }
    }

    /*
     * note: converting these bitboards to positions
     * is JUST faster than storing an array of positions as ints it seems.
     * 
     * However, testing was only done with "simulatons" for checks, that is
     * move/undo was being used a lot.
     */
    std::vector<int> ally_piece_positions;
    std::vector<int> axis_piece_positions;
    Bitboards::bitboard_to_positions(ally_piece_positions, ally_bitboard);
    Bitboards::bitboard_to_positions(axis_piece_positions, axis_bitboard);

    // std::vector<int> king_attackers;   // pieces that are attacking the king

    bool is_in_check = false;

    // determine danger zones first
    for (auto & pos : axis_piece_positions) {
        Piece piece = board[pos];
        Bitboard piece_captures = 0ULL;

        // for every piece, get moves based on type
        switch (piece & Pieces::FILTER_PIECE) {
            case Pieces::PAWN:
                switch_turns();
                piece_captures = Pieces::get_pawn_captures(pos, *this);
                switch_turns();
                // if (piece_captures & king_position) {
                //     // we are attacking the king
                //     king_attackers.push_back(pos);
                // }
                break;
            case Pieces::KNIGHT:
                piece_captures = Pieces::get_knight_moves(pos, *this);
                // if (piece_captures & king_position) {
                //     // we are attacking the king
                //     king_attackers.push_back(pos);
                // }
                break;
            case Pieces::BISHOP:
                piece_captures = Pieces::get_bishop_moves(pos, *this);
                // if (piece_captures & king_position) {
                //     // we are attacking the king
                //     king_attackers.push_back(pos);
                // }
                break;
            case Pieces::ROOK:
                piece_captures = Pieces::get_rook_moves(pos, *this);
                // if (piece_captures & king_position) {
                //     // we are attacking the king
                //     king_attackers.push_back(pos);
                // }
                break;
            case Pieces::QUEEN:
                piece_captures = Pieces::get_queen_moves(pos, *this);
                // if (piece_captures & king_position) {
                //     // we are attacking the king
                //     king_attackers.push_back(pos);
                // }
                break;
            case Pieces::KING:
                piece_captures = Pieces::get_king_moves(pos, *this);
                // if (piece_captures & king_position) {
                //     // we are attacking the king
                //     king_attackers.push_back(pos);
                // }
                break;
        }

        attacked_squares |= piece_captures;
    }

    // // update if we are in check or not
    is_in_check = attacked_squares & king_position;

    // if (in_check) {
    //     // if our king attackers is 2, we can ONLY move our king to a safe spot.
    //     if (king_attackers.size() == 2) {
            
    //     } else {    // otherwise, there are a LOT more possibilities.

    //     }
    // }

    /*
     * notes: 
     * if we are in check, there are only a few possible scenarios and responses
     * 
     * First: we are in check by ONE piece. in that case, we can respond by
     * - moving our king to a safe spot
     * - (if it is a sliding piece) blocking that piece
     * - capturing that piece
     *
     * however, if we are blocking or capturing, that piece must NOT be pinned! 
     * In other worse, that piece cannot
     * be in the line of fire between the king and an attacking piece.
     *
     * If we are in check by TWO pieces, we can ONLY respond by moving the king. 
     * if we cannot move the king, its game over.
     *
     *
     * so in order it would be:
     * check if we are in check. if so, determine the attacker
     * 
     */

    for (auto & pos : ally_piece_positions) {
        Piece piece = board[pos];
        Bitboard piece_moves = 0ULL;

        // for every piece, get moves based on type
        switch (piece & Pieces::FILTER_PIECE) {
            case Pieces::PAWN:
            {
                piece_moves = Pieces::get_pawn_moves(pos, *this) & not_game_bitboard;
                piece_moves |= (Pieces::get_pawn_captures(pos, *this) & axis_bitboard);

                // if first move ignore
                if (history.empty()) break;                
                // check if we can en passant
                int y = pos / 8;
                int x = pos % 8;
                Move last_move = history.top();
                if (
                    //((is_blacks_turn() && y == 3) || (!is_blacks_turn() && y == 4))     // our piece is in the right position
                    (std::abs(last_move.to - pos) == 1)
                    && (std::abs(last_move.to % 8 - x) == 1)
                    && (last_move.piece & Pieces::FILTER_PIECE) == Pieces::PAWN         // enemy's last move was a double pawn jump
                    && std::abs(last_move.to - last_move.from) == 16
                ) {
                    // rather than add overhead later, we can just manually add the move now
                    int target_pos = is_blacks_turn() ? last_move.to - 8 : last_move.to + 8;

                    Move potential_move(
                        pos, target_pos, piece, last_move.piece, EN_PASSANT
                    );
                    move(potential_move);
                    if (!in_check()) {
                        moves.push_back(potential_move);
                    }
                    undo();
                }
                break;
            }
            case Pieces::KNIGHT:
                piece_moves = Pieces::get_knight_moves(pos, *this);
                break;
            case Pieces::BISHOP:
                piece_moves = Pieces::get_bishop_moves(pos, *this);
                break;
            case Pieces::ROOK:
                piece_moves = Pieces::get_rook_moves(pos, *this);
                break;
            case Pieces::QUEEN:
                piece_moves = Pieces::get_queen_moves(pos, *this);
                break;
            case Pieces::KING:
                piece_moves = Pieces::get_king_moves(pos, *this);

                if (!is_in_check) {
                    // check if we can castle
                    if (is_blacks_turn()) {
                        MoveFlag castling_rights = CASTLE_QUEENSIDE_ALLOWED;
                        if (can_black_castle_kingside && can_black_castle_queenside) {
                            castling_rights = CASTLE_BOTH_ALLOWED;
                        } else if (can_black_castle_kingside) {
                            castling_rights = CASTLE_KINGSIDE_ALLOWED;
                        }

                        if (can_black_castle_kingside) {
                            // make sure there is a clear path
                            // and we dont pass through check
                            if ((
                                Bitboards::BLACK_KINGSIDE_CASTLE & game_bitboard) == 0
                                && (Bitboards::BLACK_KINGSIDE_CASTLE & attacked_squares) == 0
                                && board[56] == (Pieces::BLACK | Pieces::ROOK) // TODO REMOVE
                            ) {
                                Move potential_move(
                                    pos, pos - 2, piece, Pieces::EMPTY, castling_rights
                                );
                                move(potential_move);
                                if (!in_check()) {
                                    moves.push_back(potential_move);
                                }
                                undo();
                            }
                        }
                        if (can_black_castle_queenside) {
                            if ((
                                Bitboards::BLACK_QUEENSIDE_CASTLE_SPACES & game_bitboard) == 0
                                && (Bitboards::BLACK_QUEENSIDE_CASTLE_ATTACK_FREE & attacked_squares) == 0
                                && board[63] == (Pieces::BLACK | Pieces::ROOK) // TODO REMOVE
                            ) {
                                Move potential_move(
                                    pos, pos + 2, piece, Pieces::EMPTY, castling_rights
                                );

                                move(potential_move);
                                if (!in_check()) {
                                    moves.push_back(potential_move);
                                }
                                undo();
                            }
                        }
                    } else {
                        MoveFlag castling_rights = CASTLE_QUEENSIDE_ALLOWED;
                        if (can_white_castle_kingside && can_white_castle_queenside) {
                            castling_rights = CASTLE_BOTH_ALLOWED;
                        } else if (can_white_castle_kingside) {
                            castling_rights = CASTLE_KINGSIDE_ALLOWED;
                        }

                        if (can_white_castle_kingside) {
                            // make sure there is a clear path
                            // and we dont pass through check
                            if ((
                                Bitboards::WHITE_KINGSIDE_CASTLE & game_bitboard) == 0
                                && (Bitboards::WHITE_KINGSIDE_CASTLE & attacked_squares) == 0
                                && board[0] == (Pieces::WHITE | Pieces::ROOK) // TODO REMOVE
                            ) {
                                Move potential_move(
                                    pos, pos - 2, piece, Pieces::EMPTY, castling_rights
                                );

                                move(potential_move);
                                if (!in_check()) {
                                    moves.push_back(potential_move);
                                }
                                undo();
                            }
                        }
                        if (can_white_castle_queenside) {
                            if ((
                                Bitboards::WHITE_QUEENSIDE_CASTLE_SPACES & game_bitboard) == 0
                                && (Bitboards::WHITE_QUEENSIDE_CASTLE_ATTACK_FREE & attacked_squares) == 0
                                && board[7] == (Pieces::WHITE | Pieces::ROOK) // TODO REMOVE
                            ) {
                                Move potential_move(
                                    pos, pos + 2, piece, Pieces::EMPTY, castling_rights
                                );
                                move(potential_move);
                                if (!in_check()) {
                                    moves.push_back(potential_move);
                                }
                                undo();
                            }
                        }
                    }
                }
                break;
        }

        // omit capture of allied pieces
        piece_moves &= not_ally_bitboard;

        // skip any pieces with no moves
        if (piece_moves == 0) continue;

        // apply legal moves

        // are we in check ?

        // convert set bits to individual moves
        std::vector<int> positions;
        Bitboards::bitboard_to_positions(positions, piece_moves);
        for (auto & target_pos : positions) {
            int captured = board[target_pos];
            Move potential_move(pos, target_pos, piece, captured);

            if ((piece & Pieces::FILTER_PIECE) == Pieces::PAWN) {
                if (target_pos / 8 == 7 || target_pos < 8) {
                    Move promotions[4] = {
                        Move(pos, target_pos, Pieces::QUEEN | turn, captured, PROMOTION),
                        Move(pos, target_pos, Pieces::ROOK | turn, captured, PROMOTION),
                        Move(pos, target_pos, Pieces::KNIGHT | turn, captured, PROMOTION),
                        Move(pos, target_pos, Pieces::BISHOP | turn, captured, PROMOTION)
                    };

                    move(promotions[0]);
                    if (!in_check()) {
                        for (int i = 0; i < 4; ++i) {
                            moves.push_back(promotions[i]);
                        }
                    }
                    undo();

                    continue;
                }
            }


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

    // move actual pieces
    board[move.from] = Pieces::EMPTY;
    board[move.to] = move.piece;

    switch(move.flag) {
        case EN_PASSANT:
        {
            int target_square = is_blacks_turn() ? move.to + 8 : move.to - 8;
            board[target_square] = Pieces::EMPTY;
            Bitboard en_passant_target = Bitboards::get_i(target_square);

            // update bitboards
            game_bitboard |= to_bitboard;
            game_bitboard &= ~from_bitboard;
            game_bitboard &= ~en_passant_target;
            if (is_blacks_turn()) {
                black_bitboard |= to_bitboard;
                black_bitboard &= ~from_bitboard;
                white_bitboard &= ~en_passant_target;
            } else {
                white_bitboard |= to_bitboard;
                white_bitboard &= ~from_bitboard;
                black_bitboard &= ~en_passant_target;
            }

            break;
        }

        case CASTLE_BOTH_ALLOWED:
        case CASTLE_KINGSIDE_ALLOWED:
        case CASTLE_QUEENSIDE_ALLOWED:
        {
            // king has been moved to its proper position, now we must move the rook.
            if (is_blacks_turn()) {
                can_black_castle_kingside = false;
                can_black_castle_queenside = false;
            } else {
                can_white_castle_kingside = false;
                can_white_castle_queenside = false;
            }

            // update bitboard for king position
            game_bitboard |= to_bitboard;
            game_bitboard &= ~from_bitboard;
            if (is_blacks_turn()) {
                black_bitboard |= to_bitboard;
                black_bitboard &= ~from_bitboard;
            } else {
                white_bitboard |= to_bitboard;
                white_bitboard &= ~from_bitboard;
            }

            // king vs queenside rook
            if (move.to > move.from) {  // queenside
                board[move.to + 2] = Pieces::EMPTY;
                board[move.to - 1] = Pieces::ROOK | turn;

                game_bitboard &= ~(to_bitboard << 2);
                game_bitboard |= (to_bitboard >> 1);

                if (is_blacks_turn()) {
                    black_bitboard &= ~(to_bitboard << 2);
                    black_bitboard |= (to_bitboard >> 1);
                } else {
                    white_bitboard &= ~(to_bitboard << 2);
                    white_bitboard |= (to_bitboard >> 1);
                }
            } else {    // kingside
                board[move.to - 1] = Pieces::EMPTY;
                board[move.to + 1] = Pieces::ROOK | turn;

                game_bitboard &= ~(to_bitboard >> 1);
                game_bitboard |= (to_bitboard << 1);

                if (is_blacks_turn()) {
                    black_bitboard &= ~(to_bitboard >> 1);
                    black_bitboard |= (to_bitboard << 1);
                } else {
                    white_bitboard &= ~(to_bitboard >> 1);
                    white_bitboard |= (to_bitboard << 1);
                }
            }
            break;
        }
        default:
        {
            game_bitboard &= ~from_bitboard;
            game_bitboard |= to_bitboard;
            if (is_blacks_turn()) {
                black_bitboard &= ~from_bitboard;
                black_bitboard |= to_bitboard;

                if (move.captured != 0) {
                    white_bitboard &= ~to_bitboard;
                }
            } else {
                white_bitboard &= ~from_bitboard;
                white_bitboard |= to_bitboard;

                if (move.captured != 0) {
                    black_bitboard &= ~to_bitboard;
                }
            }

            // update if we have moved for the first time for castling
            switch(move.piece & Pieces::FILTER_PIECE) {
                case Pieces::ROOK:
                    if (is_blacks_turn()) {
                        if (can_black_castle_queenside && move.from == 63) {
                            can_black_castle_queenside = false;
                            move.flag = QUEENSIDE_ROOK_FIRST_MOVE;
                        } else if(can_black_castle_kingside && move.from == 56) {
                            can_black_castle_kingside = false;
                            move.flag = KINGSIDE_ROOK_FIRST_MOVE;
                        }
                    } else {
                        if (can_white_castle_queenside && move.from == 7) {
                            can_white_castle_queenside = false;
                            move.flag = QUEENSIDE_ROOK_FIRST_MOVE;
                        } else if (can_white_castle_kingside && move.from == 0) {
                            can_white_castle_kingside = false;
                            move.flag = KINGSIDE_ROOK_FIRST_MOVE;
                        }
                    }
                    break;
                case Pieces::KING:
                    if (is_blacks_turn()) {
                        if (can_black_castle_kingside || can_black_castle_queenside) {
                            if (can_black_castle_kingside && can_black_castle_queenside) {
                                move.flag = KING_FIRST_MOVE;
                            } else if (can_black_castle_kingside) {
                                move.flag = KINGSIDE_ROOK_FIRST_MOVE;
                            } else if (can_black_castle_queenside) {
                                move.flag = QUEENSIDE_ROOK_FIRST_MOVE;
                            }
                            can_black_castle_kingside = false;
                            can_black_castle_queenside = false;
                        }
                    } else {
                        if (can_white_castle_kingside || can_white_castle_queenside) {
                            if (can_white_castle_kingside && can_white_castle_queenside) {
                                move.flag = KING_FIRST_MOVE;
                            } else if (can_white_castle_kingside) {
                                move.flag = KINGSIDE_ROOK_FIRST_MOVE;
                            } else if (can_white_castle_queenside) {
                                move.flag = QUEENSIDE_ROOK_FIRST_MOVE;
                            }
                            can_white_castle_kingside = false;
                            can_white_castle_queenside = false;
                        }
                    }
                    break;
            }
            break;
        }
    }

    // flip sides
    switch_turns();

    history.push(move);
}

void Game::undo()
{
    // flip sides
    switch_turns();

    Move last_move = history.top();
    history.pop();

    // update game bitboards
    Bitboard from_bitboard = Bitboards::get_i(last_move.from);
    Bitboard to_bitboard = Bitboards::get_i(last_move.to);

    // move actual pieces
    board[last_move.to] = last_move.captured == 0 ? Pieces::EMPTY : last_move.captured;
    board[last_move.from] = last_move.piece;

    switch(last_move.flag) {
        case EN_PASSANT:
        {
            board[last_move.to] = Pieces::EMPTY;
            int target_square = is_blacks_turn() ? last_move.to + 8 : last_move.to - 8;
            board[target_square] = last_move.captured;
            Bitboard en_passant_target = Bitboards::get_i(target_square);

            game_bitboard &= ~to_bitboard;
            game_bitboard |= from_bitboard;
            game_bitboard |= en_passant_target;
            if (is_blacks_turn()) {
                black_bitboard &= ~to_bitboard;
                black_bitboard |= from_bitboard;
                white_bitboard |= en_passant_target;
            } else {
                white_bitboard &= ~to_bitboard;
                white_bitboard |= from_bitboard;
                black_bitboard |= en_passant_target;
            }
            break;
        }
        case KING_FIRST_MOVE:
            if (is_blacks_turn()) {
                can_black_castle_kingside = true;
                can_black_castle_queenside = true;
            } else {
                can_white_castle_kingside = true;
                can_white_castle_queenside = true;
            }
            game_bitboard |= from_bitboard;
            game_bitboard &= ~to_bitboard;

            if (is_blacks_turn()) {
                black_bitboard |= from_bitboard;
                black_bitboard &= ~to_bitboard;

                if (last_move.captured != 0) {
                    white_bitboard |= to_bitboard;
                    game_bitboard |= to_bitboard;
                }
            } else {
                white_bitboard |= from_bitboard;
                white_bitboard &= ~to_bitboard;

                if (last_move.captured != 0) {
                    black_bitboard |= to_bitboard;
                    game_bitboard |= to_bitboard;
                }
            }
            break;
        case KINGSIDE_ROOK_FIRST_MOVE:
            if (is_blacks_turn()) {
                can_black_castle_kingside = true;
            } else {
                can_white_castle_kingside = true;
            }
            game_bitboard |= from_bitboard;
            game_bitboard &= ~to_bitboard;

            if (is_blacks_turn()) {
                black_bitboard |= from_bitboard;
                black_bitboard &= ~to_bitboard;

                if (last_move.captured != 0) {
                    white_bitboard |= to_bitboard;
                    game_bitboard |= to_bitboard;
                }
            } else {
                white_bitboard |= from_bitboard;
                white_bitboard &= ~to_bitboard;

                if (last_move.captured != 0) {
                    black_bitboard |= to_bitboard;
                    game_bitboard |= to_bitboard;
                }
            }
            break;
        case QUEENSIDE_ROOK_FIRST_MOVE:
            if (is_blacks_turn()) {
                can_black_castle_queenside = true;
            } else {
                can_white_castle_queenside = true;
            }
            
            game_bitboard |= from_bitboard;
            game_bitboard &= ~to_bitboard;

            if (is_blacks_turn()) {
                black_bitboard |= from_bitboard;
                black_bitboard &= ~to_bitboard;

                if (last_move.captured != 0) {
                    white_bitboard |= to_bitboard;
                    game_bitboard |= to_bitboard;
                }
            } else {
                white_bitboard |= from_bitboard;
                white_bitboard &= ~to_bitboard;

                if (last_move.captured != 0) {
                    black_bitboard |= to_bitboard;
                    game_bitboard |= to_bitboard;
                }
            }
            break;
        case CASTLE_BOTH_ALLOWED:
        {
            // king was moved back, lets move the rook back
            if (is_blacks_turn()) {
                can_black_castle_kingside = true;
                can_black_castle_queenside = true;
            } else {
                can_white_castle_kingside = true;
                can_white_castle_queenside = true;
            }

            game_bitboard &= ~to_bitboard;
            game_bitboard |= from_bitboard;
            if (is_blacks_turn()) {
                black_bitboard &= ~to_bitboard;
                black_bitboard |= from_bitboard;
            } else {
                white_bitboard &= ~to_bitboard;
                white_bitboard |= from_bitboard;
            }

            // king vs queenside rook
            if (last_move.to > last_move.from) {  // queenside
                board[last_move.to + 2] = Pieces::ROOK | turn;
                board[last_move.to - 1] = Pieces::EMPTY;

                game_bitboard |= (to_bitboard << 2);
                game_bitboard &= ~(to_bitboard >> 1);

                if (is_blacks_turn()) {
                    black_bitboard |= (to_bitboard << 2);
                    black_bitboard &= ~(to_bitboard >> 1);
                } else {
                    white_bitboard |= (to_bitboard << 2);
                    white_bitboard &= ~(to_bitboard >> 1);
                }
            } else {    // kingside
                board[last_move.to - 1] = Pieces::ROOK | turn;
                board[last_move.to + 1] = Pieces::EMPTY;

                game_bitboard |= (to_bitboard >> 1);
                game_bitboard &= ~(to_bitboard << 1);

                if (is_blacks_turn()) {
                    black_bitboard |= (to_bitboard >> 1);
                    black_bitboard &= ~(to_bitboard << 1);
                } else {
                    white_bitboard |= (to_bitboard >> 1);
                    white_bitboard &= ~(to_bitboard << 1);
                }
            }
            break;
        }
        case CASTLE_KINGSIDE_ALLOWED:
        {
            // king was moved back, lets move the rook back
            if (is_blacks_turn()) {
                can_black_castle_kingside = true;
            } else {
                can_white_castle_kingside = true;
            }

            game_bitboard &= ~to_bitboard;
            game_bitboard |= from_bitboard;
            if (is_blacks_turn()) {
                black_bitboard &= ~to_bitboard;
                black_bitboard |= from_bitboard;
            } else {
                white_bitboard &= ~to_bitboard;
                white_bitboard |= from_bitboard;
            }

            // king vs queenside rook
            if (last_move.to > last_move.from) {  // queenside
                board[last_move.to + 2] = Pieces::ROOK | turn;
                board[last_move.to - 1] = Pieces::EMPTY;

                game_bitboard |= (to_bitboard << 2);
                game_bitboard &= ~(to_bitboard >> 1);

                if (is_blacks_turn()) {
                    black_bitboard |= (to_bitboard << 2);
                    black_bitboard &= ~(to_bitboard >> 1);
                } else {
                    white_bitboard |= (to_bitboard << 2);
                    white_bitboard &= ~(to_bitboard >> 1);
                }
            } else {    // kingside
                board[last_move.to - 1] = Pieces::ROOK | turn;
                board[last_move.to + 1] = Pieces::EMPTY;

                game_bitboard |= (to_bitboard >> 1);
                game_bitboard &= ~(to_bitboard << 1);

                if (is_blacks_turn()) {
                    black_bitboard |= (to_bitboard >> 1);
                    black_bitboard &= ~(to_bitboard << 1);
                } else {
                    white_bitboard |= (to_bitboard >> 1);
                    white_bitboard &= ~(to_bitboard << 1);
                }
            }
            break;
        }
        case CASTLE_QUEENSIDE_ALLOWED:
        {
            // king was moved back, lets move the rook back
            if (is_blacks_turn()) {
                can_black_castle_queenside = true;
            } else {
                can_white_castle_queenside = true;
            }

            game_bitboard &= ~to_bitboard;
            game_bitboard |= from_bitboard;
            if (is_blacks_turn()) {
                black_bitboard &= ~to_bitboard;
                black_bitboard |= from_bitboard;
            } else {
                white_bitboard &= ~to_bitboard;
                white_bitboard |= from_bitboard;
            }

            // king vs queenside rook
            if (last_move.to > last_move.from) {  // queenside
                board[last_move.to + 2] = Pieces::ROOK | turn;
                board[last_move.to - 1] = Pieces::EMPTY;

                game_bitboard |= (to_bitboard << 2);
                game_bitboard &= ~(to_bitboard >> 1);

                if (is_blacks_turn()) {
                    black_bitboard |= (to_bitboard << 2);
                    black_bitboard &= ~(to_bitboard >> 1);
                } else {
                    white_bitboard |= (to_bitboard << 2);
                    white_bitboard &= ~(to_bitboard >> 1);
                }
            } else {    // kingside
                board[last_move.to - 1] = Pieces::ROOK | turn;
                board[last_move.to + 1] = Pieces::EMPTY;

                game_bitboard |= (to_bitboard >> 1);
                game_bitboard &= ~(to_bitboard << 1);

                if (is_blacks_turn()) {
                    black_bitboard |= (to_bitboard >> 1);
                    black_bitboard &= ~(to_bitboard << 1);
                } else {
                    white_bitboard |= (to_bitboard >> 1);
                    white_bitboard &= ~(to_bitboard << 1);
                }
            }
            break;
        }
        case PROMOTION:
            board[last_move.from] = Pieces::PAWN | turn;
            game_bitboard |= from_bitboard;
            game_bitboard &= ~to_bitboard;

            if (is_blacks_turn()) {
                black_bitboard |= from_bitboard;
                black_bitboard &= ~to_bitboard;

                if (last_move.captured != 0) {
                    white_bitboard |= to_bitboard;
                    game_bitboard |= to_bitboard;
                }
            } else {
                white_bitboard |= from_bitboard;
                white_bitboard &= ~to_bitboard;

                if (last_move.captured != 0) {
                    black_bitboard |= to_bitboard;
                    game_bitboard |= to_bitboard;
                }
            }
            break;
        default:
            game_bitboard |= from_bitboard;
            game_bitboard &= ~to_bitboard;

            if (is_blacks_turn()) {
                black_bitboard |= from_bitboard;
                black_bitboard &= ~to_bitboard;

                if (last_move.captured != 0) {
                    white_bitboard |= to_bitboard;
                    game_bitboard |= to_bitboard;
                }
            } else {
                white_bitboard |= from_bitboard;
                white_bitboard &= ~to_bitboard;

                if (last_move.captured != 0) {
                    black_bitboard |= to_bitboard;
                    game_bitboard |= to_bitboard;
                }
            }
            break;
    }
}