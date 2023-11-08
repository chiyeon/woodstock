#include <stdio.h>
#include <vector>
#include <string.h>
#include <iostream>
#include <sstream>
#include "game.h"
#include "measure.h"
#include "bitboard.h"
#include "search.h"
#include "utils.h"
/*
int square_to_index(char * square) {
    int x, y;

    switch (square[0]) {
        case 'a': x = 7;
            break;
        case 'b': x = 6;
            break;
        case 'c': x = 5;
            break;
        case 'd': x = 4;
            break;
        case 'e': x = 3;
            break;
        case 'f': x = 2;
            break;
        case 'g': x = 1;
            break;
        case 'h': x = 0;
            break;
    }

    y = (int)(square[1]) - '1';

    return y * 8 + x;
}

std::string index_to_square(int index)
{
   int x = 7 - (index % 8);
   int y = index / 8;
   std::stringstream ss;
   ss << (char)('a' + x) << (char)('1' + y);
   return ss.str();
}

// converts move to algebreaic notation
std::string move_to_an(Move move) {
    std::stringstream ss;

    bool is_pawn = (Moves::get_piece(move) & Pieces::FILTER_PIECE) != Pieces::PAWN;
    bool capture = Moves::get_captured(move) != 0;

    if (!is_pawn) {
        ss << Pieces::name_short(Moves::get_piece(move));
    }

    if (capture) {
        if (is_pawn) {
            // get column
            ss << index_to_square(Moves::get_from(move) % 8);
        }
      ss << 'x';
   }

   ss << index_to_square(Moves::get_to(move));
   return ss.str();
}

// converst move to long algebraic notiation
std::string move_to_lan(Move move)
{
   std::stringstream ss;

   if ((Moves::get_piece(move) & Pieces::FILTER_PIECE) != Pieces::PAWN) {
      ss << Pieces::name_short(Moves::get_piece(move)); 
   }

   ss << index_to_square(Moves::get_from(move));

   if (Moves::get_captured(move) != 0) {
      ss << 'x';
   }

   ss << index_to_square(Moves::get_to(move));
   return ss.str();
}*/

#ifdef EMSCRIPTEN
#include <emscripten/emscripten.h>
#endif


void run_game_simulation(int depth1, int depth2)
{
    Game game;
    Search search(game);

    printf("Running simulation of depth %d vs depth %d. (WvB)\n", depth1, depth2);
    
    do {
        Move move = search.get_best_move(game.is_whites_turn() ? depth1 : depth2);
        game.move(move, true);
        printf("Move Made\n");
    } while (!game.is_gameover());

    game.print();

    // if (game.bcm) {
    //     printf("Black wins!\n");
    // } else if (game.wcm) {
    //     printf("White wins!\n");
    // } else {
    //     printf("Draw.\n");
    // }
}



#ifdef EMSCRIPTEN
#ifdef __cplusplus
#define EXTERN extern "C"
#else
#define EXTERN
#endif

// Game game("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R");
Game game;
Search search(game);
int search_depth = 6;
int num_threads = 4;
bool is_player_black = false;

std::vector<Move> selected_piece_moves;

EM_JS(void, highlight_squares, (int sq1, int sq2), {
    board.highlight_squares(sq1, sq2);
});

EM_JS(void, highlight_moves, (int * squares), {
    board.remove_move_highlights();

    // 27 = max num of moves a single piece can go
    for (let i = 0; i < 27; i++) {
        const INT_SIZE = 4;
        let val = HEAP32[(squares + INT_SIZE * i) >> 2];

        if (val == -1) return;
        board.highlight_move(val);    // squares are -1 if no data
    }
});

EM_JS(void, update_chessboard, (Piece * board_data), {
    board.remove_move_highlights();

    for (let i = 0; i < 64; i++) {
        const INT_SIZE = 4;

        board.board[i] = HEAP32[(board_data + INT_SIZE * i) >> 2];
    }

    board.update_chessboard();
})

EXTERN EMSCRIPTEN_KEEPALIVE void make_best_move(int argc, char ** argv)
{
    if (game.is_gameover()) {
        if (game.wcm) printf("White checkmate\n");
        else if (game.bcm) printf("Black checkmate\n");
        else if (game.draw) printf("Draw\n");
        return;
    }

    // do some resetting
    selected_piece_moves.clear();
    EM_ASM({board.selected_piece = undefined});

    bool move_exists = false;
    int depth_searched = 0;
    //int time_elapsed_search = 0;

    auto make_best_move = [&]() {
        search.start_search_timer();
        Move best_move = search.get_best_move(search_depth);
        depth_searched = search_depth;
        //Move best_move = search.get_best_move_iterative_deepening(depth_searched);
        if (Moves::get_to(best_move) != Moves::get_from(best_move)) {
            game.move(best_move, true);
            move_exists = true;
        }
    };

    int time_elapsed = measure(make_best_move);

    if (!move_exists) {
        printf("No moves exist.\n");
        return;
    }

    printf("Found best move at depth %d with %d evaluations in %dms!\n", depth_searched, search.num_positions_evaluated, time_elapsed);
    search.num_positions_evaluated = 0;

    Move last_move = game.get_last_move();

    update_chessboard(game.get_board());
    highlight_squares(Moves::get_from(last_move), Moves::get_to(last_move));
}

EXTERN EMSCRIPTEN_KEEPALIVE void click_square(int index)
{
    if (game.is_gameover()) {
        if (game.wcm) printf("White checkmate\n");
        else if (game.bcm) printf("Black checkmate\n");
        else if (game.draw) printf("Draw\n");
        return;
    }
    std::vector<Move> moves = game.get_moves_at_square(index);
    int moves_size = moves.size();

    if (moves_size == 0 
        && (game.get(index) & Pieces::FILTER_COLOR) != game.get_turn()) {
        
        for (auto & move : selected_piece_moves)
        {
            if (Moves::get_to(move) == index) {
                game.move(move, true);
                update_chessboard(game.get_board());
                highlight_squares(Moves::get_from(move), Moves::get_to(move));
                EM_ASM({make_ai_move()});
                break;
            }
        }
    } else {
        /* USER IS HIGHLIGHTING A PIECE */
        
        // highlight pieces & select our piece
        EM_ASM({board.select_piece($0)}, index);

        int move_indexes[27]; // 27 is maximum num of moves a single piece can make
        
        int i;
        for (i = 0; i < moves_size; ++i) {
            move_indexes[i] = Moves::get_to(moves[i]);
        }
        move_indexes[i] = -1;

        highlight_moves(move_indexes);
        selected_piece_moves = moves;
    }
}

EXTERN EMSCRIPTEN_KEEPALIVE void load_fen(char * fen)
{
    game.read_fen((std::string)fen);
    EM_ASM({board.remove_highlight_squares()});
    update_chessboard(game.get_board());
}

EXTERN EMSCRIPTEN_KEEPALIVE void undo()
{
    game.undo();
    update_chessboard(game.get_board());

    if (game.is_history_empty()) {
        EM_ASM({board.remove_highlight_squares()});
    } else {
        Move last_move = game.get_last_move();
        highlight_squares(Moves::get_from(last_move), Moves::get_to(last_move));
    }
}

EXTERN EMSCRIPTEN_KEEPALIVE void print_pgn(char * name)
{
   std::cout << game.get_pgn(name, is_player_black) << std::endl;
}

EXTERN EMSCRIPTEN_KEEPALIVE void switch_sides()
{
   is_player_black = !is_player_black;
   EM_ASM({make_ai_move()});
}

EXTERN EMSCRIPTEN_KEEPALIVE void reset_game()
{
   game.reset();
   game.read_fen();
   EM_ASM({board.remove_highlight_squares()});
   update_chessboard(game.get_board());
}

int main()
{
    printf("woodstock!\n");
    EM_ASM({hide_loading_hint()});
    return 0;
}
#else

int main()
{
   
   run_perft_suite_mini();
   std::string input;
   std::cin >> input;
   Game g;
   Search s(g);
   bool uci_mode = (input == "uci");

   if (uci_mode) {
      std::cout << "id name Woodstock" << std::endl;
      std::cout << "id author Benjamin W." << std::endl;
      std::cout << "uciok" << std::endl;
   }

   while(uci_mode && (std::cin >> input)) {
      std::stringstream ss(input);
      std::string curr;

      ss >> curr;

      if (curr == "ucinewgame") {
         g.reset();
         std::cout << "isready" << std::endl;
      } else if (curr == "position") {
         ss >> curr; // fen or startpos
         if (curr == "startpos") {
            g.read_fen();
         } else if (curr == "fen") {
            ss >> curr; // fen string
            g.read_fen(curr);
         }
      } else if (curr == "go") {
         ss >> curr;
         // next arg
         if (curr == "depth") {
            int depth;
            ss >> curr;
            std::cout << curr << std::endl;
            Move best_move = s.get_best_move(depth);
            std::cout << "bestmove " << Utils::move_to_lan(best_move) << std::endl;
         }
      }
   }
   
   return 0;
}

#endif
