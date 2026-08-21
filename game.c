struct game {
    int* board;
    int turn;
};

/*
Takes as input a pointer to a connect4 game board, a player (0/1), and a int column.
Places a new piece in `col` on the game board.
*/
int make_move(struct game*, int player, int col) {

}

/*
Takes as input a pointer to a connect4 game board.
Returns 0 or 1 if player 0 or 1 has won respectively.
Returns -1 if there is no winner.
*/
int check_win(struct game*) {

}

/*
Allocates a new game board.
Returns a pointer to the newly allocated board.
*/
struct game* new_game() {
    struct game* g = malloc(sizeof(struct game));
    g->board = calloc(sizeof(int)*7*6);
    for(int i = 0; i < 7*6; i++) {
        g->board[i] = -1;
    }
    g->turn = 0;
}