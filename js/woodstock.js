const   PAWN = "p",
        KNIGHT = "n",
        BISHOP = "b",
        ROOK = "r",
        QUEEN = "q",
        KING = "k",
        WHITE = "w",
        BLACK = "b"

const piece_values = {
    [PAWN]: 10,
    [KNIGHT]: 30,
    [BISHOP]: 31,
    [ROOK]: 45,
    [QUEEN]: 90,
    [KING]: 2000
}

let positions_evaluated = 0
var openings = []
var is_opening = true

var reverse_array = (array) => {
    return array.slice().reverse();
};

const white_pawn_table = [
    [0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0],
    [5.0,  6.0,  5.0,  5.0,  5.0,  6.0,  5.0,  5.0],
    [1.0,  1.0,  2.0,  3.0,  3.0,  2.0,  1.0,  1.0],
    [0.5,  0.5,  1.0,  2.5,  2.5,  1.0,  0.5,  0.5],
    [0.0,  0.0,  0.0,  2.0,  2.0,  0.0,  0.0,  0.0],
    [0.5, -0.5, -1.0,  0.0,  0.0, -1.0, -0.5,  0.5],
    [-2,  1.0, -1.0,  -3.0, -2.0,  1.0,  1.0,  -1],
    [0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0]
]

const white_knight_table = [
    [-5.0, -4.0, -3.0, -3.0, -3.0, -3.0, -4.0, -5.0],
    [-4.0, -2.0,  0.0,  0.0,  0.0,  0.0, -2.0, -4.0],
    [-3.0,  0.0,  1.0,  1.5,  1.5,  1.0,  0.0, -3.0],
    [-3.0,  0.5,  1.5,  2.0,  2.0,  1.5,  0.5, -3.0],
    [-3.0,  0.0,  1.5,  2.0,  2.0,  1.5,  0.0, -3.0],
    [-3.0,  0.5,  1.0,  1.5,  1.5,  1.0,  0.5, -3.0],
    [-4.0, -2.0,  0.0,  0.5,  0.5,  0.0, -2.0, -4.0],
    [-5.0, -4.0, -3.0, -3.0, -3.0, -3.0, -4.0, -5.0]
]

const white_bishop_table = [
    [ -2.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -2.0],
        [ -1.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -1.0],
        [ -1.0,  0.0,  0.5,  1.0,  1.0,  0.5,  0.0, -1.0],
        [ -1.0,  0.5,  0.5,  1.0,  1.0,  0.5,  0.5, -1.0],
        [ -1.0,  0.0,  1.0,  1.0,  1.0,  1.0,  0.0, -1.0],
        [ -1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0, -1.0],
        [ -1.0,  0.5,  0.0,  0.0,  0.0,  0.0,  0.5, -1.0],
        [ -2.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -2.0]
]

const white_rook_table = [
    [  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0],
        [  0.5,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  0.5],
        [ -0.5,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -0.5],
        [ -0.5,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -0.5],
        [ -0.5,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -0.5],
        [ -0.5,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -0.5],
        [ -0.5,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -0.5],
        [  0.0,   0.0, 0.0,  0.5,  0.5,  0.0,  0.0,  0.0]
]

const white_queen_table = [
    [ -2.0, -1.0, -1.0, -0.5, -0.5, -1.0, -1.0, -2.0],
        [ -1.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -1.0],
        [ -1.0,  0.0,  0.5,  0.5,  0.5,  0.5,  0.0, -1.0],
        [ -0.5,  0.0,  0.5,  0.5,  0.5,  0.5,  0.0, -0.5],
        [  0.0,  0.0,  0.5,  0.5,  0.5,  0.5,  0.0, -0.5],
        [ -1.0,  0.5,  0.5,  0.5,  0.5,  0.5,  0.0, -1.0],
        [ -1.0,  0.0,  0.5,  0.0,  0.0,  0.0,  0.0, -1.0],
        [ -2.0, -1.0, -1.0, -0.5, -0.5, -1.0, -1.0, -2.0]
]

const white_king_table = [
    [ -3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0],
        [ -3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0],
        [ -3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0],
        [ -3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0],
        [ -2.0, -3.0, -3.0, -4.0, -4.0, -3.0, -3.0, -2.0],
        [ -1.0, -2.0, -2.0, -2.0, -2.0, -2.0, -2.0, -1.0],
        [  2.0,  2.0,  0.0,  0.0,  0.0,  0.0,  2.0,  2.0 ],
        [  2.0,  3.0,  1.0,  0.0,  0.0,  1.0,  3.0,  2.0 ]
]

const black_pawn_table = reverse_array(white_pawn_table)

const black_knight_table = reverse_array(white_knight_table)

const black_bishop_table = reverse_array(white_bishop_table)

const black_rook_table = reverse_array(white_rook_table)

const black_queen_table = reverse_array(white_queen_table)

const black_king_table = reverse_array(white_king_table)

const white_position_table = {
    [PAWN]: [
        [0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0],
        [5.0,  6.0,  5.0,  5.0,  5.0,  6.0,  5.0,  5.0],
        [1.0,  1.0,  2.0,  3.0,  3.0,  2.0,  1.0,  1.0],
        [0.5,  0.5,  1.0,  2.5,  2.5,  1.0,  0.5,  0.5],
        [0.0,  0.0,  0.0,  2.0,  2.0,  0.0,  0.0,  0.0],
        [0.5, -0.5, -1.0,  0.0,  0.0, -1.0, -0.5,  0.5],
        [-2,  1.0, -1.0,  -3.0, -2.0,  1.0,  1.0,  -1],
        [0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0]
    ],
    [KNIGHT]: [
        [-5.0, -4.0, -3.0, -3.0, -3.0, -3.0, -4.0, -5.0],
        [-4.0, -2.0,  0.0,  0.0,  0.0,  0.0, -2.0, -4.0],
        [-3.0,  0.0,  1.0,  1.5,  1.5,  1.0,  0.0, -3.0],
        [-3.0,  0.5,  1.5,  2.0,  2.0,  1.5,  0.5, -3.0],
        [-3.0,  0.0,  1.5,  2.0,  2.0,  1.5,  0.0, -3.0],
        [-3.0,  0.5,  1.0,  1.5,  1.5,  1.0,  0.5, -3.0],
        [-4.0, -2.0,  0.0,  0.5,  0.5,  0.0, -2.0, -4.0],
        [-5.0, -4.0, -3.0, -3.0, -3.0, -3.0, -4.0, -5.0]
    ],
    [BISHOP]: [
        [ -2.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -2.0],
        [ -1.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -1.0],
        [ -1.0,  0.0,  0.5,  1.0,  1.0,  0.5,  0.0, -1.0],
        [ -1.0,  0.5,  0.5,  1.0,  1.0,  0.5,  0.5, -1.0],
        [ -1.0,  0.0,  1.0,  1.0,  1.0,  1.0,  0.0, -1.0],
        [ -1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0, -1.0],
        [ -1.0,  0.5,  0.0,  0.0,  0.0,  0.0,  0.5, -1.0],
        [ -2.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -2.0]
    ],
    [ROOK]: [
        [  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0],
        [  0.5,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  0.5],
        [ -0.5,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -0.5],
        [ -0.5,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -0.5],
        [ -0.5,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -0.5],
        [ -0.5,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -0.5],
        [ -0.5,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -0.5],
        [  0.0,   0.0, 0.0,  0.5,  0.5,  0.0,  0.0,  0.0]
    ],
    [QUEEN]: [
        [ -2.0, -1.0, -1.0, -0.5, -0.5, -1.0, -1.0, -2.0],
        [ -1.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -1.0],
        [ -1.0,  0.0,  0.5,  0.5,  0.5,  0.5,  0.0, -1.0],
        [ -0.5,  0.0,  0.5,  0.5,  0.5,  0.5,  0.0, -0.5],
        [  0.0,  0.0,  0.5,  0.5,  0.5,  0.5,  0.0, -0.5],
        [ -1.0,  0.5,  0.5,  0.5,  0.5,  0.5,  0.0, -1.0],
        [ -1.0,  0.0,  0.5,  0.0,  0.0,  0.0,  0.0, -1.0],
        [ -2.0, -1.0, -1.0, -0.5, -0.5, -1.0, -1.0, -2.0]
    ],
    [KING]: [

        [ -3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0],
        [ -3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0],
        [ -3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0],
        [ -3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0],
        [ -2.0, -3.0, -3.0, -4.0, -4.0, -3.0, -3.0, -2.0],
        [ -1.0, -2.0, -2.0, -2.0, -2.0, -2.0, -2.0, -1.0],
        [  2.0,  2.0,  0.0,  0.0,  0.0,  0.0,  2.0,  2.0 ],
        [  2.0,  3.0,  1.0,  0.0,  0.0,  1.0,  3.0,  2.0 ]
    ]
}

const black_position_table = {
    [PAWN]: reverse_array([
        [0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0],
        [5.0,  5.0,  5.0,  5.0,  5.0,  5.0,  5.0,  5.0],
        [1.0,  1.0,  2.0,  3.0,  3.0,  2.0,  1.0,  1.0],
        [0.5,  0.5,  1.0,  2.5,  2.5,  1.0,  0.5,  0.5],
        [0.0,  0.0,  0.0,  2.0,  2.0,  0.0,  0.0,  0.0],
        [0.5, -0.5, -1.0,  0.0,  0.0, -1.0, -0.5,  0.5],
        [0.5,  1.0, 1.0,  -2.0, -2.0,  1.0,  1.0,  0.5],
        [0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0]
    ]),
    [KNIGHT]: reverse_array([
        [-5.0, -4.0, -3.0, -3.0, -3.0, -3.0, -4.0, -5.0],
        [-4.0, -2.0,  0.0,  0.0,  0.0,  0.0, -2.0, -4.0],
        [-3.0,  0.0,  1.0,  1.5,  1.5,  1.0,  0.0, -3.0],
        [-3.0,  0.5,  1.5,  2.0,  2.0,  1.5,  0.5, -3.0],
        [-3.0,  0.0,  1.5,  2.0,  2.0,  1.5,  0.0, -3.0],
        [-3.0,  0.5,  1.0,  1.5,  1.5,  1.0,  0.5, -3.0],
        [-4.0, -2.0,  0.0,  0.5,  0.5,  0.0, -2.0, -4.0],
        [-5.0, -4.0, -3.0, -3.0, -3.0, -3.0, -4.0, -5.0]
    ]),
    [BISHOP]: reverse_array([
        [ -2.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -2.0],
        [ -1.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -1.0],
        [ -1.0,  0.0,  0.5,  1.0,  1.0,  0.5,  0.0, -1.0],
        [ -1.0,  0.5,  0.5,  1.0,  1.0,  0.5,  0.5, -1.0],
        [ -1.0,  0.0,  1.0,  1.0,  1.0,  1.0,  0.0, -1.0],
        [ -1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0, -1.0],
        [ -1.0,  0.5,  0.0,  0.0,  0.0,  0.0,  0.5, -1.0],
        [ -2.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -2.0]
    ]),
    [ROOK]: reverse_array([
        [  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0],
        [  0.5,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  0.5],
        [ -0.5,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -0.5],
        [ -0.5,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -0.5],
        [ -0.5,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -0.5],
        [ -0.5,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -0.5],
        [ -0.5,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -0.5],
        [  0.0,   0.0, 0.0,  0.5,  0.5,  0.0,  0.0,  0.0]
    ]),
    [QUEEN]: reverse_array([
        [ -2.0, -1.0, -1.0, -0.5, -0.5, -1.0, -1.0, -2.0],
        [ -1.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -1.0],
        [ -1.0,  0.0,  0.5,  0.5,  0.5,  0.5,  0.0, -1.0],
        [ -0.5,  0.0,  0.5,  0.5,  0.5,  0.5,  0.0, -0.5],
        [  0.0,  0.0,  0.5,  0.5,  0.5,  0.5,  0.0, -0.5],
        [ -1.0,  0.5,  0.5,  0.5,  0.5,  0.5,  0.0, -1.0],
        [ -1.0,  0.0,  0.5,  0.0,  0.0,  0.0,  0.0, -1.0],
        [ -2.0, -1.0, -1.0, -0.5, -0.5, -1.0, -1.0, -2.0]
    ]),
    [KING]: reverse_array([

        [ -3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0],
        [ -3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0],
        [ -3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0],
        [ -3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0],
        [ -2.0, -3.0, -3.0, -4.0, -4.0, -3.0, -3.0, -2.0],
        [ -1.0, -2.0, -2.0, -2.0, -2.0, -2.0, -2.0, -1.0],
        [  2.0,  2.0,  0.0,  0.0,  0.0,  0.0,  2.0,  2.0 ],
        [  2.0,  3.0,  1.0,  0.0,  0.0,  1.0,  3.0,  2.0 ]
    ])
}

// from rust-chess.org
// these use indices... KING = 0 down to pawn = 4
const MVV_LVA_table = [
    [0, 0, 0, 0, 0, 0, 0],       // victim K, attacker K, Q, R, B, N, P, None
    [50, 51, 52, 53, 54, 55, 0], // victim Q, attacker K, Q, R, B, N, P, None
    [40, 41, 42, 43, 44, 45, 0], // victim R, attacker K, Q, R, B, N, P, None
    [30, 31, 32, 33, 34, 35, 0], // victim B, attacker K, Q, R, B, N, P, None
    [20, 21, 22, 23, 24, 25, 0], // victim N, attacker K, Q, R, B, N, P, None
    [10, 11, 12, 13, 14, 15, 0], // victim P, attacker K, Q, R, B, N, P, None
    [0, 0, 0, 0, 0, 0, 0],       // victim None, attacker K, Q, R, B, N, P, None
]

const char_to_index = (piece) => {
    switch (piece) {
        case KING: return 0;
        case QUEEN: return 1;
        case ROOK: return 2;
        case BISHOP: return 3;
        case KNIGHT: return 4;
        case PAWN: return 5;
        default: return 6;
    }
}

const piece_score = (piece, x, y) => {
    if (!piece) return 0
    let score = piece_values[piece.type] + (piece.color == "w" ? white_position_table[piece.type][y][x] : black_position_table[piece.type][y][x])
    
    return score
}

// evaluates the position where positive = white favor 
const evaluate_position = (game, raw_moves) => {
    let board = game.board()

    let white_score = 0
    let black_score = 0

    // for (let x = 0; x < 8; x++) {
    //     for (let y = 0; y < 8; y++) {
    //         let piece = board[y][x]

    //         if (!piece) continue

    //         if (piece.color == "w") {
    //             white_score += piece_score(piece, x, y)
    //         } else {
    //             black_score += piece_score(piece, x, y)
    //         }
    //     }
    // }

    // return white_score - black_score

    // keep record of number of number of black pieces attacked by white and vice versa
    let white_attacking_score = 0
    let black_attacking_score = 0

    // number of possible mvoes per side
    let white_mobility = 0,
        black_mobility = 0
    
    let white_bishop_count = 0,
        black_bishop_count = 0
    
    let white_double_pawn_count = 0,
        black_double_pawn_count = 0

    for (let x = 0; x < 8; x++) {
        for (let y = 0; y < 8; y++) {
            let piece = board[y][x]

            if (!piece) continue

            if (piece.color == "w") {
                switch(piece.type) {
                    case PAWN:
                        white_score = white_score
                            + piece_values[piece.type]
                            + white_pawn_table[y][x]
                        break
                    case KNIGHT:
                        white_score = white_score
                            + piece_values[piece.type]
                            + white_knight_table[y][x]
                        break
                    case BISHOP:
                        white_score = white_score
                            + piece_values[piece.type]
                            + white_bishop_table[y][x]
                        break
                    case ROOK:
                        white_score = white_score
                            + piece_values[piece.type]
                            + white_rook_table[y][x]
                        break
                    case QUEEN:
                        white_score = white_score
                            + piece_values[piece.type]
                            + white_queen_table[y][x]
                        break
                    case KING:
                        white_score = white_score
                            + piece_values[piece.type]
                            + white_king_table[y][x]
                        break
                }
                
                // switch(piece.type) {
                //     case BISHOP:
                //         white_bishop_count++;
                //         break;
                //         case PAWN:
                //             // check for "stacked" pawns in front only
                //             if (y < 7 && board[y][x].type == "p" && board[y][x].color == "w") white_double_pawn_count++
                //             break;
                // }
            } else {
                switch(piece.type) {
                    case PAWN:
                        black_score = black_score
                            + piece_values[piece.type]
                            + black_pawn_table[y][x]
                        break
                    case KNIGHT:
                        black_score = black_score
                            + piece_values[piece.type]
                            + black_knight_table[y][x]
                        break
                    case BISHOP:
                        black_score = black_score
                            + piece_values[piece.type]
                            + black_bishop_table[y][x]
                        break
                    case ROOK:
                        black_score = black_score
                            + piece_values[piece.type]
                            + black_rook_table[y][x]
                        break
                    case QUEEN:
                        black_score = black_score
                            + piece_values[piece.type]
                            + black_queen_table[y][x]
                        break
                    case KING:
                        black_score = black_score
                            + piece_values[piece.type]
                            + black_king_table[y][x]
                        break
                }
                
                // switch(piece.type) {
                //     case BISHOP:
                //         black_bishop_count++;
                //         break;
                //     case PAWN:
                //         // check for "stacked" pawns in front only
                //         if (y < 7 && board[y][x].type == "p" && board[y][x].color == "b") black_double_pawn_count++
                //         break;
                // }
            }

        }
    }

    for (let i = 0; i < raw_moves.length; i++) {
        let move = raw_moves[i]
        if (move.color == "w") {
            white_mobility++
        } else {
            black_mobility++
        }
    }

    let bishop_bonus = 0
    if (white_bishop_count == 2) bishop_bonus += 10
    if (black_bishop_count == 2) bishop_bonus -= 10
    
    return (white_score - black_score)
            //+ 0.05 * (white_attacking_score - black_attacking_score)
            + 0.1 * (white_mobility - black_mobility)
            + bishop_bonus
}

const compare_moves = (a, b) => {
    return MVV_LVA_table[char_to_index(b.captured)][char_to_index(b.piece)] - MVV_LVA_table[char_to_index(a.captured)][char_to_index(a.piece)]
}

const score_move = (move) => {
    return MVV_LVA_table[char_to_index(move.captured)][char_to_index(move.piece)]
}

const score_moves = (raw_moves) => {
    let scored_moves = []

    for (let i = 0; i < raw_moves.length; i++) {
        scored_moves.push(score_move(raw_moves[i]))
    }

    return scored_moves
}

const swap_elements = (array, i1, i2) => {
    let temp = array[i1]
    array[i1] = array[i2]
    array[i2] = temp
}

const determine_best_move = (start_depth, game, maximizing_player) => {
    let best_move_eval = -9999
    let best_move = null

    let game_moves = game.moves()

    // first loop of min-maxing
    for (let i = 0; i < game_moves.length; i++) {
        let move = game_moves[i]

        game.move(move)

        let eval = (maximizing_player ? -1 : 1) * alphabeta(game, start_depth - 1, -10000, 10000, maximizing_player)
        //let eval = alpha_beta_max(game, start_depth - 1, -9999999, 9999999)

        game.undo()

        if (eval >= best_move_eval) {
            best_move = move
            best_move_eval = eval
        }
    }

    console.log("determined best move with score " + best_move_eval)

    return best_move
}

const alphabeta = (game, depth, alpha, beta, maximizing_player) => {
    positions_evaluated++

    /*
     * so theres some commented out code below:
    * rustic-chess online supposes that physically sorting the array is a waste of 
     * computing power as alpha-beta pruning will remove some of these eventualities meaning
     * we are sorting elements we have no need to sort
     * in my experience (thanks js?) there is no difference, so the
     * code for both will stay.
     */
    
    let game_moves = game.moves()
    //game_moves = game_moves.sort(compare_moves)
    let scored_moves = score_moves(game.moves({ raw: true }))
    if (depth == 0) return -evaluate_position(game, scored_moves)

    if (maximizing_player) {
        let best_move_eval = -999999

        for (let i = 0; i < game_moves.length; i++)
        {
            // why dont we just sort the arrays ?
            // this lets us find the biggest scorer one by one rather than processing them all
            for (let j = i; j < game_moves.length; j++) {
                if (scored_moves[j] > scored_moves[i])
                    swap_elements(game_moves, i, j)
            }

            let move = game_moves[i]

            game.move(move)
            best_move_eval = Math.max(best_move_eval, alphabeta(game, depth - 1, alpha, beta, !maximizing_player))
            game.undo()

            alpha = Math.max(alpha, best_move_eval)
            if (beta <= alpha) break
        }

        return best_move_eval
    } else {
        let best_move_eval = 999999

        for (let i = 0; i < game_moves.length; i++) 
        {
            // why dont we just sort the arrays ?
            // this lets us find the biggest scorer one by one rather than processing them all
            for (let j = i; j < game_moves.length; j++) {
                if (scored_moves[j] > scored_moves[i])
                    swap_elements(game_moves, i, j)
            }

            let move = game_moves[i]

            game.move(move)
            best_move_eval = Math.min(best_move_eval, alphabeta(game, depth - 1, alpha, beta, !maximizing_player))
            game.undo()

            beta = Math.min(beta, best_move_eval)
            if (beta <= alpha) break
        }

        return best_move_eval
    }
}

const alpha_beta_max = (game, depth, alpha, beta) => {
    if (depth == 0) return -evaluate_position(game)

    let game_moves = game.moves()
    let scored_moves = score_moves(game.moves({ raw: true }))
    
    for (let i = 0; i < game_moves.length; i++) {
        for (let j = i; j < game_moves.length; j++) {
            if (scored_moves[j] > scored_moves[i])
                swap_elements(game_moves, i, j)
        }

        let move = game_moves[i]
        game.move(move)
        let score = alpha_beta_min(game, depth - 1, alpha, beta)
        game.undo()
        if (score >= beta) return beta
        if (score > alpha) alpha = score
    }

    return alpha
}

const alpha_beta_min = (game, depth, alpha, beta) => {
    if (depth == 0) return evaluate_position(game)

    let game_moves = game.moves()
    let scored_moves = score_moves(game.moves({ raw: true }))
    
    for (let i = 0; i < game_moves.length; i++) {
        for (let j = i; j < game_moves.length; j++) {
            if (scored_moves[j] > scored_moves[i])
                swap_elements(game_moves, i, j)
        }

        let move = game_moves[i]
        game.move(move)
        let score = alpha_beta_max(game, depth - 1, alpha, beta)
        game.undo()
        if (score <= alpha) return alpha
        if (score < beta) beta = score
    }

    return beta
}

const load_openings = async () => {
    let raw_openings = (await (await (fetch("../lib/openings.txt"))).text()).split("\n")
    let openings = []

    for (let i = 0; i < raw_openings.length; i++) {
        openings.push(raw_openings[i].split(" "))
    }

    return openings
}

const perform_random_opening_move = async (game) => {
    if (openings.length == 0) openings = await load_openings()
    
    is_opening = true
    console.log("performing random opening move to start")
    game.move(openings[Math.floor(Math.random() * openings.length)][0])
}

const perform_best_move = async (game, player_color) => {

    let start_time = performance.now()
    // opening set
    if (openings.length == 0) openings = await load_openings()

    // even if white, odd if black turn
    // next_move_index should never be zero
    let next_move_index = (game.moveNumber() - 1) * 2 + (player_color == "w" ? 1 : 0)
    
    if (is_opening && next_move_index <= 9) {
        let found_opener = false

        opening_loop:
        while (!found_opener) {
            if (openings.length == 0) {
                is_opening = false
                console.log("ran out of openings!")
                break
            }

            // pick random opening
            let i = Math.floor(Math.random() * openings.length)

            if (openings[i].length <= next_move_index) {
                openings.splice(i, 1) // opening doesn't have enough moves to support anyway
                continue 
            }

            let history = game.history()

            for (let j = 0; j < history.length; j++) {
                if (openings[i][j] != history[j]) {
                    openings.splice(i, 1)
                    continue opening_loop
                }
            }

            // if previous move matches opening, send it
            console.log("playing from opening book")
            let move = openings[i][next_move_index]
            if (move) {
                game.move(move)
                let time_elapsed = performance.now() - start_time

                $(".stats #time").text("Time Elapsed: " + time_elapsed / 1000 + " seconds")
                $(".stats #positions").text("Positions Evaluated: " + positions_evaluated)
                positions_evaluated = 0
                return
            }
        }
    }

    
    let best_move = determine_best_move(3, game, player_color == "b")
    let time_elapsed = performance.now() - start_time

    $(".stats #time").text("Time Elapsed: " + time_elapsed / 1000 + " seconds")
    $(".stats #positions").text("Positions Evaluated: " + positions_evaluated)
    positions_evaluated = 0

    if (best_move) game.move(best_move)
}

const evaluate_position_old = (game, raw_moves) => {
    let board = game.board()

    let white_score = 0
    let black_score = 0

    // for (let x = 0; x < 8; x++) {
    //     for (let y = 0; y < 8; y++) {
    //         let piece = board[y][x]

    //         if (!piece) continue

    //         if (piece.color == "w") {
    //             white_score += piece_score(piece, x, y)
    //         } else {
    //             black_score += piece_score(piece, x, y)
    //         }
    //     }
    // }

    // return white_score - black_score

    // keep record of number of number of black pieces attacked by white and vice versa
    let white_attacking_score = 0
    let black_attacking_score = 0

    // number of possible mvoes per side
    let white_mobility = 0,
        black_mobility = 0
    
    let white_bishop_count = 0,
        black_bishop_count = 0
    
    let white_double_pawn_count = 0,
        black_double_pawn_count = 0

    for (let x = 0; x < 8; x++) {
        for (let y = 0; y < 8; y++) {
            let piece = board[y][x]

            if (!piece) continue

            if (piece.color == "w") {
                white_score = white_score
                            + piece_values[piece.type]
                            + white_position_table[piece.type][y][x]
                
                // switch(piece.type) {
                //     case BISHOP:
                //         white_bishop_count++;
                //         break;
                //         case PAWN:
                //             // check for "stacked" pawns in front only
                //             if (y < 7 && board[y][x].type == "p" && board[y][x].color == "w") white_double_pawn_count++
                //             break;
                // }
            } else {
                black_score = black_score
                            + piece_values[piece.type]
                            + black_position_table[piece.type][y][x]
                
                // switch(piece.type) {
                //     case BISHOP:
                //         black_bishop_count++;
                //         break;
                //     case PAWN:
                //         // check for "stacked" pawns in front only
                //         if (y < 7 && board[y][x].type == "p" && board[y][x].color == "b") black_double_pawn_count++
                //         break;
                // }
            }

        }
    }

    for (let i = 0; i < raw_moves.length; i++) {
        let move = raw_moves[i]
        if (move.captured) {
            if (move.color == "w") {
                white_mobility++
                white_attacking_score   = white_attacking_score
                                        + score_move(move)
            } else {
                black_mobility++
                black_attacking_score   = black_attacking_score
                                        + score_move(move)
            }
        }
    }

    let bishop_bonus = 0
    if (white_bishop_count == 2) bishop_bonus += 10
    if (black_bishop_count == 2) bishop_bonus -= 10
    
    return (white_score - black_score)
            + 0.05 * (white_attacking_score - black_attacking_score)
            // + 0.1 * (white_mobility - black_mobility)
            + bishop_bonus
}

