/*
    Why TCP? - 
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 3490

void printBoard(char board[3][3]) {
    printf("\nCurrent Board:\n");
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            printf("%c ", board[i][j]);
        }
        printf("\n");
    }
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[1024];
    char board[3][3];
    int making_move = 1;
    int valid_move = 0;

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        exit(1);
    }

    while (1) {
        // Receive the current board from the server and print
        recv(sockfd, board, sizeof(board), 0);
        printBoard(board);

        recv(sockfd, buffer, strlen(buffer), 0);
        printf("%s\n", buffer);

        while (making_move) {
            // Get move from user
            printf("Enter your move (row and column): ");
            fgets(buffer, sizeof(buffer), stdin);

            // Send the move to the server
            send(sockfd, buffer, strlen(buffer), 0);

            recv(sockfd, buffer, strlen(buffer), 0);
            sscanf(buffer, "%d", &valid_move);

            if (valid_move == 1) {
                making_move = 0;
            }
            else {
                printf("Invalid Move\n");
            }

        }

        // Check for game over conditions
        if (strstr(buffer, "wins") != NULL || strstr(buffer, "draw") != NULL) {
            close(sockfd);
            return 0;
        }

        making_move = 1;
    }

    close(sockfd);
    return 0;
}