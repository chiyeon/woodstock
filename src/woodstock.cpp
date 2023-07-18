#include <stdio.h>
#include <vector>
#include "game.h"
#include "measure.h"
#include "bitboard.h"

int get_num_castles_at_depth(int depth, Game & g)
{
    std::vector<Move> moves;
    g.get_moves(moves);
    int count = 0;

    for (auto & move : moves) {
        if (depth <= 1) {
            if (move.flag == CASTLE_BOTH_ALLOWED
            || move.flag == CASTLE_QUEENSIDE_ALLOWED
            || move.flag == CASTLE_KINGSIDE_ALLOWED) {
                count++;
            }
        } else {
            g.move(move);
            count += get_num_castles_at_depth(depth - 1, g);
            g.undo();
        }
    }

    return count;
}

int main()
{
    printf("woodstock!\n");
    
    Game g("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R");
    measure_count_bulk_positions(g, 1);
    measure_count_bulk_positions(g, 2);
    measure_count_bulk_positions(g, 3);
    measure_count_bulk_positions(g, 4);
    measure_count_bulk_positions(g, 5);
    measure_count_bulk_positions(g, 6);

    // Game g("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R");
    

    // measure_count_bulk_positions(g, 1);
    // measure_count_bulk_positions(g, 2);
    // measure_count_bulk_positions(g, 3);
    // measure_count_bulk_positions(g, 4);
    // measure_count_bulk_positions(g, 5);
    // measure_count_bulk_positions(g, 6);

    // measure_count_bulk_positions(g, 3);
    // printf("There were %d En Passants.\n", g.en_passant_count);
    // g.en_passant_count = 0;
    // measure_count_bulk_positions(g, 4);
    // printf("There were %d En Passants.\n", g.en_passant_count);
    // g.en_passant_count = 0;

    // Game g("8/8/8/8/8/8/8/R3K2R");
    // g.print();
    // // g.switch_turns();
    // // printf(g.in_check() ? "We are in check!\n" : "Not in check.\n");
    // measure_count_bulk_positions(g, 1, true);
    // g.print();

    // test en passant basic case
    // Game g("8/8/8/8/p2p4/8/1P6/8");
    // std::vector<Move> moves;
    // g.get_moves(moves);
    // g.move(moves[1]);
    // // moves.clear();
    // // g.get_moves(moves);
    // // g.move(moves[0]);
    // g.print();
    // measure_count_bulk_positions(g, 1, true);
    // g.print();
  
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

    // Game g("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R");
    // std::vector<Move> s;
    // g.get_moves(s);
    // g.move(s[0]);
    // g.print();
    // measure_count_bulk_positions(g, 1, true);

    // Game g("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R");
    // g.print();
    // measure_count_bulk_positions(g, 1);
    // measure_count_bulk_positions(g, 2);
    // printf("There were %d En Passants.\n", g.en_passant_count);
    // g.en_passant_count = 0;
    // measure_count_bulk_positions(g, 3);
    // printf("There were %d En Passants.\n", g.en_passant_count);
    // g.en_passant_count = 0;
    // measure_count_bulk_positions(g, 4);
    // printf("There were %d En Passants.\n", g.en_passant_count);
    // g.en_passant_count = 0;
    // measure_count_bulk_positions(g, 5);
    // measure_count_bulk_positions(g, 6);
    // printf("There were %d En Passants.\n", g.en_passant_count);
    // printf("There were %d captures.\n", g.captures);

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