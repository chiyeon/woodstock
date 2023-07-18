#include <stdio.h>
#include <vector>
#include "game.h"
#include "measure.h"
#include "bitboard.h"

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
    std::vector<Move> moves;

    do {
        game.print();

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
                goto end;
            }
        }

        printf("Invalid move!\n");

        end:
        printf("\n");
    } while(moves.size() != 0);

    return 0;
}