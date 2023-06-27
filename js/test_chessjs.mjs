import { Chess } from "chess.js"

var game = new Chess()

const count_bulk_positions = (depth) => {
    if (depth <= 0) return 1

    let moves = game.moves()
    let count = 0
    
    for (let i = 0; i < moves.length; i++) {
        game.move(moves[i])
        count += count_bulk_positions(depth - 1)
        game.undo()
    }

    return count
}

const measure_count_bulk_positions = (depth) => {
    let start_time = performance.now()
    let count = count_bulk_positions(depth)
    console.log("Depth: " + depth + "\tNumber of positions: " + count + "\tTime: " + (performance.now() - start_time) + "ms")
}

measure_count_bulk_positions(1)
measure_count_bulk_positions(2)
measure_count_bulk_positions(3)
measure_count_bulk_positions(4)