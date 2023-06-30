#include <stdio.h>
#include "bitboard.h"

int main()
{
    printf("woodstock!\n");
    Bitboard b = Bitboards::get(0, 0);
    Bitboards::print(b);
    return 0;
}