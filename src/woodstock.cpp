#include <stdio.h>
#include <vector>
#include "game.h"
#include "measure.h"
#include "bitboard.h"

int main()
{
    printf("woodstock!\n");
  
    // Game g;
    // g.print();

    //test_switchcase_vs_loopbuild_for_diagonals();

    Game g;
    g.print();
    // std::vector<Move> moves = g.get_moves();
    // for (auto & move : moves) {
    //     printf("Move from %d to %d\n", move.from, move.to);
    // }
    // measure_count_bulk_positions(g, 1, true);

    measure_count_bulk_positions(g, 1);
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