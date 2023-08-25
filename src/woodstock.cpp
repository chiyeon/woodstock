#include <stdio.h>
#include <vector>
#include <string.h>
#include "game.h"
#include "measure.h"
#include "bitboard.h"
#include "search.h"

#ifdef EMSCRIPTEN
#include <emscripten/emscripten.h>
#endif

int square_to_index(char * square) {
    int x, y;

    switch (square[0]) {
        case 'a': x = 7;
            break;
        case 'b': x = 6;
            break;
        case 'c': x = 5;
            break;
        case 'd': x = 4;
            break;
        case 'e': x = 3;
            break;
        case 'f': x = 2;
            break;
        case 'g': x = 1;
            break;
        case 'h': x = 0;
            break;
    }

    y = (int)(square[1]) - '1';

    return y * 8 + x;
}

void run_game_simulation(int depth1, int depth2)
{
    Game game;
    Search search(game);

    printf("Running simulation of depth %d vs depth %d. (WvB)\n", depth1, depth2);
    
    do {
        Move move = search.get_best_move(game.is_whites_turn() ? depth1 : depth2);
        game.move(move);
        printf("Move Made\n");
    // } while (!game.bcm && !game.wcm && !game.draw);
    } while (false);

    game.print();

    // if (game.bcm) {
    //     printf("Black wins!\n");
    // } else if (game.wcm) {
    //     printf("White wins!\n");
    // } else {
    //     printf("Draw.\n");
    // }
}

#ifdef EMSCRIPTEN
#ifdef __cplusplus
#define EXTERN extern "C"
#else
#define EXTERN
#endif

// Game game("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R");
Game game;
Search search(game);
int search_depth = 5;

std::vector<Move> selected_piece_moves;

EM_JS(void, highlight_squares, (int sq1, int sq2), {
    board.highlight_squares(sq1, sq2);
});

EM_JS(void, highlight_moves, (int * squares), {
    board.remove_move_highlights();

    // 27 = max num of moves a single piece can go
    for (let i = 0; i < 27; i++) {
        const INT_SIZE = 4;
        let val = HEAP32[(squares + INT_SIZE * i) >> 2];

        if (val == -1) return;
        board.highlight_move(val);    // squares are -1 if no data
    }
});

EM_JS(void, update_chessboard, (Piece * board_data), {
    board.remove_move_highlights();

    for (let i = 0; i < 64; i++) {
        const INT_SIZE = 4;

        board.board[i] = HEAP32[(board_data + INT_SIZE * i) >> 2];
    }

    board.update_chessboard();
})

EXTERN EMSCRIPTEN_KEEPALIVE void make_best_move(int argc, char ** argv)
{
    // do some resetting
    selected_piece_moves.clear();
    EM_ASM({board.selected_piece = undefined});

    bool move_exists = false;

    auto make_best_move = [&]() {
        Move best_move = search.get_best_move(search_depth);
        if (Moves::get_to(best_move) != Moves::get_from(best_move) && Moves::get_to(best_move) != 0) {
            game.move(best_move);
            move_exists = true;
        }
    };

    int time_elapsed = measure(make_best_move);

    if (!move_exists) {
        printf("No moves exist.\n");
        return;
    }

    printf("Found best move at depth %d with %d evaluations in %dms!\n", search_depth, search.num_positions_evaluated, time_elapsed);
    search.num_positions_evaluated = 0;

    Move last_move = game.get_last_move();

    update_chessboard(game.get_board());
    highlight_squares(Moves::get_from(last_move), Moves::get_to(last_move));
}

EXTERN EMSCRIPTEN_KEEPALIVE void click_square(int index)
{
    std::vector<Move> moves = game.get_moves_at_square(index);
    int moves_size = moves.size();

    if (moves_size == 0 
        && (game.get(index) & Pieces::FILTER_COLOR) != game.get_turn()) {
        
        for (auto & move : selected_piece_moves)
        {
            if (Moves::get_to(move) == index) {
                game.move(move);
                update_chessboard(game.get_board());
                highlight_squares(Moves::get_from(move), Moves::get_to(move));
                // EM_ASM({make_ai_move()});
                break;
            }
        }
    } else {
        /* USER IS HIGHLIGHTING A PIECE */
        
        // highlight pieces & select our piece
        EM_ASM({board.select_piece($0)}, index);

        int move_indexes[27]; // 27 is maximum num of moves a single piece can make
        
        int i;
        for (i = 0; i < moves_size; ++i) {
            move_indexes[i] = Moves::get_to(moves[i]);
        }
        move_indexes[i] = -1;

        highlight_moves(move_indexes);
        selected_piece_moves = moves;
    }
}

EXTERN EMSCRIPTEN_KEEPALIVE void load_fen(char * fen)
{
    game.read_fen((std::string)fen);
    update_chessboard(game.get_board());
}

EXTERN EMSCRIPTEN_KEEPALIVE void undo()
{
    game.undo();
    update_chessboard(game.get_board());

    if (game.is_history_empty()) {
        EM_ASM({board.remove_highlight_squares()});
    } else {
        Move last_move = game.get_last_move();
        highlight_squares(Moves::get_from(last_move), Moves::get_to(last_move));
    }
}

int main()
{
    printf("woodstock!\n");
    EM_ASM({hide_loading_hint()});
    return 0;
}
#else
int main()
{
    printf("woodstock!\n");

    // bug occurs when searching beyond checkmates.
    // Game g("8/8/8/5N2/6p1/5k1p/2R4p/1K4n1");
    // Search search(g);

    // g.print();
    // // measure_count_bulk_positions(g, 1, true);
    // for (int i = 0; i < 1; i++) {
    //     Move b = search.get_best_move(5);
    //     printf("Move from %d to %d as piece %d. Flags are: %d.\n", b.from, b.to, b.piece, b.flags);
    //     // g.move(b);
    // }
    // g.print();
    // return 0;

    // Game g;
    // g.print();
    // std::vector<Move> moves;
    // g.get_moves(moves);
    // g.move(moves[3]);
    // g.print();
    // g.undo();
    // g.print();
    // g.move(moves[12]);
    // g.print();
    // g.undo();
    // g.print();

    // measure_nps_with_eval(6, 5);

    // run_perft_suite();
    // measure_nps(6);

    // measure_nps(5, "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ");

    // measure_nps(6);
    // printf("\n\n");
    // measure_nps(5, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R");   // position 2
    Game g("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R");
    for (int i = 1; i <= 5; ++i) {
        printf("%d en passants at depth %d\n", get_num_flags_at_depth(i, Moves::EN_PASSANT, g), i);
        printf("%d castles at depth %d\n", get_num_flags_at_depth(i, Moves::CASTLE, g), i);
    }

    // printf("\n\n");

    // Game g2;
    // for (int i = 1; i <= 6; ++i) {
    //     printf("%d en passants at depth %d\n", get_num_flags_at_depth(i, Move::EN_PASSANT, g2), i);
    //     printf("%d castles at depth %d\n", get_num_flags_at_depth(i, Move::CASTLE, g2), i);
    // }
    // Game g;
    // measure_count_bulk_positions(g, 2, true);

    // measure_nps(6);      // measure start position
    return 0;

    // Game game;
    // Search search(game);
    // std::vector<Move> moves;
    // int search_depth = 5;

    // auto make_best_move = [&]() {
    //     Move best_move = search.get_best_move(search_depth);
    //     game.move(best_move);
    // };

    // do {
    //     game.print();

    //     moves.clear();
    //     game.get_moves(moves);
    //     float eval = search.evaluate_position(game);

    //     printf("Evaluation: %f\n\n", eval);

    //     if (moves.size() == 0) {
    //         if (eval < 0) {
    //             game.print();
    //             printf("Checkmate! %s wins!\n", game.is_whites_turn() ? "Black" : "White");
    //         } else {
    //             game.print();
    //             printf("Draw!\n");
    //         }
    //         break;
    //     }

    //     char from[10], to[10];
    //     printf("Move from: ");
    //     scanf("%s", &from); 

    //     if (from[0] == 'u') {
    //         game.undo();
    //         game.undo();
    //         continue;
    //     } else if (from[0] == 'z') {
    //         goto ai_move;
    //     }

    //     printf("To: ");
    //     scanf("%s", &to);

    //     for (auto & move : moves) {
    //         if (move.from == square_to_index(from) && move.to == square_to_index(to)) {
    //             game.move(move);
    //             printf("\n");
    //             goto ai_move;
    //         }
    //     }

    //     printf("Invalid move!\n\n");
    //     continue;

    //     ai_move:
    //     printf("Woodstock is thinking... ");

    //     int time_elapsed = measure(make_best_move);
    //     printf("Found best move at depth %d with %d evaluations in %dms!\n", search_depth, search.num_positions_evaluated, time_elapsed);
    //     search.num_positions_evaluated = 0;
    //     printf("\n");
    // } while(moves.size() != 0);

    return 0;
}

#endif
