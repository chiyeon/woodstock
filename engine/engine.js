const EMPTY     = 0b000
const PAWN      = 0b001
const KNIGHT    = 0b010
const BISHOP    = 0b011
const ROOK      = 0b100
const QUEEN     = 0b101
const KING      = 0b110

const WHITE     = 0b01000
const BLACK     = 0b10000

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

const FILTER_COLOR      = 0b11000
const FILTER_PIECE      = 0b00111

var board = [
    ROOK | BLACK, KNIGHT | BLACK, BISHOP | BLACK, QUEEN | BLACK, KING | BLACK, BISHOP | BLACK, KNIGHT | BLACK, ROOK | BLACK,
    PAWN | BLACK, PAWN | BLACK, PAWN | BLACK, PAWN | BLACK, PAWN | BLACK, PAWN | BLACK, PAWN | BLACK, PAWN | BLACK, 
    EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, 
    EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, 
    EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, 
    EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, 
    PAWN | WHITE, PAWN | WHITE, PAWN | WHITE, PAWN | WHITE, PAWN | WHITE, PAWN | WHITE, PAWN | WHITE, PAWN | WHITE, 
    ROOK | WHITE, KNIGHT | WHITE, BISHOP | WHITE, QUEEN | WHITE, KING | WHITE, BISHOP | WHITE, KNIGHT | WHITE, ROOK | WHITE,
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

    for (let y = 0; y < 8; y++) {
        for (let x = 0; x < 8; x++) {
            board_out += "| " + get_piece_info_short(board[y * 8 + x]) + " "
        }
        board_out += "|\n-----------------------------------------\n"
    }

    console.log(board_out)
}

const print_bitboard = (bit_board) => {
    let bit_board_str = bit_board.toString(2).padStart(64, "0")
    let out = ""

    for (let y = 0; y < 8; y++) {
        for (let x = 0; x < 8; x++) {
            out += bit_board_str[y * 8 + x] + " "
        }
        out += "\n"
    }

    console.log(out)
}

const get_board_index = (x, y) => {
    return y * 8 + x
}

const move_piece = (board, from, to) => {
    // record our last move. from and to is self explanatory, lost_to_value is the value in to that is lost to the move.
    history.push({
        from,
        to,
        lost_to_value: board[to]
    })

    //console.log("piece is goign from " + get_piece_info_short(board[from]) + " to " + get_piece_info_short(board[to]))
    
    board[to] = board[from]
    board[from] = EMPTY
    //console.log("piece lost stored as " + get_piece_info_short(last_move.piece_lost))
}

const get_bitboard_ = (x, y) => {
    return 
}

/*
 * returns an empty bitboard with the space at x, y filled
 * uses string concattenation. slow?
 */
const get_bitboard_i = (i) => {
    let out = "0b"
    for (let j = 0; j < 64; j++) {
        out += (j == i) ? "1" : "0"
    }
    return BigInt(out)
}

const get_bitboard = (x, y) => {
    let out = "0b"
    for (let i = 0; i < 8; i++) {
        for (let j = 0; j < 8; j++) {
            out += (i == y && j == x) ? "1" : "0"
        }
    }
    return BigInt(out)
}

const get_bitboard_row = (y) => {
    let out = "0b"
    for (let i = 0; i < 8; i++) {
        if (i == y) {
            out += "11111111"
        } else {
            out += "00000000"
        }
    }
    return BigInt(out)
}

// TODO FIX
const get_bitboard_diagonals = (x, y) => {
    return  get_bitboard_diagonal_downwards_left(x, y)
            | get_bitboard_diagonal_upwards_left(x, y,)
            | get_bitboard_diagonal_downwards_right(x, y)
            | get_bitboard_diagonal_upwards_right(x, y)
}

const get_bitboard_column = (x) => {
    let out = "0b"
    // assemble 'column'
    let col = ""
    for (let i = 0; i < 8; i++) {
        col += i == x ? "1" : "0"
    }

    for (let j = 0; j < 8; j++) {
        out += col
    }

    return BigInt(out)
}

// returns a bitboard with a target column segment given (inclusive both)
// string concat creation like this is WAY faster than .split .join! (almost 2x as fast!)
const get_bitboard_column_segment = (x, y1, y2) => {
    let out = "0b"

    // assemble 'column'
    let col = ""
    for (let i = 0; i < 8; i++) {
        col += i == x ? "1" : "0"
    }

    // append column if we are within the y ranges, otherwise empty
    for (let j = 0; j < 8; j++) {
        out += (j >= y1 && j <= y2) ? col : "00000000"
    }

    return BigInt(out)
}

const get_bitboard_row_segment = (y, x1, x2) => {
    let out = "0b"

    for (let j = 0; j < 8; j++) {
        if (j != y) {
            out += "00000000"
        } else {
            for (let i = 0; i < 8; i++) {
                out += (i >= x1 && i <= x2) ? "1" : "0"
            }
        }
    }

    return BigInt(out)
}

// for ...downwards_right, ...downwards_left and so on (4 total)
// we get the diagonal up to the end of the board in the specified direction STARTING AT x, y

const get_bitboard_diagonal_downwards_right = (x, y) => {
    let start = get_bitboard(x, y)
    let out = start

    for (let i = 1; i < 8 - x; i++) {
        if (y + i >= 8) break

        out = out | out >> 9n
    }

    return out ^ start
}

const get_bitboard_diagonal_downwards_left = (x, y) => {
    let start = get_bitboard(x, y)
    let out = start

    for (let i = 1; i <= x; i++) {
        if (y + i >= 8) break

        out = out | out >> 7n
    }

    return out ^ start
}

const get_bitboard_diagonal_upwards_right = (x, y) => {
    let start = get_bitboard(x, y)
    let out = start

    for (let i = 1; i < 8 - x; i++) {
        if (y - i < 0) break

        out = out | out << 7n
    }

    return out ^ start
}

const get_bitboard_diagonal_upwards_left = (x, y) => {
    let start = get_bitboard(x, y)
    let out = start

    for (let i = 1; i <= x; i++) {
        if (y - i < 0) break

        out = out | out << 9n
    }

    return out ^ start
}

// print_bitboard(BigInt("0b1111111111111111111111111111111111111111111111111111111111111111"))

// print_board(board)
// move(1 * 8 + 3, 2 * 8 + 3)
// print_board(board)

// console.log(BigInt(0b0000000000000000000000000000000000000000000000000000000010000000n))
// console.log(BigInt(0b0000000000000000000000000000000000000000000000001000000000000000n))
// console.log(BigInt(0b0000000000000000000000000000000000000000100000000000000000000000n))

// let start_time = performance.now()
// for (let i = 0; i < 1000000; i++) {
//     get_bitboard(0, 0)
// }
// console.log(performance.now() - start_time)

const get_board_as_bitboard = (board) => {
    let bb = "0b"
    for (let y = 0; y < 8; y++) {
        for (let x = 0; x < 8; x++) {
            bb += board[y * 8 + x] == EMPTY ? "0" : "1"
        }
    }
    return BigInt(bb)
}

// gets the bitboard of only piees of a certain color
const get_board_as_bitboard_of_color = (board, color) => {
    let bb = "0b"
    for (let y = 0; y < 8; y++) {
        for (let x = 0; x < 8; x++) {
            bb += ((board[y * 8 + x] & FILTER_COLOR) == color) ? "1" : "0"
        }
    }
    return BigInt(bb)
}

// returns a list of positions of all 1 bits in a bitboard
const get_bits_as_positions = (bitboard) => {
    if (bitboard == 0n) return []

    let bit_board_str = bitboard.toString(2).padStart(64, "0")
    let pos = []
    
    for (let y = 0; y < 8; y++) {
        for (let x = 0; x < 8; x++) {
            if (bit_board_str[y * 8 + x] == "1") {
                pos.push([x, y])
            }
        }
    }

    return pos
}


//print_bitboard(get_bitboard_diagonals(1, 1))
//print_bitboard(get_bitboard_diagonals(3, 2))
//print_bitboard(get_bitboard_diagonals(2, 4))
// print_bitboard(get_bitboard_diagonals(6, 6))
// print_bitboard(get_bitboard_diagonals(1, 7))

/*
 * BOARD BLOCKER DICTIONARIES
 */

const get_rook_blocker_masks = () => {
    let out = []

    for (let y = 0; y < 8; y++) {
        for (let x = 0; x < 8; x++) {
            let i = y * 8 + x

            out[i] = get_bitboard_column(x) ^ get_bitboard_row(y)
        }
    }

    return out
}

const get_bishop_blocker_masks = () => {
    let out = []

    for (let y = 0; y < 8; y++) {
        for (let x = 0; x < 8; x++) {
            let i = y * 8 + x

            out[i] = get_bitboard_diagonals(x, y)
        }
    }

    return out
}

const rook_blocker_masks = get_rook_blocker_masks()
const bishop_blocker_masks = get_bishop_blocker_masks()

// let b = [
//     EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
//     EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, WHITE | KING, EMPTY, EMPTY,
//     EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
//     EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
//     EMPTY, EMPTY, WHITE | BISHOP, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
//     EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
//     EMPTY, EMPTY, EMPTY, EMPTY, BLACK | KING, EMPTY, EMPTY, EMPTY,
//     EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
// ]

//print_board(b)
let last_move = {}
var history = []

const get_moves = (board, turn) => {
    let moves = []
    let board_bitboard = get_board_as_bitboard(board)
    let white_bitboard = get_board_as_bitboard_of_color(board, WHITE)
    let black_bitboard = get_board_as_bitboard_of_color(board, BLACK)


    for (let y = 0; y < 8; y++) {
        for (let x = 0; x < 8; x++) {
            let index = x + y * 8
            let piece = board[index]
            if (piece != EMPTY) {
                let pos_bitboard = get_bitboard(x, y)
                let piece_color = piece & FILTER_COLOR
                let piece_type = piece & FILTER_PIECE

                if (piece_color != turn) continue

                switch(piece_type) {
                    case PAWN:
                        let pawn_moves = ((piece_color == BLACK ? pos_bitboard >> 8n : pos_bitboard << 8n))
                        // move double if we are in home rows
                        if (y == 1 && piece_color == BLACK) {
                            pawn_moves = pawn_moves | pawn_moves >> 8n
                        } else if (y == 6 && piece_color == WHITE) {
                            pawn_moves = pawn_moves | pawn_moves << 8n
                        }

                        // check for no collisions
                        let pawn_collisions = get_bits_as_positions(pawn_moves & board_bitboard)
                        for (let i = 0; i < pawn_collisions.length; i++) {
                            let col_y = pawn_collisions[i][1]
                            // check for occupied space + 1 backwards, stops pawns from jumping in home row
                            let mask = piece_color == BLACK
                                ? get_bitboard_column_segment(x, col_y, col_y + 1)
                                : get_bitboard_column_segment(x, col_y - 1, col_y)
                            pawn_moves = pawn_moves & ~mask
                        }

                        let pawn_positions = get_bits_as_positions(pawn_moves)

                        for (let i = 0; i < pawn_positions.length; i++) {
                            let col = pawn_positions[i]
                            let col_x = col[0]
                            let col_y = col[1]

                            moves.push({
                                to: col_x + col_y * 8,
                                from: index
                            })
                        }

                        // check for capturing diagonally
                        let pawn_captures = ((piece_color == BLACK) ? pos_bitboard >> 9n | pos_bitboard >> 7n : pos_bitboard << 9n | pos_bitboard << 7n)
                        let pawn_capture_collisions = get_bits_as_positions(pawn_captures & board_bitboard & (turn == BLACK ? white_bitboard : black_bitboard))

                        for (let i = 0; i < pawn_capture_collisions.length; i++) {
                            let col = pawn_capture_collisions[i]
                            let col_x = col[0]
                            let col_y = col[1]

                            moves.push({
                                to: col_x + col_y * 8,
                                from: index
                            })
                        }
                        break
                    case KNIGHT:
                        // calcluate all possible spots
                        let knight_starting_pos = get_bitboard(x, y)
                        let knight_moves = 0n
                        
                        if (piece_color == BLACK) {
                            if (y + 2 < 8 && x + 1 < 8) knight_moves = knight_moves | knight_starting_pos >> 17n
                            if (y + 2 < 8 && x - 1 >= 0) knight_moves = knight_moves | knight_starting_pos >> 15n
                            if (y + 1 < 8 && x + 2 < 8) knight_moves = knight_moves | knight_starting_pos >> 10n
                            if (y + 1 < 8 && x - 2 >= 0) knight_moves = knight_moves | knight_starting_pos >> 6n
                            if (y - 2 >= 0 && x + 1 < 8) knight_moves = knight_moves | knight_starting_pos >> 15n
                            if (y - 2 >= 0 && x - 1 >= 0) knight_moves = knight_moves | knight_starting_pos >> 17n
                            if (y - 1 >= 0 && x + 2 < 8) knight_moves = knight_moves | knight_starting_pos >> 6n
                            if (y - 1 >= 0 && x - 2 >= 0) knight_moves = knight_moves | knight_starting_pos >> 10n
                        } else {
                            if (y + 2 < 8 && x + 1 < 8) knight_moves = knight_moves | knight_starting_pos >> -17n
                            if (y + 2 < 8 && x - 1 >= 0) knight_moves = knight_moves | knight_starting_pos >> -15n
                            if (y + 1 < 8 && x + 2 < 8) knight_moves = knight_moves | knight_starting_pos >> -10n
                            if (y + 1 < 8 && x - 2 >= 0) knight_moves = knight_moves | knight_starting_pos >> -6n
                            if (y - 2 >= 0 && x + 1 < 8) knight_moves = knight_moves | knight_starting_pos >> -15n
                            if (y - 2 >= 0 && x - 1 >= 0) knight_moves = knight_moves | knight_starting_pos >> -17n
                            if (y - 1 >= 0 && x + 2 < 8) knight_moves = knight_moves | knight_starting_pos >> -6n
                            if (y - 1 >= 0 && x - 2 >= 0) knight_moves = knight_moves | knight_starting_pos >> -10n
                        }

                        let knight_positions = get_bits_as_positions(knight_moves & ~(piece_color == BLACK ? black_bitboard : white_bitboard))

                        for (let i = 0; i < knight_positions.length; i++) {
                            let pos = knight_positions[i]
                            moves.push({
                                to: pos[0] + pos[1] * 8,
                                from: index
                            })
                        }
                        break
                    case BISHOP:
                        let bishop_moves = bishop_blocker_masks[index]

                        // get pieces that are in line with our axis of movement
                        let bishop_collisions = get_bits_as_positions(bishop_moves & board_bitboard)
                        
                        for (let i = 0; i < bishop_collisions.length; i++) {
                            // for each collision, remove excess spaces in our mask
                            let col = bishop_collisions[i]
                            let col_x = col[0]
                            let col_y = col[1]
                            let col_index = col_x + col_y * 8
                            let mask = 0n
                            let target_capturable = (board[col_index] & FILTER_COLOR) == piece_color

                            // assuming printed normally:

                            // lower right
                            if (col_y > y && col_x > x) mask = get_bitboard_diagonal_downwards_right(col_x, col_y)
                            // lower left
                            else if (col_y > y && col_x < x) mask = get_bitboard_diagonal_downwards_left(col_x, col_y)
                            // upper right
                            else if (col_y < y && col_x > x) mask = get_bitboard_diagonal_upwards_right(col_x, col_y)
                            // uper left
                            else if (col_y < y && col_x < x) mask = get_bitboard_diagonal_upwards_left(col_x, col_y)

                            // ugh !!!
                            if (target_capturable) mask = mask ^ get_bitboard(col_x, col_y)

                            bishop_moves = bishop_moves & (~mask)
                        }

                        let bishop_positions = get_bits_as_positions(bishop_moves)
                        for (let i = 0; i < bishop_positions.length; i++) {
                            let col = bishop_positions[i]
                            moves.push({
                                to: col[0] + col[1] * 8,
                                from: index
                            })
                        }
                        break
                    case ROOK:
                        let rook_moves = rook_blocker_masks[index]

                        // get pieces that are in line with our axis of movement
                        let collision_pos = get_bits_as_positions(rook_moves & board_bitboard)

                        for (let i = 0; i < collision_pos.length; i++) {
                            // for each collision, remove excess spaces in our mask
                            let col = collision_pos[i]
                            let col_x = col[0]
                            let col_y = col[1]
                            let col_index = col_x + col_y * 8
                            let mask = 0n
                            let target_capturable = (board[col_index] & FILTER_COLOR) == piece_color

                            if (col_y > y) mask = get_bitboard_column_segment(x, target_capturable ? col_y : col_y + 1, 7)
                            else if (col_y < y) mask = get_bitboard_column_segment(x, 0, target_capturable ? col_y + 1 : col_y - 1)
                            else if (col_x > x) mask = get_bitboard_row_segment(y, target_capturable ? col_x : col_x + 1, 7)
                            else if (col_x < x) mask = get_bitboard_row_segment(y, 0, target_capturable ? col_x : col_x - 1)

                            rook_moves = rook_moves & (~mask)
                        }

                        let rook_positions = get_bits_as_positions(rook_moves)
                        for (let i = 0; i < rook_positions.length; i++) {
                            let col = rook_positions[i]
                            moves.push({
                                to: col[0] + col[1] * 8,
                                from: index
                            })
                        }
                        break
                    case QUEEN:
                        let queen_moves = rook_blocker_masks[index] | bishop_blocker_masks[index]

                        let queen_collision_positions = get_bits_as_positions(queen_moves & board_bitboard)

                        for (let i = 0; i < queen_collision_positions.length; i++) {
                            let col = queen_collision_positions[i]
                            let col_x = col[0]
                            let col_y = col[1]
                            let col_index = col_x + col_y * 8
                            let mask = 0n
                            let target_capturable = (board[col_index] & FILTER_COLOR) == piece_color

                            if (col_y > y) mask = get_bitboard_column_segment(x, target_capturable ? col_y : col_y + 1, 7)
                            else if (col_y < y) mask = get_bitboard_column_segment(x, 0, target_capturable ? col_y + 1 : col_y - 1)
                            else if (col_x > x) mask = get_bitboard_row_segment(y, target_capturable ? col_x : col_x + 1, 7)
                            else if (col_x < x) mask = get_bitboard_row_segment(y, 0, target_capturable ? col_x : col_x - 1)

                            // lower right
                            if (col_y > y && col_x > x) mask = mask | get_bitboard_diagonal_downwards_right(col_x, col_y)
                            // lower left
                            else if (col_y > y && col_x < x) mask = mask | get_bitboard_diagonal_downwards_left(col_x, col_y)
                            // upper right
                            else if (col_y < y && col_x > x) mask = mask | get_bitboard_diagonal_upwards_right(col_x, col_y)
                            // uper left
                            else if (col_y < y && col_x < x) mask = mask | get_bitboard_diagonal_upwards_left(col_x, col_y)

                            // ugh !!!
                            if (target_capturable) mask = mask ^ get_bitboard(col_x, col_y)

                            queen_moves = queen_moves & (~mask)
                        }

                        let queen_positions = get_bits_as_positions(queen_moves & ~(piece_color == BLACK ? black_bitboard : white_bitboard))
                        for (let i = 0; i < queen_positions.length; i++) {
                            let col = queen_positions[i]
                            moves.push({
                                to: col[0] + col[1] * 8,
                                from: index
                            })
                        }
                        break
                    case KING:
                        let king_start = get_bitboard(x, y)
                        let king_moves  = king_start >> 8n
                                        | king_start << 8n
                                        | king_start >> 9n
                                        | king_start << 9n
                                        | king_start >> 7n
                                        | king_start << 7n
                        
                        let king_positions = get_bits_as_positions(king_moves & ~(piece_color == BLACK ? black_bitboard : white_bitboard))

                        for (let i = 0; i < king_positions; i++) {
                            let col = king_positions[i]
                            moves.push({
                                to: col[0] + col[1] * 8,
                                from: index
                            })
                        }
                        break
                }
            }
        }
    }

    return moves
}

const undo = (board) => {
    let i = history.length - 1
    if (i >= 0) {
        board[history[i].from] = board[history[i].to]
        board[history[i].to] = history[i].lost_to_value
    }
    history.pop()
}



let turn = WHITE

const update_turn = () => {
    turn = (turn == WHITE) ? BLACK : WHITE
}

let start_time = performance.now()
let moves = get_moves(board, WHITE)
console.log(performance.now() - start_time)

moves.forEach(move => {
    move_piece(board, move.from, move.to)
    print_board(board)
    undo(board)
})

console.log(moves.length)

const count_bulk_positions_new = (depth) => {
    if (depth <= 0) return 1

    let moves = get_moves(board, turn)
    update_turn()
    let count = 0

    for (let i = 0; i < moves.length; i++) {
        let move = moves[i]
        move_piece(board, move.from, move.to)
        count += count_bulk_positions_new(depth - 1)
        undo(board)
    }

    return count   
}

const meausre_count_bulk_positions_new = (depth) => {
    let start_time = performance.now()
    let count = count_bulk_positions_new(depth)
    console.log("Depth: " + depth + "\tNumber of positions: " + count + "\tTime: " + (performance.now() - start_time) + "ms")
}

// for (let i = 1; i <= 4; i++) {
//     meausre_count_bulk_positions_new(i)
// }