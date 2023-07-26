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

int main()
{
    printf("woodstock!\n");

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

EXTERN EMSCRIPTEN_KEEPALIVE void make_best_move(int argc, char ** argv)
{
    Move best_move = search.get_best_move(search_depth);
    game.move(best_move);

    // :sob: :skull: :coffin:
    std::string cmd = "set_board([";
    for (int i = 63; i >= 0; --i) {
        cmd += std::to_string(game.get(i)) + ",";
    }
    cmd += "])";

    emscripten_run_script(cmd.c_str());
}
#endif