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

    for (int x = 0; x < 8; x++)
    {
        for (int y = 0; y < 8; y++) {
            if (g.get(x, y) != Pieces::EMPTY) {
                Bitboards::print(Pieces::get_bishop_moves(x, y, g));
            }
        }
    }

    return 0;
}