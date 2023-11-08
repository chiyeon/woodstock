# Alpha 0.7 
Overall, v0.7 of Woodstock brings foundational changes important for future steps (Fully working transposition table & iterative deepening) as well as fixes to essential systems. This update updates the visuals of the game, making the interface more usable and communicating checks and end of game states better.
 - More accurate checkmate/draw evaluations 
 - Prioritizes "faster" checkmates, making Woodstock better at closing games:
    - Previously woodstock would sometimes "stall" mate in ones if it kept seeing another checkmate one more move in the future.
 - Zobrist hashing is now incremental, vastly increasing performance
 - Fixed bugs where checks/checkmates were calculated incorrectly
 - Fixed bugs where promotions wouldn't act as intended
 - Added Reset button
 - Added Export button 
    - Games can now be exported to PGN format
 - Options menu wraps properly based on resolution
 - "AI Move" option changed to "Switch"
 - Fixed some GUI bugs
 - Added Check notification
 - Added Checkmate and Draw notification as well as windows that alert the closing of a game

# Alpha 0.3
 - Continues play even if defeat is inevitable
 - Actively pursues checkmates in all cases
 - Searches up to 6 moves ahead (up from 5)
 - Acknowledges and respects draws (repetition & no moves left)
 - More accurate search evaluation
