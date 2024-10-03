#include "crow.h"
#include <iostream>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <ctime>

// Генерация случайного 4-значного числа
std::string generateNumber() {
    std::string number = "1234567890";
    std::random_shuffle(number.begin(), number.end());
    return number.substr(0, 4);
}

// Расчет количества быков и коров
std::pair<int, int> calculateBullsAndCows(const std::string& secret, const std::string& guess) {
    int bulls = 0, cows = 0;
    for (int i = 0; i < 4; ++i) {
        if (secret[i] == guess[i]) {
            bulls++;
        } else if (secret.find(guess[i]) != std::string::npos) {
            cows++;
        }
    }
    return {bulls, cows};
}

// Хранение состояния игры для каждого клиента
std::unordered_map<std::string, std::string> games; // session_id -> secret_number

int main() {
    srand(static_cast<unsigned>(time(0)));

    crow::SimpleApp app;

    // Старт новой игры
    CROW_ROUTE(app, "/start")([&]() {
        std::string session_id = std::to_string(rand());
        std::string secret_number = generateNumber();
        games[session_id] = secret_number;

        return crow::json::wvalue{{"session_id", session_id}};
    });

    // Обработка догадки
    CROW_ROUTE(app, "/guess/<string>/<string>")([&](std::string session_id, std::string guess) {
        if (games.find(session_id) == games.end()) {
            return crow::json::wvalue{{"error", "Invalid session_id. Start a new game."}};
        }

        if (guess.length() != 4 || !std::all_of(guess.begin(), guess.end(), ::isdigit)) {
            return crow::json::wvalue{{"error", "Guess must be a 4-digit number."}};
        }

        std::string secret_number = games[session_id];
        auto [bulls, cows] = calculateBullsAndCows(secret_number, guess);

        if (bulls == 4) {
            games.erase(session_id); // Удаляем игру после победы
            return crow::json::wvalue{{"message", "Congratulations! You guessed the number."}};
        }

        return crow::json::wvalue{{"bulls", bulls}, {"cows", cows}};
    });

    // Главная страница с HTML интерфейсом
    CROW_ROUTE(app, "/")([]() {
        // Используем обычную строку для HTML
        std::string html_page = "<!DOCTYPE html>\n"
            "<html lang=\"en\">\n"
            "<head>\n"
            "    <meta charset=\"UTF-8\">\n"
            "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
            "    <title>Bulls and Cows</title>\n"
            "    <script>\n"
            "        let sessionId = '';\n"
            "\n"
            "        function startGame() {\n"
            "            fetch('/start')\n"
            "                .then(response => response.json())\n"
            "                .then(data => {\n"
            "                    sessionId = data.session_id;\n"
            "                    document.getElementById('status').innerText = 'Game started. Session ID: ' + sessionId;\n"
            "                });\n"
            "        }\n"
            "\n"
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

        return crow::response(html_page);
    });

    // Запуск сервера на порту 18080
    app.port(18080).multithreaded().run();
}
