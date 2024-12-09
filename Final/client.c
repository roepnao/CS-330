/*
    Why TCP? - Reliable connection is more desirable for a game
               where speed is not important such as tic tac toe, 
               it's more important to ensure player's moves are
               received and sent correctly rather than as fast as possible.

    Sources - https://beej.us/guide/bgnet/html/split/client-server-background.html
              https://www.geeksforgeeks.org/tcp-server-client-implementation-in-c/
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 3490

void printBoard(char board[3][3]) {
    printf("\nCurrent Board:\n\n");
    printf(" %c | %c | %c\n", board[0][0], board[0][1], board[0][2]);
    printf("---|---|---\n");
    printf(" %c | %c | %c\n", board[1][0], board[1][1], board[1][2]);
    printf("---|---|---\n");
    printf(" %c | %c | %c\n\n", board[2][0], board[2][1], board[2][2]);
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[1024];
    char board[3][3];
    int checking = 1;

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

    system("clear");

    printf("Please wait for your turn.\n");

    while (1) {
        
        // Receive the current board from the server
        recv(sockfd, (char*)board, sizeof(board), 0);

        system("clear");

        printBoard(board);

        memset(buffer, 0, sizeof(buffer));

        // Receive prompt for the player's turn or game ended
        recv(sockfd, buffer, sizeof(buffer), 0);
        if (strstr(buffer, "wins") || strstr(buffer, "draw")) {
            printf("%s", buffer);
            break;
        }
        printf("%s", buffer);

        memset(buffer, 0, sizeof(buffer));

        // Enter the move
        printf("Enter your move (row col): ");
        fgets(buffer, sizeof(buffer), stdin);
        send(sockfd, buffer, strlen(buffer), 0);

        while (checking) {
            memset(buffer, 0, sizeof(buffer));

            recv(sockfd, buffer, sizeof(buffer), 0);
            if (strstr(buffer, "Invalid")) {
                system("clear");
                printBoard(board);

                printf("%s", buffer);

                memset(buffer, 0, sizeof(buffer));

                printf("Enter your move (row col): ");
                fgets(buffer, sizeof(buffer), stdin);
                send(sockfd, buffer, strlen(buffer), 0);
            }
            else {
                checking = 0;
            }
        }

        memset(buffer, 0, sizeof(buffer));

        system("clear");
        
        recv(sockfd, (char*)board, sizeof(board), 0);
        printBoard(board);

        // Check if the game ended
        recv(sockfd, buffer, sizeof(buffer), 0);
        if (strstr(buffer, "wins") || strstr(buffer, "draw")) {
            printf("%s", buffer);
            break;
        }
        else {
            printf("%s", buffer);
        }

        checking = 1;

        memset(buffer, 0, sizeof(buffer));
    }

    close(sockfd);
    return 0;
}