#include <stdio.h>
#include "game.h"
// #include "tests.h"

int main()
{
    printf("woodstock!\n");
  
    // Game g;
    // g.print();

    //test_switchcase_vs_loopbuild_for_diagonals();

    Game g("B6B/8/2B2B2/8/4B3/1B6/5B2/B6B");
    g.print();

    int * positions = Bitboards::bitboard_to_positions(g.get_game_bitboard());
    for (int i = 0; i <= sizeof(positions); ++i) {
        printf("Found at %d\n", positions[i]);
    }

    // Bitboards::print(g.get_game_bitboard());
    // printf("Number of pieces: %d\n", Bitboards::bit_count(g.get_game_bitboard()));


    return 0;
}