#include <chrono>
#include <vector>
#include "game.h"
#include "move.h"
#include "search.h"

int measure(auto fn)
{
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    fn();
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    return std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
}

// int get_num_castles_at_depth(int depth, Game & g)
// {
//     std::vector<Move> moves;
//     g.get_moves(moves);
//     int count = 0;

//     for (auto & move : moves) {
//         if (depth <= 1) {
//             if (move.flag == CASTLE_BOTH_ALLOWED
//             || move.flag == CASTLE_QUEENSIDE_ALLOWED
//             || move.flag == CASTLE_KINGSIDE_ALLOWED) {
//                 count++;
//             }
//         } else {
//             g.move(move);
//             count += get_num_castles_at_depth(depth - 1, g);
//             g.undo();
//         }
//     }

//     return count;
// }

int count_bulk_positions(Game & game, int depth, bool print_positions = false) {
    if (depth <= 0) return 1;

    std::vector<Move> moves;
    game.get_moves(moves);
    int count = 0;

    int moves_size = moves.size();
    for (int i = 0; i < moves_size; ++i) {
        game.move(moves[i]);
        if (print_positions) game.print();
        count += count_bulk_positions(game, depth - 1, print_positions);
        game.undo();
    }
    return count;
}

int count_and_eval_bulk_positions(Game & game, Search & search, int depth, int best_move_search_depth, bool print_positions = false) {
    if (depth <= 0) return 1;

    std::vector<Move> moves;
    game.get_moves(moves);
    int count = 0;

    int moves_size = moves.size();
    for (int i = 0; i < moves_size; ++i) {
        game.move(moves[i]);
        if (print_positions) game.print();
        search.get_best_move(best_move_search_depth);
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

    int moves_size = moves.size();
    for (int i = 0; i < moves_size; ++i)
    {
        int count = 0;
        game.move(moves[i]);
        count += count_bulk_positions(game, depth - 1);
        game.undo();

        printf("%d to %d: %d moves\n", moves[i].from, moves[i].to, count);
    }
}

int measure_count_bulk_positions(Game & game, int depth, bool print_positions = false) {
    int count = 0;
    auto fn = [&]() {
        count = count_bulk_positions(game, depth, print_positions);
    };

    int time = measure(fn);
    
    printf("Depth: %d\tPositions: %d\tTime: %dms\n", depth, count, time);

    return count;
}

int measure_count_and_eval_bulk_positions(Game & game, Search & search, int depth, int best_move_search_depth, bool print_positions = false) {
    int count = 0;
    auto fn = [&]() {
        count = count_and_eval_bulk_positions(game, search, depth, best_move_search_depth, print_positions);
    };

    int time = measure(fn);
    
    printf("Depth: %d\tPositions: %d\tTime: %dms\n", depth, count, time);

    return count;
}

void measure_nps(int max_depth, std::string position = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR")
{
    Game g(position);
    int total_count = 0, i = 1;

    auto test_first_position = [&]() {
        for (i = 1; i <= max_depth; ++i) {
            total_count += measure_count_bulk_positions(g, i);
        }
    };

    int time_elapsed = measure(test_first_position);
    printf("\nTotal Num Nodes: %d\nTime Elapsed: %dms\nNodes Per Second: %d\n", total_count, time_elapsed, (int)(static_cast<float>(total_count) / (static_cast<float>(time_elapsed) / 1000.0)));
}

void measure_nps_with_eval(int max_depth, int search_depth, std::string position = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR")
{
    Game g(position);
    Search search(g);
    int total_count = 0, i = 1;

    auto test_first_position = [&]() {
        for (i = 1; i <= max_depth; ++i) {
            total_count += measure_count_and_eval_bulk_positions(g, search, i, search_depth);
        }
    };

    int time_elapsed = measure(test_first_position);
    printf("\nTotal Num Nodes: %d\nTime Elapsed: %dms\nNodes Per Second: %d\n", total_count, time_elapsed, (int)(static_cast<float>(total_count) / (static_cast<float>(time_elapsed) / 1000.0)));
}