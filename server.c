#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "game.h"
#include "save.h"

#define SAVE_FILE "connect4.save"
#define PORT 8080

static int same_client(const struct sockaddr_in* a,
                       const struct sockaddr_in* b) {
    return a->sin_family == b->sin_family &&
           a->sin_addr.s_addr == b->sin_addr.s_addr &&
           a->sin_port == b->sin_port;
}

static int board_full(const struct game* g) {
    for (int col = 0; col < BOARD_COLS; col++) {
        if (g->board.cells[0][col] == EMPTY) {
            return 0;
        }
    }
    return 1;
}

static void send_response(int sock_fd,
                          const struct sockaddr_in* client,
                          const struct game* g,
                          int player, int status, int winner) {
    struct response reply = {0};

    reply.status = status;
    reply.player = player;
    reply.winner = winner;
    reply.game = *g;

    if (sendto(sock_fd, &reply, sizeof(reply), 0,
               (const struct sockaddr*)client,
               sizeof(*client)) == -1) {
        perror("sendto");
    }
}

int main(int argc, char* argv[]) {
    
    struct gameDB db = {0};
    
    if (argc > 1 && strcmp(argv[1], "1") == 0) {
        if (loadgame(&db, SAVE_FILE) == -1) {
            perror("loadgame");
            return EXIT_FAILURE;
        }
    }

    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd == -1) {
        perror("socket");
        return EXIT_FAILURE;
    }

    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(sock_fd, (struct sockaddr*)&server_addr,
             sizeof(server_addr)) == -1) {
        perror("bind");
        close(sock_fd);
        return EXIT_FAILURE;
    }

    printf("Connect Four server listening on port %d\n", PORT);

    // Used when rejecting a request for a nonexistent game.
    struct game invalid_game;
    new_game(&invalid_game, -1);

    while (1) {
        struct request request = {0};
        struct sockaddr_in client_addr = {0};
        socklen_t client_len = sizeof(client_addr);

        ssize_t received = recvfrom(
            sock_fd, &request, sizeof(request), 0,
            (struct sockaddr*)&client_addr, &client_len
        );

        if (received == -1) {
            if (errno == EINTR) {
                continue;
            }
            perror("recvfrom");
            break;
        }

        if (received != (ssize_t)sizeof(request)) {
            continue;
        }

        if (request.type == REQUEST_CREATE) {
            if (db.game_count >= MAX_GAMES) {
                send_response(sock_fd, &client_addr, &invalid_game,
                              -1, STATUS_ERROR, -1);
                continue;
            }

            struct server_game* selected = &db.games[db.game_count];

            new_game(&selected->game, db.game_count);
            selected->players[0] = client_addr;
            selected->player_count = 1;
            selected->finished = 0;
            db.game_count++;

            if (savegame(&db, SAVE_FILE) == -1) {
               perror("savegame");
            }

            send_response(sock_fd, &client_addr, &selected->game,
                          0, STATUS_WAITING, -1);
            continue;
        }

        if (request.gameId < 0 || request.gameId >= db.game_count) {
            send_response(sock_fd, &client_addr, &invalid_game,
                          -1, STATUS_ERROR, -1);
            continue;
        }

        struct server_game* selected = &db.games[request.gameId];

        if (request.type == REQUEST_JOIN &&
            selected->player_count == 1 &&
            !selected->finished &&
            !same_client(&client_addr, &selected->players[0])) {

            selected->players[1] = client_addr;
            selected->player_count = 2;

            if (savegame(&db, SAVE_FILE) == -1) {
                perror("savegame");
            }

            for (int player = 0; player < 2; player++) {
                send_response(sock_fd, &selected->players[player],
                              &selected->game,
                              player, STATUS_ACTIVE, -1);
            }
            continue;
        }

        if (request.type == REQUEST_MOVE &&
            selected->player_count == 2 &&
            !selected->finished &&
            request.player >= 0 && request.player <= 1 &&
            same_client(&client_addr,
                        &selected->players[request.player])) {

            if (make_move(&selected->game,
                          request.player, request.col) == 0) {
                int winner = check_win(&selected->game);
                int status = STATUS_ACTIVE;

                if (winner != -1) {
                    status = STATUS_WIN;
                    selected->finished = 1;
                } else if (board_full(&selected->game)) {
                    status = STATUS_TIE;
                    selected->finished = 1;
                }

                if (savegame(&db, SAVE_FILE) == -1) {
                    perror("savegame");
                }

                for (int player = 0; player < 2; player++) {
                    send_response(sock_fd, &selected->players[player],
                                  &selected->game,
                                  player, status, winner);
                }
                continue;
            }
        }

        int player = -1;
        for (int i = 0; i < selected->player_count; i++) {
            if (same_client(&client_addr, &selected->players[i])) {
                player = i;
                break;
            }
        }

        send_response(sock_fd, &client_addr, &selected->game,
                      player, STATUS_ERROR, -1);
    }

    close(sock_fd);
    return EXIT_FAILURE;
}