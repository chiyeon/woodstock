> THIS BRANCH IS NO LONGER IN USE

You can still play against this rudimentary AI at https://chiyeon.github.io

# Woodstock with Chess.js
This branch contains the first ever draft of the Woodstock chess ai, implemented with the `chess.js` engine and `chessboard.js` visuals. With third party scripts controlling chess legality & moves as well as the user interface, this branch focuses entirely on the AI opponent.

### Features
- Alpha/Beta pruning
- Move ordering with MVV/LVA
- An opening book of just over 3000 popular openings
- Playable as black or white
- Evaluation with mobility and piece square tables
- Maximum search depth of 3 (Moves generally 1 second or less)
