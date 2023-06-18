import { Chess } from "../lib/chess.js"

var game = new Chess()
var board = null
var board_flipped = false

// 'w' or 'b'
var player_color = "w"

const set_highlight_square = (square, is_highlighted) => {
    let el = $(`#board .square-${square}`)
    is_highlighted ? el.addClass("highlighted") : el.removeClass("highlighted")
}

const remove_highlighted_squares = () => {
    $("#board .square-55d63").removeClass("highlighted")
}

const highlight_possible_moves = (square, piece) => {
    if (piece[0] != player_color) return;

    let moves = game.moves({
        square: square,
        verbose: true
    })

    if (moves.length == 0) return

    moves.forEach(move => {
        set_highlight_square(move.to, true)
    })
}

const attempt_move_piece = (source, target) => {
    if (game.get(source).color != player_color) return console.log("attempted to move non-player picee!")

    remove_highlighted_squares()

    try {
        game.move({
            from: source,
            to: target,
            promotion: "q"
        })
    } catch {
        return "snapback"
    }
    show_evaluated_position()

    if (game.isGameOver()) return player_win()

    window.setTimeout(async () => {
        await perform_best_move(game, player_color)
        board.position(game.fen())

        show_evaluated_position()
        if (game.isGameOver()) return computer_win()
    }, 200)
}

const player_win = () => {
    $("#winner").text("You won!").removeClass("hidden")
}

const computer_win = () => {
    $("#winner").text("Woodstock won!").removeClass("hidden")
}

board = Chessboard("board", {
    draggable: true,
    position: "start",
    onMouseoutSquare: remove_highlighted_squares,
    onMouseoverSquare: highlight_possible_moves,
    onDragStart: (source, piece, destination, orientation) => {
        return piece[0] == player_color
    },
    onSnapEnd: () => {
        board.position(game.fen())
        show_evaluated_position()
    },
    onDrop: attempt_move_piece
})

const set_board = async () => {
    
    game.reset()
    board.position(game.fen())

    if (player_color == "b") {
        if (!board_flipped) {
            board.flip()
            board_flipped = !board_flipped
        }

        await perform_random_opening_move(game)
        board.position(game.fen())
        show_evaluated_position()
    }
}

const toggle_player_color = () => {
    if (player_color == "b") {
        board.flip()
        board_flipped = !board_flipped
    }

    player_color = (player_color == "w" ? "b" : "w")

    set_board()
}

const undo_move = () => {
    game.undo()
    if (game.turn() != player_color) game.undo()
    board.position(game.fen())
    show_evaluated_position()
}

var turn = "b"

const show_evaluated_position = () => {
    let score = evaluate_position(game, game.moves({ raw: true }))
    $("#evaluation").text(score + "(+white, -black)")

    // if (!game.isGameOver()) {
    //     window.setTimeout(async () => {
    //         await perform_best_move(game, turn)
    //         turn = (turn == "w" ? "b" : "w")
    //         board.position(game.fen())
    //         show_evaluated_position()
    //     }, 200)
    // }
}

$("#undo-button").on("click", () => { undo_move() })
$("#swap-button").on("click", () => { toggle_player_color() })
$("#random-button").on("click", () => { if (Math.random() < 0.5) toggle_player_color() })
$("#reset-button").on("click", () => { set_board() })

set_board()
show_evaluated_position()
//board.flip()