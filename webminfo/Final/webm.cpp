#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <bitset>
#include <unistd.h>

#define BUFSIZE 1048576 // 1MB

typedef enum ebml_element_type {
    MASTER,
    UINT,
    INT,
    STRING,
    UTF8,
    BINARY,
    FLOAT,
    DATE
} ebml_element_type;

typedef struct simple_vint {
    uint8_t width;
    uint8_t data[8];
} simple_vint;

uint64_t read_vint(FILE *file, uint8_t *size_length) {
    uint8_t first_byte;
    if (fread(&first_byte, 1, 1, file) != 1) {
        return 0;  // Ошибка чтения
    }

    *size_length = 0;
    uint8_t mask = 0x80;  // 10000000
    while (!(first_byte & mask)) {
        mask >>= 1;
        (*size_length)++;
    }

    (*size_length)++;
    uint64_t value = first_byte & (~mask);

    for (uint8_t i = 1; i < *size_length; ++i) {
        uint8_t next_byte;
        if (fread(&next_byte, 1, 1, file) != 1) {
            return 0;  // Ошибка чтения
        }
        value = (value << 8) | next_byte;
    }

    return value;
}

bool is_container_element(uint64_t element_id) {
    switch (element_id) {
        case 0x1A45DFA3:  // EBML
        case 0x18538067:  // Segment
        case 0x1549A966:  // Info
        case 0x1654AE6B:  // Tracks
        case 0x1F43B675:  // Cluster
            return true;
        default:
            return false;
    }
}

void parse_element(FILE *file, uint64_t element_size) {
    uint64_t bytes_read = 0;
    uint8_t buffer[BUFSIZE];
    while (bytes_read < element_size && !feof(file)) {
        uint8_t id_length;
        uint64_t element_id = read_vint(file, &id_length);
        if (feof(file)) break;
        printf("Element ID: 0x%lx (length: %d bytes)\n", (unsigned long)element_id, id_length);
        bytes_read += id_length;

        uint8_t size_length;
        uint64_t sub_element_size = read_vint(file, &size_length);
        if (feof(file)) break;
        printf("Size: %lu (length: %d bytes)\n", (unsigned long)sub_element_size, size_length);
        bytes_read += size_length;

        if (is_container_element(element_id)) {
            parse_element(file, sub_element_size);
        } else {
            if ((fread(buffer, 1, sub_element_size, file)) != sub_element_size) {
                printf("Ошибка чтения данных элемента\n");
                break;
            }
        }
        bytes_read += sub_element_size;
    }
}

void parse_ebml(FILE *file) {
    uint8_t buffer[BUFSIZE];
    int len;
    while ((len = fread(buffer, 1, BUFSIZE, file)) > 0) {
        uint8_t *pos = buffer;
        while (pos < buffer + len) {
            uint8_t id_length;
            uint64_t element_id = read_vint(file, &id_length);
            printf("Element ID: 0x%lx (length: %d bytes)\n", (unsigned long)element_id, id_length);
            pos += id_length;

            uint8_t size_length;
            uint64_t element_size = read_vint(file, &size_length);
            printf("Size: %lu (length: %d bytes)\n", (unsigned long)element_size, size_length);
            pos += size_length;

            if (is_container_element(element_id)) {
                parse_element(file, element_size);
            } else {
                if ((fread(buffer, 1, element_size, file)) != element_size) {
                    printf("Ошибка чтения данных элемента\n");
                    break;
                }
                pos += element_size;
            }
        }
    }
}

int main() {
    FILE *file = fopen("example1.webm", "rb");
    if (!file) {
        printf("Не удалось открыть файл!\n");
        return 1;
    }

    parse_ebml(file);

    fclose(file);
    return 0;
}