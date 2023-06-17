import { Chess } from "../lib/chess.js"

var game = new Chess()
var board = null

const set_highlight_square = (square, is_highlighted) => {
    let el = $(`#board .square-${square}`)
    is_highlighted ? el.addClass("highlighted") : el.removeClass("highlighted")
}

const remove_highlighted_squares = () => {
    $("#board .square-55d63").removeClass("highlighted")
}

const highlight_possible_moves = (square, piece) => {
    if (piece[0] == "b") return;

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
    if (game.get(source).color == "b") return console.log("attempted to move non-player picee!")

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
        make_cheez_move(board, game)
        board.position(game.fen())
    }, 100)
}

board = Chessboard("board", {
    draggable: true,
    position: "start",
    onMouseoutSquare: remove_highlighted_squares,
    onMouseoverSquare: highlight_possible_moves,
    onSnapEnd: () => {
        board.position(game.fen())
    },
    onDrop: attempt_move_piece
})