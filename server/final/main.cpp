#include "crow.h"
#include <vector>
#include <mutex>
#include <string>
#include <sstream>
#include <iostream>

// Структура для хранения сообщений
struct Message {
    std::string name;
    std::string message;
};

// Вектор для хранения сообщений и мьютекс для потокобезопасности
std::vector<Message> messages;
std::mutex mtx;

int main() {
    crow::SimpleApp app;

    // Маршрут для главной страницы
    CROW_ROUTE(app, "/")
    ([](const crow::request& req, crow::response& res) {
        std::lock_guard<std::mutex> lock(mtx);
        std::ostringstream os;
        os << "<html><body>";
        os << "<h1>Message Board</h1>";
        os << "<form action=\"/submit\" method=\"post\">";
        os << "Name: <input type=\"text\" name=\"name\"><br>";
        os << "Message: <textarea name=\"message\"></textarea><br>";
        os << "<input type=\"submit\" value=\"Submit\">";
        os << "</form>";
        os << "<h2>Messages:</h2>";
        for (const auto& msg : messages) {
            os << "<p><strong>" << msg.name << ":</strong> " << msg.message << "</p>";
        }
        os << "</body></html>";
        res.set_header("Content-Type", "text/html");
        res.write(os.str());
        res.end();
    });

    // Маршрут для отправки сообщения
    CROW_ROUTE(app, "/submit").methods(crow::HTTPMethod::Post)
    ([](const crow::request& req, crow::response& res) {
        std::string body = req.body;
        std::unordered_map<std::string, std::string> params;
        std::istringstream iss(body);
        std::string token;

        while (std::getline(iss, token, '&')) {
            auto pos = token.find('=');
            if (pos != std::string::npos) {
                std::string key = token.substr(0, pos);
                std::string value = token.substr(pos + 1);
                params[key] = value;
            }
        }

        std::string name = params["name"].empty() ? "Anonymous" : params["name"];
        std::string message = params["message"];

        // Для отладки, выводим параметры запроса в консоль
        std::cout << "Received POST request: name=" << name << ", message=" << message << std::endl;

        if (!message.empty()) {
            std::lock_guard<std::mutex> lock(mtx);
            messages.push_back({name, message});
            // Для отладки, выводим текущее состояние сообщений
            std::cout << "Current messages:" << std::endl;
            for (const auto& msg : messages) {
                std::cout << "Name: " << msg.name << ", Message: " << msg.message << std::endl;
            }
        }

        res.code = 303;
        res.set_header("Location", "/");
        res.end();
    });

    // Запуск сервера на порту 8080
    app.port(8080).multithreaded().run();
}
