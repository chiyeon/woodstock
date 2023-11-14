const set_board = (b) => {
    board.set_board(b)
    board.update_chessboard()
 }

const firebaseConfig = {
    apiKey: "AIzaSyDGcF7ZloMazDNkorPIZL1Equ1OvymJ6Mw",
    authDomain: "woodstock-chess.firebaseapp.com",
    projectId: "woodstock-chess",
    storageBucket: "woodstock-chess.appspot.com",
    messagingSenderId: "323843313184",
    appId: "1:323843313184:web:2ba35d9b63c2512f5ff645"
};
const app = firebase.initializeApp(firebaseConfig);
const db = firebase.firestore();
var id = Math.random().toString(24).slice(2)

 const hide_element = (selector) => {
    document.querySelector(selector).classList.add("hidden")
 }

 const show_element = (selector) => {
    document.querySelector(selector).classList.remove("hidden")
 }

 var make_ai_move;
 var update_pgn;
 var set_status;

 // button functions for game-related actions
 var game_undo
    , game_switch
    , game_flip
    , game_reset
    , game_export
    , game_load

 // button functions for exporting
 var pgn_generate, pgn_generate_and_export
 var pgn = ""

 var change_theme = (e) => {
    board.theme = e.value;
    board.update_chessboard();
 }

var change_color, change_board_color

var choose_promotion;

 let board = new Chessboard("board");

 let status_el = document.querySelector("#status");
 let status_box_el = document.querySelector("#status-box");
 let pgn_el = document.querySelector("#pgn");

 let upload_after = false; // upload after we're done getting pgn

 document.addEventListener("DOMContentLoaded", async () => {
    const woodstock = await WoodstockModule();

    choose_promotion = (piece) => {
      woodstock.ccall("choose_promotion", null, ["number"], [piece]);
       hide_element("#promotions")
    }

    change_color = (e) => { woodstock.ccall("set_player_color", null, ["boolean"], [e.value == "black"]) }

    change_board_color = (e) => { board.set_color(e.value == "black") }

    game_undo = () => { woodstock.ccall("undo", null, null, null) }
    game_switch = () => { woodstock.ccall("switch_sides", null, null, null) }
    game_flip = () => { board.flip() }
    game_reset = () => { woodstock.ccall("reset_game", null, null, null) }
    game_export = () => {
       pgn = "";
       pgn_el.textContent = "Your PGN will be written here!"
       show_element("#game-export")
    }
    game_load = () => {
       let fen_str = document.querySelector("#fen-input").value;
       let buffer = woodstock._malloc(fen_str.length + 1);
       woodstock.stringToUTF8(fen_str, buffer, fen_str.length + 1);
       woodstock.ccall("load_fen", null, [ "number" ], [ buffer ]);
       woodstock._free(buffer);
       hide_element("#game-options")
    }

    // call to WASM to generate PGN
    // will callback update_pgn when finished
    call_set_pgn = () => {
       let name_str = document.querySelector("#game-export-name").value;
       let buffer = woodstock._malloc(name_str.length + 1);

       woodstock.stringToUTF8(name_str, buffer, name_str.length + 1);
       woodstock.ccall("set_pgn", null, [], [buffer]);
       woodstock._free(buffer);
    }

    // generates pgn only
    pgn_generate = () => {
       if (pgn == "") {
          pgn_el.textContent = "Loading PGN...";
          setTimeout(() => {
             call_set_pgn();
          }, 10);
       }
    }

    // calls to generate pgn & activates flag for exporting
    pgn_generate_and_export = () => {
       if (pgn == "") {
          pgn_el.textContent = "Loading PGN...";
          setTimeout(() => { call_set_pgn() }, 10);
          upload_after = true;
       } else {
          upload_pgn()
       }
    }

    // callback, run from WASM after pgn is generated
    update_pgn = (ptr, len) => {
       pgn = woodstock.UTF8ToString(ptr, len);
       console.log(pgn);
       pgn_el.textContent = pgn;
       
       if (upload_after) {
          upload_pgn();
       }
    }

    upload_pgn = () => {
       upload_after = false
      
      db.collection("games").doc(id).set({
         pgn: pgn
      })

       //refresh our id
      id = Math.random().toString(24).slice(2)

       status_el.innerHTML = "Upload submitted. Thank You!"
       show_element("#status-box")
    }

    set_status = (s) => {
       switch(s) {
          case 0:
             status_el.innerHTML = "Draw.";
             document.querySelector(".king.black").classList.add("draw");
             document.querySelector(".king.white").classList.add("draw");
             status_box_el.classList.remove("hidden");
             break;
          case 1:
             status_el.innerHTML = "White Wins!";
             document.querySelector(".king.black").classList.add("checkmated");
             status_box_el.classList.remove("hidden");
             break;
          case 2:
             status_el.innerHTML = "Black Wins!";
             document.querySelector(".king.white").classList.add("checkmated");
             status_box_el.classList.remove("hidden");
             break;
          case 4:
             document.querySelector(".king.white").classList.add("checked");
             break;
          case 5:
             document.querySelector(".king.black").classList.add("checked");
             break;
          default:
             status_el.innerHTML = "";
             status_box_el.classList.add("hidden");
             break;
       }
    }

    make_ai_move = () => {
       setTimeout(() => {
          woodstock.ccall(
          "make_best_move",
          null,
          null,
          null,
       );
       }, 10)
    }

    for (let i = 0; i < 64; i++) {
       let square = document.getElementById(`sq-${i}`)
       square.addEventListener("click", () => {
          woodstock.ccall(
             "click_square",
             null,
             [ "number" ],
             [ board.flipped ? 63 - i : i ]
          )
       })
    }
 });
