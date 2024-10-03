#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <SFML/Graphics.hpp>

// Структура для хранения данных спектрограммы
struct SpectrogramData {
    int time_window;
    int frequency_bin;
    double magnitude;
};

// Функция для чтения данных из файла result.txt
std::vector<SpectrogramData> read_spectrogram_data(const std::string& file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + file_path);
    }

    std::string line;
    std::vector<SpectrogramData> data;

    // Пропуск заголовка
    std::getline(file, line);

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        SpectrogramData datum;
        if (!(iss >> datum.time_window >> datum.frequency_bin >> datum.magnitude)) {
            break;
        }
        data.push_back(datum);
    }

    file.close();
    return data;
}

// Функция для построения спектрограммы
void plot_spectrogram(const std::vector<SpectrogramData>& data, const std::string& output_image) {
    // Определение размеров спектрограммы
    int max_time_window = 0;
    int max_frequency_bin = 0;

    for (const auto& d : data) {
        if (d.time_window > max_time_window) {
            max_time_window = d.time_window;
        }
        if (d.frequency_bin > max_frequency_bin) {
            max_frequency_bin = d.frequency_bin;
        }
    }

    // Создание изображения спектрограммы
    const int width = max_time_window + 1;
    const int height = max_frequency_bin + 1;
    sf::Image image;
    image.create(width, height, sf::Color::Black);

    // Нормализация величин
    double max_magnitude = 0.0;
    for (const auto& d : data) {
        if (d.magnitude > max_magnitude) {
            max_magnitude = d.magnitude;
        }
    }

    // Заполнение изображения спектрограммы
    for (const auto& d : data) {
        int intensity = static_cast<int>((d.magnitude / max_magnitude) * 255);
        sf::Color color(intensity, intensity, intensity);
        image.setPixel(d.time_window, height - 1 - d.frequency_bin, color);
    }

    // Сохранение изображения
    if (!image.saveToFile(output_image)) {
        throw std::runtime_error("Could not save image: " + output_image);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_file> <output_image>" << std::endl;
        return 1;
    }

    try {
        // Чтение данных из файла
        auto data = read_spectrogram_data(argv[1]);

        // Построение и сохранение спектрограммы
        plot_spectrogram(data, argv[2]);

        std::cout << "Spectrogram saved to " << argv[2] << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
