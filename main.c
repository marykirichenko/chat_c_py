#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAX_CLIENTS 3
#define BUFFER_SIZE 1024
#define PORT 8080

int clients[MAX_CLIENTS] = {0};
pthread_t threads[MAX_CLIENTS];
int server_socket;

void *handle_client(void *arg) {
    int client_socket = *(int*) arg;
    char buffer[BUFFER_SIZE] = {0};
    while (1) {
        int read_size = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (read_size < 0) {
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i] == client_socket) {
                    clients[i] = 0;
                    printf("Client disconnected: %d\n", client_socket);
                    break;
                }
            }
            pthread_exit(NULL);
        }
        printf("message :%s\n", buffer);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i] != 0 && clients[i] != client_socket) {
                int write_size = send(clients[i], buffer, strlen(buffer), 0);
                if (write_size < 0) {
                    printf("Error sending message to client %d\n", clients[i]);
                }
            }
        }
        memset(buffer, 0, BUFFER_SIZE);
    }
}

int main() {
    struct sockaddr_in server_address;
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        printf("Error creating socket\n");
        return 1;
    }
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address)) < 0) {
        printf("Error binding socket\n");
        return 1;
    }
    if (listen(server_socket, MAX_CLIENTS) < 0) {
        printf("Error listening on socket\n");
        return 1;
    }
    printf("Server listening on %d\n", PORT);
    while (1) {
        int new_socket = accept(server_socket, NULL, NULL);
        if (new_socket < 0) {
            printf("Error accepting client\n");
            continue;
        }
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i] == 0) {
                clients[i] = new_socket;
                pthread_create(&threads[i], NULL, handle_client, (void*) &new_socket);
                printf("New client connected \n");
                break;
            }
        }
    }
    return 0;
}
