class Piece {
    static EMPTY = 0b000
    static PAWN = 0b001
    static KNIGHT = 0b010
    static BISHOP = 0b011
    static ROOK = 0b100
    static QUEEN = 0b101
    static KING = 0b110

    static WHITE = 0b01000
    static BLACK = 0b10000

    static FILTER_COLOR = 0b11000
    static FILTER_PIECE = 0b00111

    static names = {
        [Piece.PAWN]: "pawn",
        [Piece.KNIGHT]: "knight",
        [Piece.BISHOP]: "bishop",
        [Piece.ROOK]: "rook",
        [Piece.QUEEN]: "queen",
        [Piece.KING]: "king",
        [Piece.EMPTY]: ""
    }

    static names_short = {
        [Piece.PAWN]: "p",
        [Piece.KNIGHT]: "n",
        [Piece.BISHOP]: "b",
        [Piece.ROOK]: "r",
        [Piece.QUEEN]: "q",
        [Piece.KING]: "k",
        [Piece.EMPTY]: " "
    }

    static colors = {
        [Piece.WHITE]: "white",
        [Piece.BLACK]: "black",
        [Piece.EMPTY]: ""
    }

    static colors_short = {
        [Piece.WHITE]: "w",
        [Piece.BLACK]: "b",
        [Piece.EMPTY]: " "
    }

    static get_color = (piece) => {
        return piece & Piece.FILTER_COLOR
    }

    static get_type = (piece) => {
        return piece & Piece.FILTER_PIECE
    }

    static get_info = (piece) => {
        return Piece.colors[Piece.get_color(piece)] + " " + Piece.names[Piece.get_type(piece)]
    }

    static get_info_short = (piece) => {
        return Piece.colors_short[Piece.get_color(piece)] + Piece.names_short[Piece.get_type(piece)]
    }
}

class BitBoard {
    static print = (bit_board) => {
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


    /*
    * returns an empty bitboard with the space at x, y filled
    * uses string concattenation. slow?
    */
    static get_i = (i) => {
        let out = "0b"
        for (let j = 0; j < 64; j++) {
            out += (j == i) ? "1" : "0"
        }
        return BigInt(out)
    }

    static get = (x, y) => {
        let out = "0b"
        for (let i = 0; i < 8; i++) {
            for (let j = 0; j < 8; j++) {
                out += (i == y && j == x) ? "1" : "0"
            }
        }
        return BigInt(out)
    }

    static get_row = (y) => {
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

    static get_diagonals = (x, y) => {
        return BitBoard.get_diagonal_downwards_left(x, y)
            | BitBoard.get_diagonal_upwards_left(x, y,)
            | BitBoard.get_diagonal_downwards_right(x, y)
            | BitBoard.get_diagonal_upwards_right(x, y)
    }

    static get_column = (x) => {
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
    static get_column_segment = (x, y1, y2) => {
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

    static get_row_segment = (y, x1, x2) => {
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

    static get_diagonal_downwards_right = (x, y) => {
        let start = BitBoard.get(x, y)
        let out = start

        for (let i = 1; i < 8 - x; i++) {
            if (y + i >= 8) break

            out = out | out >> 9n
        }

        return out ^ start
    }

    static get_diagonal_downwards_left = (x, y) => {
        let start = BitBoard.get(x, y)
        let out = start

        for (let i = 1; i <= x; i++) {
            if (y + i >= 8) break

            out = out | out >> 7n
        }

        return out ^ start
    }

    static get_diagonal_upwards_right = (x, y) => {
        let start = BitBoard.get(x, y)
        let out = start

        for (let i = 1; i < 8 - x; i++) {
            if (y - i < 0) break

            out = out | out << 7n
        }

        return out ^ start
    }

    static get_diagonal_upwards_left = (x, y) => {
        let start = BitBoard.get(x, y)
        let out = start

        for (let i = 1; i <= x; i++) {
            if (y - i < 0) break

            out = out | out << 9n
        }

        return out ^ start
    }

    // returns a list of positions of all 1 bits in a bitboard
    static get_positions_list = (bitboard) => {
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
}

class Board {
    // could be more elegant but this might be fastest ?
    static fen_dictionary = {
        "p": Piece.BLACK | Piece.PAWN,
        "r": Piece.BLACK | Piece.ROOK,
        "n": Piece.BLACK | Piece.KNIGHT,
        "b": Piece.BLACK | Piece.BISHOP,
        "q": Piece.BLACK | Piece.QUEEN,
        "k": Piece.BLACK | Piece.KING,
        "P": Piece.WHITE | Piece.PAWN,
        "R": Piece.WHITE | Piece.ROOK,
        "N": Piece.WHITE | Piece.KNIGHT,
        "B": Piece.WHITE | Piece.BISHOP,
        "Q": Piece.WHITE | Piece.QUEEN,
        "K": Piece.WHITE | Piece.KING,
    }

    constructor(starting_position = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR") {
        this.board = []
        this.history = []
        this.turn = Piece.WHITE
        this.move_masks = new MoveMasks()

        this.read_fen_string(starting_position)
    }

    create_move = (from, to) => {
        return {
            from: from,
            to: to,
            piece: this.board[from],
            captured: this.board[to]
        }
    }

    read_fen_string = (fen) => {
        let lines = fen.split("/")

        for (let i = 0; i < lines.length; i++) {
            let line = lines[i]
            for (let j = 0; j < line.length; j++) {
                // split into chars
                let ch = line[j]
                let space_count = parseInt(ch)

                // if is not a number, try to read in as a piece
                if (isNaN(space_count)) {
                    this.board.push(Board.fen_dictionary[ch])
                    // otherwise skip as many spaces
                } else {
                    for (let k = 0; k < space_count; k++) {
                        this.board.push(Piece.EMPTY)
                    }
                }
            }
        }
    }

    print = () => {
        let board_out = "-----------------------------------------\n"

        for (let y = 0; y < 8; y++) {
            for (let x = 0; x < 8; x++) {
                board_out += "| " + Piece.get_info_short(this.board[y * 8 + x]) + " "
            }
            board_out += "|\n-----------------------------------------\n"
        }

        console.log(board_out)
    }

    get_index = (x, y) => {
        return x + y * 8
    }

    move = (move) => {
        // for strings
        if (typeof (move) === String) {

        } else {
            // record last move
            this.history.push(move)

            this.board[move.to] = this.board[move.from]
            this.board[move.from] = Piece.EMPTY
        }

        this.update_turn()
    }

    get_bitboard = () => {
        let bb = "0b"
        for (let y = 0; y < 8; y++) {
            for (let x = 0; x < 8; x++) {
                bb += this.board[y * 8 + x] == Piece.EMPTY ? "0" : "1"
            }
        }
        return BigInt(bb)
    }

    // gets the bitboard of only piees of a certain color
    get_bitboard_of_color = (color) => {
        let bb = "0b"
        for (let y = 0; y < 8; y++) {
            for (let x = 0; x < 8; x++) {
                bb += ((this.board[y * 8 + x] & Piece.FILTER_COLOR) == color) ? "1" : "0"
            }
        }
        return BigInt(bb)
    }

    get_pawn_moves = (x, y, index, piece, board_bitboard, black_bitboard, white_bitboard) => {
        let pos_bitboard = BitBoard.get(x, y)
        let piece_color = piece & Piece.FILTER_COLOR
        //let piece_type = piece & Piece.FILTER_PIECE
        let moves = []
        let pawn_moves = ((piece_color == Piece.BLACK ? pos_bitboard >> 8n : pos_bitboard << 8n))

        // move double if we are in home rows
        if (y == 1 && piece_color == Piece.BLACK) {
            pawn_moves = pawn_moves | pawn_moves >> 8n
        } else if (y == 6 && piece_color == Piece.WHITE) {
            pawn_moves = pawn_moves | pawn_moves << 8n
        }

        // check for no collisions
        let pawn_collisions = BitBoard.get_positions_list(pawn_moves & board_bitboard)
        for (let i = 0; i < pawn_collisions.length; i++) {
            let col_y = pawn_collisions[i][1]
            // check for occupied space + 1 backwards, stops pawns from jumping in home row
            let mask = piece_color == Piece.BLACK
                ? BitBoard.get_column_segment(x, col_y, col_y + 1)
                : BitBoard.get_column_segment(x, col_y - 1, col_y)
            pawn_moves = pawn_moves & ~mask
        }

        let pawn_positions = BitBoard.get_positions_list(pawn_moves)

        for (let i = 0; i < pawn_positions.length; i++) {
            let col = pawn_positions[i]
            let col_x = col[0]
            let col_y = col[1]

            moves.push(this.create_move(index, col_x + col_y * 8))
        }

        // check for capturing diagonally
        let pawn_captures = ((piece_color == Piece.BLACK) ? pos_bitboard >> 9n | pos_bitboard >> 7n : pos_bitboard << 9n | pos_bitboard << 7n)
        let pawn_capture_collisions = BitBoard.get_positions_list(pawn_captures & board_bitboard & (this.turn == Piece.BLACK ? white_bitboard : black_bitboard))

        for (let i = 0; i < pawn_capture_collisions.length; i++) {
            let col = pawn_capture_collisions[i]
            let col_x = col[0]
            let col_y = col[1]

            moves.push(this.create_move(index, col_x + col_y * 8))
        }

        return moves
    }

    get_knight_moves = (x, y, index, piece, board_bitboard, black_bitboard, white_bitboard) => {
        let pos_bitboard = BitBoard.get(x, y)
        let piece_color = piece & Piece.FILTER_COLOR
        let piece_type = piece & Piece.FILTER_PIECE
        let moves = []
        // calcluate all possible spots
        let knight_starting_pos = BitBoard.get(x, y)
        let knight_moves = 0n

        if (piece_color == Piece.BLACK) {
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

        let knight_positions = BitBoard.get_positions_list(knight_moves & ~(piece_color == Piece.BLACK ? black_bitboard : white_bitboard))

        for (let i = 0; i < knight_positions.length; i++) {
            let pos = knight_positions[i]

            moves.push(this.create_move(index, pos[0] + pos[1] * 8))
        }

        return moves
    }

    get_bishop_moves = (x, y, index, piece, board_bitboard, black_bitboard, white_bitboard) => {
        let pos_bitboard = BitBoard.get(x, y)
        let piece_color = piece & Piece.FILTER_COLOR
        let piece_type = piece & Piece.FILTER_PIECE
        let moves = []

        let bishop_moves = this.move_masks.bishop_masks[index]

        // get pieces that are in line with our axis of movement
        let bishop_collisions = BitBoard.get_positions_list(bishop_moves & board_bitboard)

        for (let i = 0; i < bishop_collisions.length; i++) {
            // for each collision, remove excess spaces in our mask
            let col = bishop_collisions[i]
            let col_x = col[0]
            let col_y = col[1]
            let col_index = col_x + col_y * 8
            let mask = 0n
            let target_capturable = (this.board[col_index] & Piece.FILTER_COLOR) == piece_color

            // assuming printed normally:

            // lower right
            if (col_y > y && col_x > x) mask = BitBoard.get_diagonal_downwards_right(col_x, col_y)
            // lower left
            else if (col_y > y && col_x < x) mask = BitBoard.get_diagonal_downwards_left(col_x, col_y)
            // upper right
            else if (col_y < y && col_x > x) mask = BitBoard.get_diagonal_upwards_right(col_x, col_y)
            // uper left
            else if (col_y < y && col_x < x) mask = BitBoard.get_diagonal_upwards_left(col_x, col_y)

            // ugh !!!
            if (target_capturable) mask = mask ^ BitBoard.get(col_x, col_y)

            bishop_moves = bishop_moves & (~mask)
        }

        let bishop_positions = BitBoard.get_positions_list(bishop_moves)
        for (let i = 0; i < bishop_positions.length; i++) {
            let col = bishop_positions[i]
            let col_x = col[0]
            let col_y = col[1]
            moves.push(this.create_move(index, col_x + col_y * 8))
        }

        return moves
    }

    get_rook_moves = (x, y, index, piece, board_bitboard, black_bitboard, white_bitboard) => {
        let pos_bitboard = BitBoard.get(x, y)
        let piece_color = piece & Piece.FILTER_COLOR
        let piece_type = piece & Piece.FILTER_PIECE
        let moves = []

        let rook_moves = this.move_masks.rook_masks[index]

        // get pieces that are in line with our axis of movement
        let collision_pos = BitBoard.get_positions_list(rook_moves & board_bitboard)

        for (let i = 0; i < collision_pos.length; i++) {
            // for each collision, remove excess spaces in our mask
            let col = collision_pos[i]
            let col_x = col[0]
            let col_y = col[1]
            let col_index = col_x + col_y * 8
            let mask = 0n
            let target_capturable = (this.board[col_index] & Piece.FILTER_COLOR) == piece_color

            if (col_y > y) mask = BitBoard.get_column_segment(x, target_capturable ? col_y : col_y + 1, 7)
            else if (col_y < y) mask = BitBoard.get_column_segment(x, 0, target_capturable ? col_y + 1 : col_y - 1)
            else if (col_x > x) mask = BitBoard.get_row_segment(y, target_capturable ? col_x : col_x + 1, 7)
            else if (col_x < x) mask = BitBoard.get_row_segment(y, 0, target_capturable ? col_x : col_x - 1)

            rook_moves = rook_moves & (~mask)
        }

        let rook_positions = BitBoard.get_positions_list(rook_moves)
        for (let i = 0; i < rook_positions.length; i++) {
            let col = rook_positions[i]
            let col_x = col[0]
            let col_y = col[1]
            moves.push(this.create_move(index, col_x + col_y * 8))
        }

        return moves
    }

    get_queen_moves = (x, y, index, piece, board_bitboard, black_bitboard, white_bitboard) => {
        let pos_bitboard = BitBoard.get(x, y)
        let piece_color = piece & Piece.FILTER_COLOR
        let piece_type = piece & Piece.FILTER_PIECE
        let moves = []

        let queen_moves = this.move_masks.rook_masks[index] | this.move_masks.bishop_masks[index]

        let queen_collision_positions = BitBoard.get_positions_list(queen_moves & board_bitboard)

        for (let i = 0; i < queen_collision_positions.length; i++) {
            let col = queen_collision_positions[i]
            let col_x = col[0]
            let col_y = col[1]
            let col_index = col_x + col_y * 8
            let mask = 0n
            let target_capturable = (this.board[col_index] & Piece.FILTER_COLOR) == piece_color

            if (col_y > y) mask = BitBoard.get_column_segment(x, target_capturable ? col_y : col_y + 1, 7)
            else if (col_y < y) mask = BitBoard.get_column_segment(x, 0, target_capturable ? col_y + 1 : col_y - 1)
            else if (col_x > x) mask = BitBoard.get_row_segment(y, target_capturable ? col_x : col_x + 1, 7)
            else if (col_x < x) mask = BitBoard.get_row_segment(y, 0, target_capturable ? col_x : col_x - 1)

            // lower right
            if (col_y > y && col_x > x) mask = BitBoard.get_diagonal_downwards_right(col_x, col_y)
            // lower left
            else if (col_y > y && col_x < x) mask = BitBoard.get_diagonal_downwards_left(col_x, col_y)
            // upper right
            else if (col_y < y && col_x > x) mask = BitBoard.get_diagonal_upwards_right(col_x, col_y)
            // uper left
            else if (col_y < y && col_x < x) mask = BitBoard.get_diagonal_upwards_left(col_x, col_y)

            // ugh !!!
            if (target_capturable) mask = mask ^ BitBoard.get(col_x, col_y)

            queen_moves = queen_moves & (~mask)
        }

        let queen_positions = BitBoard.get_positions_list(queen_moves & ~(piece_color == Piece.BLACK ? black_bitboard : white_bitboard))
        for (let i = 0; i < queen_positions.length; i++) {
            let col = queen_positions[i]
            let col_x = col[0]
            let col_y = col[1]
            moves.push(this.create_move(index, col_x + col_y * 8))
        }

        return moves
    }

    get_king_moves = (x, y, index, piece, board_bitboard, black_bitboard, white_bitboard) => {
        let pos_bitboard = BitBoard.get(x, y)
        let piece_color = piece & Piece.FILTER_COLOR
        let piece_type = piece & Piece.FILTER_PIECE
        let moves = []

        let king_start = BitBoard.get(x, y)
        let king_moves = king_start >> 8n
            | king_start << 8n
            | king_start >> 9n
            | king_start << 9n
            | king_start >> 7n
            | king_start << 7n

        let king_positions = BitBoard.get_positions_list(king_moves & ~(piece_color == Piece.BLACK ? black_bitboard : white_bitboard))

        for (let i = 0; i < king_positions; i++) {
            let col = king_positions[i]
            let col_x = col[0]
            let col_y = col[1]
            moves.push(this.create_move(index, col_x + col_y * 8))
        }
        return moves
    }

    moves = () => {
        let moves = []
        let board_bitboard = this.get_bitboard()
        let white_bitboard = this.get_bitboard_of_color(Piece.WHITE)
        let black_bitboard = this.get_bitboard_of_color(Piece.BLACK)

        /*
         * plan is for here to calculate possible movements of all pieces, stored in dictionary
         * can be recalled for later
         */

        let white_pieces = []
        for (let i = 0; i < 64; i++) {
            if (this.board[i] != Piece.EMPTY) {
                let data = {
                    pos: i,
                    piece: this.board[i]
                }
            }
        }

        // check if king is in check
        let attacked_spaces = 0n


        for (let y = 0; y < 8; y++) {
            for (let x = 0; x < 8; x++) {
                let index = x + y * 8
                let piece = this.board[index]
                if (piece != Piece.EMPTY) {
                    if ((piece & Piece.FILTER_COLOR) != this.turn) continue

                    switch (piece & Piece.FILTER_PIECE) {
                        case Piece.PAWN:
                            moves = moves.concat(this.get_pawn_moves(x, y, index, piece, board_bitboard, black_bitboard, white_bitboard))
                            break
                        case Piece.KNIGHT:
                            moves = moves.concat(this.get_knight_moves(x, y, index, piece, board_bitboard, black_bitboard, white_bitboard))
                            break
                        case Piece.BISHOP:
                            moves = moves.concat(this.get_bishop_moves(x, y, index, piece, board_bitboard, black_bitboard, white_bitboard))
                            break
                        case Piece.ROOK:
                            moves = moves.concat(this.get_rook_moves(x, y, index, piece, board_bitboard, black_bitboard, white_bitboard))
                            break
                        case Piece.QUEEN:
                            moves = moves.concat(this.get_queen_moves(x, y, index, piece, board_bitboard, black_bitboard, white_bitboard))
                            break
                        case Piece.KING:
                            moves = moves.concat(this.get_king_moves(x, y, index, piece, board_bitboard, black_bitboard, white_bitboard))
                            break
                    }
                }
            }
        }
        return moves
    }

    undo = () => {
        let last_move = this.history.pop()
        if (last_move) {
            this.board[last_move.from] = this.board[last_move.to]
            this.board[last_move.to] = last_move.captured
            this.update_turn()
        }
    }

    update_turn = () => {
        this.turn = (this.turn == Piece.WHITE) ? Piece.BLACK : Piece.WHITE
    }
}

/*
 * BOARD BLOCKER DICTIONARIES
 *
 * run once on startup, dictionaries are then recalled
 */
class MoveMasks {
    constructor() {
        const get_rook_masks = () => {
            let out = []

            for (let y = 0; y < 8; y++) {
                for (let x = 0; x < 8; x++) {
                    let i = y * 8 + x
                    out[i] = BitBoard.get_column(x) ^ BitBoard.get_row(y)
                }
            }

            return out
        }

        const get_bishop_masks = () => {
            let out = []

            for (let y = 0; y < 8; y++) {
                for (let x = 0; x < 8; x++) {
                    let i = y * 8 + x
                    out[i] = BitBoard.get_diagonals(x, y)
                }
            }

            return out
        }

        this.rook_masks = get_rook_masks()
        this.bishop_masks = get_bishop_masks()
    }
}

// let start_time = performance.now()
// let moves = get_moves(board, WHITE)
// console.log(performance.now() - start_time)

// moves.forEach(move => {
//     move_piece(board, move.from, move.to)
//     print_board(board)
//     undo(board)
// })

// console.log(moves.length)

let board = new Board()

const count_bulk_positions = (depth) => {
    if (depth <= 0) return 1

    let moves = board.moves()
    let count = 0

    for (let i = 0; i < moves.length; i++) {
        board.move(moves[i])
        count += count_bulk_positions(depth - 1)
        board.undo()
    }

    return count
}

const measure_count_bulk_positions = (depth) => {
    let start_time = performance.now()
    let count = count_bulk_positions(depth)
    console.log("Depth: " + depth + "\tNumber of positions: " + count + "\tTime: " + (performance.now() - start_time) + "ms")
}

measure_count_bulk_positions(4)

// for (let i = 1; i <= 4; i++) {
//     meausre_count_bulk_positions_new(i)
// }

