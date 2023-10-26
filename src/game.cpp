#include <stdio.h>
#include <cctype>
#include <vector>
#include <stack>
#include <sstream>
#include "game.h"
#include "piece.h"
#include "move.h"

Game::Game(std::string fen)
{
    read_fen(fen);
}

void Game::reset()
{
    // Unable to castle by default, fixed when reading castling rights!
    has_white_king_moved                = true;
    has_white_queenside_rook_moved      = true;
    has_white_kingside_rook_moved       = true;
    has_black_king_moved                = true;
    has_black_queenside_rook_moved      = true;
    has_black_kingside_rook_moved       = true;

    wcm = false;
    bcm = false;
    draw = false;

    history.clear();
}

void Game::read_fen(std::string fen)
{
    reset();
    std::istringstream ss(fen);

    std::string next;
    ss >> next;       // first 'word' is the board pieces

    int fen_length = next.size();
    int index = 63;
    for (int i = 0; i < fen_length; ++i) {
        // read number of spaces
        if (isdigit(next[i])) {
            int num_spaces = int(next[i] - '0');
            for (int j = 0; j < num_spaces; ++j) {
                board[index--] = Pieces::EMPTY;
            }
        } else if (next[i] != '/') {
            board[index] = Pieces::piece_from_name_short(next[i]);
            piece_bbs[board[index]] |= Bitboards::get_i(index);
            index--;
        }
    }

    ss >> next;     // next is current turn
    if (next == "w") {
        turn = Pieces::WHITE;
        not_turn = Pieces::BLACK;
        black_turn = false;
    } else {
        turn = Pieces::BLACK;
        not_turn = Pieces::WHITE;
        black_turn = true;
    }

    ss >> next;     // then castling rights
    int cr_size = next.size();
    for (int i = 0; i < cr_size; ++i) {
        switch(next[i]) {
            case 'k':
                has_black_king_moved = false;
                has_black_kingside_rook_moved = false;
                break;
            case 'q':
                has_black_king_moved = false;
                has_black_queenside_rook_moved = false;
                break;
            case 'K':
                has_white_king_moved = false;
                has_white_kingside_rook_moved = false;
                break;
            case 'Q':
                has_white_king_moved = false;
                has_white_queenside_rook_moved = false;
                break;
        }
    }

    // we dont care about whats next c:

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

    if (!history.is_empty()) {
        last_move_from = Moves::get_from(history.get_last_move());
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

History & Game::get_history()
{
    return history;
}

bool Game::is_history_empty()
{
    return history.is_empty();
}

Move Game::get_last_move()
{
    return history.get_last_move();
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
        if (Moves::get_from(moves[i]) == sq) {
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

bool Game::is_king_in_check()
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
    // bool is_in_check = attacked_squares & king_position;

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
                            Move potential_move = Moves::create(pos, target_pos, pieces[i], captured, Moves::PROMOTION);

                            move(potential_move);
                            if (!last_move_resulted_in_check()) piece_moves |= Bitboards::get_i(target_pos);
                            // if (!king_in_check()) moves.push_back(potential_move);
                            undo();
                        }
                    }
                    continue;
                }

                // if first move ignore
                if (history.is_empty()) break;                
                // check if we can en passant
                // int y = pos / 8;
                int x = pos % 8;
                Move last_move = history.get_last_move();
                if (
                    //((is_blacks_turn() && y == 3) || (!is_blacks_turn() && y == 4))     // our piece is in the right position
                    (Moves::get_piece(last_move) & Pieces::FILTER_PIECE) == Pieces::PAWN         // enemy's last move was a double pawn jump
                    &&(std::abs(Moves::get_to(last_move) - pos) == 1)
                    && (std::abs(Moves::get_to(last_move) % 8 - x) == 1)
                    && std::abs(Moves::get_to(last_move) - Moves::get_from(last_move)) == 16
                ) {
                    // rather than add overhead later, we can just manually add the move now
                    int target_pos = is_blacks_turn() ? Moves::get_to(last_move) - 8 : Moves::get_to(last_move) + 8;

                    Move potential_move = Moves::create(
                        pos, target_pos, piece, Moves::get_piece(last_move), Moves::EN_PASSANT
                    );
                    move(potential_move);
                    if (!last_move_resulted_in_check()) {
                        piece_moves |= Bitboards::get_i(target_pos);
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
                    piece_moves |= Bitboards::get_i(pos - 2);
                }

                if (can_castle_queenside(attacked_squares)) {
                    piece_moves |= Bitboards::get_i(pos - 2);
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

        if (piece_moves) return false; // cut us out early if any move exists
    }

    return true;
}

bool Game::is_gameover()
{
   return wcm || bcm || draw;
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
    // bool is_in_check = attacked_squares & king_position;

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
                            Move potential_move = Moves::create(pos, target_pos, pieces[i], captured, Moves::PROMOTION);

                            move(potential_move);
                            if (!last_move_resulted_in_check()) moves.push_back(potential_move);
                            // if (!king_in_check()) moves.push_back(potential_move);
                            undo();
                        }
                    }
                    continue;
                }

                // if first move ignore
                if (history.is_empty()) break;                
                // check if we can en passant
                // int y = pos / 8;
                int x = pos % 8;
                Move last_move = history.get_last_move();
                if (
                    //((is_blacks_turn() && y == 3) || (!is_blacks_turn() && y == 4))     // our piece is in the right position
                    (Moves::get_piece(last_move) & Pieces::FILTER_PIECE) == Pieces::PAWN         // enemy's last move was a double pawn jump
                    &&(std::abs(Moves::get_to(last_move) - pos) == 1)
                    && (std::abs(Moves::get_to(last_move) % 8 - x) == 1)
                    && std::abs(Moves::get_to(last_move) - Moves::get_from(last_move)) == 16
                ) {
                    // rather than add overhead later, we can just manually add the move now
                    int target_pos = is_blacks_turn() ? Moves::get_to(last_move) - 8 : Moves::get_to(last_move) + 8;

                    Move potential_move = Moves::create(
                        pos, target_pos, piece, Moves::get_piece(last_move), Moves::EN_PASSANT
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
                    Move potential_move = Moves::create(pos, pos - 2, piece, Pieces::EMPTY, Moves::CASTLE);
                    moves.push_back(potential_move);
                }

                if (can_castle_queenside(attacked_squares)) {
                    Move potential_move = Moves::create(pos, pos + 2, piece, Pieces::EMPTY, Moves::CASTLE);
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
            moves.push_back(Moves::create(pos, target_pos, piece, board[target_pos]));
        }
    }
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

void Game::move(Move & move, bool verbose)
{
    wcm = false;
    bcm = false;
    draw = false;

    const int from = Moves::get_from(move), to = Moves::get_to(move);
    const Piece piece = Moves::get_piece(move), captured = Moves::get_captured(move);
    Flag flags = Moves::get_flags(move);

    Bitboard from_bb    = Bitboards::get_i(from);
    Bitboard to_bb      = Bitboards::get_i(to);
    Bitboard fromto_bb  = from_bb ^ to_bb;

    board[from]         = Pieces::EMPTY;
    board[to]           = piece;

    game_bb             ^= fromto_bb;
    piece_bbs[turn]     ^= fromto_bb;
    piece_bbs[piece]    ^= fromto_bb;
/*
    game_bb |= to_bb;
    game_bb &= ~from_bb;

    piece_bbs[turn] |= to_bb;
    piece_bbs[turn] &= ~from_bb;

    piece_bbs[piece] |= to_bb;
    piece_bbs[piece] &= ~from_bb;*/

    if (captured != 0) {
        game_bb             |= to_bb;
        //piece_bbs[not_turn] ^= to_bb;
        //piece_bbs[captured] ^= to_bb;

        piece_bbs[not_turn] &= ~to_bb;
        piece_bbs[captured] &= ~to_bb;
    }

    // at this point we dont care if its the first move
    // instead of looping through we can just filter FIRST_MOVE which
    // interferes with castling for the AI !
    flags &= ~Moves::FIRST_MOVE;

    switch(flags) {
        case Moves::EN_PASSANT:
        {
            int ep_square = is_blacks_turn() ? Moves::get_to(move) + 8 : Moves::get_to(move) - 8;
            Bitboard ep_square_bb = Bitboards::get_i(ep_square);
            
            board[ep_square]    = Pieces::EMPTY;    // delete our piece!

            piece_bbs[not_turn] ^= ep_square_bb;
            piece_bbs[captured] ^= ep_square_bb;
            game_bb             ^= ep_square_bb;

            // there was nothing in the to slot to begin with, so switch it back
            //game_bb             ^= to_bb;
            //piece_bbs[not_turn] ^= to_bb;
            //piece_bbs[captured] ^= to_bb;
            break;
        }
        case Moves::CASTLE:
        {
            // king vs queenside rook
            if (to > from) {  // queenside
                Piece rook              = Pieces::ROOK | turn;
                board[to + 2]           = Pieces::EMPTY;
                board[to - 1]           = rook;

                Bitboard rook_to        = (to_bb >> 1);
                Bitboard rook_from      = (to_bb << 2);
                Bitboard rook_fromto    = rook_to ^ rook_from;

                piece_bbs[turn]         ^= rook_fromto;
                piece_bbs[rook]         ^= rook_fromto;
                game_bb                 ^= rook_fromto;

                is_blacks_turn() ? has_black_queenside_rook_moved = true : has_white_queenside_rook_moved = true;
            } else {    // kingside
                Piece rook              = Pieces::ROOK | turn;
                board[to - 1]           = Pieces::EMPTY;
                board[to + 1]           = Pieces::ROOK | turn;

                Bitboard rook_to        = (to_bb << 1);
                Bitboard rook_from      = (to_bb >> 1);
                Bitboard rook_fromto    = rook_to ^ rook_from;

                piece_bbs[turn]         ^= rook_fromto;
                piece_bbs[rook]         ^= rook_fromto;
                game_bb                 ^= rook_fromto;

                is_blacks_turn() ? has_black_kingside_rook_moved = true : has_white_kingside_rook_moved = true;
            }
            break;
        }
        case Moves::PROMOTION:
        {
            piece_bbs[turn | Pieces::PAWN]  ^= to_bb;
            piece_bbs[piece]                |= to_bb;
            break;
        }
        //piece_bbs[Pieces::WHITE] = Bitboards::board_to_bitboard(board, Pieces::FILTER_COLOR, Pieces::WHITE);
        //piece_bbs[Pieces::BLACK] = Bitboards::board_to_bitboard(board, Pieces::FILTER_COLOR, Pieces::BLACK);
        //game_bb = piece_bbs[Pieces::WHITE] | piece_bbs[Pieces::BLACK];
    }

    switch (piece & Pieces::FILTER_PIECE) {
        case Pieces::KING:      // update for castling
            if (is_blacks_turn()) {
                if (!has_black_king_moved) {
                    has_black_king_moved = true;
                    Moves::add_flag(move, Moves::FIRST_MOVE);
                }
            }
            else {
                if (!has_white_king_moved) {
                    has_white_king_moved = true;
                    Moves::add_flag(move, Moves::FIRST_MOVE);
                }
            }
            break;
        case Pieces::ROOK:      // update for castling
            if (is_blacks_turn()) {
                if (from == 56 && !has_black_kingside_rook_moved) {
                    has_black_kingside_rook_moved = true;
                    Moves::add_flag(move, Moves::FIRST_MOVE);
                } else if (from == 63 && !has_black_queenside_rook_moved) {
                    has_black_queenside_rook_moved = true;
                    Moves::add_flag(move, Moves::FIRST_MOVE);
                }
            } else {
                if (from == 0 && !has_white_kingside_rook_moved) {
                    has_white_kingside_rook_moved = true;
                    Moves::add_flag(move, Moves::FIRST_MOVE);
                } else if (from == 7 && !has_white_queenside_rook_moved) {
                    has_white_queenside_rook_moved = true;
                    Moves::add_flag(move, Moves::FIRST_MOVE);
                }
            }
            break;
        }

    switch_turns();
    history.record(move, board);

    if (history.check_threefold_repetition(board)) {
        draw = true;
        if (verbose) printf("Draw by repetition.\n");
    } else if (no_moves_left()) {
        if (is_king_in_check()) {
            // these are reversed !
            is_whites_turn() ? bcm = true : wcm = true;

            if (verbose) {
                if (wcm) printf("white wins\n");
                else printf("black wins\n");
            }
        } else {
            draw = true;
            if (verbose) printf("Draw by no moves left.\n");
        }
    }
}

void Game::undo(bool verbose)
{
    wcm = false;
    bcm = false;
    draw = false;

    switch_turns();

    Move move = history.pop_last_move();

    int from = Moves::get_from(move), to = Moves::get_to(move);
    Piece piece = Moves::get_piece(move), captured = Moves::get_captured(move);
    Flag flags = Moves::get_flags(move);

    Bitboard from_bb    = Bitboards::get_i(from);
    Bitboard to_bb      = Bitboards::get_i(to);
    Bitboard fromto_bb  = from_bb ^ to_bb;

    board[from]         = piece;
    board[to]           = captured;

    game_bb             ^= fromto_bb;
    piece_bbs[turn]     ^= fromto_bb;
    piece_bbs[piece]    ^= fromto_bb;
    
    if (captured != 0) {
        game_bb             |= to_bb;
        piece_bbs[not_turn] |= to_bb;
        piece_bbs[captured] |= to_bb;
    }

    //piece_bbs[Pieces::WHITE] = Bitboards::board_to_bitboard(board, Pieces::FILTER_COLOR, Pieces::WHITE);
    //piece_bbs[Pieces::BLACK] = Bitboards::board_to_bitboard(board, Pieces::FILTER_COLOR, Pieces::BLACK);
    //game_bb = piece_bbs[Pieces::WHITE] | piece_bbs[Pieces::BLACK];

    while (flags != 0) {
        int flag = Moves::pop_flag(flags);
        switch(flag) {
            case Moves::EN_PASSANT:
            {
                int ep_square = is_blacks_turn() ? to + 8 : to - 8;
                Bitboard ep_square_bb = Bitboards::get_i(ep_square);
                
                board[to]               = Pieces::EMPTY;
                board[ep_square]        = captured;

                // undo from earlier now
                game_bb                 |= ep_square_bb;
                game_bb                 ^= to_bb;
                piece_bbs[not_turn]     |= ep_square_bb;
                piece_bbs[not_turn]     ^= to_bb;
                piece_bbs[captured]     |= ep_square_bb;
                piece_bbs[captured]     ^= to_bb;
                //piece_bbs[turn]         ^= to_bb;
                //piece_bbs[piece]        ^= to_bb;
                
                break;
            }
            case Moves::CASTLE:
            {
                // castling with our king is ALWAYS a first move for the king & rook

                // king vs queenside rook
                if (to > from) {  // queenside
                    Piece rook              = Pieces::ROOK | turn;
                    board[to + 2]           = rook;
                    board[to - 1]           = Pieces::EMPTY;

                    Bitboard rook_to        = (to_bb << 2);
                    Bitboard rook_from      = (to_bb >> 1);
                    Bitboard rook_fromto    = rook_to ^ rook_from;

                    piece_bbs[turn]         ^= rook_fromto;
                    piece_bbs[rook]         ^= rook_fromto;
                    game_bb                 ^= rook_fromto;

                    is_blacks_turn() ? has_black_queenside_rook_moved = false : has_white_queenside_rook_moved = false;
                } else {    // kingside
                    Piece rook              = Pieces::ROOK | turn;
                    board[to - 1]           = rook;
                    board[to + 1]           = Pieces::EMPTY;

                    Bitboard rook_to        = (to_bb >> 1);
                    Bitboard rook_from      = (to_bb << 1);
                    Bitboard rook_fromto    = rook_to ^ rook_from;

                    piece_bbs[turn]         ^= rook_fromto;
                    piece_bbs[rook]         ^= rook_fromto;
                    game_bb                 ^= rook_fromto;

                    is_blacks_turn() ? has_black_kingside_rook_moved = false : has_white_kingside_rook_moved = false;
                }
                break;
            }
            case Moves::PROMOTION:
            {
                Piece pawn          = Pieces::PAWN | turn;
                board[from]         = pawn;
                piece_bbs[pawn]     |= from_bb;
                piece_bbs[piece]    ^= to_bb;
                break;
            }
            case Moves::FIRST_MOVE:
            {
                switch (piece & Pieces::FILTER_PIECE) {
                    case Pieces::KING:
                        is_blacks_turn() ? has_black_king_moved = false : has_white_king_moved = false;
                        break;
                    case Pieces::ROOK:
                        if (is_blacks_turn()) {
                            if (from == 56)
                                has_black_kingside_rook_moved = false;
                            else if (from == 63)
                                has_black_queenside_rook_moved = false;
                        } else {
                            if (from == 0)
                                has_white_kingside_rook_moved = false;
                            else if (from == 7)
                                has_white_queenside_rook_moved = false;
                        }
                        break;
                }
                break;
            }
        }
    }

    /*
    if (history.check_threefold_repetition(board)) {
        draw = true;
    } else if (is_king_in_check() && no_moves_left()) {
         is_whites_turn() ? wcm = true : bcm = true;
    }
    */
      
}

Piece * Game::get_board()
{
    return board;
}
