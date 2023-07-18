#include <chrono>
#include <vector>
#include "game.h"
#include "move.h"

int measure(auto fn)
{
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    fn();
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    return std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
}

int count_bulk_positions(Game & game, int depth, bool print_positions = false) {
    if (depth <= 0) return 1;

    std::vector<Move> moves;
    game.get_moves(moves);
    int count = 0;

    for (int i = 0; i < moves.size(); ++i) {
        game.move(moves[i]);
        if (print_positions) game.print();
        count += count_bulk_positions(game, depth - 1, print_positions);
        game.undo();
    }
    return count;
}

void print_num_positions_from_starting(Game & game, int depth)
{
    if (depth <= 0) return;

    std::vector<Move> moves;
    game.get_moves(moves);

    for (int i = 0; i < moves.size(); ++i)
    {
        int count = 0;
        game.move(moves[i]);
        count += count_bulk_positions(game, depth - 1);
        game.undo();

        printf("%d to %d: %d moves\n", moves[i].from, moves[i].to, count);
    }
}

void measure_count_bulk_positions(Game & game, int depth, bool print_positions = false) {
    int count = 0;
    auto fn = [&]() {
        count = count_bulk_positions(game, depth, print_positions);
    };

    int time = measure(fn);
    
    printf("Depth: %d\tPositions: %d\tTime: %dms\n", depth, count, time);
}