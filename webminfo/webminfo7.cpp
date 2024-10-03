/*#include <iostream>
#include <fstream>
#include <cstdint>

// Константы для ошибок
const long long E_FILE_FORMAT_INVALID = -1;
const long long E_BUFFER_NOT_FULL = -2;

// Функция для чтения ID элемента
long long ReadID(std::ifstream &file, long long pos, long &len) {
    if (!file || pos < 0)
        return E_FILE_FORMAT_INVALID;

    // Переход на нужную позицию в файле
    file.seekg(pos);
    if (!file.good())
        return E_FILE_FORMAT_INVALID;

    // Чтение первого байта
    unsigned char temp_byte = 0;
    file.read(reinterpret_cast<char*>(&temp_byte), 1);

    if (file.gcount() < 1)
        return E_BUFFER_NOT_FULL;

    if (temp_byte == 0)  // ID длиной более 8 байт; неверный файл
        return E_FILE_FORMAT_INVALID;

    int bit_pos = 0;
    const int kMaxIdLengthInBytes = 4;
    const int kCheckByte = 0x80;

    // Нахождение первого установленного бита
    bool found_bit = false;
    for (; bit_pos < kMaxIdLengthInBytes; ++bit_pos) {
        if ((kCheckByte >> bit_pos) & temp_byte) {
            found_bit = true;
            break;
        }
    }

    if (!found_bit) {
        // Значение слишком велико, чтобы быть допустимым ID
        return E_FILE_FORMAT_INVALID;
    }

    // Чтение оставшихся байтов ID (если есть)
    const int id_length = bit_pos + 1;
    long long ebml_id = temp_byte;
    for (int i = 1; i < id_length; ++i) {
        ebml_id <<= 8;
        file.read(reinterpret_cast<char*>(&temp_byte), 1);

        if (file.gcount() < 1)
            return E_BUFFER_NOT_FULL;

        ebml_id |= temp_byte;
    }

    len = id_length;
    return ebml_id;
}

// Функция для чтения размера элемента
long long ReadSize(std::ifstream &file, long long pos, long &len) {
    if (!file || pos < 0)
        return E_FILE_FORMAT_INVALID;

    // Переход на нужную позицию в файле
    file.seekg(pos);
    if (!file.good())
        return E_FILE_FORMAT_INVALID;

    // Чтение первого байта
    unsigned char temp_byte = 0;
    file.read(reinterpret_cast<char*>(&temp_byte), 1);

    if (file.gcount() < 1)
        return E_BUFFER_NOT_FULL;

    int bit_pos = 0;
    const int kMaxSizeLengthInBytes = 8;
    const int kCheckByte = 0x80;

    // Нахождение первого установленного бита
    bool found_bit = false;
    for (; bit_pos < kMaxSizeLengthInBytes; ++bit_pos) {
        if ((kCheckByte >> bit_pos) & temp_byte) {
            found_bit = true;
            break;
        }
    }

    if (!found_bit) {
        // Значение слишком велико, чтобы быть допустимым размером
        return E_FILE_FORMAT_INVALID;
    }

    // Чтение оставшихся байтов размера (если есть)
    const int size_length = bit_pos + 1;
    long long size = temp_byte & ((1 << (7 - bit_pos)) - 1); // Очистка ведущих битов
    for (int i = 1; i < size_length; ++i) {
        size <<= 8;
        file.read(reinterpret_cast<char*>(&temp_byte), 1);

        if (file.gcount() < 1)
            return E_BUFFER_NOT_FULL;

        size |= temp_byte;
    }

    len = size_length;
    return size;
}

// Функция для обработки элементов EBML
void parse_ebml_element(std::ifstream &file, long long end_position) {
    while (file.tellg() < end_position) {
        long id_length;
        long long element_id = ReadID(file, file.tellg(), id_length);
        if (element_id < 0) {
            std::cerr << "Error reading element ID." << std::endl;
            break;
        }

        std::cout << "Element ID: 0x" << std::hex << element_id << " (length: " << id_length << " bytes)" << std::dec << std::endl;

        // Чтение размера элемента
        long size_length;
        long long size = ReadSize(file, file.tellg(), size_length);
        if (size < 0) {
            std::cerr << "Error reading element size." << std::endl;
            break;
        }

        std::cout << "Size: " << size << " (length: " << size_length << " bytes)" << std::endl;

        // Пропуск элемента, перемещаем указатель чтения
        file.seekg(file.tellg() + std::streamoff(size));
    }
}

int main() {
    std::string file_path = "example.webm"; // Замените на путь к вашему файлу .webm
    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Could not open the file!" << std::endl;
        return 1;
    }

    file.seekg(0, std::ios::end);
    long long file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    parse_ebml_element(file, file_size);

    file.close();
    return 0;
}
*/

/*
#include <iostream>
#include <fstream>
#include <cstdint>
#include <iomanip>

// Константы для ошибок
const long long E_FILE_FORMAT_INVALID = -1;
const long long E_BUFFER_NOT_FULL = -2;

// Идентификаторы элементов Matroska/WebM
const long long kEBML = 0x1A45DFA3;
const long long kSegment = 0x18538067;
const long long kTracks = 0x1654AE6B;
const long long kTrackEntry = 0xAE;
const long long kVideo = 0xE0;
const long long kPixelWidth = 0xB0;
const long long kPixelHeight = 0xBA;

// Функция для чтения ID элемента
long long ReadID(std::ifstream &file, long long pos, long &len) {
    if (!file || pos < 0)
        return E_FILE_FORMAT_INVALID;

    // Переход на нужную позицию в файле
    file.seekg(pos);
    if (!file.good())
        return E_FILE_FORMAT_INVALID;

    // Чтение первого байта
    unsigned char temp_byte = 0;
    file.read(reinterpret_cast<char*>(&temp_byte), 1);

    if (file.gcount() < 1)
        return E_BUFFER_NOT_FULL;

    if (temp_byte == 0)  // ID длиной более 8 байт; неверный файл
        return E_FILE_FORMAT_INVALID;

    int bit_pos = 0;
    const int kMaxIdLengthInBytes = 4;
    const int kCheckByte = 0x80;

    // Нахождение первого установленного бита
    bool found_bit = false;
    for (; bit_pos < kMaxIdLengthInBytes; ++bit_pos) {
        if ((kCheckByte >> bit_pos) & temp_byte) {
            found_bit = true;
            break;
        }
    }

    if (!found_bit) {
        // Значение слишком велико, чтобы быть допустимым ID
        return E_FILE_FORMAT_INVALID;
    }

    // Чтение оставшихся байтов ID (если есть)
    const int id_length = bit_pos + 1;
    long long ebml_id = temp_byte;
    for (int i = 1; i < id_length; ++i) {
        ebml_id <<= 8;
        file.read(reinterpret_cast<char*>(&temp_byte), 1);

        if (file.gcount() < 1)
            return E_BUFFER_NOT_FULL;

        ebml_id |= temp_byte;
    }

    len = id_length;
    return ebml_id;
}

// Функция для чтения размера элемента
long long ReadSize(std::ifstream &file, long long pos, long &len) {
    if (!file || pos < 0)
        return E_FILE_FORMAT_INVALID;

    // Переход на нужную позицию в файле
    file.seekg(pos);
    if (!file.good())
        return E_FILE_FORMAT_INVALID;

    // Чтение первого байта
    unsigned char temp_byte = 0;
    file.read(reinterpret_cast<char*>(&temp_byte), 1);

    if (file.gcount() < 1)
        return E_BUFFER_NOT_FULL;

    int bit_pos = 0;
    const int kMaxSizeLengthInBytes = 8;
    const int kCheckByte = 0x80;

    // Нахождение первого установленного бита
    bool found_bit = false;
    for (; bit_pos < kMaxSizeLengthInBytes; ++bit_pos) {
        if ((kCheckByte >> bit_pos) & temp_byte) {
            found_bit = true;
            break;
        }
    }

    if (!found_bit) {
        // Значение слишком велико, чтобы быть допустимым размером
        return E_FILE_FORMAT_INVALID;
    }

    // Чтение оставшихся байтов размера (если есть)
    const int size_length = bit_pos + 1;
    long long size = temp_byte & ((1 << (7 - bit_pos)) - 1); // Очистка ведущих битов
    for (int i = 1; i < size_length; ++i) {
        size <<= 8;
        file.read(reinterpret_cast<char*>(&temp_byte), 1);

        if (file.gcount() < 1)
            return E_BUFFER_NOT_FULL;

        size |= temp_byte;
    }

    len = size_length;
    return size;
}

// Функция для обработки элементов EBML
void parse_ebml_element(std::ifstream &file, long long end_position, int depth = 0) {
    while (file.tellg() < end_position) {
        long id_length;
        long long element_id = ReadID(file, file.tellg(), id_length);
        if (element_id < 0) {
            std::cerr << "Error reading element ID." << std::endl;
            break;
        }

        std::cout << std::setw(depth * 2) << "" << "Element ID: 0x" << std::hex << element_id << " (length: " << id_length << " bytes)" << std::dec << std::endl;

        // Чтение размера элемента
        long size_length;
        long long size = ReadSize(file, file.tellg(), size_length);
        if (size < 0) {
            std::cerr << "Error reading element size." << std::endl;
            break;
        }

        std::cout << std::setw(depth * 2) << "" << "Size: " << size << " (length: " << size_length << " bytes)" << std::endl;

        long long element_end = file.tellg() + size;

        // Если это контейнер, рекурсивно обработаем его содержимое
        if (element_id == kEBML 
        || element_id == kSegment 
        || element_id == kTracks 
        || element_id == kTrackEntry 
        || element_id == kVideo) {
            parse_ebml_element(file, element_end, depth + 1);
        } else if (element_id == kPixelWidth || element_id == kPixelHeight) {
            // Чтение значения пикселей
            unsigned long long value = 0;
            file.read(reinterpret_cast<char*>(&value), size);
            if (element_id == kPixelWidth) {
                std::cout << std::setw(depth * 2) << "" << "Pixel Width: " << value << " pixels" << std::endl;
            } else if (element_id == kPixelHeight) {
                std::cout << std::setw(depth * 2) << "" << "Pixel Height: " << value << " pixels" << std::endl;
            }
        } else {
            // Пропуск элемента, перемещаем указатель чтения
            file.seekg(element_end);
        }
    }
}

int main() {
    std::string file_path = "example.webm"; // Замените на путь к вашему файлу .webm
    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Could not open the file!" << std::endl;
        return 1;
    }

    file.seekg(0, std::ios::end);
    long long file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    parse_ebml_element(file, file_size);

    file.close();
    return 0;
}
*/

#include <iostream>
#include <fstream>
#include <cstdint>
#include <iomanip>

// Константы для ошибок
const long long E_FILE_FORMAT_INVALID = -1;
const long long E_BUFFER_NOT_FULL = -2;

// Идентификаторы элементов Matroska/WebM
const long long kEBML = 0x1A45DFA3;
const long long kSegment = 0x18538067;
const long long kTracks = 0x1654AE6B;
const long long kTrackEntry = 0xAE;
const long long kVideo = 0xE0;
const long long kPixelWidth = 0xB0;
const long long kPixelHeight = 0xBA;

// Функция для чтения ID элемента
long long ReadID(std::ifstream &file, long long pos, long &len) {
    if (!file || pos < 0)
        return E_FILE_FORMAT_INVALID;

    // Переход на нужную позицию в файле
    file.seekg(pos);
    if (!file.good())
        return E_FILE_FORMAT_INVALID;

    // Чтение первого байта
    unsigned char temp_byte = 0;
    file.read(reinterpret_cast<char*>(&temp_byte), 1);

    if (file.gcount() < 1)
        return E_BUFFER_NOT_FULL;

    if (temp_byte == 0)  // ID длиной более 8 байт; неверный файл
        return E_FILE_FORMAT_INVALID;

    int bit_pos = 0;
    const int kMaxIdLengthInBytes = 4;
    const int kCheckByte = 0x80;

    // Нахождение первого установленного бита
    bool found_bit = false;
    for (; bit_pos < kMaxIdLengthInBytes; ++bit_pos) {
        if ((kCheckByte >> bit_pos) & temp_byte) {
            found_bit = true;
            break;
        }
    }

    if (!found_bit) {
        // Значение слишком велико, чтобы быть допустимым ID
        return E_FILE_FORMAT_INVALID;
    }

    // Чтение оставшихся байтов ID (если есть)
    const int id_length = bit_pos + 1;
    long long ebml_id = temp_byte;
    for (int i = 1; i < id_length; ++i) {
        ebml_id <<= 8;
        file.read(reinterpret_cast<char*>(&temp_byte), 1);

        if (file.gcount() < 1)
            return E_BUFFER_NOT_FULL;

        ebml_id |= temp_byte;
    }

    len = id_length;
    return ebml_id;
}

// Функция для чтения размера элемента
long long ReadSize(std::ifstream &file, long long pos, long &len) {
    if (!file || pos < 0)
        return E_FILE_FORMAT_INVALID;

    // Переход на нужную позицию в файле
    file.seekg(pos);
    if (!file.good())
        return E_FILE_FORMAT_INVALID;

    // Чтение первого байта
    unsigned char temp_byte = 0;
    file.read(reinterpret_cast<char*>(&temp_byte), 1);

    if (file.gcount() < 1)
        return E_BUFFER_NOT_FULL;

    int bit_pos = 0;
    const int kMaxSizeLengthInBytes = 8;
    const int kCheckByte = 0x80;

    // Нахождение первого установленного бита
    bool found_bit = false;
    for (; bit_pos < kMaxSizeLengthInBytes; ++bit_pos) {
        if ((kCheckByte >> bit_pos) & temp_byte) {
            found_bit = true;
            break;
        }
    }

    if (!found_bit) {
        // Значение слишком велико, чтобы быть допустимым размером
        return E_FILE_FORMAT_INVALID;
    }

    // Чтение оставшихся байтов размера (если есть)
    const int size_length = bit_pos + 1;
    long long size = temp_byte & ((1 << (7 - bit_pos)) - 1); // Очистка ведущих битов
    for (int i = 1; i < size_length; ++i) {
        size <<= 8;
        file.read(reinterpret_cast<char*>(&temp_byte), 1);

        if (file.gcount() < 1)
            return E_BUFFER_NOT_FULL;

        size |= temp_byte;
    }

    len = size_length;
    return size;
}

// Функция для обработки элементов EBML
void parse_ebml_element(std::ifstream &file, long long end_position, int depth = 0) {
    while (file.tellg() < end_position) {
        long id_length;
        long long element_id = ReadID(file, file.tellg(), id_length);
        if (element_id < 0) {
            std::cerr << "Error reading element ID." << std::endl;
            break;
        }

        std::cout << std::setw(depth * 2) << "" << "Element ID: 0x" << std::hex << element_id << " (length: " << id_length << " bytes)" << std::dec << std::endl;

        // Чтение размера элемента
        long size_length;
        long long size = ReadSize(file, file.tellg(), size_length);
        if (size < 0) {
            std::cerr << "Error reading element size." << std::endl;
            break;
        }

        std::cout << std::setw(depth * 2) << "" << "Size: " << size << " (length: " << size_length << " bytes)" << std::endl;

        long long element_end = static_cast<std::streamoff>(file.tellg()) + size; //Статик каст чтобы исправить предупреждение

       if (element_id == kEBML 
       || element_id == kSegment 
       || element_id == kTracks 
       || element_id == kTrackEntry 
       || element_id == kVideo) {
            // Если это контейнер, рекурсивно обработаем его содержимое
            parse_ebml_element(file, element_end, depth + 1);
        } else if (element_id == kPixelWidth || element_id == kPixelHeight) {
            // Чтение значения пикселей в формате big-endian
            unsigned long long value = 0;
            for (int i = 0; i < size; ++i) {
                unsigned char byte;
                file.read(reinterpret_cast<char*>(&byte), 1);
                value = (value << 8) | byte;
            }
            if (element_id == kPixelWidth) {
                std::cout << "\n" << std::setw(depth * 2 + 1) << "" << "Pixel Width: " << value << " pixels" << "\n" << std::endl;
            } else if (element_id == kPixelHeight) {
                std::cout << "\n" << std::setw(depth * 2 + 1) << "" << "Pixel Height: " << value << " pixels" << "\n"<< std::endl;
            }
        } else {
            // Пропуск элемента, перемещаем указатель чтения
            file.seekg(element_end);
        }
    }
}

int main(int argc, char** argv) {
    if(argc != 2) {
        std::cout << "Usage: " << argv[0] << " <yourfile.webm>" << std::endl;
        return -1;
    }
    std::string file_path = argv[1]; // Не забыть сделать ввод имени файла как argv
    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Could not open the file!" << std::endl;
        return 1;
    }

    file.seekg(0, std::ios::end);
    long long file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    parse_ebml_element(file, file_size);

    file.close();
    return 0;
}
