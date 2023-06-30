#include "piece.h"

// std::string Pieces::name_full(Piece piece)
// {

// }

char Pieces::name_short(Piece piece)
{
    return piece_to_name_short[piece];
}

Piece Pieces::piece_from_name_short(char name)
{
    return name_short_to_piece[name];
}

// std::map<Piece, std::string> Pieces::piece_to_name_full = {
//     {WHITE | PAWN, 'white pawn'},
//     {WHITE | KNIGHT, 'white knight'},
//     {WHITE | BISHOP, 'white bishop'},
//     {WHITE | ROOK, 'white rook'},
//     {WHITE | QUEEN, 'white queen'},
//     {WHITE | KING, 'white king'},
//     {BLACK | PAWN, 'black pawn'},
//     {BLACK | KNIGHT, 'black knight'},
//     {BLACK | BISHOP, 'black bishop'},
//     {BLACK | ROOK, 'black rook'},
//     {BLACK | QUEEN, 'black queen'},
//     {BLACK | KING, 'black king'},
// };

std::map<Piece, char> Pieces::piece_to_name_short = {
    {WHITE | PAWN, 'P'},
    {WHITE | KNIGHT, 'N'},
    {WHITE | BISHOP, 'B'},
    {WHITE | ROOK, 'R'},
    {WHITE | QUEEN, 'Q'},
    {WHITE | KING, 'K'},
    {BLACK | PAWN, 'p'},
    {BLACK | KNIGHT, 'n'},
    {BLACK | BISHOP, 'b'},
    {BLACK | ROOK, 'r'},
    {BLACK | QUEEN, 'q'},
    {BLACK | KING, 'k'},
};

std::map<char, Piece> Pieces::name_short_to_piece = {
    {'P', WHITE | PAWN},
    {'N', WHITE | KNIGHT},
    {'B', WHITE | BISHOP},
    {'R', WHITE | ROOK},
    {'Q', WHITE | QUEEN},
    {'K', WHITE | KING},
    {'p', BLACK | PAWN},
    {'n', BLACK | KNIGHT},
    {'b', BLACK | BISHOP},
    {'r', BLACK | ROOK},
    {'q', BLACK | QUEEN},
    {'k', BLACK | KING}
};

// will eventually include get_[...]_moves definitions