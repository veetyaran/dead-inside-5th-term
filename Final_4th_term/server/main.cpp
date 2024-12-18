#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define BUFFER_SIZE 8192
#define MAX_MESSAGES 100

typedef struct {
    char name[100];
    char message[1000];
} Message;

Message messages[MAX_MESSAGES];
int message_count = 0;
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

void *handle_client(void *client_socket);
void send_response(int client_socket, const char *header, const char *body);
void decode_url(char *src, char *dest);

int main() {
    int server_fd, client_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Создаем сокет
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Настраиваем сокет
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Привязываем сокет к порту
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Слушаем подключения
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d\n", PORT);

    while (1) {
        // Принимаем подключение клиента
        if ((client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("accept");
            continue;
        }

        // Создаем новый поток для обработки клиента
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, handle_client, (void *)(intptr_t)client_socket);
        pthread_detach(thread_id);
    }

    return 0;
}

void *handle_client(void *client_socket) {
    int sock = (intptr_t)client_socket;
    char buffer[BUFFER_SIZE] = {0};
    read(sock, buffer, BUFFER_SIZE);

    if (strstr(buffer, "POST /submit") != NULL) {
        // Обрабатываем POST-запрос
        char *body = strstr(buffer, "\r\n\r\n");
        if (body) {
            body += 4;
            char name[100] = "Anonymous";
            char message[1000] = "";

            char *name_pos = strstr(body, "name=");
            char *message_pos = strstr(body, "message=");
            if (name_pos) {
                name_pos += 5;
                sscanf(name_pos, "%99[^&]", name);
            }
            if (message_pos) {
                message_pos += 8;
                sscanf(message_pos, "%999[^&]", message);
            }

            char decoded_name[100];
            char decoded_message[1000];
            decode_url(name, decoded_name);
            decode_url(message, decoded_message);

            pthread_mutex_lock(&mtx);
            if (message_count < MAX_MESSAGES) {
                strncpy(messages[message_count].name, decoded_name, sizeof(messages[message_count].name));
                strncpy(messages[message_count].message, decoded_message, sizeof(messages[message_count].message));
                message_count++;
                printf("New message from %s: %s\n", decoded_name, decoded_message);
            }
            pthread_mutex_unlock(&mtx);

            send_response(sock, "HTTP/1.1 303 See Other\r\nLocation: /\r\n", "");
        }
    } else {
        // Обрабатываем GET-запрос
        pthread_mutex_lock(&mtx);
        char body[BUFFER_SIZE] = "<html><body><h1>Message Board</h1>";
        strcat(body, "<form action=\"/submit\" method=\"post\">Name: <input type=\"text\" name=\"name\"><br>Message: <textarea name=\"message\"></textarea><br><input type=\"submit\" value=\"Submit\"></form><h2>Messages:</h2>");
        for (int i = 0; i < message_count; i++) {
            strcat(body, "<p><strong>");
            strcat(body, messages[i].name);
            strcat(body, ":</strong> ");
            strcat(body, messages[i].message);
            strcat(body, "</p>");
        }
        strcat(body, "</body></html>");
        pthread_mutex_unlock(&mtx);

        send_response(sock, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n", body);
    }

    close(sock);
    return NULL;
}

void send_response(int client_socket, const char *header, const char *body) {
    char response[BUFFER_SIZE];
    snprintf(response, sizeof(response), "%sContent-Length: %zu\r\n\r\n%s", header, strlen(body), body);
    write(client_socket, response, strlen(response));
}

void decode_url(char *src, char *dest) {
    char *p = src;
    char code[3] = {0};
    while (*p) {
        if (*p == '%') {
            strncpy(code, p + 1, 2);
            *dest++ = (char)strtol(code, NULL, 16);
            p += 3;
        } else if (*p == '+') {
            *dest++ = ' ';
            p++;
        } else {
            *dest++ = *p++;
        }
    }
    *dest = '\0';
}