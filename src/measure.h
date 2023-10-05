#include <chrono>
#include <vector>
#include "game.h"
#include "move.h"
#include "search.h"

template <typename function>
int measure(function&& fn)
{
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    fn();
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    return std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
}

int get_num_flags_at_depth(int depth, Flag flag, Game & g)
{
    std::vector<Move> moves;
    g.get_moves(moves);
    int count = 0;

    for (auto & move : moves) {
        if (depth <= 1) {
            if (Moves::get_flags(move) & flag) {
                count++;
            }
        } else {
            g.move(move);
            count += get_num_flags_at_depth(depth - 1, flag, g);
            g.undo();
        }
    }

    return count;
}

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

void run_perft(std::string fen, const std::initializer_list<int> & correct_counts)
{
    int total_count = 0;
    int current_count = 0;
    int time_elapsed = 0;
    int depth = 1;
    int errors = 0;
    Game game(fen);

    auto fn = [&]() {
        current_count = count_bulk_positions(game, depth);
    };

    for (auto & correct_count : correct_counts) {
        time_elapsed += measure(fn);

        if (correct_count != current_count) {
            printf("\tERROR! Expected count of %d at depth %d, received %d.\n", correct_count, depth, current_count);
            ++errors;
        }

        total_count += current_count;
        ++depth;
    }

    if (errors == 0) {
        printf("\tPassed.\n");
    }

    printf("\t%d NPS at depth %d.\n", (int)(static_cast<float>(total_count) / (static_cast<float>(time_elapsed) / 1000.0)), depth - 1);
}

void run_perft_suite()
{
    printf("Running Perft Suite. All positions/counts are based on https://www.chessprogramming.org/Perft_Results.\n\n");

    printf("Position 1\n");
    run_perft("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", {20, 400, 8902, 197281, 4865609, 119060324});
    
    printf("Position 2\n");
    run_perft("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq", {48, 2039, 97862, 4085603, 193690690});
    
    printf("Position 3\n");
    run_perft("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -", {14, 191, 2812, 43238, 674624, 11030083});
    
    printf("Position 4\n");
    run_perft("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq", {6, 264, 9467, 422333, 15833292});
    
    printf("Position 5\n");
    run_perft("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ", {44, 1486, 62379, 2103487, 89941194});
    
    printf("Position 6\n");
    run_perft("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w -", {46, 2079, 89890, 3894594, 164075551});
}

void run_ai_test()
{
   Game game;
   Search search(game);
   int search_depth = 5;

   int max_turns = 100;
   int turns = 0;

   auto fn = [&]() {
      while (turns < max_turns && !game.is_checkmated() && !game.are_no_moves_left()) {
         Move best_move = search.get_best_move(3);
         if (Moves::get_to(best_move) != Moves::get_from(best_move)) {
            game.move(best_move);
         } else {
            return;
         }
         turns++;
      }
   };

   float time_elapsed = measure(fn);

   printf("Simulated at depth %d in %d turns. %d TPS.\n", search_depth, turns, (int)(static_cast<float>(turns) / (static_cast<float>(time_elapsed) / 1000.0)));
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

        printf("%d to %d: %d moves\n", Moves::get_from(moves[i]), Moves::get_to(moves[i]), count);
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
