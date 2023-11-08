# Alpha 0.7 
 - Added Reset button
 - Added Export button 
    - Games can now be exported to PGN format
 - Options menu wraps properly based on resolution
 - "AI Move" option changed to "Switch"
 - More accurate checkmate/endgame calculations (at cost of performance for this patch)
 - Fixed some GUI bugs
 - Prioritizes "faster" checkmates, making Woodstock better at closing games:
    - Previously woodstock would sometimes "stall" mate in ones if it kept seeing another checkmate one more move in the future.
 - Added rudimentary ending-game notifier (announces checkmate/draw) 

# Alpha 0.3
 - Continues play even if defeat is inevitable
 - Actively pursues checkmates in all cases
 - Searches up to 6 moves ahead (up from 5)
 - Acknowledges and respects draws (repetition & no moves left)
 - More accurate search evaluation
