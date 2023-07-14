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

    Game g("N6N/1N4N1/8/3N4/5N2/8/1N4N1/N6N");
    // g.print();
    // g.get_moves();

    measure_count_bulk_positions(g, 1);
    // measure_count_bulk_positions(g, 2);
    // measure_count_bulk_positions(g, 3);
    // measure_count_bulk_positions(g, 4);
    // measure_count_bulk_positions(g, 5);
    // measure_count_bulk_positions(g, 6);

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