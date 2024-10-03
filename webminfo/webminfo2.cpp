#include <fstream>
#include <iostream>
#include <vector>

std::vector<unsigned char> read_bytes(std::ifstream& file, size_t num_bytes) {
    std::vector<unsigned char> buffer(num_bytes);
    file.read(reinterpret_cast<char*>(buffer.data()), num_bytes);
    return buffer;
}

unsigned int parse_vint(const std::vector<unsigned char>& data, size_t& index) {
    unsigned int vint = 0;
    unsigned char first_byte = data[index];
    int num_following = 0; // количество следующих байтов

    // Определение количества следующих байтов
    while (first_byte & (0x80 >> num_following)) {
        num_following++;
    }

    // Декодирование переменной длины
    vint = first_byte & ((1 << (7 - num_following)) - 1);
    for (int i = 0; i < num_following; i++) {
        index++;
        vint <<= 8;
        vint |= data[index];
    }

    index++; // Переходим к следующему элементу
    return vint;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <WebM file>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1], std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << argv[1] << std::endl;
        return 1;
    }

    // Предположим, что сначала идет EBML ID, а затем размер
    std::vector<unsigned char> header = read_bytes(file, 100); // Читаем первые 100 байтов для примера
    size_t index = 0;

    // Чтение EBML ID
    unsigned int ebml_id = parse_vint(header, index);
    std::cout << "EBML ID: " << std::hex << ebml_id << std::dec << std::endl;

    // Чтение размера
    unsigned int size = parse_vint(header, index);
    std::cout << "Size: " << size << std::endl;

    file.close();
    return 0;
}
