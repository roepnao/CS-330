/*
    Why TCP? - 
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 3490

// Function to print the Tic Tac Toe board
void printBoard(char board[3][3]) {
    printf("\nCurrent Board:\n");
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            printf("%c ", board[i][j]);
        }
        printf("\n");
    }
}

// Function to check if a player has won
int checkWinner(char board[3][3]) {
    // Check rows and columns
    for (int i = 0; i < 3; i++) {
        if ((board[i][0] == board[i][1] && board[i][1] == board[i][2] && board[i][0] != ' ') ||
            (board[0][i] == board[1][i] && board[1][i] == board[2][i] && board[0][i] != ' ')) {
            return 1;  // Player wins
        }
    }
    // Check diagonals
    if ((board[0][0] == board[1][1] && board[1][1] == board[2][2] && board[0][0] != ' ') ||
        (board[0][2] == board[1][1] && board[1][1] == board[2][0] && board[0][2] != ' ')) {
        return 1;  // Player wins
    }
    return 0;
}

// Function to check if the game ended in a draw
int checkDraw(char board[3][3]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == ' ') {
                return 0;  // There is an empty spot, game isn't a draw
            }
        }
    }
    return 1;  // No empty spots left, it's a draw
}

int main() {
    int sockfd, new_fd[2];
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    char buffer[1024];
    char board[3][3] = {{' ', ' ', ' '}, {' ', ' ', ' '}, {' ', ' ', ' '}};
    int player_turn = 0;  // 0 for player X, 1 for player O
    int row, col;
    char player = 'X';
    int has_lost = 0;
    int is_draw = 0;
    int checking = 1;

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(PORT);

    // Bind socket to address
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(1);
    }

    // Listen for connections
    if (listen(sockfd, 2) < 0) {
        perror("Listen failed");
        exit(1);
    }

    printf("Server is waiting for connections...\n");

    // Accept two clients
    addr_size = sizeof(client_addr);
    for (int i = 0; i < 2; i++) {
        new_fd[i] = accept(sockfd, (struct sockaddr *)&client_addr, &addr_size);
        if (new_fd[i] < 0) {
            perror("Accept failed");
            exit(1);
        }
        printf("Client %d connected\n", i + 1);
    }

    // Game loop
    while (1) {
        // send the board
        send(new_fd[player_turn], board, sizeof(board), 0);

        memset(buffer, 0, sizeof(buffer));

        if (has_lost) {
            snprintf(buffer, sizeof(buffer), "Player %c wins :(\n", (player == 'X') ? 'O' : 'X');
            send(new_fd[player_turn], buffer, strlen(buffer), 0);
            break;
        }
        else if (is_draw) {
            snprintf(buffer, sizeof(buffer), "It's a draw!\n");
            send(new_fd[player_turn], buffer, strlen(buffer), 0);
            break;
        }

        snprintf(buffer, sizeof(buffer), "Player %c, it's your turn.\n", player);
        send(new_fd[player_turn], buffer, strlen(buffer), 0);

        recv(new_fd[player_turn], buffer, sizeof(buffer), 0);
        sscanf(buffer, "%d %d", &row, &col);

        memset(buffer, 0, sizeof(buffer));

        while (checking) {
            memset(buffer, 0, sizeof(buffer));
            if (row < 0 || row >= 3 || col < 0 || col >= 3 || board[row][col] != ' ') {
                snprintf(buffer, sizeof(buffer), "Invalid move, try again.\n");
                send(new_fd[player_turn], buffer, strlen(buffer), 0);

                memset(buffer, 0, sizeof(buffer));

                recv(new_fd[player_turn], buffer, sizeof(buffer), 0);
                sscanf(buffer, "%d %d", &row, &col);
            }
            else {
                snprintf(buffer, sizeof(buffer), " ");
                send(new_fd[player_turn], buffer, strlen(buffer), 0);
                checking = 0;
            }
        }

        memset(buffer, 0, sizeof(buffer));

        system("clear");
        
        // Make the move
        board[row][col] = player;
        printBoard(board);

        memset(buffer, 0, sizeof(buffer));
        send(new_fd[player_turn], board, sizeof(board), 0);

        // Check if the game has a winner
        if (checkWinner(board)) {
            snprintf(buffer, sizeof(buffer), "Player %c wins!\n", player);
            send(new_fd[player_turn], buffer, strlen(buffer), 0);
            has_lost = 1;
        }
        // Check if it's a draw
        else if (checkDraw(board)) {
            snprintf(buffer, sizeof(buffer), "It's a draw!\n");
            send(new_fd[player_turn], buffer, strlen(buffer), 0);
            is_draw = 1;
        }
        else {
            snprintf(buffer, sizeof(buffer), "Please wait for your turn.\n");
            send(new_fd[player_turn], buffer, strlen(buffer), 0);
        }

        // Switch player turn
        player = (player == 'X') ? 'O' : 'X';
        player_turn = (player_turn + 1) % 2;
        checking = 1;

        memset(buffer, 0, sizeof(buffer));
    }

    close(new_fd[0]);
    close(new_fd[1]);
    close(sockfd);

    return 0;
}