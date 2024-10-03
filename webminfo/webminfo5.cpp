#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>

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

// Функция для чтения и анализа EBML заголовка
void readEBMLHeader(std::ifstream& file) {
    auto [ebmlID, _] = readVint(file);
    auto [ebmlSize, __] = readVint(file);

    std::vector<uint8_t> data(ebmlSize);
    file.read(reinterpret_cast<char*>(data.data()), ebmlSize);

    std::cout << "EBML ID: " << std::hex << ebmlID << std::endl;
    std::cout << "EBML Size: " << ebmlSize << std::endl;
    std::cout << "EBML Data: ";
    for (auto byte : data) {
        std::cout << std::hex << static_cast<int>(byte);
    }
    std::cout << std::endl;
}

int main() {
    std::ifstream file("pileoftrash.webm", std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file" << std::endl;
        return 1;
    }

    readEBMLHeader(file);
    file.close();
    return 0;
}
