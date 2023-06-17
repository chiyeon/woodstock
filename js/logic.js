import { Chess } from "../lib/chess.js"

var game = new Chess()
var board = null

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

    window.setTimeout(() => {
        make_cheez_move(game, player_color)
        board.position(game.fen())
    }, 200)
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
    },
    onDrop: attempt_move_piece
})

const set_board = () => {
    
    game.reset()
    board.position(game.fen())

    if (player_color == "b") {
        make_cheez_move(game, player_color)
        board.position(game.fen())
        board.flip()
    }
}

const toggle_player_color = () => {
    if (player_color == "b") {
        board.flip()
    }

    player_color = (player_color == "w" ? "b" : "w")

    set_board()
}

const undo_move = () => {
    game.undo()
    if (game.turn() != player_color) game.undo()
    board.position(game.fen())
}

$("#undo-button").on("click", () => { undo_move() })
$("#swap-button").on("click", () => { toggle_player_color() })
$("#reset-button").on("click", () => { set_board() })

set_board()