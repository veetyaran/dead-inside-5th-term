#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <tuple>

// Функция для считывания переменной длины (VINT)
std::pair<uint64_t, int> readVint(std::ifstream& file) {
    uint8_t first_byte;
    file.read(reinterpret_cast<char*>(&first_byte), 1);
    int num_zero_bits = 0; // Количество ведущих нулей

    while (!(first_byte & (0x80 >> num_zero_bits))) {
        num_zero_bits++;
    }

    uint64_t id = first_byte & ((1 << (7 - num_zero_bits)) - 1);
    for (int i = 0; i < num_zero_bits; ++i) {
        uint8_t byte;
        file.read(reinterpret_cast<char*>(&byte), 1);
        id = (id << 8) | byte;
    }
    return {id, num_zero_bits + 1};
}

void readElements(std::ifstream& file, uint64_t targetID, uint64_t parentSize, int level = 1) {
    uint64_t bytesRead = 0;
    while (bytesRead < parentSize) {
        uint64_t ebmlID;
        uint64_t ebmlSize;

        auto [id, idSize] = readVint(file);
        ebmlID = id;
        if (!file.good()) break;

        auto [size, sizeSize] = readVint(file);
        ebmlSize = size;
        if (!file.good()) break;

        bytesRead += idSize + sizeSize;

        if (level == 4 && ebmlID == targetID) { // Уровень 4, TrackWidth
            double trackWidth;
            file.read(reinterpret_cast<char*>(&trackWidth), sizeof(double)); // Читаем значение ширины трека
            std::cout << "TrackWidth: " << trackWidth << std::endl;
            return;
        } else {
            if (ebmlID == 0xAE || ebmlID == 0x1654AE6B) { // TrackEntry или Tracks
                readElements(file, targetID, ebmlSize, level + 1); // Рекурсивно читаем элементы следующего уровня
            } else {
                file.seekg(ebmlSize, std::ios_base::cur);
            }
            bytesRead += ebmlSize;
        }
    }
}

int main() {
    std::ifstream file("example.webm", std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file" << std::endl;
        return 1;
    }

    uint64_t targetID = 0xB0; // EBML ID TrackWidth
    uint64_t rootID;
    uint64_t rootDataSize;

    std::tie(rootID, std::ignore) = readVint(file);
    std::tie(rootDataSize, std::ignore) = readVint(file);

    readElements(file, targetID, rootDataSize); // Чтение элементов начиная с корня

    file.close();
    return 0;
}
