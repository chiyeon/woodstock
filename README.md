> THIS BRANCH IS NO LONGER IN USE.

# About
Woodstock-js is a chess engine / ai opponent that runs in the browser. This current version runs entirely in Javascript in the browser.

In vanilla javascript, woodstock-js implements popular chess engine techniques such as Bitboards in an overall messy and incomplete attempt of an engine. While initial progress was promising, this attempt was abandoned in favor of creating an engine that compiles to web assembly.

# Features
### Chess Engine
- Use of Bit Boards for move calculation
- Interpret and produce fen strings
- Simulate and undo moves

### AI
- Alpha Beta pruning for move decisions
- Small opening book for the first few moves, switches to minimaxing when leaving openings
- Move ordering using a MVV LVA table
- Evaluates using Piece-Square tables, mobility, bishop ownership
- Searches to a depth of 3

# Representation
### Board
Woodstock represents the board in a single array, indexed from 0 to 64. The pieces align with chess positions in the following fashion:

```console
8 |  0    1    2    3    4    5    6    7    
  |
7 |  8    9   10   11   12   13   14   15    
  |
6 | 16   17   18   19   20   21   22   23    
  |
5 | 24   25   26   27   28   29   30   31    
  |
4 | 32   33   34   35   36   37   38   39    
  |
3 | 40   41   42   43   44   45   46   47    
  |
2 | 48   49   50   51   52   53   54   55    
  |
1 | 56   57   58   59   60   61   62   63    
  |---------------------------------------
     a    b    c    d    e    f    g    h 
```

### Pieces
Pieces can be represented as 5 bit numbers, with the 2 most significant bits controlling the color of the piece and the remaining 3 bits denoting the type of piece. For instance:

| Piece | Value |
| ----- | ----- |
| Empty Space | 000 |
| Pawn | 001 |
| Knight | 010 |
| Bishop | 011 |
| Rook | 100 |
| Queen | 101 |
| King | 110 |

| Color | Value |
| ----- | ----- |
| White | 01000 |
| Black | 10000 |

Pieces can then be creating using an OR (`|`) operation between a color and a piece, for instance a black pawn would be `PAWN | BLACK`. We can also use the AND (`&`) operation to quickly read a particular piece's color and type. 
