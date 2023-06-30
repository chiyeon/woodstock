#include "piece.h"
#include <map>

class Game
{
    Piece board[64];

public:
    Game();
    void read_fen(Piece * board, std::string fen);
    void print();

private:
    static std::map<char, Piece> fen_to_piece;
};