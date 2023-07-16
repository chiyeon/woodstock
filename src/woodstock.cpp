#include <stdio.h>
#include <vector>
#include "game.h"
#include "measure.h"
#include "bitboard.h"

int main()
{
    printf("woodstock!\n");
  
    // Game g("1p6/8/1R2p3/8/8/8/1R1p4/8");
    // g.print();
    // std::vector<Move> s;
    // g.get_moves(s);

    //test_switchcase_vs_loopbuild_for_diagonals();

    // Bitboards::print(Bitboards::get_diagonal_downwards_left(4, 4));
    // Bitboards::print(Bitboards::get_diagonal_downwards_left(0, 0));
    // Bitboards::print(Bitboards::get_diagonal_downwards_left(0, 1));
    // Bitboards::print(Bitboards::get_diagonal_downwards_left(1, 1));
    // Bitboards::print(Bitboards::get_diagonal_downwards_left(1, 0));

    // Game g;
    // g.print();

    // std::vector<Move> moves = g.get_moves();
    // for (auto & move : moves) {
    //     printf("Move from %d to %d\n", move.from, move.to);
    // }
    // measure_count_bulk_positions(g, 1, true);

    // auto fn = [&]() {
    //     for (int i = 0; i < 100000; ++i) {
    //         std::vector<Move> moves;
    //         g.get_moves(moves);
    //         for (int j = 0; j < moves.size(); ++j) {
    //             g.move(moves[j]);
    //             std::vector<int> pos;
    //             Bitboards::bitboard_to_positions(pos, g.get_game_bitboard());
    //             g.undo();
    //         }
    //     }
    // };

    // printf("Took %dms\n", measure(fn));

    Game g;
    g.print();
    measure_count_bulk_positions(g, 1, true);
    measure_count_bulk_positions(g, 2);
    measure_count_bulk_positions(g, 3);
    measure_count_bulk_positions(g, 4);
    measure_count_bulk_positions(g, 5);

    // auto run_get_moves = [&]() {
    //     std::vector<Move> moves = g.get_moves();
    //     printf("Calculated %d moves. ", moves.size());
    // };

    // measure(run_get_moves);

    // for (int i = 0; i < moves.size(); ++i) {
    //     Bitboards::print(Bitboards::get_i(moves[i].to) | Bitboards::get_i(moves[i].from));
    // }

    // Bitboards::print(g.get_game_bitboard());
    // printf("Number of pieces: %d\n", Bitboards::bit_count(g.get_game_bitboard()));

    return 0;
}