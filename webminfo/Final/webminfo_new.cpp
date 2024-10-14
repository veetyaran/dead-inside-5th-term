#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>

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
long long ReadID(FILE *file, long long pos, long *len) {
    if (!file || pos < 0)
        return E_FILE_FORMAT_INVALID;

    // Переход на нужную позицию в файле
    if (fseek(file, pos, SEEK_SET) != 0)
        return E_FILE_FORMAT_INVALID;

    // Чтение первого байта
    unsigned char temp_byte = 0;
    if (fread(&temp_byte, 1, 1, file) != 1)
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
        return E_FILE_FORMAT_INVALID;
    }

    // Чтение оставшихся байтов ID (если есть)
    const int id_length = bit_pos + 1;
    long long ebml_id = temp_byte;
    for (int i = 1; i < id_length; ++i) {
        ebml_id <<= 8;
        if (fread(&temp_byte, 1, 1, file) != 1)
            return E_BUFFER_NOT_FULL;

        ebml_id |= temp_byte;
    }

    *len = id_length;
    return ebml_id;
}

// Функция для чтения размера элемента
long long ReadSize(FILE *file, long long pos, long *len) {
    if (!file || pos < 0)
        return E_FILE_FORMAT_INVALID;

    // Переход на нужную позицию в файле
    if (fseek(file, pos, SEEK_SET) != 0)
        return E_FILE_FORMAT_INVALID;

    // Чтение первого байта
    unsigned char temp_byte = 0;
    if (fread(&temp_byte, 1, 1, file) != 1)
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
        return E_FILE_FORMAT_INVALID;
    }

    // Чтение оставшихся байтов размера (если есть)
    const int size_length = bit_pos + 1;
    long long size = temp_byte & ((1 << (7 - bit_pos)) - 1); // Очистка ведущих битов
    for (int i = 1; i < size_length; ++i) {
        size <<= 8;
        if (fread(&temp_byte, 1, 1, file) != 1)
            return E_BUFFER_NOT_FULL;

        size |= temp_byte;
    }

    *len = size_length;
    return size;
}

// Функция для обработки элементов EBML
void parse_ebml_element(FILE *file, long long end_position, int depth) {
    while (ftell(file) < end_position) {
        long id_length;
        long long element_id = ReadID(file, ftell(file), &id_length);
        if (element_id < 0) {
            printf("Error reading element ID.\n");
            break;
        }

        printf("%*sElement ID: 0x%llx (length: %ld bytes)\n", depth * 2, "", element_id, id_length);

        // Чтение размера элемента
        long size_length;
        long long size = ReadSize(file, ftell(file), &size_length);
        if (size < 0) {
            printf("Error reading element size.\n");
            break;
        }

        printf("%*sSize: %lld (length: %ld bytes)\n", depth * 2, "", size, size_length);

        long long element_end = ftell(file) + size;

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
                fread(&byte, 1, 1, file);
                value = (value << 8) | byte;
            }
            if (element_id == kPixelWidth) {
                printf("\n%*sPixel Width: %llu pixels\n\n", depth * 2 + 1, " ", value);
            } else if (element_id == kPixelHeight) {
                printf("\n%*sPixel Height: %llu pixels\n\n", depth * 2 + 1, " ", value);
            }
        } else {
            // Пропуск элемента, перемещаем указатель чтения
            fseek(file, element_end, SEEK_SET);
        }
    }
}

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: %s <yourfile.webm>\n", argv[0]);
        return -1;
    }

    const char* file_path = argv[1];
    FILE *file = fopen(file_path, "r+b");
    if (!file) {
        printf("Could not open the file!\n");
        return 1;
    }

    fseek(file, 0, SEEK_END);
    long long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    parse_ebml_element(file, file_size, 0);

    fclose(file);
    return 0;
}
