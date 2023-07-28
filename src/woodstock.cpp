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
    std::vector<Move> moves;

    printf("Running simulation of depth %d vs depth %d. (WvB)\n", depth1, depth2);
    
    do {
        moves.clear();
        game.get_moves(moves);
        float eval = search.evaluate_position(game, moves);
        if (moves.size() == 0) {
            if (eval < 0) {
                game.print();
                printf("Checkmate! %s wins!\n", game.is_whites_turn() ? "Black" : "White");
            } else {
                game.print();
                printf("Draw!\n");
            }
            break;
        }

        Move move = search.get_best_move(game.is_whites_turn() ? depth1 : depth2);
        game.move(move);
        printf("Move Made\n");
    } while (moves.size() != 0);
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
    auto make_best_move = [&]() {
        Move best_move = search.get_best_move(search_depth);
        game.move(best_move);
    };

    int time_elapsed = measure(make_best_move);
    printf("Found best move at depth %d with %d evaluations in %dms!\n", search_depth, search.num_positions_evaluated, time_elapsed);
    search.num_positions_evaluated = 0;

    Move last_move = game.get_last_move();

    update_chessboard(game.get_board());    
    highlight_squares(last_move.from, last_move.to);
}

EXTERN EMSCRIPTEN_KEEPALIVE void click_square(int index)
{
    std::vector<Move> moves = game.get_moves_at_square(index);
    int moves_size = moves.size();

    if (moves_size == 0 
        && (game.get(index) & Pieces::FILTER_COLOR) != game.get_turn()) {
        
        for (auto & move : selected_piece_moves)
        {
            if (move.to == index) {
                game.move(move);
                update_chessboard(game.get_board());
                highlight_squares(move.from, move.to);
                EM_ASM({make_ai_move()});
                break;
            }
        }
    } else {
        /* USER IS HIGHLIGHTING A PIECE */
        
        // highlight pieces & select our piece
        EM_ASM({board.selected_piece = $0}, index);

        int move_indexes[27]; // 27 is maximum num of moves a single piece can make
        
        int i;
        for (i = 0; i < moves_size; ++i) {
            move_indexes[i] = moves[i].to;
        }
        move_indexes[i] = -1;

        highlight_moves(move_indexes);
        selected_piece_moves = moves;
    }
}
#else
int main()
{
    printf("woodstock!\n");

    return 0;

    run_game_simulation(1, 3);
    return 0;
    
    Game game;
    Search search(game);
    std::vector<Move> moves;
    int search_depth = 5;

    auto make_best_move = [&]() {
        Move best_move = search.get_best_move(search_depth);
        game.move(best_move);
    };

    do {
        game.print();

        moves.clear();
        game.get_moves(moves);
        float eval = search.evaluate_position(game, moves);

        printf("Evaluation: %f\n\n", eval);

        if (moves.size() == 0) {
            if (eval < 0) {
                game.print();
                printf("Checkmate! %s wins!\n", game.is_whites_turn() ? "Black" : "White");
            } else {
                game.print();
                printf("Draw!\n");
            }
            break;
        }

        char from[10], to[10];
        printf("Move from: ");
        scanf("%s", &from);

        if (from[0] == 'u') {
            game.undo();
            game.undo();
            continue;
        } else if (from[0] == 'z') {
            goto ai_move;
        }

        printf("To: ");
        scanf("%s", &to);

        for (auto & move : moves) {
            if (move.from == square_to_index(from) && move.to == square_to_index(to)) {
                game.move(move);
                printf("\n");
                goto ai_move;
            }
        }

        printf("Invalid move!\n\n");
        continue;

        ai_move:
        printf("Woodstock is thinking... ");

        int time_elapsed = measure(make_best_move);
        printf("Found best move at depth %d with %d evaluations in %dms!\n", search_depth, search.num_positions_evaluated, time_elapsed);
        search.num_positions_evaluated = 0;
        printf("\n");
    } while(moves.size() != 0);

    return 0;
}

#endif