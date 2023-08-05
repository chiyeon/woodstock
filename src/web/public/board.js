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
      this.load_fen_string("rnbkqbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBKQBNR"); // quirk of how engien does indexes means this string is actually reversed!
      this.theme = "john_pablok_improved"
      this.flipped = false;
      this.create_chessboard(id);
      this.selected_piece = -1;
   }

   static piece_to_img = {
      [Piece.EMPTY]: "",
      [Piece.BLACK | Piece.PAWN]: "bP.png",
      [Piece.BLACK | Piece.ROOK]: "bR.png",
      [Piece.BLACK | Piece.KNIGHT]: "bN.png",
      [Piece.BLACK | Piece.BISHOP]: "bB.png",
      [Piece.BLACK | Piece.QUEEN]: "bQ.png",
      [Piece.BLACK | Piece.KING]: "bK.png",
      [Piece.WHITE | Piece.PAWN]: "wP.png",
      [Piece.WHITE | Piece.ROOK]: "wR.png",
      [Piece.WHITE | Piece.KNIGHT]: "wN.png",
      [Piece.WHITE | Piece.BISHOP]: "wB.png",
      [Piece.WHITE | Piece.QUEEN]: "wQ.png",
      [Piece.WHITE | Piece.KING]: "wK.png",
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

   get_piece_image = (piece) => {
      return "imgs/chesspieces/" + this.theme + "/" + Chessboard.piece_to_img[piece];
   }

   set_board = (board) => {
      this.board = board;
   }

   empty_board = () => {
      let out = []
      for (let i = 0; i < 64; i++) {
         out.push(0);
      }

      return out;
   }

   load_fen_string = (fen) => {
      let lines = fen.split("/")
      this.board = []

      for (let i = lines.length - 1; i >= 0; i--) {
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

      for (let y = 7; y >= 0; y--) {
         let row_el = document.createElement("div")
         row_el.classList.add("row")
         for (let x = 7; x >= 0; x--) {
            let square = document.createElement("div")
            let index = y * 8 + x
            square.classList.add("square")
            square.classList.add((x + y % 2) % 2 == 1 ? "black" : "white")
            square.id = `sq-${index}`
            row_el.appendChild(square)

            // check for pieces
            if (this.board[index] != Piece.EMPTY) {
               let piece = document.createElement("div")
               piece.style = "background-image: url(" + this.get_piece_image(this.board[index]) + ")";
               piece.classList.add("piece")
               square.appendChild(piece)
            }
         }
         root.appendChild(row_el)
      }
   }

   update_chessboard = () => {
      for (let y = 7; y >= 0; y--) {
         for (let x = 7; x >= 0; x--) {
            let index = y * 8 + x
            let square = document.getElementById(`sq-${this.flipped ? 63 - index : index}`)

            // first animate it in, then pop it into existence
            if (this.board[index] != Piece.EMPTY) {
               if (square.innerHTML == "") {
                  let piece = document.createElement("div")
                  piece.style = "background-image: url(" + this.get_piece_image(this.board[index]) + ")";
                  piece.classList.add("piece")
                  square.appendChild(piece)
               } else {
                  square.childNodes[0].style = "background-image: url(" + this.get_piece_image(this.board[index]) + ")"
               }
            } else {
               square.innerHTML = ""
            }
         }
      }
   }

   flip = () => {
      this.flipped = !this.flipped;
      this.update_chessboard()

      // flip highlighted squares
      let new_highlights = []
      for (let i = 0; i < 64; i++) {
         let el = document.getElementById(`sq-${i}`)
         if (el.className.includes("last-move")) {
            el.classList.remove("last-move");
            new_highlights.push(i);
         }
      }

      for (let i = 0; i < new_highlights.length; i++) {
         document.getElementById(`sq-${63 - new_highlights[i]}`).classList.add("last-move")
      }

      // flip potential move tiles if we have them
      let new_move_highlights = []
      for (let i = 0; i < 64; i++) {
         let el = document.getElementById(`sq-${i}`)
         if (el.className.includes("possible-move")) {
            el.classList.remove("possible-move");
            new_move_highlights.push(i);
         }
      }

      for (let i = 0; i < new_move_highlights.length; i++) {
         document.getElementById(`sq-${63 - new_move_highlights[i]}`).classList.add("possible-move")
      }
   }

   select_piece = (piece) => {
      this.selected_piece = this.flipped ? 63 - piece : piece;
   }

   remove_highlight_squares = () => {
      let prev_squares = document.querySelectorAll(".last-move")
      for (let i = 0; i < prev_squares.length; i++) {
         prev_squares[i].classList.remove("last-move")
      }
   }

   highlight_squares = (square1, square2) => {
      let prev_squares = document.querySelectorAll(".last-move")
      for (let i = 0; i < prev_squares.length; i++) {
         prev_squares[i].classList.remove("last-move")
      }

      let squares = [square1, square2]

      for (let i = 0; i < squares.length; i++) {
         let square = document.getElementById(`sq-${this.flipped ? 63 - squares[i] : squares[i]}`)
         square.classList.add("last-move")
      }
   }

   remove_move_highlights = () => {
      let prev_squares = document.querySelectorAll(".possible-move")
      for (let i = 0; i < prev_squares.length; i++) {
         prev_squares[i].classList.remove("possible-move");
      }
      this.selected_piece = -1;
   }

   // puts dot on one particular square
   highlight_move = (square) => {
      document.getElementById(`sq-${this.flipped ? 63 - square : square}`).classList.add("possible-move");
   }
}