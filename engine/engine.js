const EMPTY     = 0
const PAWN      = 1
const KNIGHT    = 2
const BISHOP    = 3
const ROOK      = 4
const QUEEN     = 5
const KING      = 6

const WHITE     = 16
const BLACK     = 32

const PIECE_TYPES = {
    [PAWN]: "pawn",
    [KNIGHT]: "knight",
    [BISHOP]: "bishop",
    [ROOK]: "rook",
    [QUEEN]: "queen",
    [KING]: "king"
}

const PIECE_TYPES_SHORT = {
    [PAWN]: "p",
    [KNIGHT]: "n",
    [BISHOP]: "b",
    [ROOK]: "r",
    [QUEEN]: "q",
    [KING]: "k",
    [EMPTY]: " "
}

const PIECE_COLORS = {
    [WHITE]: "white",
    [BLACK]: "black"
}

const PIECE_COLORS_SHORT = {
    [WHITE]: "w",
    [BLACK]: "b",
    [EMPTY]: " "
}

const FILTER_COLOR      = 16 + 32
const FILTER_PIECE      = 7

var board = [
    ROOK | WHITE, KNIGHT | WHITE, BISHOP | WHITE, QUEEN | WHITE, KING | WHITE, BISHOP | WHITE, KNIGHT | WHITE, ROOK | WHITE,
    PAWN | WHITE, PAWN | WHITE, PAWN | WHITE, PAWN | WHITE, PAWN | WHITE, PAWN | WHITE, PAWN | WHITE, PAWN | WHITE, 
    EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, 
    EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, 
    EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, 
    EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, 
    PAWN | BLACK, PAWN | BLACK, PAWN | BLACK, PAWN | BLACK, PAWN | BLACK, PAWN | BLACK, PAWN | BLACK, PAWN | BLACK, 
    ROOK | BLACK, KNIGHT | BLACK, BISHOP | BLACK, QUEEN | BLACK, KING | BLACK, BISHOP | BLACK, KNIGHT | BLACK, ROOK | BLACK,
]

const get_piece_color = (piece) => {
    return piece & FILTER_COLOR
}

const get_piece_type = (piece) => {
    return piece & FILTER_PIECE
}

const get_piece_info_full = (piece) => {
    return PIECE_COLORS[get_piece_color(piece)] + " " + PIECE_TYPES[get_piece_type(piece)]
}

const get_piece_info_short = (piece) => {
    return PIECE_COLORS_SHORT[get_piece_color(piece)] + PIECE_TYPES_SHORT[get_piece_type(piece)]
}

const print_board = (board) => {
    let board_out = "-----------------------------------------\n"

    for (let y = 7; y >= 0; y--) {
        for (let x = 0; x < 8; x++) {
            board_out += "| " + get_piece_info_short(board[y * 8 + x]) + " "
        }
        board_out += "|\n-----------------------------------------\n"
    }

    console.log(board_out)
}

const get_board_index = (x, y) => {
    return y * 8 + x
}

const move = (from, to) => {
    board[to] = board[from]
    board[from] = EMPTY
}

const get_pawn_moves = (piece, x, y) => {
    
}

print_board(board)
move(1 * 8 + 3, 2 * 8 + 3)
print_board(board)
