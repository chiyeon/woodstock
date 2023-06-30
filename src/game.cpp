#include <stdio.h>
#include <cctype>
#include "game.h"

Game::Game()
{
    read_fen(board, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
}

void Game::read_fen(Piece * board, std::string fen)
{
    int fen_length = fen.size();
    int index = 0;
    for (int i = 0; i < fen_length; ++i) {
        // read number of spaces
        if (isdigit(fen[i])) {
            int num_spaces = int(fen[i] - '0');
            for (int j = 0; j < num_spaces; ++j) {
                board[index] = Pieces::EMPTY;
                index++;
            }
        } else if (fen[i] != '/') {
            board[index] = Pieces::piece_from_name_short(fen[i]);
            index++;
        }
    }
}

void Game::print()
{
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            Piece piece = board[x + y * 8];
            printf("%c ", piece == Pieces::EMPTY 
                ? ((y % 2 + x) % 2 == 0 ? ',' : '.') 
                : Pieces::name_short(piece)
            );
        }
        printf("\n");
    }
}