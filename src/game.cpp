#include <stdio.h>
#include <cctype>
#include <vector>
#include <stack>
#include "game.h"
#include "piece.h"
#include "move.h"

Game::Game(std::string fen)
{
    read_fen(fen);
}

void Game::read_fen(std::string fen)
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
    // printf("    %c%c - %c%c\n",
    //     can_white_castle_kingside ? 'K' : 'x',
    //     can_white_castle_queenside ? 'Q' : 'x',
    //     can_black_castle_kingside ? 'k' : 'x',
    //     can_black_castle_queenside ? 'q' : 'x'
    // );
    
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

Move Game::get_last_move()
{
    return history.top();
}

bool Game::is_history_empty()
{
    return history.empty();
}

void Game::pop_last_move()
{
    history.pop();
}

void Game::push_to_history(Move move)
{
    history.push(move);
}

std::stack<Move> Game::get_history()
{
    return history;
}

std::vector<Move> Game::get_moves_at_square(int sq)
{
    std::vector<Move> out_moves;

    if (get(sq) == 0) return out_moves; // empty
    if ((get(sq) & Pieces::FILTER_COLOR) != turn) return out_moves; // other team

    std::vector<Move> moves;
    get_moves(moves);

    int moves_size = moves.size();
    for (int i = 0; i < moves_size; ++i)
    {
        if (moves[i].from == sq) {
            out_moves.push_back(moves[i]);
        }
    }

    return out_moves;
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

// Bitboard Game::get_attacked_squares(Bitboard pieces_of_color)
// {
//     std::vector<int> piece_positions;
//     Bitboards::bitboard_to_positions(piece_positions, pieces_of_color);
//     Bitboard attacked_squares = 0ULL;

//     for (auto & pos : piece_positions) {
//         Piece piece = board[pos];

//         // for every piece, get moves based on type
//         switch (piece & Pieces::FILTER_PIECE) {
//             case Pieces::PAWN:
//                 switch_turns();
//                 attacked_squares |= Pieces::get_pawn_captures(pos, *this);
//                 switch_turns();
//                 break;
//             case Pieces::KNIGHT:
//                 attacked_squares |= Pieces::get_knight_moves(pos, *this);
//                 break;
//             case Pieces::BISHOP:
//                 attacked_squares |= Pieces::get_bishop_moves(pos, *this);
//                 break;
//             case Pieces::ROOK:
//                 attacked_squares |= Pieces::get_rook_moves(pos, *this);
//                 break;
//             case Pieces::QUEEN:
//                 attacked_squares |= Pieces::get_queen_moves(pos, *this);
//                 break;
//             case Pieces::KING:
//                 attacked_squares |= Pieces::get_king_moves(pos, *this);
//                 break;
//         }
//     }

//     return attacked_squares;
// }

// Bitboard Game::get_attacked_squares_ignore_king(Bitboard pieces)
// {
    
// }

// returns true/false if the last move made resulted in a check
bool Game::last_move_resulted_in_check()
{
    // colors are flipped because we're checking the LAST move made;
    // the turn has already flipped.

    Bitboard axis_bitboard = is_whites_turn() ? white_bitboard : black_bitboard;
 
    // TODO change
    Bitboard king_position = 0ULL;
    Piece target = Pieces::KING | (is_blacks_turn() ? Pieces::WHITE : Pieces::BLACK);
    for (int i = 0; i < 64; ++i) {
        if (board[i] == target) {
            king_position = Bitboards::get_i(i);
            break;
        }
    }

    if (axis_bitboard == 0ULL) return false;    // early escape
    if (king_position == 0ULL) return false;    // if kind doesnt exist (testing) just return false

    // for each position, populate "danger" squares

    while (axis_bitboard != 0ULL)
    {
        int index = Bitboards::pop_lsb(axis_bitboard);
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
     * note: it is MARGINALLY faster to not constantly call is_blacks_turn.
     * marginally enough for me not to care?
     */
    Bitboard not_game_bitboard = ~game_bitboard;
    Bitboard axis_bitboard = is_blacks_turn() ? white_bitboard : black_bitboard;
    Bitboard ally_bitboard = is_blacks_turn() ? black_bitboard : white_bitboard;
    Bitboard not_axis_bitboard = ~axis_bitboard;
    Bitboard not_ally_bitboard = ~ally_bitboard;

    Bitboard attacked_squares = 0ULL;               // tiles attacked by the enemy
    Bitboard attacked_squares_king_xray = 0ULL;     // tiles attacked by enemy (king not accounted for in blockers)

    // TODO switch this to use recorded individual pieces
    Bitboard king_position;         // bitboard of the king
    int king_x = 0;
    int king_y = 0;
    Piece target = Pieces::KING | (is_blacks_turn() ? Pieces::BLACK : Pieces::WHITE);
    for (int i = 0; i < 64; ++i) {
        if (board[i] == target) {
            king_position = Bitboards::get_i(i);
            king_x = i % 8;
            king_y = i / 8;
            break;
        }
    }

    // std::vector<int> ally_piece_positions;
    // std::vector<int> axis_piece_positions;
    // Bitboards::bitboard_to_positions(ally_piece_positions, ally_bitboard);
    // Bitboards::bitboard_to_positions(axis_piece_positions, axis_bitboard);

    Bitboard attacks_on_king = 0ULL;    // pieces + attacks against the king. These spaces can be blocked/captured to stop it
    int num_king_attackers = 0;         // determine if we cannot block a check

    Bitboard blocker_no_king = game_bitboard & ~king_position;      // so we can "x-ray" the king. represents the game bitboard with the active king removed

    bool is_in_check = false;

    // determine danger zones first
    Bitboard axis_bitboard_i = axis_bitboard;
    while(axis_bitboard_i != 0ULL) {
    // for (auto & pos : axis_piece_positions) {
        int pos = Bitboards::pop_lsb(axis_bitboard_i);
        Piece piece = board[pos];
        Bitboard piece_captures = 0ULL;

        int pos_x = pos % 8;
        int pos_y = pos / 8;

        // for every piece, get moves based on type
        switch (piece & Pieces::FILTER_PIECE) {
            case Pieces::PAWN:
                // todo fix this sob
                switch_turns();
                piece_captures = Pieces::get_pawn_captures(pos, *this);
                switch_turns();
                if (piece_captures & king_position) {
                    // we are attacking the king
                    attacks_on_king |= Bitboards::get_i(pos);
                    ++num_king_attackers;
                }
                break;
            case Pieces::KNIGHT:
                piece_captures = Pieces::get_knight_moves(pos, *this);
                if (piece_captures & king_position) {
                    // we are attacking the king
                    attacks_on_king |= Bitboards::get_i(pos);
                    ++num_king_attackers;
                }
                break;
            case Pieces::BISHOP:
                piece_captures = movemasks.get_bishop_move(blocker_no_king, pos);
                // piece_captures = Pieces::get_bishop_moves(pos, *this);
                if (piece_captures & king_position) {
                    // we are attacking the king
                    attacks_on_king |= Bitboards::get_between(king_x, king_y, pos_x, pos_y);
                    ++num_king_attackers;
                }
                break;
            case Pieces::ROOK:
                piece_captures = movemasks.get_rook_move(blocker_no_king, pos);
                // piece_captures = Pieces::get_rook_moves(pos, *this);
                if (piece_captures & king_position) {
                    // we are attacking the king
                    attacks_on_king |= Bitboards::get_between(king_x, king_y, pos_x, pos_y);
                    ++num_king_attackers;
                }
                break;
            case Pieces::QUEEN:
                piece_captures = movemasks.get_rook_move(blocker_no_king, pos) | movemasks.get_bishop_move(blocker_no_king, pos);
                // piece_captures = Pieces::get_queen_moves(pos, *this);
                if (piece_captures & king_position) {
                    // we are attacking the king
                    attacks_on_king |= Bitboards::get_between(king_x, king_y, pos_x, pos_y);
                    ++num_king_attackers;
                }
                break;
            case Pieces::KING:
                piece_captures = Pieces::get_king_moves(pos, *this);
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

    // determining if we are pinned ONLY if our number of attackers
    // is less than 2. if its 2, we can ONLY move our king (or lose)
    std::vector<Bitboard> pinned_spaces;        // if any of our pieces are in one of these spaces, they can only move in that area!
    if (num_king_attackers < 2) {
        // std::vector<int> potential_pinner_positions;
        // Bitboards::bitboard_to_positions(potential_pinner_positions, Pieces::get_queen_moves_using_blocker(king_x + king_y * 8, axis_bitboard, *this) & axis_bitboard);
        Bitboard potential_pinners = Pieces::get_queen_moves_using_blocker(king_x + king_y * 8, axis_bitboard, *this) & axis_bitboard;

        while (potential_pinners != 0ULL)
        {
            int pos = Bitboards::pop_lsb(potential_pinners);
            // get ray from king to pin pos
            // check if there is only 1 ally in that area
            // if so, that ally is restricted to that area (non inclusive of ally, inclusive of axis pieces)

            int pos_x = pos % 8;
            int pos_y = pos / 8;

            // calculate the in between
            Bitboard between = Bitboards::get_between(king_x, king_y, pos_x, pos_y, (board[pos] & Pieces::FILTER_PIECE));
            
            if (!Bitboards::contains_multiple_bits(between & ally_bitboard))
                pinned_spaces.push_back(between);
        }
    }

    Bitboard ally_bitboard_i = ally_bitboard;
    while(ally_bitboard_i != 0ULL) {
    // for (auto & pos : ally_piece_positions) {
        int pos = Bitboards::pop_lsb(ally_bitboard_i);
        Piece piece = board[pos];
        Bitboard piece_moves = 0ULL;

        // if under attack by 2 pieces, the king can only move
        if (num_king_attackers == 2 && (piece & Pieces::FILTER_PIECE) != Pieces::KING) continue;

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
                        pos, target_pos, piece, last_move.piece, Move::EN_PASSANT
                    );
                    move(potential_move);
                    if (!last_move_resulted_in_check()) {
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
                piece_moves &= ~attacked_squares;       // we cannot move into attacked squares

                if (can_castle_kingside(attacked_squares)) {
                    Move potential_move(pos, pos - 2, piece, Pieces::EMPTY, Move::CASTLE);
                    moves.push_back(potential_move);
                }

                if (can_castle_queenside(attacked_squares)) {
                    Move potential_move(pos, pos + 2, piece, Pieces::EMPTY, Move::CASTLE);
                    moves.push_back(potential_move);
                }
                break;
        }

        // omit capture of allied pieces
        piece_moves &= not_ally_bitboard;

        // if we are in check by 1 attacker, we can kill it, block it, or move the king
        if (num_king_attackers == 1 && (piece & Pieces::FILTER_PIECE) != Pieces::KING) {
            // we can block/kill this or move the king
            piece_moves &= attacks_on_king;
        }

        // if we are pinned, limit movement
        for (auto & space : pinned_spaces) {
            if (Bitboards::get_i(pos) & space) {
                piece_moves &= space;   // we are pinned!
                break;
            }
        }

        // convert set bits to individual moves
        while (piece_moves != 0ULL) {
            int target_pos = Bitboards::pop_lsb(piece_moves);
            int captured = board[target_pos];
            Move potential_move(pos, target_pos, piece, captured);

            if ((piece & Pieces::FILTER_PIECE) == Pieces::PAWN) {
                if (target_pos / 8 == 7 || target_pos < 8) {
                    Move promotions[4] = {
                        Move(pos, target_pos, Pieces::QUEEN | turn, captured, Move::PROMOTION),
                        Move(pos, target_pos, Pieces::ROOK | turn, captured, Move::PROMOTION),
                        Move(pos, target_pos, Pieces::KNIGHT | turn, captured, Move::PROMOTION),
                        Move(pos, target_pos, Pieces::BISHOP | turn, captured, Move::PROMOTION)
                    };

                    for (int i = 0; i < 4; ++i) {
                        moves.push_back(promotions[i]);
                    }

                    continue;
                }
            }
            moves.push_back(potential_move);
        }
    }

    if (moves.size() == 0) {
        if (is_in_check) {
            if (is_blacks_turn()) {
                wcm = true;
            } else {
                bcm = true;
            }
        } else {
            draw = true;
        }
    }
}

bool Game::can_castle_kingside(Bitboard attacked_squares)
{
    if (is_blacks_turn()) {
        if (has_black_king_moved || has_black_kingside_rook_moved) return false;
        if (Bitboards::BLACK_KINGSIDE_CASTLE_SPACES & game_bitboard) return false;
        if (Bitboards::BLACK_KINGSIDE_CASTLE_ATTACK_FREE & attacked_squares) return false;
        if (board[56] != (Pieces::BLACK | Pieces::ROOK)) return false;  // maybe unneeded?
    } else {
        if (has_white_king_moved || has_white_kingside_rook_moved) return false;
        if (Bitboards::WHITE_KINGSIDE_CASTLE_SPACES & game_bitboard) return false;
        if (Bitboards::WHITE_KINGSIDE_CASTLE_ATTACK_FREE & attacked_squares) return false;
        if (board[0] != (Pieces::WHITE | Pieces::ROOK)) return false;
    }

    return true;
}

bool Game::can_castle_queenside(Bitboard attacked_squares)
{
    if (is_blacks_turn()) {
        if (has_black_king_moved || has_black_queenside_rook_moved) return false;
        if (Bitboards::BLACK_QUEENSIDE_CASTLE_SPACES & game_bitboard) return false;
        if (Bitboards::BLACK_QUEENSIDE_CASTLE_ATTACK_FREE & attacked_squares) return false;
        if (board[63] != (Pieces::BLACK | Pieces::ROOK)) return false;  // maybe unneeded?
    } else {
        if (has_white_king_moved || has_white_queenside_rook_moved) return false;
        if (Bitboards::WHITE_QUEENSIDE_CASTLE_SPACES & game_bitboard) return false;
        if (Bitboards::WHITE_QUEENSIDE_CASTLE_ATTACK_FREE & attacked_squares) return false;
        if (board[7] != (Pieces::WHITE | Pieces::ROOK)) return false;
    }

    return true;
}

void Game::move(Move & move)
{
    wcm = false;
    bcm = false;
    draw = false;
    wc = false;
    bc = false;

    // update game bitboards
    Bitboard from_bitboard = Bitboards::get_i(move.from);
    Bitboard to_bitboard = Bitboards::get_i(move.to);

    // move actual pieces
    board[move.from] = Pieces::EMPTY;
    board[move.to] = move.piece;

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

    switch(move.flags) {
        case Move::EN_PASSANT:
        {
            int target_square = is_blacks_turn() ? move.to + 8 : move.to - 8;
            board[target_square] = Pieces::EMPTY;
            Bitboard en_passant_target = Bitboards::get_i(target_square);

            // update bitboards
            game_bitboard &= ~en_passant_target;

            if (is_blacks_turn()) {
                white_bitboard &= ~en_passant_target;
            } else {
                black_bitboard &= ~en_passant_target;
            }
            break;
        }
        case Move::CASTLE:
        {
            // king vs queenside rook
            if (move.to > move.from) {  // queenside
                board[move.to + 2] = Pieces::EMPTY;
                board[move.to - 1] = Pieces::ROOK | turn;

                game_bitboard &= ~(to_bitboard << 2);
                game_bitboard |= (to_bitboard >> 1);

                if (is_blacks_turn()) {
                    black_bitboard &= ~(to_bitboard << 2);
                    black_bitboard |= (to_bitboard >> 1);

                    has_black_queenside_rook_moved = true;
                } else {
                    white_bitboard &= ~(to_bitboard << 2);
                    white_bitboard |= (to_bitboard >> 1);

                    has_white_queenside_rook_moved = true;
                }
            } else {    // kingside
                board[move.to - 1] = Pieces::EMPTY;
                board[move.to + 1] = Pieces::ROOK | turn;

                game_bitboard &= ~(to_bitboard >> 1);
                game_bitboard |= (to_bitboard << 1);

                if (is_blacks_turn()) {
                    black_bitboard &= ~(to_bitboard >> 1);
                    black_bitboard |= (to_bitboard << 1);

                    has_black_kingside_rook_moved = true;
                } else {
                    white_bitboard &= ~(to_bitboard >> 1);
                    white_bitboard |= (to_bitboard << 1);

                    has_white_kingside_rook_moved = true;
                }
            }
            break;
        }
    }

    switch (move.piece & Pieces::FILTER_PIECE) {
        case Pieces::KING:      // update for castling
            if (is_blacks_turn()) {
                if (!has_black_king_moved) {
                    has_black_king_moved = true;
                    move.flags |= Move::FIRST_MOVE;
                }
            }
            else {
                if (!has_white_king_moved) {
                    has_white_king_moved = true;
                    move.flags |= Move::FIRST_MOVE;
                }
            }
        case Pieces::ROOK:      // update for castling
            if (is_blacks_turn()) {
                if (move.from == 56 && !has_black_kingside_rook_moved) {
                    has_black_kingside_rook_moved = true;
                    move.flags |= Move::FIRST_MOVE;
                } else if (move.from == 63 && !has_black_queenside_rook_moved) {
                    has_black_queenside_rook_moved = true;
                    move.flags |= Move::FIRST_MOVE;
                } 
            } else {
                if (move.from == 0 && !has_white_kingside_rook_moved) {
                    has_white_kingside_rook_moved = true;
                    move.flags |= Move::FIRST_MOVE;
                } else if (move.from == 7 && !has_white_queenside_rook_moved) {
                    has_white_queenside_rook_moved = true;
                    move.flags |= Move::FIRST_MOVE;
                }
            }
            break;
    }

    // calculate checks & stuff
    // todo switch to toher one

    // flip sides
    switch_turns();

    history.push(move);
}

void Game::undo()
{
    // flip sides
    switch_turns();

    bcm = false;
    wcm = false;
    draw = false;

    Move last_move = history.top();
    history.pop();

    // update game bitboards
    Bitboard from_bitboard = Bitboards::get_i(last_move.from);
    Bitboard to_bitboard = Bitboards::get_i(last_move.to);

    // move actual pieces
    board[last_move.to] = last_move.captured == 0 ? Pieces::EMPTY : last_move.captured;
    board[last_move.from] = last_move.piece;

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

    Flag flags = last_move.flags;

    while (flags != 0) {
        int flag = Move::pop_flag(flags);
        switch(flag) {
            case Move::EN_PASSANT:
            {
                board[last_move.to] = Pieces::EMPTY;
                int target_square = is_blacks_turn() ? last_move.to + 8 : last_move.to - 8;
                board[target_square] = last_move.captured;
                Bitboard en_passant_target = Bitboards::get_i(target_square);

                game_bitboard |= en_passant_target;
                game_bitboard &= ~to_bitboard;       // undo from above

                if (is_blacks_turn()) {
                    white_bitboard |= en_passant_target;
                    white_bitboard &= ~to_bitboard;       // undo from above
                } else {
                    black_bitboard |= en_passant_target;
                    black_bitboard &= ~to_bitboard;       // undo from above
                }
                break;
            }
            case Move::CASTLE:
            {
                // castling with our king is ALWAYS a first move for the king & rook

                // king vs queenside rook
                if (last_move.to > last_move.from) {  // queenside
                    board[last_move.to + 2] = Pieces::ROOK | turn;
                    board[last_move.to - 1] = Pieces::EMPTY;

                    game_bitboard |= (to_bitboard << 2);
                    game_bitboard &= ~(to_bitboard >> 1);

                    if (is_blacks_turn()) {
                        black_bitboard |= (to_bitboard << 2);
                        black_bitboard &= ~(to_bitboard >> 1);

                        // has_black_king_moved = false;
                        has_black_queenside_rook_moved = false;
                    } else {
                        white_bitboard |= (to_bitboard << 2);
                        white_bitboard &= ~(to_bitboard >> 1);

                        // has_white_king_moved = false;
                        has_white_queenside_rook_moved = false;
                    }
                } else {    // kingside
                    board[last_move.to - 1] = Pieces::ROOK | turn;
                    board[last_move.to + 1] = Pieces::EMPTY;

                    game_bitboard |= (to_bitboard >> 1);
                    game_bitboard &= ~(to_bitboard << 1);

                    if (is_blacks_turn()) {
                        black_bitboard |= (to_bitboard >> 1);
                        black_bitboard &= ~(to_bitboard << 1);

                        // has_black_king_moved = false;
                        has_black_kingside_rook_moved = false;
                    } else {
                        white_bitboard |= (to_bitboard >> 1);
                        white_bitboard &= ~(to_bitboard << 1);

                        // has_white_king_moved = false;
                        has_white_kingside_rook_moved = false;
                    }
                }
                break;
            }
            case Move::PROMOTION:
            {
                board[last_move.from] = Pieces::PAWN | turn;
                break;
            }
            case Move::FIRST_MOVE:
            {
                switch (last_move.piece & Pieces::FILTER_PIECE) {
                    case Pieces::KING:
                        is_blacks_turn() ? has_black_king_moved = false : has_white_king_moved = false;
                        break;
                    case Pieces::ROOK:
                        if (is_blacks_turn()) {
                            if (last_move.from == 56)
                                has_black_kingside_rook_moved = false;
                            else if (last_move.from == 63)
                                has_black_queenside_rook_moved = false;
                        } else {
                            if (last_move.from == 0)
                                has_white_kingside_rook_moved = false;
                            else if (last_move.from == 7)
                                has_white_queenside_rook_moved = false;
                        }
                        break;
                }
                break;
            }
        }
    }
}

Piece * Game::get_board()
{
    return board;
}