#include "bitboard.h"
#include "timer.h"

void measure(auto fn) {
   Timer t;
   double time_taken;

   t.start();
   fn();
   t.elapsedUserTime(time_taken);

   printf("%fs\n", time_taken);
}

/*
 * tests construction of diagonals from a given position using
 * entirely bit shifting vs looping. The problem is that the diagonals
 * must be truncated, so we can either switch/case and return a preset
 * starting value, OR we can build it manually every time.
 */
// void test_switchcase_vs_loopbuild_for_diagonals()
// {
//     auto test_switchcase = []() {
//         for (int N = 0; N < 1; N++) {
//             for (int x = 0; x < 8; x++) {
//                 for (int y = 0; y < 8; y++) {

//                     if (Bitboards::get_diagonal_downwards_right_loopbuild(x,
//                     y) !=
//                     Bitboards::get_diagonal_downwards_right_switchcase(x, y))
//                     {
//                         printf("mismatch!");
//                     }
//                 }
//             }
//         }
//     };

//     auto test_loopbuild = []() {
//         for (int N = 0; N < 10000; N++) {
//             for (int x = 0; x < 8; x++) {
//                 for (int y = 0; y < 8; y++) {
//                     Bitboards::get_diagonal_downwards_right_loopbuild(x, y);
//                 }
//             }
//         }
//     };

//     printf("Testing switch/case (N=10000): ");
//     measure(test_switchcase);

//     // printf("Testing loop builder (N=10000): ");
//     // measure(test_loopbuild);
// }