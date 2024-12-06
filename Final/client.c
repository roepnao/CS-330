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
        printf("\nwaiting for board\n");
        // Receive the current board from the server
        recv(sockfd, (char*)board, sizeof(board), 0);
        printBoard(board);  // Display the current board

        memset(buffer, 0, sizeof(buffer));

        

        printf("\nwaiting for player turn\n");
        // Receive prompt for the player's turn
        recv(sockfd, buffer, sizeof(buffer), 0);
        if (strstr(buffer, "wins") || strstr(buffer, "draw")) {
            printf("%s", buffer);
            break;
        }
        printf("%s", buffer);

        memset(buffer, 0, sizeof(buffer));

        printf("\nentering move\n");
        // Enter the move
        printf("Enter your move (row col): ");
        fgets(buffer, sizeof(buffer), stdin);
        send(sockfd, buffer, strlen(buffer), 0);

        memset(buffer, 0, sizeof(buffer));

        printf("\nchecking for ending\n");
        // Check if the game ended
        recv(sockfd, buffer, sizeof(buffer), 0);
        if (strstr(buffer, "wins") || strstr(buffer, "draw")) {
            printf("%s", buffer);
            break;
        }

        memset(buffer, 0, sizeof(buffer));
    }

    close(sockfd);
    return 0;
}