#include <chrono>
#include <vector>
#include "game.h"
#include "move.h"

float measure(auto fn)
{
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    fn();
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    return std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
}

int count_bulk_positions(Game game, int depth, bool print_positions = false) {
    if (depth <= 0) return 1;

    std::vector<Move> moves = game.get_moves();
    int count = 0;

    for (int i = 0; i < moves.size(); ++i) {
        game.move(moves[i]);
        if (depth == 1) {
            game.print();
            printf("\n");
        }
        if (print_positions) game.print();
        count += count_bulk_positions(game, depth - 1, print_positions);
        game.undo();
    }

    return count;
}

void measure_count_bulk_positions(Game game, int depth, bool print_positions = false) {
    int count = 0;
    auto fn = [&]() {
        count = count_bulk_positions(game, depth, print_positions);
    };

    float time = measure(fn);
    
    printf("Depth: %d\tPositions: %d\tTime: %fms\n", depth, count, time);
}