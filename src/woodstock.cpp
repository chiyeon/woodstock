#include <stdio.h>
#include <vector>
#include "game.h"
#include "measure.h"
#include "bitboard.h"
#include "search.h"

int get_num_castles_at_depth(int depth, Game & g)
{
    std::vector<Move> moves;
    g.get_moves(moves);
    int count = 0;

    for (auto & move : moves) {
        if (depth <= 1) {
            if (move.flag == CASTLE_BOTH_ALLOWED
            || move.flag == CASTLE_QUEENSIDE_ALLOWED
            || move.flag == CASTLE_KINGSIDE_ALLOWED) {
                count++;
            }
        } else {
            g.move(move);
            count += get_num_castles_at_depth(depth - 1, g);
            g.undo();
        }
    }

    return count;
}

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

int main()
{
    printf("woodstock!\n");
    
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
        printf("Evaluation: %f\n\n", search.evaluate_position());

        moves.clear();
        game.get_moves(moves);

        char from[3], to[3];
        printf("Move from: ");
        scanf("%s", &from);
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
        printf("Found best move at depth %d with %d evaluations in %dms!\n", search_depth, search.get_num_positions_evaluated(), time_elapsed);
        printf("\n");
    } while(moves.size() != 0);

    return 0;
}