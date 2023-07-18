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

   // static names = {
   //     [Piece.PAWN]: "pawn",
   //     [Piece.KNIGHT]: "knight",
   //     [Piece.BISHOP]: "bishop",
   //     [Piece.ROOK]: "rook",
   //     [Piece.QUEEN]: "queen",
   //     [Piece.KING]: "king",
   //     [Piece.EMPTY]: ""
   // }

   // static names_short = {
   //     [Piece.PAWN]: "p",
   //     [Piece.KNIGHT]: "n",
   //     [Piece.BISHOP]: "b",
   //     [Piece.ROOK]: "r",
   //     [Piece.QUEEN]: "q",
   //     [Piece.KING]: "k",
   //     [Piece.EMPTY]: ""
   // }

   // static colors = {
   //     [Piece.WHITE]: "white",
   //     [Piece.BLACK]: "black",
   //     [Piece.EMPTY]: ""
   // }

   // static colors_short = {
   //     [Piece.WHITE]: "w",
   //     [Piece.BLACK]: "b",
   //     [Piece.EMPTY]: ""
   // }

   // static get_color = (piece) => {
   //     return piece & Piece.FILTER_COLOR
   // }

   // static get_type = (piece) => {
   //     return piece & Piece.FILTER_PIECE
   // }

   // static get_info = (piece) => {
   //     return Piece.colors[Piece.get_color(piece)] + " " + Piece.names[Piece.get_type(piece)]
   // }

   // static get_info_short = (piece) => {
   //     let short_name = Piece.names_short[Piece.get_type(piece)]
   //     if ((piece & Piece.FILTER_COLOR) == Piece.WHITE) short_name = short_name.toUpperCase()
   //     return short_name
   // }
}

class Chessboard {
   constructor(id) {
      this.load_fen_string("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
      this.create_chessboard(id);
      this.board = []
   }

   static piece_to_img = {
      [Piece.EMPTY]: "",
      [Piece.BLACK | Piece.PAWN]: "imgs/chesspieces/wikipedia/bP.png",
      [Piece.BLACK | Piece.ROOK]: "imgs/chesspieces/wikipedia/bR.png",
      [Piece.BLACK | Piece.KNIGHT]: "imgs/chesspieces/wikipedia/bN.png",
      [Piece.BLACK | Piece.BISHOP]: "imgs/chesspieces/wikipedia/bB.png",
      [Piece.BLACK | Piece.QUEEN]: "imgs/chesspieces/wikipedia/bQ.png",
      [Piece.BLACK | Piece.KING]: "imgs/chesspieces/wikipedia/bK.png",
      [Piece.WHITE | Piece.PAWN]: "imgs/chesspieces/wikipedia/wP.png",
      [Piece.WHITE | Piece.ROOK]: "imgs/chesspieces/wikipedia/wR.png",
      [Piece.WHITE | Piece.KNIGHT]: "imgs/chesspieces/wikipedia/wN.png",
      [Piece.WHITE | Piece.BISHOP]: "imgs/chesspieces/wikipedia/wB.png",
      [Piece.WHITE | Piece.QUEEN]: "imgs/chesspieces/wikipedia/wQ.png",
      [Piece.WHITE | Piece.KING]: "imgs/chesspieces/wikipedia/wK.png",
   }

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

   set_board = (board) => {
      let diffs = []
      for (let i = 0; i < 64; i++) {
         if (this.board[i] != board[i]) diffs.push(i);
      }

      this.board = board;
      return diffs.length > 2 ? [] : diffs;
   }

   load_fen_string = (fen) => {
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
               let piece = Chessboard.fen_dictionary[ch]
               this.board.push(piece)
               // otherwise skip as many spaces
            } else {
               for (let k = 0; k < space_count; k++) {
                  this.board.push(Piece.EMPTY)
               }
            }
         }
      }
   }

   create_chessboard = (id) => {
      let root = document.getElementById(id)
      root.classList.add("board")

      for (let y = 0; y < 8; y++) {
         let row_el = document.createElement("div")
         row_el.classList.add("row")
         for (let x = 0; x < 8; x++) {
            let square = document.createElement("div")
            let index = y * 8 + x
            square.classList.add("square")
            square.classList.add((x + y % 2) % 2 == 1 ? "black" : "white")
            square.id = `sq-${index}`
            row_el.appendChild(square)

            // check for pieces
            if (this.board[index] != Piece.EMPTY) {
               let piece = document.createElement("img")
               piece.src = Chessboard.piece_to_img[this.board[index]]
               square.appendChild(piece)
            }
         }
         root.appendChild(row_el)
      }
   }

   update_chessboard = (diffs) => {
      for (let y = 0; y < 8; y++) {
         for (let x = 0; x < 8; x++) {
               let index = y * 8 + x
               let square = document.getElementById(`sq-${index}`)

               // first animate it in, then pop it into existence
               if (this.board[index] != Piece.EMPTY) {
                  if (square.innerHTML == "") {
                     let piece = document.createElement("img")
                     piece.src = Chessboard.piece_to_img[this.board[index]]
                     square.appendChild(piece)
                  } else {
                     square.childNodes[0].src = Chessboard.piece_to_img[this.board[index]]
                  }
               } else {
                  square.innerHTML = ""
               }
         }
      }

      this.highlight_squares(diffs);
   }

   highlight_squares = (squares) => {
      let prev_squares = document.querySelectorAll(".last-move")
      for (let i = 0; i < prev_squares.length; i++) {
         prev_squares[i].classList.remove("last-move")
      }

      for (let i = 0; i < squares.length; i++) {
         let square = document.getElementById(`sq-${squares[i]}`)
         square.classList.add("last-move")
      }
   }

   make_move = (from, to, flag) => {
      
   }
}