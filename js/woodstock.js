const   PAWN = "p",
        KNIGHT = "n",
        BISHOP = "b",
        ROOK = "r",
        QUEEN = "q",
        KING = "k",
        WHITE = "w",
        BLACK = "b"

const piece_values = {
    [PAWN]: 100,
    [KNIGHT]: 300,
    [BISHOP]: 315,
    [ROOK]: 450,
    [QUEEN]: 950,
    [KING]: 10000
}

let positions_evaluated = 0

var reverse_array = (array) => {
    return array.slice().reverse();
};

const white_position_table = {
    [PAWN]: [
        [0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0],
        [5.0,  5.0,  5.0,  5.0,  5.0,  5.0,  5.0,  5.0],
        [1.0,  1.0,  2.0,  3.0,  3.0,  2.0,  1.0,  1.0],
        [0.5,  0.5,  1.0,  2.5,  2.5,  1.0,  0.5,  0.5],
        [0.0,  0.0,  0.0,  2.0,  2.0,  0.0,  0.0,  0.0],
        [0.5, -0.5, -1.0,  0.0,  0.0, -1.0, -0.5,  0.5],
        [0.5,  1.0, 1.0,  -2.0, -2.0,  1.0,  1.0,  0.5],
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
    let score = piece_values[piece.type] + piece.color == "w" ? white_position_table[piece.type][y][x] : black_position_table[piece.type][y][x]
    
    return piece.color == "w" ? score : -score;
}

// evaluates the position where positive = white favor 
const evaluate_position = (board) => {
    let white_score = 0
    let black_score = 0

    for (let x = 0; x < 8; x++) {
        for (let y = 0; y < 8; y++) {
            let piece = board[x][y]

            if (!piece) continue

            if (piece.color == "w") {
                white_score = white_score
                            + piece_values[piece.type]
                            + white_position_table[piece.type][y][x]
            } else {
                black_score = black_score
                            + piece_values[piece.type]
                            + black_position_table[piece.type][y][x]
            }

        }
    }
    
    return white_score - black_score
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

        let eval = alphabeta(game, start_depth - 1, -10000, 10000, maximizing_player)

        game.undo()

        if (eval >= best_move_eval) {
            best_move = move
            best_move_eval = eval
        }
    }

    return best_move
}

const alphabeta = (game, depth, alpha, beta, maximizing_player) => {
    positions_evaluated++

    if (depth == 0) return (maximizing_player ? 1 : -1) * evaluate_position(game.board())

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

const perform_best_move = async (game, player_color) => {

    let start_time = performance.now()
    let best_move = determine_best_move(3, game, player_color == "b")
    
    let time_elapsed = performance.now() - start_time

    $(".stats #time").text("Time Elapsed: " + time_elapsed / 1000 + " seconds")
    $(".stats #positions").text("Positions Evaluated: " + positions_evaluated)
    positions_evaluated = 0

    if (best_move) game.move(best_move)
}