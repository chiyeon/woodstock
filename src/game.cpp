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
    has_white_king_moved                = false;
    has_white_queenside_rook_moved      = false;
    has_white_kingside_rook_moved       = false;
    has_black_king_moved                = false;
    has_black_queenside_rook_moved      = false;
    has_black_kingside_rook_moved       = false;

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
            board[index] = Pieces::piece_from_name_short(fen[i]);
            piece_bbs[board[index]] |= Bitboards::get_i(index);
            index--;
        }
    }

    // update our game bitboard representation
    game_bb = Bitboards::board_to_bitboard(board);
    Piece all_pieces[12] = {
        Pieces::PAWN | Pieces::WHITE,
        Pieces::KNIGHT | Pieces::WHITE,
        Pieces::BISHOP | Pieces::WHITE,
        Pieces::ROOK | Pieces::WHITE,
        Pieces::QUEEN | Pieces::WHITE,
        Pieces::KING | Pieces::WHITE,
        Pieces::PAWN | Pieces::BLACK,
        Pieces::KNIGHT | Pieces::BLACK,
        Pieces::BISHOP | Pieces::BLACK,
        Pieces::ROOK | Pieces::BLACK,
        Pieces::QUEEN | Pieces::BLACK,
        Pieces::KING | Pieces::BLACK,
    };

    for (int i = 0; i < 12; ++i) {
        piece_bbs[all_pieces[i]] = Bitboards::board_to_bitboard(board, Pieces::NO_FILTER, all_pieces[i]);
    }
    piece_bbs[Pieces::WHITE] = Bitboards::board_to_bitboard(board, Pieces::FILTER_COLOR, Pieces::WHITE);
    piece_bbs[Pieces::BLACK] = Bitboards::board_to_bitboard(board, Pieces::FILTER_COLOR, Pieces::BLACK);
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
    // printf("\n");
    // printf("Zobrist Hash: %lu", hasher.compute_hash(board));
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
    return game_bb;
}

Bitboard Game::get_piece_bb(Piece filter)
{
    return piece_bbs[filter];
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
    black_turn = !black_turn;
    turn = black_turn ? Pieces::BLACK : Pieces::WHITE;
    not_turn = black_turn ? Pieces::WHITE : Pieces::BLACK;
}

Piece Game::get_turn()
{
    return turn;
}

Piece Game::get_not_turn()
{
    return not_turn;
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
    Bitboard axis_bitboard = piece_bbs[turn];
    Bitboard king_position = piece_bbs[not_turn | Pieces::KING];

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
    Bitboard axis_bitboard = piece_bbs[not_turn];
    Bitboard king_position = piece_bbs[turn | Pieces::KING];

    if (axis_bitboard == 0ULL) return false;     // for debugging
    if (king_position == 0ULL) return false;

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

bool Game::king_in_check()
{
    // check if the current turn's person is in check
    int king_pos = Bitboards::get_lsb(piece_bbs[turn | Pieces::KING]);
    int not_turn = get_not_turn();
    switch_turns();
    Bitboard pc = Pieces::get_pawn_captures(king_pos, *this);
    switch_turns();
    return (
        piece_bbs[not_turn | Pieces::QUEEN] & Pieces::get_queen_moves(king_pos, *this)
        || piece_bbs[not_turn | Pieces::ROOK] & Pieces::get_rook_moves(king_pos, *this)
        || piece_bbs[not_turn | Pieces::BISHOP] & Pieces::get_bishop_moves(king_pos, *this)
        || piece_bbs[not_turn | Pieces::KNIGHT] & Pieces::get_knight_moves(king_pos, *this)
        || piece_bbs[not_turn | Pieces::PAWN] & pc
    );
}

bool Game::no_moves_left()
{
    
}

bool Game::is_checkmated()
{
    bool can_king_move = false;


}

void Game::get_moves(std::vector<Move> & moves)
{
    moves.reserve(Constants::MAX_CHESS_MOVES_PER_POSITION);

    Bitboard not_game_bitboard = ~game_bb;
    Bitboard axis_bitboard = piece_bbs[not_turn];
    Bitboard ally_bitboard = piece_bbs[turn];
    // Bitboard not_axis_bitboard = ~axis_bitboard;
    Bitboard not_ally_bitboard = ~ally_bitboard;

    Bitboard attacked_squares = 0ULL;               // tiles attacked by the enemy

    Bitboard king_position = piece_bbs[turn | Pieces::KING];
    int king_pos = Bitboards::get_lsb(king_position);
    int king_x = king_pos % 8, king_y = king_pos / 8;

    Bitboard attacks_on_king = 0ULL;    // pieces + attacks against the king. These spaces can be blocked/captured to stop it
    int num_king_attackers = 0;         // determine if we cannot block a check

    Bitboard blocker_no_king = game_bb & ~king_position;      // so we can "x-ray" the king. represents the game bitboard with the active king removed

    // determine danger zones first
    Bitboard axis_bitboard_i = axis_bitboard;
    while(axis_bitboard_i != 0ULL) {
        int pos = Bitboards::pop_lsb(axis_bitboard_i);
        Piece piece = board[pos];
        Bitboard piece_captures = 0ULL;

        int pos_x = pos % 8;
        int pos_y = pos / 8;

        // for every piece, get moves based on type
        switch (piece & Pieces::FILTER_PIECE) {
            case Pieces::PAWN:
                switch_turns();
                piece_captures = Pieces::get_pawn_captures(pos, *this);
                switch_turns();
                if (piece_captures & king_position) {
                    attacks_on_king |= Bitboards::get_i(pos);
                    ++num_king_attackers;
                }
                break;
            case Pieces::KNIGHT:
                piece_captures = Pieces::get_knight_moves(pos, *this);
                if (piece_captures & king_position) {
                    attacks_on_king |= Bitboards::get_i(pos);
                    ++num_king_attackers;
                }
                break;
            case Pieces::BISHOP:
                piece_captures = movemasks.get_bishop_move(blocker_no_king, pos);
                if (piece_captures & king_position) {
                    attacks_on_king |= Bitboards::get_between(king_x, king_y, pos_x, pos_y);
                    ++num_king_attackers;
                }
                break;
            case Pieces::ROOK:
                piece_captures = movemasks.get_rook_move(blocker_no_king, pos);
                if (piece_captures & king_position) {
                    attacks_on_king |= Bitboards::get_between(king_x, king_y, pos_x, pos_y);
                    ++num_king_attackers;
                }
                break;
            case Pieces::QUEEN:
                piece_captures = movemasks.get_rook_move(blocker_no_king, pos) | movemasks.get_bishop_move(blocker_no_king, pos);
                if (piece_captures & king_position) {
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

    // update if we are in check or not
    bool is_in_check = attacked_squares & king_position;

    // determining if we are pinned ONLY if our number of attackers
    // is less than 2. if its 2, we can ONLY move our king (or lose)
    std::vector<Bitboard> pinned_spaces;        // if any of our pieces are in one of these spaces, they can only move in that area!
    if (num_king_attackers < 2) {
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

                // piece_moves = Pieces::get_pawn_forward<black_turn>(Bitboards::get_i(pos));

                // check for promotions
                if ((piece_moves & Bitboards::ROW_1) != 0 || (piece_moves & Bitboards::ROW_8) != 0) {
                    Bitboard pm_i = piece_moves;
                    while (pm_i != 0ULL) {
                        int target_pos = Bitboards::pop_lsb(pm_i);
                        Piece captured = board[target_pos];

                        Piece pieces[4] = {Pieces::QUEEN | turn, Pieces::ROOK | turn, Pieces::BISHOP | turn, Pieces::KNIGHT | turn};
                        for (int i = 0; i < 4; ++i) {
                            Move potential_move(pos, target_pos, pieces[i], captured, Move::PROMOTION);

                            move(potential_move);
                            if (!last_move_resulted_in_check()) moves.push_back(potential_move);
                            // if (!king_in_check()) moves.push_back(potential_move);
                            undo();
                        }
                    }
                    continue;
                }

                // if first move ignore
                if (history.empty()) break;                
                // check if we can en passant
                // int y = pos / 8;
                int x = pos % 8;
                Move last_move = get_last_move();
                if (
                    //((is_blacks_turn() && y == 3) || (!is_blacks_turn() && y == 4))     // our piece is in the right position
                    (last_move.piece & Pieces::FILTER_PIECE) == Pieces::PAWN         // enemy's last move was a double pawn jump
                    &&(std::abs(last_move.to - pos) == 1)
                    && (std::abs(last_move.to % 8 - x) == 1)
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
            moves.push_back(Move(pos, target_pos, piece, board[target_pos]));
        }
    }

    // if (moves.size() == 0) {
    //     if (is_in_check) {
    //         is_blacks_turn() ? bcm = true : wcm = true;
    //     } else {
    //         draw = true;
    //     }
    // }
}

bool Game::can_castle_kingside(Bitboard attacked_squares)
{
    if (is_blacks_turn()) {
        if (has_black_king_moved || has_black_kingside_rook_moved) return false;
        if (Bitboards::BLACK_KINGSIDE_CASTLE_SPACES & game_bb) return false;
        if (Bitboards::BLACK_KINGSIDE_CASTLE_ATTACK_FREE & attacked_squares) return false;
        if (board[56] != (Pieces::BLACK | Pieces::ROOK)) return false;  // maybe unneeded?
    } else {
        if (has_white_king_moved || has_white_kingside_rook_moved) return false;
        if (Bitboards::WHITE_KINGSIDE_CASTLE_SPACES & game_bb) return false;
        if (Bitboards::WHITE_KINGSIDE_CASTLE_ATTACK_FREE & attacked_squares) return false;
        if (board[0] != (Pieces::WHITE | Pieces::ROOK)) return false;
    }

    return true;
}

bool Game::can_castle_queenside(Bitboard attacked_squares)
{
    if (is_blacks_turn()) {
        if (has_black_king_moved || has_black_queenside_rook_moved) return false;
        if (Bitboards::BLACK_QUEENSIDE_CASTLE_SPACES & game_bb) return false;
        if (Bitboards::BLACK_QUEENSIDE_CASTLE_ATTACK_FREE & attacked_squares) return false;
        if (board[63] != (Pieces::BLACK | Pieces::ROOK)) return false;  // maybe unneeded?
    } else {
        if (has_white_king_moved || has_white_queenside_rook_moved) return false;
        if (Bitboards::WHITE_QUEENSIDE_CASTLE_SPACES & game_bb) return false;
        if (Bitboards::WHITE_QUEENSIDE_CASTLE_ATTACK_FREE & attacked_squares) return false;
        if (board[7] != (Pieces::WHITE | Pieces::ROOK)) return false;
    }

    return true;
}

void Game::move(Move & move)
{
    board[move.from]    = Pieces::EMPTY;
    board[move.to]      = move.piece;

    Bitboard from_bb    = Bitboards::get_i(move.from);
    Bitboard to_bb      = Bitboards::get_i(move.to);
    Bitboard fromto_bb  = from_bb ^ to_bb;

    game_bb                         ^= fromto_bb;
    piece_bbs[turn]                 ^= fromto_bb;
    piece_bbs[move.piece]           ^= fromto_bb;

    if (move.captured != 0) {
        game_bb                     |= to_bb;
        piece_bbs[not_turn]         ^= to_bb;
        piece_bbs[move.captured]    ^= to_bb;
    }

    // if we have flags edit piece_bbs manually. otherwise automate it with fromto
    if (move.flags != 0) {
        switch(move.flags) {
            case Move::EN_PASSANT:
            { 
                int target_square = is_blacks_turn() ? move.to + 8 : move.to - 8;
                board[target_square] = Pieces::EMPTY;
                Bitboard en_passant_target = Bitboards::get_i(target_square);

                game_bb                     ^= en_passant_target;
                piece_bbs[not_turn]         ^= en_passant_target;
                piece_bbs[move.captured]    ^= en_passant_target;
                // piece_bbs[not_turn]         ^= to_bb;
                // piece_bbs[move.captured]    ^= to_bb;
                break;
            }
            case Move::CASTLE:
            {
                // king vs queenside rook
                if (move.to > move.from) {  // queenside
                    board[move.to + 2] = Pieces::EMPTY;
                    board[move.to - 1] = Pieces::ROOK | turn;

                    Bitboard rook_to = (to_bb >> 1);
                    Bitboard rook_from = (to_bb << 2);
                    Bitboard rook_fromto = rook_to ^ rook_from;

                    piece_bbs[turn] ^= rook_fromto;
                    piece_bbs[Pieces::ROOK | turn] ^= rook_fromto;
                    // game_bb ^= rook_fromto;

                    is_blacks_turn() ? has_black_queenside_rook_moved = true : has_white_queenside_rook_moved = true;
                } else {    // kingside
                    board[move.to - 1] = Pieces::EMPTY;
                    board[move.to + 1] = Pieces::ROOK | turn;

                    Bitboard rook_to = (to_bb << 1);
                    Bitboard rook_from = (to_bb >> 1);
                    Bitboard rook_fromto = rook_to ^ rook_from;

                    piece_bbs[turn] ^= rook_fromto;
                    piece_bbs[Pieces::ROOK | turn] ^= rook_fromto;
                    // game_bb ^= rook_fromto;

                    is_blacks_turn() ? has_black_kingside_rook_moved = true : has_white_kingside_rook_moved = true;
                }
                break;
            }
            case Move::PROMOTION:
                piece_bbs[turn | Pieces::PAWN] ^= to_bb;
                piece_bbs[move.piece] |= to_bb;
            break;
        }
    }

    switch (move.piece & Pieces::FILTER_PIECE) {
            case Pieces::KING:      // update for castling
                // piece_bbs[turn | Pieces::KING] = to_bb;

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
                break;
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
    // game_bb = piece_bbs[Pieces::WHITE] | piece_bbs[Pieces::BLACK];

    Bitboard gbb = Bitboards::board_to_bitboard(board, Pieces::FILTER_COLOR, Pieces::WHITE);
    if (piece_bbs[Pieces::WHITE] != gbb) {
        printf("ERROR:\n");
        Bitboards::print(gbb);
        piece_bbs[Pieces::WHITE] = gbb;
        piece_bbs[Pieces::BLACK] = Bitboards::board_to_bitboard(board, Pieces::FILTER_COLOR, Pieces::BLACK);
        print();
        printf("\n\n");
    }


    // game_bb = Bitboards::board_to_bitboard(board);
    // Piece all_pieces[12] = {
    //     Pieces::PAWN | Pieces::WHITE,
    //     Pieces::KNIGHT | Pieces::WHITE,
    //     Pieces::BISHOP | Pieces::WHITE,
    //     Pieces::ROOK | Pieces::WHITE,
    //     Pieces::QUEEN | Pieces::WHITE,
    //     Pieces::KING | Pieces::WHITE,
    //     Pieces::PAWN | Pieces::BLACK,
    //     Pieces::KNIGHT | Pieces::BLACK,
    //     Pieces::BISHOP | Pieces::BLACK,
    //     Pieces::ROOK | Pieces::BLACK,
    //     Pieces::QUEEN | Pieces::BLACK,
    //     Pieces::KING | Pieces::BLACK,
    // };

    // for (int i = 0; i < 12; ++i) {
    //     piece_bbs[all_pieces[i]] = Bitboards::board_to_bitboard(board, Pieces::NO_FILTER, all_pieces[i]);
    // }
    // piece_bbs[Pieces::WHITE] = Bitboards::board_to_bitboard(board, Pieces::FILTER_COLOR, Pieces::WHITE);
    // piece_bbs[Pieces::BLACK] = Bitboards::board_to_bitboard(board, Pieces::FILTER_COLOR, Pieces::BLACK);
    
    switch_turns(); // flip sides
    history.push(move);
}

void Game::undo()
{
    switch_turns();

    Move move = history.top();
    history.pop();

    board[move.from]    = move.piece;
    board[move.to]      = move.captured == 0 ? Pieces::EMPTY : move.captured;

    Bitboard from_bb    = Bitboards::get_i(move.from);
    Bitboard to_bb      = Bitboards::get_i(move.to);
    Bitboard fromto_bb  = from_bb ^ to_bb;

    game_bb                         ^= fromto_bb;
    piece_bbs[turn]                 ^= fromto_bb;
    piece_bbs[move.piece]           ^= fromto_bb;

    if (move.captured != 0) {
        game_bb                     |= to_bb;
        piece_bbs[not_turn]         |= to_bb;
        piece_bbs[move.captured]    |= to_bb;
    }

    Flag flags = move.flags;

    while (flags != 0) {
        int flag = Move::pop_flag(flags);
        switch(flag) {
            case Move::EN_PASSANT:
            {
                board[move.to] = Pieces::EMPTY;
                int target_square = is_blacks_turn() ? move.to + 8 : move.to - 8;
                board[target_square] = move.captured;
                Bitboard en_passant_target = Bitboards::get_i(target_square);

                game_bb                     |= en_passant_target;
                game_bb                     ^= to_bb;
                piece_bbs[not_turn]         |= en_passant_target;
                piece_bbs[not_turn]         ^= to_bb;
                piece_bbs[move.captured]    |= en_passant_target;
                piece_bbs[move.captured]    ^= to_bb;
                piece_bbs[turn]             ^= to_bb;
                piece_bbs[move.piece]       ^= to_bb;
                
                break;
            }
            case Move::CASTLE:
            {
                // castling with our king is ALWAYS a first move for the king & rook

                // king vs queenside rook
                if (move.to > move.from) {  // queenside
                    board[move.to + 2] = Pieces::ROOK | turn;
                    board[move.to - 1] = Pieces::EMPTY;

                    Bitboard rook_to = (to_bb << 2);
                    Bitboard rook_from = (to_bb >> 1);
                    Bitboard rook_fromto = rook_to ^ rook_from;

                    piece_bbs[turn] ^= rook_fromto;
                    piece_bbs[Pieces::ROOK | turn] ^= rook_fromto;

                    is_blacks_turn() ? has_black_queenside_rook_moved = false : has_white_queenside_rook_moved = false;
                } else {    // kingside
                    board[move.to - 1] = Pieces::ROOK | turn;
                    board[move.to + 1] = Pieces::EMPTY;

                    Bitboard rook_to = (to_bb >> 1);
                    Bitboard rook_from = (to_bb << 1);
                    Bitboard rook_fromto = rook_to ^ rook_from;

                    piece_bbs[turn] ^= rook_fromto;
                    piece_bbs[Pieces::ROOK | turn] ^= rook_fromto;

                    is_blacks_turn() ? has_black_kingside_rook_moved = false : has_white_kingside_rook_moved = false;
                }
                break;
            }
            case Move::PROMOTION:
            {
                board[move.from] = Pieces::PAWN | turn;
                piece_bbs[Pieces::PAWN | turn] |= from_bb;
                piece_bbs[move.piece] ^= to_bb;
                break;
            }
            case Move::FIRST_MOVE:
            {
                switch (move.piece & Pieces::FILTER_PIECE) {
                    case Pieces::KING:
                        is_blacks_turn() ? has_black_king_moved = false : has_white_king_moved = false;
                        break;
                    case Pieces::ROOK:
                        if (is_blacks_turn()) {
                            if (move.from == 56)
                                has_black_kingside_rook_moved = false;
                            else if (move.from == 63)
                                has_black_queenside_rook_moved = false;
                        } else {
                            if (move.from == 0)
                                has_white_kingside_rook_moved = false;
                            else if (move.from == 7)
                                has_white_queenside_rook_moved = false;
                        }
                        break;
                }
                break;
            }
        }
    }

    // game_bb = piece_bbs[Pieces::WHITE] | piece_bbs[Pieces::BLACK];

    // game_bb = Bitboards::board_to_bitboard(board);
    // Piece all_pieces[12] = {
    //     Pieces::PAWN | Pieces::WHITE,
    //     Pieces::KNIGHT | Pieces::WHITE,
    //     Pieces::BISHOP | Pieces::WHITE,
    //     Pieces::ROOK | Pieces::WHITE,
    //     Pieces::QUEEN | Pieces::WHITE,
    //     Pieces::KING | Pieces::WHITE,
    //     Pieces::PAWN | Pieces::BLACK,
    //     Pieces::KNIGHT | Pieces::BLACK,
    //     Pieces::BISHOP | Pieces::BLACK,
    //     Pieces::ROOK | Pieces::BLACK,
    //     Pieces::QUEEN | Pieces::BLACK,
    //     Pieces::KING | Pieces::BLACK,
    // };

    // for (int i = 0; i < 12; ++i) {
    //     piece_bbs[all_pieces[i]] = Bitboards::board_to_bitboard(board, Pieces::NO_FILTER, all_pieces[i]);
    // }
    // piece_bbs[Pieces::WHITE] = Bitboards::board_to_bitboard(board, Pieces::FILTER_COLOR, Pieces::WHITE);
    // piece_bbs[Pieces::BLACK] = Bitboards::board_to_bitboard(board, Pieces::FILTER_COLOR, Pieces::BLACK);
}

Piece * Game::get_board()
{
    return board;
}