#include <stdio.h>
#include "game.h"
// #include "tests.h"

int main()
{
    printf("woodstock!\n");
  
    // Game g;
    // g.print();

    //test_switchcase_vs_loopbuild_for_diagonals();

    Game g("8/1N6/8/8/4N3/8/8/8");
    g.print();

    int * positions = Bitboards::bitboard_to_positions(g.get_game_bitboard());
    
    for (int i = 0; i < sizeof(positions) / sizeof(int); ++i) {
        Bitboards::print(Pieces::get_knight_moves(positions[i] % 8, positions[i] / 8, g));
    }

    // Bitboards::print(g.get_game_bitboard());
    // printf("Number of pieces: %d\n", Bitboards::bit_count(g.get_game_bitboard()));


    return 0;
}