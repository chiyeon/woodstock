const measure = (func) => {
    let start_time = performance.now()
    func()
    let time = performance.now() - start_time
    console.log("Took " + time + "ms | " + time / 1000 + "s")
}

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

class BitBoard {
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
        this.piece_positions = []

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
            this.piece_positions[this.board[move.to]] = move.to
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
     */
    
    // returns 2 bitboards: moves, captures
    get_pawn_moves = (x, y, index, piece, board_bitboard, black_bitboard, white_bitboard) => {
        let pos_bitboard = BitBoard.get(x, y)
        let piece_color = piece & Piece.FILTER_COLOR
        //let piece_type = piece & Piece.FILTER_PIECE
        //let moves = []
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
            let col = pawn_collisions[i]
            let col_x = col % 8
            let col_y = Math.floor(col / 8)
            // check for occupied space + 1 backwards, stops pawns from jumping in home row
            let mask = piece_color == Piece.BLACK
                ? BitBoard.get_column_segment(x, col_y, col_y + 1)
                : BitBoard.get_column_segment(x, col_y - 1, col_y)
            pawn_moves = pawn_moves & ~mask
        }

        // check for capturing diagonally
        let pawn_captures = ((piece_color == Piece.BLACK) ? pos_bitboard >> 9n | pos_bitboard >> 7n : pos_bitboard << 9n | pos_bitboard << 7n)
        let pawn_capture_collisions = pawn_captures & board_bitboard

        return [pawn_moves, pawn_capture_collisions]
    }

    get_knight_moves = (x, y, index, piece, board_bitboard, black_bitboard, white_bitboard) => {
        //let pos_bitboard = BitBoard.get(x, y)
        let piece_color = piece & Piece.FILTER_COLOR
        //let piece_type = piece & Piece.FILTER_PIECE
        //let moves = []
        // calcluate all possible spots
        let knight_starting_pos = BitBoard.get(x, y)
        let knight_moves = 0n

        if (y + 2 < 8 && x + 1 < 8) knight_moves = knight_moves | knight_starting_pos >> 17n
        if (y + 2 < 8 && x - 1 >= 0) knight_moves = knight_moves | knight_starting_pos >> 15n
        if (y + 1 < 8 && x + 2 < 8) knight_moves = knight_moves | knight_starting_pos >> 10n
        if (y + 1 < 8 && x - 2 >= 0) knight_moves = knight_moves | knight_starting_pos >> 6n
        if (y - 2 >= 0 && x + 1 < 8) knight_moves = knight_moves | knight_starting_pos << 15n
        if (y - 2 >= 0 && x - 1 >= 0) knight_moves = knight_moves | knight_starting_pos << 17n
        if (y - 1 >= 0 && x + 2 < 8) knight_moves = knight_moves | knight_starting_pos << 6n
        if (y - 1 >= 0 && x - 2 >= 0) knight_moves = knight_moves | knight_starting_pos << 10n

        return knight_moves
    }

    get_bishop_moves = (x, y, index, piece, board_bitboard, black_bitboard, white_bitboard) => {
        //let pos_bitboard = BitBoard.get(x, y)
        let piece_color = piece & Piece.FILTER_COLOR
        //let piece_type = piece & Piece.FILTER_PIECE
        //let moves = []

        let bishop_moves = this.move_masks.bishop_masks[index]

        // get pieces that are in line with our axis of movement
        let bishop_collisions = BitBoard.get_positions_list(bishop_moves & board_bitboard)

        for (let i = 0; i < bishop_collisions.length; i++) {
            // for each collision, remove excess spaces in our mask
            let col = bishop_collisions[i]
            let col_x = col % 8
            let col_y = Math.floor(col / 8)
            let col_index = col_x + col_y * 8
            let mask = 0n

            // lower right
            if (col_y > y && col_x > x) mask = BitBoard.get_diagonal_downwards_right(col_x, col_y)
            // lower left
            else if (col_y > y && col_x < x) mask = BitBoard.get_diagonal_downwards_left(col_x, col_y)
            // upper right
            else if (col_y < y && col_x > x) mask = BitBoard.get_diagonal_upwards_right(col_x, col_y)
            // uper left
            else if (col_y < y && col_x < x) mask = BitBoard.get_diagonal_upwards_left(col_x, col_y)

            // ugh !!!
            // remove the spot from the mask... lets us mark it as able to go there
            //mask = mask ^ BitBoard.get(col_x, col_y)

            bishop_moves = bishop_moves & (~mask)
        }

        return bishop_moves
    }

    get_rook_moves = (x, y, index, piece, board_bitboard, black_bitboard, white_bitboard) => {
        //let pos_bitboard = BitBoard.get(x, y)
        let piece_color = piece & Piece.FILTER_COLOR
        //let piece_type = piece & Piece.FILTER_PIECE
        let moves = []

        let rook_moves = this.move_masks.rook_masks[index]

        // get pieces that are in line with our axis of movement
        let collision_pos = BitBoard.get_positions_list(rook_moves & board_bitboard)

        for (let i = 0; i < collision_pos.length; i++) {
            // for each collision, remove excess spaces in our mask
            let col = collision_pos[i]
            let col_x = col % 8
            let col_y = Math.floor(col / 8)
            let col_index = col_x + col_y * 8
            let mask = 0n

            if (col_y > y) mask = BitBoard.get_column_segment(x, col_y + 1, 7)
            else if (col_y < y) mask = BitBoard.get_column_segment(x, 0, col_y - 1)
            else if (col_x > x) mask = BitBoard.get_row_segment(y, col_x + 1, 7)
            else if (col_x < x) mask = BitBoard.get_row_segment(y, 0, col_x - 1)

            rook_moves = rook_moves & (~mask)
        }

        return rook_moves
    }

    get_queen_moves = (x, y, index, piece, board_bitboard, black_bitboard, white_bitboard) => {
        return this.get_rook_moves(x, y, index, piece, board_bitboard, black_bitboard, white_bitboard)
            | this.get_bishop_moves(x, y, index, piece, board_bitboard, black_bitboard, white_bitboard)
    }

    get_king_moves = (x, y, index, piece, board_bitboard, black_bitboard, white_bitboard) => {
        //let pos_bitboard = BitBoard.get(x, y)
        let piece_color = piece & Piece.FILTER_COLOR
        //let piece_type = piece & Piece.FILTER_PIECE
        let moves = []

        let king_start = BitBoard.get(x, y)
        let king_moves = king_start >> 8n
            | king_start << 8n
            | king_start >> 9n
            | king_start << 9n
            | king_start >> 7n
            | king_start << 7n
            | king_start << 1n
            | king_start >> 1n
        
        return king_moves
    }

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
                        piece: this.board[i],
                    }

                    let move_bitboard
                    let non_capture_bitboard // only for pawns
                    switch(piece & Piece.FILTER_PIECE) {
                        case Piece.PAWN: 
                            // because pawns dont have the same captures/moves, we separate them
                            // .move_bitboard is now ONLY CAPTURES
                            // .non_capture_bitboard is ONLY MOVES
                            let pawn_moves = this.get_pawn_moves(x, y, i, piece, board_bitboard, black_bitboard, white_bitboard)
                            non_capture_bitboard = pawn_moves[0]
                            move_bitboard = pawn_moves[1]
                            break
                        case Piece.KNIGHT: move_bitboard = this.get_knight_moves(x, y, i, piece, board_bitboard, black_bitboard, white_bitboard) 
                            break
                        case Piece.BISHOP: move_bitboard = this.get_bishop_moves(x, y, i, piece, board_bitboard, black_bitboard, white_bitboard) 
                            break
                        case Piece.ROOK: move_bitboard = this.get_rook_moves(x, y, i, piece, board_bitboard, black_bitboard, white_bitboard) 
                            break
                        case Piece.QUEEN: move_bitboard = this.get_queen_moves(x, y, i, piece, board_bitboard, black_bitboard, white_bitboard) 
                            break
                        case Piece.KING: move_bitboard = this.get_king_moves(x, y, i, piece, board_bitboard, black_bitboard, white_bitboard) 
                            break
                    }

                    data.move_bitboard = move_bitboard
                    if ((piece & Piece.FILTER_PIECE) == Piece.PAWN) {
                        data.non_capture_bitboard = non_capture_bitboard
                    }

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
        let king_pos_bitboard = BitBoard.get_i(this.piece_positions[this.turn | Piece.KING] || 0n)
        let in_check = false
        let blocker_spaces = 0n

        // we must also capture squares that are able to be "blocked"
        
        // calculate attacked squares from axis pieces
        for (let i = 0; i < axis_pieces.length; i++) {
            let axis_piece_data = axis_pieces[i]
            attacked_squares = attacked_squares | axis_piece_data.move_bitboard

            // there is at least ONE person attacking the king
            if (king_pos_bitboard & axis_piece_data.move_bitboard) {
                king_attackers.push({
                    piece: axis_piece_data.piece,
                    index: axis_piece_data.pos
                })
                blocker_spaces = blocker_spaces | axis_piece_data.move_bitboard
                in_check = true
            }
        }

        //console.log(in_check ? "King is in check!" : "King is not in check.")

        // now we need to fix the kings' moves to prevent them from walking into attacked squares

        for (let i = 0; i < ally_pieces.length; i++) {
            let piece = ally_pieces[i]

            if ((piece.piece & Piece.FILTER_PIECE) == Piece.KING) {
                piece.move_bitboard = piece.move_bitboard & ~attacked_squares
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
            let all_moves_bitboard = piece_data.move_bitboard
            // if a pawn, also remember to record our captures as well
            if ((piece_data.piece & Piece.FILTER_PIECE) == Piece.PAWN) {
                all_moves_bitboard = all_moves_bitboard | piece_data.non_capture_bitboard
            }

            // filter our moves: remove invalid (capturing our own pieces) moves
            all_moves_bitboard = all_moves_bitboard & ~((this.turn == Piece.BLACK) ? black_bitboard : white_bitboard)

            // if we are in check, only consider moves that will block checks or kill lone attackers for non king pieces
            if (in_check) {
                // for kings consider only captures and evades
                if ((piece_data.piece & Piece.FILTER_PIECE) == Piece.KING) {
                    all_moves_bitboard = all_moves_bitboard | (all_moves_bitboard & king_attackers_bitboard)
                } else {
                    // for other pieces, consider only blocks and captures. again this is only possible if we have 1 attacker
                    if (king_attackers.length <= 1) {
                        all_moves_bitboard = (all_moves_bitboard & blocker_spaces) 
                        | (all_moves_bitboard & king_attackers_bitboard)
                    } else {
                        all_moves_bitboard = 0n
                    }
                }
            }

            if (all_moves_bitboard) {
                // mask positions: we can only go where our pieces are NOT
                let positions = BitBoard.get_positions_list(all_moves_bitboard)
                for (let j = 0; j < positions.length; j++) {
                    moves.push(this.create_move(piece_data.pos, positions[j]))
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

let board = new Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR")
// board.print()
// board.moves()
// console.log(board.moves())
//measure_count_bulk_positions(4)

// for (let i = 1; i <= 5; i++) {
//     measure_count_bulk_positions(i)
// }

