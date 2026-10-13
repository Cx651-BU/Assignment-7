#include "game.h"
#include <stdlib.h>
#include <stddef.h>

/*
Places a piece in the lowest empty cell of the chosen column.
Returns 0 on success, or -1 for an invalid move.
*/
int make_move(struct game* g, int player, int col) {
    if (g == NULL || player < 0 || player > 1 ||
        col < 0 || col >= BOARD_COLS) {
        return -1;
    }

    if (player != g->current_player) {
        return -1;
    }

    for (int row = BOARD_ROWS - 1; row >= 0; row--) {
        if (g->board.cells[row][col] == EMPTY) {
            g->board.cells[row][col] = player;
            g->current_player = 1 - player;
            return 0;
        }
    }

    return -1;  // The column is full.
}
 
/*
Returns the winning player (0 or 1), or -1 if there is no winner.
*/
int check_win(struct game* g) {
    if (g == NULL) {
        return -1;
    }

    // Horizontal, vertical, and the two diagonal directions.
    const int directions[4][2] = {
        {0, 1},
        {1, 0},
        {1, 1},
        {1, -1}
    };

    for (int row = 0; row < BOARD_ROWS; row++) {
        for (int col = 0; col < BOARD_COLS; col++) {
            int player = g->board.cells[row][col];

            if (player == EMPTY) {
                continue;
            }

            for (int d = 0; d < 4; d++) {
                int dr = directions[d][0];
                int dc = directions[d][1];

                int end_row = row + 3 * dr;
                int end_col = col + 3 * dc;

                // All four cells must be inside the board.
                if (end_row < 0 || end_row >= BOARD_ROWS ||
                    end_col < 0 || end_col >= BOARD_COLS) {
                    continue;
                }

                int count;
                for (count = 1; count < 4; count++) {
                    if (g->board.cells[row + count * dr]
                                      [col + count * dc] != player) {
                        break;
                    }
                }

                if (count == 4) {
                    return player;
                }
            }
        }
    }

    return -1;
}
 
/*
Initializes an existing game with an empty board.
Player 0 takes the first turn.
*/
void new_game(struct game* g, int gameId) {
    g->gameId = gameId;
    g->current_player = 0;

    for (int row = 0; row < BOARD_ROWS; row++) {
        for (int col = 0; col < BOARD_COLS; col++) {
            g->board.cells[row][col] = EMPTY;
        }
    }
}

