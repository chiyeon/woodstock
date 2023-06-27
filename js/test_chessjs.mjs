import { Chess } from "chess.js"

var game = new Chess("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -")

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

for (let i = 1; i <= 4; i++) {
    measure_count_bulk_positions(i)
}
