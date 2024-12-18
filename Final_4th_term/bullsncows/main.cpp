#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>
#include <ctype.h>
#include <utility> // Для std::pair
#include <algorithm> // Для std::random_shuffle

#define PORT 18080
#define BUFFER_SIZE 8192
#define MAX_GAMES 100
#define SECRET_LENGTH 4

typedef struct {
    char session_id[100];
    char secret_number[SECRET_LENGTH + 1];
    char attempts[100][SECRET_LENGTH + 1];
    int attempt_count;
} Game;

Game games[MAX_GAMES];
int game_count = 0;
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

void *handle_client(void *client_socket);
void send_response(int client_socket, const char *header, const char *body);
void generate_secret_number(char *secret);
std::pair<int, int> calculate_bulls_and_cows(const char *secret, const char *guess);

int main() {
    srand(time(NULL));

    int server_fd, client_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Создаем сокет для работы с сетью
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Настраиваем сокет для многократного использования порта
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Задаем параметры адреса сервера
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Привязываем сокет к указанному порту
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Переводим сервер в режим прослушивания
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d\n", PORT);

    while (1) {
        // Принимаем входящее подключение от клиента
        if ((client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("accept");
            continue;
        }

        // Создаем новый поток для обработки клиента
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, handle_client, (void *)(intptr_t)client_socket);
        pthread_detach(thread_id); // Отсоединяем поток, чтобы не ждать его завершения
    }

    return 0;
}

void *handle_client(void *client_socket) {
    int sock = (intptr_t)client_socket;
    char buffer[BUFFER_SIZE] = {0};
    read(sock, buffer, BUFFER_SIZE); // Читаем данные от клиента

    if (strstr(buffer, "GET /start") != NULL) {
        // Старт новой игры
        pthread_mutex_lock(&mtx);
        if (game_count < MAX_GAMES) {
            char session_id[100];
            snprintf(session_id, sizeof(session_id), "%d", rand());
            char secret_number[SECRET_LENGTH + 1];
            generate_secret_number(secret_number);      
            strncpy(games[game_count].session_id, session_id, sizeof(games[game_count].session_id));
            strncpy(games[game_count].secret_number, secret_number, sizeof(games[game_count].secret_number));
            games[game_count].attempt_count = 0;
            game_count++;

            char body[BUFFER_SIZE];
            snprintf(body, sizeof(body), "{\"session_id\": \"%s\"}", session_id);
            send_response(sock, "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n", body);
        } else {
            send_response(sock, "HTTP/1.1 503 Service Unavailable\r\n", "{\"error\": \"Maximum number of games reached. Try again later.\"}");
        }
        pthread_mutex_unlock(&mtx);
    } else if (strstr(buffer, "GET /guess/") != NULL) {
        // Обработка догадки
        char session_id[100];
        char guess[SECRET_LENGTH + 1];
        if (sscanf(buffer, "GET /guess/%99[^/]/%4s", session_id, guess) == 2) {
            pthread_mutex_lock(&mtx);
            int game_index = -1;
            for (int i = 0; i < game_count; i++) {
                if (strcmp(games[i].session_id, session_id) == 0) {
                    game_index = i;
                    break;
                }
            }

            if (game_index == -1) {
                send_response(sock, "HTTP/1.1 404 Not Found\r\n", "{\"error\": \"Invalid session_id. Start a new game.\"}");
            } else if (strlen(guess) != SECRET_LENGTH || !std::all_of(guess, guess + SECRET_LENGTH, ::isdigit)) {
                send_response(sock, "HTTP/1.1 400 Bad Request\r\n", "{\"error\": \"Guess must be a 4-digit number.\"}");
            } else {
                auto [bulls, cows] = calculate_bulls_and_cows(games[game_index].secret_number, guess);
                printf("Session ID: %s, Attempt: %s, Bulls: %d, Cows: %d\n", session_id, guess, bulls, cows);
                strncpy(games[game_index].attempts[games[game_index].attempt_count], guess, SECRET_LENGTH + 1);
                games[game_index].attempt_count++;
                char body[BUFFER_SIZE];

                if (bulls == SECRET_LENGTH) {
                    snprintf(body, sizeof(body), "{\"message\": \"Congratulations! You guessed the number.\"}");
                    // Удаляем игру после победы
                    games[game_index] = games[game_count - 1];
                    game_count--;
                } else {
                    snprintf(body, sizeof(body), "{\"bulls\": %d, \"cows\": %d, \"attempts\": [", bulls, cows);
                    for (int i = 0; i < games[game_index].attempt_count; i++) {
                        strcat(body, "\"" );
                        strcat(body, games[game_index].attempts[i]);
                        strcat(body, "\"");
                        if (i < games[game_index].attempt_count - 1) {
                            strcat(body, ", ");
                        }
                    }
                    strcat(body, "]}");
                }
                send_response(sock, "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n", body);
            }
            pthread_mutex_unlock(&mtx);
        } else {
            send_response(sock, "HTTP/1.1 400 Bad Request\r\n", "{\"error\": \"Invalid request format.\"}");
        }
    } else {
        // Обработка запроса на главную страницу с HTML интерфейсом
        const char *html_page = "<!DOCTYPE html>\n"
                               "<html lang=\"en\">\n"
                               "<head>\n"
                               "    <meta charset=\"UTF-8\">\n"
                               "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
                               "    <title>Bulls and Cows</title>\n"
                               "    <script>\n"
                               "        let sessionId = '';\n"
                               "        function startGame() {\n"
                               "            fetch('/start')\n"
                               "                .then(response => response.json())\n"
                               "                .then(data => {\n"
                               "                    sessionId = data.session_id;\n"
                               "                    document.getElementById('status').innerText = 'Game started. Session ID: ' + sessionId;\n"
                               "                });\n"
                               "        }\n"
                               "        function makeGuess() {\n"
                               "            const guess = document.getElementById('guess').value;\n"
                               "            fetch('/guess/' + sessionId + '/' + guess)\n"
                               "                .then(response => response.json())\n"
                               "                .then(data => {\n"
                               "                    if (data.error) {\n"
                               "                        document.getElementById('status').innerText = data.error;\n"
                               "                    } else if (data.message) {\n"
                               "                        document.getElementById('status').innerText = data.message;\n"
                               "                    } else {\n"
                               "                        document.getElementById('status').innerText = 'Bulls: ' + data.bulls + ', Cows: ' + data.cows;\n"
                               "                    }\n"
                               "                });\n"
                               "        }\n"
                               "    </script>\n"
                               "</head>\n"
                               "<body>\n"
                               "    <h1>Bulls and Cows Game</h1>\n"
                               "    <button onclick=\"startGame()\">Start New Game</button><br><br>\n"
                               "    <input type=\"text\" id=\"guess\" placeholder=\"Enter your guess\" required>\n"
                               "    <button onclick=\"makeGuess()\">Make Guess</button><br><br>\n"
                               "    <div id=\"status\"></div>\n"
                               "</body>\n"
                               "</html>\n";

        send_response(sock, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n", html_page);
    }

    close(sock); // Закрываем сокет клиента
    return NULL;
}

void send_response(int client_socket, const char *header, const char *body) {
    // Формируем и отправляем HTTP-ответ клиенту
    char response[BUFFER_SIZE];
    snprintf(response, sizeof(response), "%sContent-Length: %zu\r\n\r\n%s", header, strlen(body), body);
    write(client_socket, response, strlen(response));
}

void generate_secret_number(char *secret) {
    char digits[] = "1234567890";
    std::random_shuffle(digits, digits + 10);
    strncpy(secret, digits, SECRET_LENGTH);    printf("\n");
    secret[SECRET_LENGTH] = '\0';
}

std::pair<int, int> calculate_bulls_and_cows(const char *secret, const char *guess) {
    int bulls = 0, cows = 0;
    for (int i = 0; i < SECRET_LENGTH; ++i) {
        if (secret[i] == guess[i]) {
            bulls++;
        } else if (strchr(secret, guess[i]) != NULL) {
            cows++;
        }
    }
    return {bulls, cows};
}