const measure = (func) => {
    let start_time = performance.now()
    func()
    let time = performance.now() - start_time
    console.log("Took " + time + "ms | " + time / 1000 + "s")
}

let stats = {
    castles: 0,
    captures: 0,
    ep: 0,
    checks: 0
}

const count_bulk_positions = (depth, print_positions) => {
    if (depth <= 0) return 1

    let moves = board.moves()
    let count = 0

    for (let i = 0; i < moves.length; i++) {
        board.move(moves[i])
        if (print_positions) board.print()
        count += count_bulk_positions(depth - 1)
        board.undo()
    }

    return count
}

const get_all_moves_at = (board, depth) => {
    let moves = board.moves()   
    
    if (depth <= 1) return moves
    let cumulative = []

    for (let i = 0; i < moves.length; i++) {
        board.move(moves[i])
        cumulative = cumulative.concat(get_all_moves_at(board, depth - 1))
        board.undo()
    }

    return cumulative
}

const get_all_moves_verbose_at = (board, depth) => {
    let moves = board.moves({verbose: true})   
    
    if (depth <= 1) return moves
    let cumulative = []

    for (let i = 0; i < moves.length; i++) {
        board.move(moves[i])
        cumulative = cumulative.concat(get_all_moves_verbose_at(board, depth - 1))
        board.undo()
    }

    return cumulative
}

const get_all_chessjs_moves_at = (depth) => {
    let moves = board.moves()
    
    if (depth <= 1) return moves
    let cumulative = []

    for (let i = 0; i < moves.length; i++) {
        board.move(moves[i])
        cumulative = cumulative.concat(get_all_moves_at(depth - 1))
        board.undo()
    }

    return cumulative
}

const CASTLE_QUEENSIDE = 1
const CASTLE_KINGSIDE = 2
const KING_FIRST_MOVE = 3
const ROOK_QUEEN_FIRST_MOVE = 4
const ROOK_KING_FIRST_MOVE = 5
const EN_PASSANT = 6

const measure_count_bulk_positions = (depth, print_positions = false) => {
    let start_time = performance.now()
    let count = count_bulk_positions(depth, print_positions)
    console.log("Depth: " + depth + "\tNumber of positions: " + count + "\tTime: " + (performance.now() - start_time) + "ms")
}

export class Piece {
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
        [Piece.EMPTY]: ""
    }

    static colors = {
        [Piece.WHITE]: "white",
        [Piece.BLACK]: "black",
        [Piece.EMPTY]: ""
    }

    static colors_short = {
        [Piece.WHITE]: "w",
        [Piece.BLACK]: "b",
        [Piece.EMPTY]: ""
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
        let short_name = Piece.names_short[Piece.get_type(piece)]
        if ((piece & Piece.FILTER_COLOR) == Piece.WHITE) short_name = short_name.toUpperCase()
        return short_name
    }
}

export class BitBoard {
    /**
     * prints a valid bitboard
     */
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

    /*
     * alright. wuts going on !
     *
     * rather than iterating through each level, we can target the exact bit which must
     * be changed and set it with bit shifting.
     * rather than shifting to the left (which will invert our results)
     * we can start the bit at its most significant point, then shift it right
     * as many times as we need. 
     */
    static get_i = (i) => {
        return (0b1000000000000000000000000000000000000000000000000000000000000000n >> BigInt(i))
    }

    static get = (x, y) => {
        return (0b1000000000000000000000000000000000000000000000000000000000000000n >> BigInt(x + y * 8))
    }

    static get_row = (y) => {
        return (0b1111111100000000000000000000000000000000000000000000000000000000n >> BigInt(y * 8))
    }

    static get_column = (x) => {
        return (0b1000000010000000100000001000000010000000100000001000000010000000n >> BigInt(x))
    }

    static get_diagonals = (x, y) => {
        return BitBoard.get_diagonal_downwards_left(x, y)
            | BitBoard.get_diagonal_upwards_left(x, y,)
            | BitBoard.get_diagonal_downwards_right(x, y)
            | BitBoard.get_diagonal_upwards_right(x, y)
    }

    // returns a bitboard with a target column segment given (inclusive both)
    static get_column_segment = (x, y1, y2) => {
        let column = this.get_column(x)
        // truncate column to proper size
        column = column >> BigInt((7 - (y2 - y1)) * 8)
        // move to proper position
        column = column << BigInt((7 - y2) * 8)
        return column
    }

    static get_row_segment = (y, x1, x2) => {
        let row = this.get_row(7)
        // truncate row = proper size
        row = row >> BigInt(7 - (x2 - x1))
        // move to proper position
        row = row << BigInt((7 - x2) + (7 - y) * 8)
        return row
    }

    // for ...downwards_right, ...downwards_left and so on (4 total)
    // we get the diagonal up to the end of the board in the specified direction STARTING AT x, y

    // seems a bit inelegant to find the starting bits like this but
    // it prevents wrapping... still about 2x faster than old method !
    //
    // testing some additional methods (using bit shifting to limit the starting bits)
    // but performance was about the same
    static get_diagonal_downwards_right = (x, y) => {
        let starting_bits = 0n
        for (let i = 0; i < 7 - x; i++) {
            starting_bits = (starting_bits >> 9n) | (0b0000000001000000000000000000000000000000000000000000000000000000n)
        }
        return (starting_bits >> BigInt(x + y * 8))
    }

    static get_diagonal_downwards_left = (x, y) => {
        let starting_bits = 0n
        for (let i = 0; i < x; i++) {
            starting_bits = (starting_bits >> 7n) | (0b000000100000000000000000000000000000000000000000000000000000000n)
        }
        return (starting_bits >> BigInt(x + y * 8))
    }

    static get_diagonal_upwards_right = (x, y) => {
        let starting_bits = 0n
        for (let i = 0; i < Math.min(y, (7 - x)); i++) {
            starting_bits = (starting_bits << 7n) | (0b100000000000000n)
        }
        return (starting_bits >> BigInt(x - (7 - y) * 8))
    }

    static get_diagonal_upwards_left = (x, y) => {
        let starting_bits = 0n
        for (let i = 0; i < Math.min(x, y); i++) {
            starting_bits = (starting_bits << 9n) | (0b1000000000n)
        }

        return starting_bits << BigInt((7 - x) + (7 - y) * 8)
    }

    // todo count number of bits and use that instead of 64

    // returns a list of positions of all 1 bits in a bitboard

    // this string conversion method is actually faster than
    // my previous math method...
    static get_positions_list = (bitboard) => {
        let bit_board_str = bitboard.toString(2).padStart(64, "0")
        let positions = []

        for (let i = 0; i < 64; i++) {
            if (bit_board_str[i] == "1") positions.push(i)
        }

        return positions
    }
}

export class Board {
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

    static piece_to_fen = {
        [Piece.BLACK | Piece.PAWN]: "p",
        [Piece.BLACK | Piece.ROOK]: "r",
        [Piece.BLACK | Piece.KNIGHT]: "n",
        [Piece.BLACK | Piece.BISHOP]: "b",
        [Piece.BLACK | Piece.QUEEN]: "q",
        [Piece.BLACK | Piece.KING]: "k",
        [Piece.WHITE | Piece.PAWN]: "P",
        [Piece.WHITE | Piece.ROOK]: "R",
        [Piece.WHITE | Piece.KNIGHT]: "N",
        [Piece.WHITE | Piece.BISHOP]: "B",
        [Piece.WHITE | Piece.QUEEN]: "Q",
        [Piece.WHITE | Piece.KING]: "K",
    }

    static piece_to_img = {
        [Piece.BLACK | Piece.PAWN]: "../img/chesspieces/wikipedia/bP.png",
        [Piece.BLACK | Piece.ROOK]: "../img/chesspieces/wikipedia/bR.png",
        [Piece.BLACK | Piece.KNIGHT]: "../img/chesspieces/wikipedia/bN.png",
        [Piece.BLACK | Piece.BISHOP]: "../img/chesspieces/wikipedia/bB.png",
        [Piece.BLACK | Piece.QUEEN]: "../img/chesspieces/wikipedia/bQ.png",
        [Piece.BLACK | Piece.KING]: "../img/chesspieces/wikipedia/bK.png",
        [Piece.WHITE | Piece.PAWN]: "../img/chesspieces/wikipedia/wP.png",
        [Piece.WHITE | Piece.ROOK]: "../img/chesspieces/wikipedia/wR.png",
        [Piece.WHITE | Piece.KNIGHT]: "../img/chesspieces/wikipedia/wN.png",
        [Piece.WHITE | Piece.BISHOP]: "../img/chesspieces/wikipedia/wB.png",
        [Piece.WHITE | Piece.QUEEN]: "../img/chesspieces/wikipedia/wQ.png",
        [Piece.WHITE | Piece.KING]: "../img/chesspieces/wikipedia/wK.png",
    }

    constructor({id = undefined, start_position = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR"}) {
        this.board = []
        this.history = []
        this.turn = Piece.WHITE
        this.move_masks = new MoveMasks()
        this.piece_positions = []

        // some additional data for castling
        this.game_info = {
            [Piece.BLACK]: {
                has_king_moved: false,
                has_left_rook_moved: false,
                has_right_rook_moved: false,
            },
            [Piece.WHITE]: {
                has_king_moved: false,
                has_left_rook_moved: false,
                has_right_rook_moved: false,
            }
        }

        this.read_fen_string(start_position)
        
        if (id != undefined) {
            this.create_chessboard(id)
        }
    }

    // create_chessboard = (id) => {
    //     let root = document.getElementById(id)
    //     root.classList.add("board")

    //     for (let y = 0; y < 8; y++) {
    //         let row_el = document.createElement("div")
    //         row_el.classList.add("row")
    //         for (let x = 0; x < 8; x++) {
    //             let square = document.createElement("div")
    //             let index = y * 8 + x
    //             square.classList.add("square")
    //             square.classList.add((x + y % 2) % 2 == 1 ? "black" : "white")
    //             square.id = `sq-${index}`
    //             row_el.appendChild(square)

    //             // check for pieces
    //             if (this.board[index] != Piece.EMPTY) {
    //                 let piece = document.createElement("img")
    //                 piece.src = Board.piece_to_img[this.board[index]]
    //                 square.appendChild(piece)
    //             }
    //         }
    //         root.appendChild(row_el)
    //     }
    // }

    // update_chessboard = (id) => {
    //     for (let y = 0; y < 8; y++) {
    //         for (let x = 0; x < 8; x++) {
    //             let index = y * 8 + x
    //             let square = document.getElementById(`sq-${index}`)

    //             // first animate it in, then pop it into existence
    //             if (this.board[index] != Piece.EMPTY) {
    //                 if (square.innerHTML == "") {
    //                     let piece = document.createElement("img")
    //                     piece.src = Board.piece_to_img[this.board[index]]
    //                     square.appendChild(piece)
    //                 } else {
    //                     square.childNodes[0].src = Board.piece_to_img[this.board[index]]
    //                 }
    //             } else {
    //                 square.innerHTML = ""
    //             }
    //         }
    //     }
    // }

    // highlight_squares = (squares) => {
    //     let prev_squares = document.querySelectorAll(".last-move")
    //     for (let i = 0; i < prev_squares.length; i++) {
    //         prev_squares[i].classList.remove("last-move")
    //     }

    //     for (let i = 0; i < squares.length; i++) {
    //         let square = document.getElementById(`sq-${squares[i]}`)
    //         square.classList.add("last-move")
    //     }
    // }

    create_move = (from, to, flags = []) => {
        return {
            from: from,
            to: to,
            piece: this.board[from],
            captured: this.board[to],
            flags: flags,
            fen: this.get_fen_string() // debug only
        }
    }

    read_fen_string = (fen) => {
        let lines = fen.split("/")
        this.board = []

        for (let i = 0; i < lines.length; i++) {
            let line = lines[i]
            for (let j = 0; j < line.length; j++) {
                // split into chars
                let ch = line[j]
                let space_count = parseInt(ch)

                // if is not a number, try to read in as a piece
                if (isNaN(space_count)) {
                    let piece = Board.fen_dictionary[ch]
                    this.board.push(piece)
                    this.piece_positions[piece] = this.board.length - 1
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
        let board_out = ""

        for (let y = 0; y < 8; y++) {
            for (let x = 0; x < 8; x++) {
                let piece = Piece.get_info_short(this.board[y * 8 + x])
                
                board_out += ((piece == "") ? ((x + (y % 2)) % 2 == 1 ? "," : ".") : piece) + " "
            }
            board_out += "\n"
        }

        console.log(board_out)
    }

    print_positions = () => {
        let board_out = ""

        for (let y = 0; y < 8; y++) {
            board_out += (8 - y) + " | "
            for (let x = 0; x < 8; x++) {
                let num = x + y * 8
                board_out += ((num < 10) ? " " + num : num) + "   "
            }
            board_out += " \n  |"
            if (y != 7) board_out += "\n"
            else board_out += "---------------------------------------\n     "
        }

        for (let i = 0; i < 8; i++) {
            board_out += String.fromCharCode(("a".charCodeAt(0) + i)) + "    "
        }

        console.log(board_out)
    }

    get_fen_string = () => {
        let empty_spaces_count = 0
        let fen_string = ""
        for (let i = 0; i < 64; i++) {
            let piece = this.board[i]

            // started scanning a new line
            if (i % 8 == 0 && i != 0) {
                // if we were counting empty spaces prior, add and reset
                if (empty_spaces_count != 0) {
                    fen_string += empty_spaces_count
                    empty_spaces_count = 0
                }
                fen_string += "/"
            }

            if (piece == Piece.EMPTY) {
                empty_spaces_count++
            } else {
                // also record/reset num of spaces when we
                // were just counting but hit a piece
                if (empty_spaces_count != 0) {
                    fen_string += empty_spaces_count
                    empty_spaces_count = 0
                }

                fen_string += Board.piece_to_fen[piece]
            }
        }

        return fen_string
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

            // update our game state for rook/king stuff
            switch (this.board[move.from] & Piece.FILTER_PIECE) {
                case Piece.ROOK:
                    let rook_x = move.from % 8
                    if (rook_x == 0) {
                        this.game_info[this.turn].has_left_rook_moved = true
                    } else if (rook_x == 7) {
                        this.game_info[this.turn].has_right_rook_moved = true
                    }
                    break
                case Piece.KING:
                    let king_x = move.from % 8
                    
                    if (!this.game_info[this.turn].has_king_moved) {
                        let target_x = move.to % 8

                        if (king_x == 4) {
                            this.game_info[this.turn].has_king_moved = true
                            break
                        }
                    }
                    break
            }

            // TODO fix piece positions

            this.board[move.to] = this.board[move.from]
            this.board[move.from] = Piece.EMPTY
            this.piece_positions[this.board[move.to]] = move.to

            for (let i = 0; i < move.flags.length; i++) {
                switch (move.flags[i]) {
                    case CASTLE_KINGSIDE:
                        // move rook
                        this.board[move.to + 1] = Piece.EMPTY
                        this.board[move.from + 1] = (Piece.ROOK | this.turn)
                        this.piece_positions[this.board[move.from + 1]] = move.from + 1
                        break
                    case CASTLE_QUEENSIDE:
                        // move rook
                        this.board[move.to - 2] = Piece.EMPTY
                        this.board[move.from - 1] = (Piece.ROOK | this.turn)
                        this.piece_positions[this.board[move.from - 1]] = move.from - 1
                        break
                    case EN_PASSANT:
                        // capture da piece
                        let captured_pawn_position = move.to + (this.turn == Piece.BLACK ? -8 : 8)

                        move.captured = this.board[captured_pawn_position]
                        this.board[captured_pawn_position] = Piece.EMPTY
                        break
                }
            }
        }

        //this.update_chessboard()
        //light_squares([move.from, move.to])
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

    /*
     * On get_..._moves
     *
     * These functions return a BitBoard representing all the spaces which the particular piece at the particular position
     * can move to. This bit board INCLUDES SOME SPACES THAT IT CANT NECESSARILY MOVE TO. Why?
     * 
     * For other calculations, its nice for us to know also which pieces we are attacking, whether friendly or not. Accordingly,
     * we know that the opponent's pieces we can capture should be in the bit board. our allied pieces should not be in the bitboard.
     * However, it is still useful for us to know which pieces we are protecting, so it is included in the
     * returns for this bit board. This CAN AND SHOULD BE FILTERED OUT FOR MOVE GENERATION
     * 
     * In general however, this will return an array of bit boards, each representing a possible direction of movement.
     * in some cases, like for pawns, this will instead return different ways of making a move (moving forward, cpaturing diagonally, en passant)
     */
    
    // returns 2 bitboards: moves, captures
    get_pawn_moves = (x, y, index, piece, board_bitboard, black_bitboard, white_bitboard) => {
        let starting_position = BitBoard.get(x, y)
        let piece_color = piece & Piece.FILTER_COLOR
        let axis_bitboard

        let movement = 0n
        let captures = 0n

        // while it is quite ugly to seperate into if/else rather than using piece_color == Piece.Black ? ... : ..., I believe this is SLIGHTLY more performant. almost to such a small degree it doesnt matter?
        // moving forward 1/2 spaces
        if (piece_color == Piece.BLACK) {
            movement = starting_position >> 8n
            if (y == 1) movement = movement | movement >> 8n

            if (x != 0) {
                captures = captures | starting_position >> 7n
            }
            if (x != 7) {
                captures = captures | starting_position >> 9n
            }

            axis_bitboard = white_bitboard
        } else {
            movement = starting_position << 8n
            if (y == 6) movement = movement | movement << 8n
            
            if (x != 0) {
                captures = captures | starting_position << 9n
            }
            if (x != 7) {
                captures = captures | starting_position << 7n
            }

            axis_bitboard = black_bitboard
        }
        
        /*
         * now we must check for pieces blocking forward movement.
         * to do so, we can check for collisions in our path with any piece in front of us,
         * and if there are any we can mask out a column segment starting at that
         * piece and extending one unit forwards to ensure no jumping
         */

        // we will have at most 2 collisions, and the second we dont care about. we either get the one in front of us or nothing
        // make sure to mask our starting position, or else it will count that as a collision!
        let movement_collisions = (movement & (board_bitboard ^ starting_position))
        
        if (movement_collisions) {
            // extend 1 square backwards to encompass entire range of movement (prevents jumping from home row)
            movement_collisions = movement_collisions | (piece_color == Piece.BLACK ? movement_collisions >> 8n : movement_collisions << 8n)
            // crop our movement
            movement = movement & ~movement_collisions
        }

        /*
         * Now we must check for capture collisions
         */

        // possible captures/protections = captures AND all pieces
        captures = captures & board_bitboard

        // check for en passant
        if (this.history.length != 0) {
            let last_move = this.history[this.history.length - 1]
            if (last_move && (last_move.piece & Piece.FILTER_PIECE) == Piece.PAWN && Math.abs(last_move.from - last_move.to) >= 16) {
                // left side en passant
                if (index - last_move.to == 1 && (index % 8 != 0)) {
                    captures = captures | ((piece_color == Piece.BLACK) ? starting_position >> 7n : starting_position << 9n)
                } else if(index - last_move.to == -1 && (index % 8 != 7)) {
                    // right side
                    captures = captures | ((piece_color == Piece.BLACK) ? starting_position >> 9n : starting_position << 7n)
                }
            }
        }


        // normally here we'd concern ourselves with separating directions of movement,
        // but pawn attacks cannot be blocked so its okay!

        // the output is then [movements, captures]
        return {
            movements: [movement, captures]
        }
    }

    get_knight_moves = (x, y, index, piece, board_bitboard, black_bitboard, white_bitboard) => {
        let starting_position = BitBoard.get(x, y)

        // just like pawns, knight's attacks CANNOT be blocked, so we dont need to
        // add multiple directions of attack in a list
        let movement = 0n

        if (y + 2 < 8 && x + 1 < 8) movement = movement | starting_position >> 17n
        if (y + 2 < 8 && x - 1 >= 0) movement = movement | starting_position >> 15n
        if (y + 1 < 8 && x + 2 < 8) movement = movement | starting_position >> 10n
        if (y + 1 < 8 && x - 2 >= 0) movement = movement | starting_position >> 6n
        if (y - 2 >= 0 && x + 1 < 8) movement = movement | starting_position << 15n
        if (y - 2 >= 0 && x - 1 >= 0) movement = movement | starting_position << 17n
        if (y - 1 >= 0 && x + 2 < 8) movement = movement | starting_position << 6n
        if (y - 1 >= 0 && x - 2 >= 0) movement = movement | starting_position << 10n

        return {
            movements: [movement]
        }
    }

    get_bishop_moves = (x, y, index, piece, board_bitboard, black_bitboard, white_bitboard) => {
        let mask = this.move_masks.bishop_masks[index] 
        let all_movement = mask.all_movement
        let movements = [...mask.directions] // dont overwrite!

        // get pieces that are in line with our axis of movement
        let collisions = BitBoard.get_positions_list(all_movement & board_bitboard)
        
        // what is king exception ?

        // when we are checking a king, we should also store
        // our attack ray as if the king wasn't blocking it, to
        // ensure the king doesnt go where we may also be attacking later
        let king_exception = 0n

        for (let i = 0; i < collisions.length; i++) {
            // for each collision, remove unreachable spaces
            let col = collisions[i]
            let col_x = col % 8
            let col_y = Math.floor(col / 8)

            if (col_y > y && col_x < x) {
                if ((this.board[col] & Piece.FILTER_PIECE) == Piece.KING) {
                    king_exception = mask.directions[0]
                }
                movements[0] = movements[0] & ~BitBoard.get_diagonal_downwards_left(col_x, col_y)
            }
            else if (col_y < y && col_x < x) {
                if ((this.board[col] & Piece.FILTER_PIECE) == Piece.KING) {
                    king_exception = mask.directions[1]
                }
                movements[1] = movements[1] & ~BitBoard.get_diagonal_upwards_left(col_x, col_y)
            }
            else if (col_y > y && col_x > x) {
                if ((this.board[col] & Piece.FILTER_PIECE) == Piece.KING) {
                    king_exception = mask.directions[2]
                }
                movements[2] = movements[2] & ~BitBoard.get_diagonal_downwards_right(col_x, col_y)
            }
            else if (col_y < y && col_x > x) {
                if ((this.board[col] & Piece.FILTER_PIECE) == Piece.KING) {
                    king_exception = mask.directions[3]
                }
                movements[3] = movements[3] & ~BitBoard.get_diagonal_upwards_right(col_x, col_y)
            }
        }

        return {
            movements,
            king_exception
        }
    }

    get_rook_moves = (x, y, index, piece, board_bitboard, black_bitboard, white_bitboard) => {
        let mask = this.move_masks.rook_masks[index]
        let all_movement = mask.all_movement
        let movements = [...mask.directions]

        let king_exception = 0n

        // get all pieces in line with moves
        let collisions = BitBoard.get_positions_list(all_movement & board_bitboard)

        for (let i = 0; i < collisions.length; i++) {
            // remove unreachable spaces for every collision
            let col = collisions[i]
            let col_x = col % 8
            let col_y = Math.floor(col / 8)

            if (col_y > y) {
                if ((this.board[col] & Piece.FILTER_PIECE) == Piece.KING) {
                    king_exception = mask.directions[0]
                }
                movements[0] = movements[0] & ~BitBoard.get_column_segment(x, col_y + 1, 7)
            }
            else if (col_y < y) {
                if ((this.board[col] & Piece.FILTER_PIECE) == Piece.KING) {
                    king_exception = mask.directions[1]
                }
                movements[1] = movements[1] & ~BitBoard.get_column_segment(x, 0, col_y - 1)
            }
            else if (col_x > x) {
                if ((this.board[col] & Piece.FILTER_PIECE) == Piece.KING) {
                    king_exception = mask.directions[2]
                }
                movements[2] = movements[2] & ~BitBoard.get_row_segment(y, col_x + 1, 7)
            }
            else if (col_x < x) {
                if ((this.board[col] & Piece.FILTER_PIECE) == Piece.KING) {
                    king_exception = mask.directions[3]
                }
                movements[3] = movements[3] & ~BitBoard.get_row_segment(y, 0, col_x - 1)
            }
        }

        return {
            movements,
            king_exception
        }
    }

    get_queen_moves = (x, y, index, piece, board_bitboard, black_bitboard, white_bitboard) => {
        let rook_moves = this.get_rook_moves(x, y, index, piece, board_bitboard, black_bitboard, white_bitboard)
        let bishop_moves = this.get_bishop_moves(x, y, index, piece, board_bitboard, black_bitboard, white_bitboard)

        return {
            movements: [...rook_moves.movements, ...bishop_moves.movements],
            king_exception: rook_moves.king_exception | bishop_moves.king_exception
        }
    }

    get_king_moves = (x, y, index, piece, board_bitboard, black_bitboard, white_bitboard) => {
        let starting_position = BitBoard.get(x, y)
        let movements = []
        let movement = 0n

        if (y < 7) movement |= starting_position >> 8n
        if (y < 7 && x < 7) movement |= starting_position >> 9n
        if (y < 7 && x > 0) movement |= starting_position >> 7n
        if (x > 0) movement |= starting_position << 1n
        if (x < 7) movement |= starting_position >> 1n
        if (y > 0) movement |= starting_position << 8n
        if (y > 0 && x > 0) movement |= starting_position << 9n
        if (y > 0 && x < 7) movement |= starting_position << 7n

        movements = [movement]

        // check for castling
        if (!this.game_info[this.turn].has_king_moved) {
            // basic checks are:
            // if the rook hasnt moved & is in its proper position
            // and the king is in its proepr position

            if (!this.game_info[this.turn].has_left_rook_moved
                && (
                    (this.turn == Piece.WHITE && this.board[56] == (this.turn | Piece.ROOK))
                    || (this.turn == Piece.BLACK && this.board[0] == (this.turn | Piece.ROOK))
                )
                && (
                    (this.turn == Piece.WHITE && this.board[60] == (this.turn | Piece.KING))
                    || (this.turn == Piece.BLACK && this.board[4] == (this.turn | Piece.KING))
                )
            ) {
                let left_castle_check = (starting_position << 1n) | (starting_position << 2n) | (starting_position << 3n)
                if ((left_castle_check & board_bitboard) == 0n) {
                    // if there are no obstructions, add castling to our moves
                    movements.push(starting_position << 2n)
                }
            }

            if (!this.game_info[this.turn].has_right_rook_moved 
                && (
                    (this.turn == Piece.WHITE && this.board[63] == (this.turn | Piece.ROOK))
                    || (this.turn == Piece.BLACK && this.board[7] == (this.turn | Piece.ROOK))
                )
                && (
                    (this.turn == Piece.WHITE && this.board[60] == (this.turn | Piece.KING))
                    || (this.turn == Piece.BLACK && this.board[4] == (this.turn | Piece.KING))
                )
            ) {
                let right_castle_check = (starting_position >> 1n) | (starting_position >> 2n)
                if ((right_castle_check & board_bitboard) == 0n) {
                    // if there are no obstructions, add castling to our moves
                    movements.push(starting_position >> 2n)
                }
            }
        }
        
        return {
            movements: movements
        }
    }

    /*
     * moves function is in DIRE need of a rewrite. how should moves be calculated?
     *
     * first: */

    moves = () => {
        let moves = []
        let board_bitboard = this.get_bitboard()
        let black_bitboard = this.get_bitboard_of_color(Piece.BLACK)
        let white_bitboard = this.get_bitboard_of_color(Piece.WHITE)

        /*
         * plan is for here to calculate possible movements of all pieces, stored in dictionary
         * can be recalled for later
         */

        
        /* 
         * store all pieces into these arrays, which contain
         * the piece in question, its position, and its possible moves as a bitboard
         */
        let white_pieces = []
        let black_pieces = []
        for (let y = 0; y < 8; y++) {
            for (let x = 0; x < 8; x++) {
                let i = x + y * 8
                let piece = this.board[i]

                if (piece != Piece.EMPTY) {
                    let data = {
                        pos: i,
                        piece: piece,
                    }

                    let move_bitboards
                    switch(piece & Piece.FILTER_PIECE) {
                        case Piece.PAWN: 
                            let pawn_data = this.get_pawn_moves(x, y, i, piece, board_bitboard, black_bitboard, white_bitboard)
                            data.non_capture_bitboard = pawn_data.movements[0]
                            move_bitboards = [pawn_data.movements[1]]
                            break
                        case Piece.KNIGHT: move_bitboards = this.get_knight_moves(x, y, i, piece, board_bitboard, black_bitboard, white_bitboard).movements
                            break
                        case Piece.BISHOP: 
                            let bishop_data = this.get_bishop_moves(x, y, i, piece, board_bitboard, black_bitboard, white_bitboard)
                            data.king_exception = bishop_data.king_exception
                            move_bitboards = bishop_data.movements
                            break
                        case Piece.ROOK: 
                            let rook_data = this.get_rook_moves(x, y, i, piece, board_bitboard, black_bitboard, white_bitboard) 
                            data.king_exception = rook_data.king_exception
                            move_bitboards = rook_data.movements
                            break
                        case Piece.QUEEN: 
                            let queen_data = this.get_queen_moves(x, y, i, piece, board_bitboard, black_bitboard, white_bitboard) 
                            data.king_exception = queen_data.king_exception
                            move_bitboards = queen_data.movements
                            break
                        case Piece.KING: move_bitboards = this.get_king_moves(x, y, i, piece, board_bitboard, black_bitboard, white_bitboard).movements
                            break
                    }

                    data.move_bitboards = move_bitboards

                    if ((piece & Piece.FILTER_COLOR) == Piece.BLACK) {
                        black_pieces.push(data)
                    } else {
                        white_pieces.push(data)
                    }
                }
            }
        }

        /*
         * plan for checks
         * 
         * when pieces have their moves calculated, we trim as normal and such, except we do NOT MASK EXISTING BOARD PIECES
         * so all moves INCLUDE pieces that are in the way (not necessarily that they can pass through them, but captures or
         * allied pieces that are blocking are "counted" in the bitboard)
         * 
         * This just gives us more freedom to calculate which moves are safe (for example, a king cant capture a piece that is checking
         * him if the piece is protected by another piece)
         * 
         * of course, for move generation, we can simply AND it with the mask or NOT of whichever bitboard we need (for capturing moves,
         * & with the opposite side)
         * 
         * this covers being checked by pieces and capturing checking pieces, but this doesnt cover some things, like blocking or pins.
         * I imagine there are 2 main scenarios: 
         *  1. a piece that is pinned cannot move to block
         *  2. a piece with valid moves towards a particular spot that can block is a block
         * 
         * i suppose the natural thing to do would be to calculate a "blocking_spaces" bitboard, of spaces that are valid blocks.
         * this bit board should NOT include spaces that would only block one attacker if 2 different angles were attacked for example.
         * it should be a board such that any unpinned piece moving into any of those squares should be a valid move
         */

        // check if king is in check
        // obtain "danger spaces"
        // again, these danger spaces INCLUDE spaces which pieces are on top of, allied or otherwise
        let attacked_squares = 0n
        // pieces that are attacking the king
        let king_attackers = []
        let king_attackers_bitboard = 0n
        let ally_pieces = (this.turn == Piece.BLACK) ? black_pieces : white_pieces
        let axis_pieces = (this.turn == Piece.BLACK) ? white_pieces : black_pieces
        let king_pos_bitboard = 0n
        let in_check = false
        let blocker_spaces = 0n

        // TODO store king's position somewhere and recall instead of searchign!
        for (let i = 0; i < 64; i++) {
            if (this.board[i] == (Piece.KING | this.turn)) {
                king_pos_bitboard = BitBoard.get_i(i)
            }
        }

        // console.log(this.piece_positions)
        // console.log(this.turn | Piece.KING)
        // if (this.piece_positions[(this.turn | Piece.KING)]) {
        //     king_pos_bitboard = this.piece_positions[this.turn | Piece.KING]
        // }

        // we must also capture squares that are able to be "blocked"

        let attacks_on_king = []
        
        // calculate attacked squares from axis pieces
        for (let i = 0; i < axis_pieces.length; i++) {
            let axis_piece_data = axis_pieces[i]
            
            // if the king is in LINE of an attack (not in check) record the attack on the king
            if (axis_piece_data.king_exception && (king_pos_bitboard & axis_piece_data.king_exception)) {
                // check also: only add if ONE piece is pinned, not more than one

                // remove the king from the king exception
                let king_exception_truncated = (~king_pos_bitboard & axis_piece_data.king_exception)
                let pinned_pieces = BitBoard.get_positions_list(king_exception_truncated & board_bitboard)
                if (pinned_pieces.length == 1)
                    attacks_on_king.push(axis_piece_data.king_exception)
            }

            // add all attacking moves to attacked squares
            for (let j = 0; j < axis_piece_data.move_bitboards.length; j++) {
                let direction_of_attack = axis_piece_data.move_bitboards[j]
                
                attacked_squares = attacked_squares | direction_of_attack
                //BitBoard.print(direction_of_attack)
                // check if that direction of attack is checking the king

                if (king_pos_bitboard & direction_of_attack) {                    
                    king_attackers.push({
                        piece: axis_piece_data.piece,
                        index: axis_piece_data.pos
                    })
                    blocker_spaces = blocker_spaces | direction_of_attack
                    in_check = true

                    // in this instance, we also need to consider our king exception spaces
                    // only if we are a sliding piece
                    switch (axis_piece_data.piece & Piece.FILTER_PIECE) {
                        case Piece.BISHOP:
                        case Piece.ROOK:
                        case Piece.QUEEN:
                            attacked_squares = attacked_squares | axis_piece_data.king_exception
        
                            break
                    }
                }
            }
        }

        //console.log(in_check ? "King is in check!" : "King is not in check.")

        // now we need to fix the kings' moves to prevent them from walking into attacked squares

        for (let i = 0; i < ally_pieces.length; i++) {
            let piece = ally_pieces[i]

            if ((piece.piece & Piece.FILTER_PIECE) == Piece.KING) {
                for (let j = 0; j < piece.move_bitboards.length; j++) {
                    piece.move_bitboards[j] = piece.move_bitboards[j] & ~attacked_squares
                }
            }
        }

        if (in_check && king_attackers.length <= 1) {
            // lets calculate the spaces where allied blocker pieces can goto. in these positions, the king must NOT be in check after the move.
            
            // first lets filter the attacking spaces to only include the spaces where no pieces are
            blocker_spaces = blocker_spaces & ~board_bitboard

            // get a bitboard of the positions of attacking pieces
            for (let j = 0; j < king_attackers.length; j++) {
                king_attackers_bitboard = king_attackers_bitboard | BitBoard.get_i(king_attackers[j].index)
            }

            //console.log("there are " + king_attackers.length + " attackers")

            //BitBoard.print(king_attackers_bitboard)

            /* brainstorm
             *
             * lets think.. a piece can only "block" from sliding pieces. knights, pawns, kings are except.
             * sliding/diagonal pieces can only attack from one of 8 directions.
             * 
             * we can also only block from ONE direction. so if we are attacked with more than one direction, we simply cannot block.
             * so if number_of_pieces_attacking_king is greater than 1, its gg
             */
        }

        for (let i = 0; i < ally_pieces.length; i++) {
            let piece_data = ally_pieces[i]
            let all_moves_bitboard = 0n

            // get all of our possible moves
            for (let j = 0; j < piece_data.move_bitboards.length; j++) {
                all_moves_bitboard = all_moves_bitboard | piece_data.move_bitboards[j]
            }

            // if a pawn, also remember to record our captures as well
            if ((piece_data.piece & Piece.FILTER_PIECE) == Piece.PAWN) {
                all_moves_bitboard = all_moves_bitboard | piece_data.non_capture_bitboard
            }

            // filter our moves: remove invalid (capturing our own pieces) moves
            all_moves_bitboard = all_moves_bitboard & ~((this.turn == Piece.BLACK) ? black_bitboard : white_bitboard)
           
            if ((piece_data.piece & Piece.FILTER_PIECE) != Piece.KING) {
                for (let j = 0; j < attacks_on_king.length; j++) {
                    let attack = attacks_on_king[j]

                    // this piece is pinned! restrict our movement
                    if (BitBoard.get_i(piece_data.pos) & attack) {
                        all_moves_bitboard = all_moves_bitboard & attack & ~((this.turn == Piece.BLACK) ? black_bitboard : white_bitboard)
                    }
                }
            }
            
            // if we are in check, only consider moves that will block checks or kill lone attackers for non king pieces
            if (in_check) {
                // for kings consider only captures and evades
                if ((piece_data.piece & Piece.FILTER_PIECE) == Piece.KING) {
                    all_moves_bitboard = all_moves_bitboard | (all_moves_bitboard & king_attackers_bitboard)
                } else {
                    // for other pieces, consider only blocks and captures. again this is only possible if we have 1 attacker
                    // also ensure we dont move improerply if we are pinned

                    if (king_attackers.length <= 1) {
                        all_moves_bitboard = (all_moves_bitboard & blocker_spaces) 
                        | (all_moves_bitboard & king_attackers_bitboard)
                    } else {
                        all_moves_bitboard = 0n
                    }
                }
            }

            if (all_moves_bitboard) {
                let positions = BitBoard.get_positions_list(all_moves_bitboard)
                positions_eval:
                for (let j = 0; j < positions.length; j++) {
                    let flags = [] // no flag by default

                    // special cases
                    switch (piece_data.piece & Piece.FILTER_PIECE) {
                        case Piece.KING:
                            if (!this.game_info[this.turn].has_king_moved) {
                                flags.push(KING_FIRST_MOVE)
                                let king_x = piece_data.pos % 8
                                let target_x = positions[j] % 8
    
                                if (king_x - target_x >= 2) {   // queen side castle
                                    if (in_check) continue // we cannot castle out of check
                                    flags.push(CASTLE_QUEENSIDE)
                                    flags.push(ROOK_QUEEN_FIRST_MOVE)
                                } else if (target_x - king_x >= 2) {    // king side castle
                                    if (in_check) continue // we cannot castle out of check
                                    flags.push(CASTLE_KINGSIDE)
                                    flags.push(ROOK_KING_FIRST_MOVE)
                                }
                            }
                            break
                        case Piece.ROOK:
                            let rook_x = piece_data.pos % 8
                            if (rook_x == 0 && !this.game_info[this.turn].has_left_rook_moved) {
                                flags.push(ROOK_QUEEN_FIRST_MOVE)
                            } else if (rook_x == 7 && !this.game_info[this.turn].has_right_rook_moved) {
                                flags.push(ROOK_KING_FIRST_MOVE)
                            }
                            break
                        case Piece.PAWN:
                            // filter out movement only spaces: we are concerned with captures
                            let only_movement_spaces = BitBoard.get_column(piece_data.pos % 8)

                            // if we are capturing an empty space, we must be doing en passant!
                            let capture_square_bitboard = BitBoard.get_i(positions[j])
                            if (this.board[positions[j]] == Piece.EMPTY && (capture_square_bitboard & ~only_movement_spaces)) {
                                // lets check though.. will the king be in danger?
                                // we can do it slow way... because en passant is so rare anyways

                                
                                let ally_pawn_pos_bitboard = BitBoard.get_i(piece_data.pos)
                                //let axis_pawn_pos_
                                
                                // prevent en passant when pinned
                                for (let k = 0; k < axis_pieces.length; k++) {
                                    let axis_piece_data = axis_pieces[k]
                                    // our pawn is in an attacking line/pinned!
                                    if (axis_piece_data.king_exception && axis_piece_data.king_exception & ally_pawn_pos_bitboard) {
                                        continue positions_eval
                                    }
                                }

                                flags.push(EN_PASSANT)
                            }
                    }

                    //BitBoard.print(all_moves_bitboard)

                    if (in_check) stats.checks++
                    if (flags.includes(EN_PASSANT)) stats.ep++
                    if (flags.includes(CASTLE_KINGSIDE) || flags.includes(CASTLE_QUEENSIDE)) stats.castles++
                    if (this.board[positions[j]] != Piece.EMPTY) stats.captures++

                    moves.push(this.create_move(piece_data.pos, positions[j], flags))
                }
            }
        }

        return moves
    }

    undo = () => {
        let last_move = this.history.pop()
        this.update_turn()
        if (last_move) {

            for (let i = 0; i < last_move.flags.length; i++) {
                switch (last_move.flags[i]) {
                    case CASTLE_QUEENSIDE:
                        // undo our rook movement
                        this.board[last_move.to - 2] = (this.turn | Piece.ROOK)
                        this.board[last_move.to + 1] = Piece.EMPTY
                        this.piece_positions[(this.turn | Piece.ROOK)] = last_move.to - 2
                        //last_move.captured = (this.turn | Piece.KING) // just so we put the king back where it was
                        break
                    case CASTLE_KINGSIDE:
                        this.board[last_move.to + 1] = (this.turn | Piece.ROOK)
                        this.board[last_move.to - 1] = Piece.EMPTY
                        this.piece_positions[(this.turn | Piece.ROOK)] = last_move.to + 1
                        //last_move.captured = (this.turn | Piece.KING)
                        break
                    case KING_FIRST_MOVE:
                        this.game_info[this.turn].has_king_moved = false
                        break
                    case ROOK_KING_FIRST_MOVE:
                        this.game_info[this.turn].has_right_rook_moved = false
                        break
                    case ROOK_QUEEN_FIRST_MOVE:
                        this.game_info[this.turn].has_left_rook_moved = false
                        break
                    case EN_PASSANT:
                        // recorded it as captured for record keeping purposes. now thats its gone, bye bye    
                        this.board[(this.turn == Piece.BLACK ? last_move.to - 8 : last_move.to + 8)] = last_move.captured
                        last_move.captured = Piece.EMPTY
                }
            }

            this.board[last_move.from] = this.board[last_move.to]
            this.board[last_move.to] = last_move.captured
            this.piece_positions[last_move.captured] = last_move.to
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
 * Because of that, we aren't too concern with performance here
 * 
 * But what does this return ? For any given position, we can get
 * the rook/bishop's movement in bitboards for that position in an array.
 * this is filtered into ALL POSSIBLE MOVES in 1 board, and different directions
 */
class MoveMasks {
    constructor() {
        const get_rook_masks = () => {
            let out = []

            for (let y = 0; y < 8; y++) {
                for (let x = 0; x < 8; x++) {
                    let i = y * 8 + x
                    let starting_position = BitBoard.get(x, y)

                    let directions = [
                        BitBoard.get_column_segment(x, y, 7) & ~starting_position,
                        BitBoard.get_column_segment(x, 0, y) & ~starting_position,
                        BitBoard.get_row_segment(y, x, 7) & ~starting_position,
                        BitBoard.get_row_segment(y, 0, x) & ~starting_position
                    ]
                    let all_movement = 0n

                    directions.forEach(direction => {
                        all_movement = all_movement | direction
                    })

                    // separate into different directions
                    out[i] = {
                        all_movement,
                        directions
                    }
                }
            }

            return out
        }

        const get_bishop_masks = () => {
            let out = []

            for (let y = 0; y < 8; y++) {
                for (let x = 0; x < 8; x++) {
                    let i = y * 8 + x

                    let directions = [
                        BitBoard.get_diagonal_downwards_left(x, y),
                        BitBoard.get_diagonal_upwards_left(x, y,),
                        BitBoard.get_diagonal_downwards_right(x, y),
                        BitBoard.get_diagonal_upwards_right(x, y)
                    ]
                    let all_movement = 0n

                    directions.forEach(direction => {
                        all_movement = all_movement | direction
                    })

                    // separate into different directions
                    out[i] = {
                        all_movement,
                        directions
                    }
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

// console.log("Testing string concatenation vs bit shifting bit board instantiation")
// console.log("testing string concatenation")
// measure(() => {
//     for (let i = 0; i < 10000; i++) {
//         for (let j = 0; j < 64; j++) {
//             BitBoard.old_get_i(j)
//         }
//     }
// })
// console.log("testing bit shifting")
// measure(() => {
//     for (let i = 0; i < 10000; i++) {
//         for (let j = 0; j < 64; j++) {
//             BitBoard.get_i(j)
//         }
//     }
// })

/*static old_get_i = (i) => {
        let out = "0b"
        for (let j = 0; j < 64; j++) {
            out += (j == i) ? "1" : "0"
        }
        return BigInt(out)
    }*/

// let board = new Board("8/3r4/8/6R1/3K4/r7/8/8")
// board.print()
// let moves
// measure(() => {
//     moves = board.moves()
// })

// console.log(moves.length + " moves possible.")
// moves.forEach(move => {
//     board.move(move)
//     board.print()
//     board.undo()
// })

// let board = new Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR")

// for (let j = 0; j < 8; j++) {
//     BitBoard.print(BitBoard.get_diagonal_downwards_right(j, j))
//     BitBoard.print(BitBoard.get_diagonal_downwards_right_old(j, j))
// }

// BitBoard.print(BitBoard.get_diagonal_upwards_left(1, 1))
// BitBoard.print(BitBoard.get_diagonal_upwards_left_old(1, 1))

// for (let y = 0; y < 8; y++) {
//     for (let x = 0; x < 8; x++) {
//         if (BitBoard.get_diagonal_upwards_left(x, y) != BitBoard.get_diagonal_upwards_left_old(x, y)) console.log("failed!")
//     }
// }

// let bb = BitBoard.get(3, 3)

// BitBoard.print(BitBoard.get_diagonal_downwards_left(2, 0))

// console.log("new:")
// measure(() => {
//     for (let i = 0; i < 10000; i++) {
//         for (let y = 0; y < 8; y++) {
//             for (let x = 0; x < 8; x++) {
//                 let bb = BitBoard.get(x, y)
        
//                 BitBoard.get_positions_list(bb)
//             }
//         }
//     }
// })

// console.log("old:")
// measure(() => {
//     for (let i = 0; i < 10000; i++) {
//         for (let y = 0; y < 8; y++) {
//             for (let x = 0; x < 8; x++) {
//                 let bb = BitBoard.get(x, y)
        
//                 BitBoard.get_positions_list_old(bb)
//             }
//         }
//     }
// })

// console.log("new:")
// measure(() => {
//     for (let i = 0; i < 100; i++) {
//         for (let j = 0; j < 8; j++) {
//             for (let x = 0; x < 8; x++) {
//                 for (let y = 0; y < 8; y++) {
//                     BitBoard.get_column_segment(j, x, y)
//                 }
//             }
//         }
//     }
// })
// console.log("old:")
// measure(() => {
//     for (let i = 0; i < 100; i++) {
//         for (let j = 0; j < 8; j++) {
//             for (let x = 0; x < 8; x++) {
//                 for (let y = 0; y < 8; y++) {
//                     BitBoard.get_column_segment_old(j, x, y)
//                 }
//             }
//         }
//     }
// })

// let board = new Board()
// board.move(board.create_move(12, 28))
// board.update_turn()
// board.print()
// board.move(board.create_move(36, 28))
// board.move(board.create_move(11, 27))
// board.print()

// let moves = board.moves()

// moves.forEach(m => {
//     board.move(m)
//     board.print()
//     board.undo()
// })

//board.print_positions()
// board.moves()
// console.log(board.moves())
// for (let i = 1; i <= 4; i++) {
//     measure_count_bulk_positions(i)
//     console.log(stats)
//     stats = {
//         checks: 0,
//         ep: 0,
//         castles: 0,
//         captures: 0
//     }
// }


const get_chess_pos_to_index_table = () => {
    let letters = ["a", "b", "c", "d", "e", "f", "g", "h"]
    let numbers = [8, 7, 6, 5, 4, 3, 2, 1]
    let table = {}

    for (let i = 0; i < 8; i++) {
        for (let j = 0; j < 8; j++) {
            let key = letters[j] + numbers[i]
            table[key] = i * 8 + j
        }
    }

    return table
}

const position_to_index = get_chess_pos_to_index_table()
const piece_str_to_int = {
    "p": Piece.PAWN,
    "n": Piece.KNIGHT,
    "b": Piece.BISHOP,
    "r": Piece.ROOK,
    "q": Piece.QUEEN,
    "k": Piece.KING,
}

const compare_chessjs_to_woodstock_position = (woodstock_pos, chessjs_pos) => {
    let chessjs_to = position_to_index[chessjs_pos.to]
    let chessjs_from = position_to_index[chessjs_pos.from]
    let chessjs_piece = piece_str_to_int[chessjs_pos.piece]
    let woodstock_to = woodstock_pos.to
    let woodstock_from = woodstock_pos.from
    let woodstock_piece = woodstock_pos.piece & Piece.FILTER_PIECE

    return chessjs_to == woodstock_to 
            && chessjs_from == woodstock_from
            && chessjs_piece == woodstock_piece
            && woodstock_pos.fen == chessjs_pos.before.split(" ")[0]
}


let board = new Board({
    //id: "board",
})
board.print()
// let a = board.moves()
// measure_count_bulk_positions(4)

// const make_random_move = (board) => {
//     let moves = board.moves()
//     board.move(moves[Math.floor(Math.random() * moves.length)])
// }

// setInterval(() => {
//     make_random_move(b)
// }, 1000)


// import { Chess } from "../lib/chess.js"
// let chessjs_board = new Chess("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -")
// let board = new Board("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R")
// board.print()
// // console.log(chessjs_board.moves({ verbose: true }))
// //console.log(chessjs_board.moves({verbose: true})[0])

// // // console.log(get_chess_pos_to_index_table())
// let woodstock_moves = get_all_moves_at(board, 2)
// let chessjs_moves = get_all_moves_verbose_at(chessjs_board, 2)

// console.log("Found " + woodstock_moves.length + " woodstock moves")
// console.log("Found " + chessjs_moves.length + " actual moves.")

// // get the bad positions
// let unmatched_positions = []
// let encountered = new Set([])

// // for (let i = 0; i < 5; i++) {
// //     board.move(woodstock_moves[i])
// //     console.log(woodstock_moves[i])
// //     board.print()
// //     board.undo()
// // }

// for (let i = 0; i < woodstock_moves.length; i++) {
//     let has_matched = false
//     let pos = woodstock_moves[i]

//     // if (encountered.has(pos)) {
//     //     console.log("Found duplicate move! Adding to unmatched and skipping...")
//     //     unmatched_positions.push(pos)
//     //     continue
//     // }
//     // encountered.add(pos)

//     for (let j = 0; j < chessjs_moves.length; j++) {
//         if (compare_chessjs_to_woodstock_position(pos, chessjs_moves[j])) {
//             if (has_matched) {
//                 console.log("duplicate found!")
//                 break
//             } else {
//                 has_matched = true
//                 break
//             }
//         }
//     }

//     if (!has_matched) unmatched_positions.push(pos)
// }

// console.log(unmatched_positions.length == 0 ? "All positions verified!" : ("There were " + unmatched_positions.length + " incorrect positions."))
// if (unmatched_positions.length != 0) console.log(unmatched_positions)