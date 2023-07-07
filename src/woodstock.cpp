#include <stdio.h>
#include "game.h"
// #include "tests.h"

int main()
{
    printf("woodstock!\n");
  
    // Game g;
    // g.print();

    //test_switchcase_vs_loopbuild_for_diagonals();

    Bitboards::print(Bitboards::get_diagonal_upwards_left(3, 4));

    return 0;
}