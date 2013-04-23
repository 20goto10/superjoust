; Ben Chadwick
; Genetic Algorithm
;
; The purpose of this program is to implement a genetic learning
; TIC TAC TOE game.  The goal is for both players to always tie,
; since any smart Tic-Tac-Toe players will always tie.
; Perhaps you are wondering how this will work? Badly.  No.  Just kidding.
; What will happen is, each player will have a code of what move to make next.
; If that spot is already taken, it will skip to the next one and so forth.
; In the case that all the moves in it's code have already taken place, it will
; append a new move which will be a random spot.  If the random spot is taken
; (enough is enough) it will increment the position until it finds the first
; open spot, and that will be the new position.
; Once the game loses (it plays against itself) or wins (is the glass half-empty
; or half-full?) it will restart using the revised code.
; The goal is for neither player to win, but for both to tie, which in fact
; sort of disputes Darwin, but hey... if survival of the fittest was always
; the case then there wouldn't be any of us democrats left...
; The human overlord (myself) is not involved, but can watch the progress.
; ...and it works!

(defun set_place (places PLAYER) ; set a spot on the board to PLAYER ('X or 'O)
  (SETQ board
            (REPNTH (cadr places)
                 (REPNTH (car places) PLAYER (GETNTH (cadr places) BOARD))
                 BOARD ) )
)

(defun spot (row COL) ; return what's at (ROW COL) (one of 'C, 'X, or 'O)
  (getnth row (getnth col board))
)

(defun repnth (N ELT LST) ; replace the Nth unit of LST with ELT, by Steve Tan.
 (COND ((EQUAL N 1)(CONS ELT (CDR LST)))
   (T (CONS (CAR LST)
            (REPNTH (SUB1 N) ELT (CDR LST)) )) ) )

(DEFUN SHOW () ; display the whole board (uses show_row and print_place)
 (prog ()
 (tyo 197)(tyo 196)(tyo 197)(tyo 196)(tyo 197)(tyo 196)(tyo 197)(terpri)
 (MAPCAR 'SHOW_ROW BOARD)
 )
)

(DEFUN SHOW_ROW (ROW) ; display one row of the board
  (PROG () (tyo 179) (MAPCAR 'PRINT_PLACE ROW) (Terpri)
          (tyo 197)(tyo 196)(tyo 197)(tyo 196)(tyo 197)(tyo 196)(tyo 197)(terpri))
)

(DEFUN PRINT_PLACE (PLACE) ; Simple function to print one "blank" or X or O
  (COND ((EQ PLACE 'C)(TYO 32)(TYO 179))
        (T (PRIN1 PLACE)(TYO 179)) )
)

(defun pspot(row col player) ; Does (row col) belong to "player" ? T/Nil
  (eq (spot row col) player)
)

(defun winner(player) ; Checks all possible winning combos for this player
  (setq playerwin     ; It may not be small code, but it's efficient & works
  (cond ((and (pspot 1 1 player) (pspot 1 2 player) (pspot 1 3 player))
         player)
        ((and (pspot 2 1 player) (pspot 2 2 player) (pspot 2 3 player))
         player)
        ((and (pspot 3 1 player) (pspot 3 2 player) (pspot 3 3 player))
         player)
        ((and (pspot 1 1 player) (pspot 2 1 player) (pspot 3 1 player))
         player)
        ((and (pspot 1 2 player) (pspot 2 2 player) (pspot 3 2 player))
         player)
        ((and (pspot 1 3 player) (pspot 2 3 player) (pspot 3 3 player))
         player)
        ((and (pspot 1 1 player) (pspot 2 2 player) (pspot 3 3 player))
         player)
        ((and (pspot 3 1 player) (pspot 2 2 player) (pspot 1 3 player))
         player)
    )
  )
)

(defun rand(num1 num2) ; random number between num1 and num2, inclusive
  (prog ()
    (setq randseed (times 4 (times randseed (difference 1 randseed))))
    (setq temp (round (difference(plus num1 (times (difference (add1 num2) num1) randseed) ) 0.49)))
    (cond ((greaterp temp num2) (setq temp (sub1 temp))))
    (return temp)
  )
)

(defun find_next_open(oldc oldr col row) ;Finds next open tic-tac-toe square
 (prog ()
  (setq col (add1 col)) ; Skip off of the current square
  (cond ((greaterp col 3) (setq row (add1 row)) (setq col 1))) ; increase row
  (cond ((greaterp row 3) (setq col 1)(setq row 1))) ; if past lower right,
                                                     ; start in upper left
  (cond ((eq (spot col row) 'C) (setq targle (list col row)))
        ((and (num_eq oldc col) (num_eq oldr row))(setq targle (list col row)))
        (T (find_next_open oldc oldr col row))  )
  (return targle)
 )
)

(defun nthcdr(num strig) ; returns all but the first num elements of a list
 (prog nil
  (cond ((num_eq num 0)(setq strog strig))
        (T (nthcdr (sub1 num) (cdr strig))))
  (return strog)
 )
)

(defun firstelements(num strig) ; returns the first num elements of a list
                                ; before calling this, strog is set to nil
                                ; does not check for overflow (caller's caller
                                ; does that, thus no need here)
 (prog nil
  (cond ((eq (eq strig nil) nil)   ; don't know how to "NOT" with this compiler
                                   ; checks to see if it's not empty
  (cond ((greaterp num 0)(setq strog (append (list (getnth num strig)) strog))
         (firstelements (sub1 num) strig))) )
  )
  (return strog)
 )
)

(defun firsts(num strig) ; this calls firstelements with strog reset to Nil
  (prog nil
    (setq strog Nil)
    (return (firstelements num strig))
  )
)

(defun movb() ; move player B
  (prog ()
  (cond ((lessp (length playerb) (getnth 2 moveptrs))
          (setq playerb
          (append playerb (list (list (rand 1 3) (rand 1 3)))))))
          ;^ If the player is out of moves in his code, add a new move at random
  (setq curmove (getnth (getnth 2 moveptrs) playerb)) ; get the current move
  (cond ((eq (spot (car curmove) (cadr curmove)) 'C) (set_place curmove 'O))
         (T (set_place (find_next_open (car curmove) (cadr curmove)
                                       (car curmove) (cadr curmove)) 'O) )
         ; If the spot is taken, advance to the next open one.
  )
  (setq moveptrs (repnth 2 (add1 (getnth 2 moveptrs)) moveptrs)) ; adjust ptrs
  )
)

(defun mova() ; move player A (see B)
  (prog ()
  (cond ((lessp (length playera) (getnth 1 moveptrs)) (setq playera (append playera (list (list (rand 1 3) (rand 1 3))) ))))
  (setq curmove (getnth (getnth 1 moveptrs) playera))
  (cond ((eq (spot (car curmove) (cadr curmove)) 'C)
        (set_place curmove 'X) )
        (T (set_place (find_next_open (car curmove) (cadr curmove)
                                      (car curmove) (cadr curmove)) 'X) )
  )
  (setq moveptrs (repnth 1 (add1 (getnth 1 moveptrs)) moveptrs))
  )
)

(defun gamecycle ()
 (prog ()

   (print (append strng3 (list gamecounter))) ; print the current turn #
   (cond ( (or (num_eq gamecounter 1)   (num_eq gamecounter 3)
               (num_eq gamecounter 5)   (num_eq gamecounter 7)
               (num_eq gamecounter 9))  (movb))
          (T (mova)))  ; pick either A or B (isn't there an "ODD" function?)
   (cond ((eq (winner 'x) 'x) (printm Player A won...)) ;winner check
         ((eq (winner 'o) 'o) (printm Player B won...)))
   (setq gamecounter (add1 gamecounter)) ; increase turn #
   (show)
   (cond ((and (eq playerwin Nil) (lessp gamecounter 10)) (gamecycle)))
          ; keep going until the board is full or somebody won
  )
)

(defun reset()
  (prog ()
   (setq playerwin 0)

   (SETQ MOVEPTRS '(1 1))  ; player A = first move, player B = first move
   (SETQ ROWS '(C C C))    ; clear a row
   (SETQ BOARD (LIST ROWS ROWS ROWS)) ; clear three columns
   (setq gamecounter 1)    ; initialize the turn counter
  )
)

(defun fixstuff()                      ; cross over the two strands of "DNA" at
 (prog ()                              ; a random spot called "switchpoint"
  (reset)
  (setq maxnum (min (length playera) (length playerb))) ; Don't pick a num too high
  (setq switchpoint (rand 1 (sub1 maxnum))) ; pick switching spot
                                            ; (sub1 maxnum) insures a change
  (setq temp playerb)
  (setq olda playera)
  (setq playerb (append (firsts switchpoint temp) (nthcdr switchpoint olda)))
  (setq playera (append (firsts switchpoint olda) (nthcdr switchpoint temp)))
 )
)

(defun randomize() ; initialize random seed
 (setq randseed (quotient (car (sys_time)) 10000.0))
)

(DEFUN MAINGY ()
  (prog nil

   (reset)  ; set what needs to be set
   (print (append strng (list roundcounter)))
   (gamecycle)
   (cond ((eq playerwin nil)  ; If nobody won...
            (print (append strng2 (list roundcounter)))) ; ...then we're done
         (T (setq roundcounter (add1 roundcounter)) ; otherwise inc. round #
            (fixstuff)                              ; adjust DNA
            (maingy)                                ; and then re-run
         )
   )
 )
)

(defun main ()
 (prog ()
   (SETQ roundCOUNTER 1) ; chief initialization
   (SETQ PLAYERB '( (1 1) (1 2) (1 3))) ; this ensures there is a winner at 1st
   (SETQ PLAYERA '( (2 2) (3 2) (3 1))) ; because player A is so stupid
   (randomize)                          ; initialize random seed
   (setq strng '(ROUND NUMBER:))        ; nifty little string thing
   (setq strng2 '(It worked on round:)) ; another nifty little string thing
   (setq strng3 '(Turn number:))        ; Yet another...
   (maingy)                             ; Now start the slaughter!
 )
)

(defun start () ; sometimes I type (start) instead of (main)
  (main)        ; this exists simply as an alias so I don't get irritated.
)               ; It may sound cheesy, but I don't care, I'm not Steve Tanimoto.

