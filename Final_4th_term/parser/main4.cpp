#include <cstdio>
#include <cstdint>
#include <cstring>
#include <vector>
#include <memory>

// Определение перечисления для элементов EBML
enum ElementType {
    SEGMENT,
    CLUSTER,
    TRACKS,
    TRACK_ENTRY,
    TRACK_TYPE,
    TRACK_NUMBER,
    VIDEO,
    PIXEL_WIDTH,
    PIXEL_HEIGHT,
    SIMPLE_BLOCK,
    BLOCK,
    UNKNOWN
};

// Определение класса для хранения информации об элементе EBML
class EBMLElementInfo {
public:
    int idWidth;
    int sizeWidth;
    uint64_t dataSize;
    uint64_t totalElementSize;
    uint8_t elementID[8];
    ElementType elementType;
    uint64_t value = 0; // Добавленное поле для хранения значения элемента
    std::vector<std::shared_ptr<EBMLElementInfo>> children; // Вложенные элементы

    void printInfo(FILE* out, int level = 0) const {
        for (int i = 0; i < level; ++i) {
            fprintf(out, "  "); // Отступ для вложенности
        }
        fprintf(out, "Element ID: ");
        for (int i = 0; i < idWidth; ++i) {
            fprintf(out, "%02X ", elementID[i]);
        }
        fprintf(out, "\n");
        for (int i = 0; i < level; ++i) {
            fprintf(out, "  ");
        }
        fprintf(out, "Data Size: %lu\n", dataSize);
        for (int i = 0; i < level; ++i) {
            fprintf(out, "  ");
        }
        fprintf(out, "Total Element Size: %lu\n", totalElementSize);
        for (int i = 0; i < level; ++i) {
            fprintf(out, "  ");
        }
        fprintf(out, "Element Type: %s\n", getElementTypeName(elementType));
        if (value != 0) {
            for (int i = 0; i < level; ++i) {
                fprintf(out, "  ");
            }
            fprintf(out, "Value: %lu\n", value);
        }
        for (const auto& child : children) {
            child->printInfo(out, level + 1);
        }
    }

private:
    const char* getElementTypeName(ElementType type) const {
        switch (type) {
            case SEGMENT: return "Segment";
            case CLUSTER: return "Cluster";
            case TRACKS: return "Tracks";
            case TRACK_ENTRY: return "TrackEntry";
            case TRACK_TYPE: return "TrackType";
            case TRACK_NUMBER: return "TrackNumber";
            case VIDEO: return "Video";
            case PIXEL_WIDTH: return "PixelWidth";
            case PIXEL_HEIGHT: return "PixelHeight";
            case SIMPLE_BLOCK: return "SimpleBlock";
            case BLOCK: return "Block";
            default: return "Unknown";
        }
    }
};

// Функция для определения типа элемента по его ID
ElementType getElementType(const uint8_t vintArray[8]) {
    if (vintArray[0] == 0x18 && vintArray[1] == 0x53 && vintArray[2] == 0x80 && vintArray[3] == 0x67) { // ID сегмента: 0x18538067
        return SEGMENT;
    } else if (vintArray[0] == 0x1F && vintArray[1] == 0x43 && vintArray[2] == 0xB6 && vintArray[3] == 0x75) { // ID Cluster: 0x1F43B675
        return CLUSTER;
    } else if (vintArray[0] == 0x16 && vintArray[1] == 0x54 && vintArray[2] == 0xAE && vintArray[3] == 0x6B) { // ID треков: 0x1654AE6B
        return TRACKS;
    } else if (vintArray[0] == 0xAE) { // ID TrackEntry: 0xAE
        return TRACK_ENTRY;
    } else if (vintArray[0] == 0x83) {
        return TRACK_TYPE;
    } else if (vintArray[0] == 0xE0) {
        return VIDEO;
    } else if (vintArray[0] == 0xB0) {
        return PIXEL_WIDTH;
    } else if (vintArray[0] == 0xBA) {
        return PIXEL_HEIGHT;
    } else if (vintArray[0] == 0xA3) { // ID SimpleBlock: 0xA3
        return SIMPLE_BLOCK;
    } else if (vintArray[0] == 0xA1) { // ID Block: 0xA1
        return BLOCK;
    } else if(vintArray[0] == 0xD7) {
        return TRACK_NUMBER;
    } else {
        return UNKNOWN;
    }
}

// Функция для вывода типа элемента
void printElementType(ElementType type, FILE* out) {
    switch (type) {
        case SEGMENT:
            fprintf(out, "Entering Segment element...\n");
            break;
        case CLUSTER:
            fprintf(out, "Entering Cluster element...\n");
            break;
        case TRACKS:
            fprintf(out, "Entering Tracks element...\n");
            break;
        case TRACK_ENTRY:
            fprintf(out, "Entering TrackEntry element...\n");
            break;
        case VIDEO:
            fprintf(out, "Entering Video element...\n");
            break;
        default:
            break;
    }
}

// Функция для определения количества байтов на основе vint маркера
int getVintWidth(uint8_t firstByte) {
    int width = 1;
    while ((firstByte & (0x80 >> (width - 1))) == 0) {
        width++;
    }
    return width;
}

// Функция для чтения vint из потока FILE* и сохранения его в массив
int readVint(FILE* in, uint8_t vintArray[8]) {
    memset(vintArray, 0, 8);
    uint8_t firstByte;
    // Считываем первый байт
    if (fread(&firstByte, 1, 1, in) != 1) {
        if (feof(in)) {
            throw "End of file reached";
            return -1;
        }
        throw "Error reading from file";
        return -1;
    }

    // Определяем количество байтов на основе маркера
    int width = getVintWidth(firstByte);

    // Сохраняем первый байт в массив
    vintArray[0] = firstByte;

    // Считываем остальные байты (если есть) и добавляем их в массив
    for (int i = 1; i < width; ++i) {
        uint8_t nextByte;
        if (fread(&nextByte, 1, 1, in) != 1) {
            if (feof(in)) {
                throw "End of file reached";
            }
            throw "Error reading from file";
        }
        vintArray[i] = nextByte;
    }
    return width;
}

// Функция для чтения размера данных (Data Size) из потока FILE*
uint64_t readVintDataSize(FILE* in, int* width) {
    uint8_t firstByte;
    // Считываем первый байт
    if (fread(&firstByte, 1, 1, in) != 1) {
        if (feof(in)) {
            throw "End of file reached";
        }
        throw "Error reading from file";
    }

    // Определяем количество байтов на основе маркера
    *width = getVintWidth(firstByte);

    // Извлекаем значение, убирая маркерные биты из первого байта
    uint64_t value = firstByte & (0xFF >> *width);

    // Считываем остальные байты (если есть) и добавляем их к значению
    for (int i = 1; i < *width; ++i) {
        uint8_t nextByte;
        if (fread(&nextByte, 1, 1, in) != 1) {
            if (feof(in)) {
                throw "End of file reached";
            }
            throw "Error reading from file";
        }
        value = (value << 8) | nextByte;
    }

    return value;
}

// Функция для получения значения элемента в формате uint
uint64_t getUIntValue(FILE* in, uint64_t dataSize) {
    uint64_t value = 0;
    for (uint64_t i = 0; i < dataSize; ++i) {
        uint8_t byte;
        if (fread(&byte, 1, 1, in) != 1) {
            if (feof(in)) {
                throw "End of file reached";
            }
            throw "Error reading from file";
        }
        value = (value << 8) | byte;
    }
    return value;
}

uint64_t convertVintToNumber(const uint8_t vintArray[8], int width) {
    uint64_t value = vintArray[0] & (0xFF >> width);
    for (int i = 1; i < width; ++i) {
        value = (value << 8) | vintArray[i];
    }
    return value;
}

// Функция для чтения всех элементов до конца файла (обработка сегментов, кластеров, треков и трековых записей)
void readAllElementsWithSegments(FILE* in, FILE* out, std::shared_ptr<EBMLElementInfo> parentElement) {
    try {
        while (true) {
            auto elementInfo = std::make_shared<EBMLElementInfo>();
            readVint(in, elementInfo->elementID);
            elementInfo->idWidth = getVintWidth(elementInfo->elementID[0]);
            elementInfo->dataSize = readVintDataSize(in, &elementInfo->sizeWidth);
            elementInfo->totalElementSize = elementInfo->idWidth + elementInfo->sizeWidth + elementInfo->dataSize;
            elementInfo->elementType = getElementType(elementInfo->elementID);

            // Печатаем информацию об элементе в выходной файл
            elementInfo->printInfo(out);

            // Добавляем элемент в список детей родительского элемента
            parentElement->children.push_back(elementInfo);

            if (elementInfo->elementType == PIXEL_HEIGHT 
            || elementInfo->elementType == PIXEL_WIDTH 
            || elementInfo->elementType == TRACK_TYPE 
            || elementInfo->elementType == TRACK_NUMBER) {
                uint64_t value = getUIntValue(in, elementInfo->dataSize);
                switch (elementInfo->elementType) {
                    case PIXEL_HEIGHT:
                        printf("Pixel height = %lu\n", value);
                        break;
                    case PIXEL_WIDTH:
                        printf("Pixel width = %lu\n", value);
                        break;
                    case TRACK_TYPE:
                        elementInfo->value = value;
                        printf("Track Type = %lu\n", value);
                        parentElement->children.push_back(elementInfo);
                        break;
                    case TRACK_NUMBER:
                        elementInfo->value = value;
                        printf("Track Number = %lu\n", value);
                        parentElement->children.push_back(elementInfo);
                        break;
                    default:
                        break;
                }
                continue;
            }

            // Обработка SimpleBlock и Block: вывод информации о TrackNumber
            if (elementInfo->elementType == SIMPLE_BLOCK || elementInfo->elementType == BLOCK) {
                uint8_t Array[8];
                uint64_t trackNumber;
                int width = readVint(in, Array);
                trackNumber = convertVintToNumber(Array, width);
                elementInfo->value = trackNumber;
                //printf("SimpleBlock/Block belongs to Track Number = %lu\n", trackNumber);
                if (fseek(in, elementInfo->dataSize - width, SEEK_CUR) != 0) {
                    if (feof(in)) {
                        fprintf(out, "End of file reached\n");
                        break;
                    }
                    throw "Error seeking in file";
                }
                parentElement->children.push_back(elementInfo);
                continue;
            }

            // Если элемент — сегмент, кластер, трек или трековая запись, читаем его содержимое рекурсивно
            if (elementInfo->elementType == SEGMENT 
            || elementInfo->elementType == CLUSTER 
            || elementInfo->elementType == TRACKS 
            || elementInfo->elementType == TRACK_ENTRY 
            || elementInfo->elementType == VIDEO) {
                readAllElementsWithSegments(in, out, elementInfo);
            } else {
                // Пропускаем dataSize байтов, чтобы перейти к следующему элементу
                if (fseek(in, elementInfo->dataSize, SEEK_CUR) != 0) {
                    if (feof(in)) {
                        fprintf(out, "End of file reached\n");
                        break;
                    }
                    throw "Error seeking in file";
                }
            }
        }
    } catch (const char* errMsg) {
        if (strcmp(errMsg, "End of file reached") == 0) {
            fprintf(out, "End of file reached\n");
        } else {
            fprintf(stderr, "%s\n", errMsg);
        }
    }
}

int main(int argc, char** argv){
	if (argc != 3) {
		printf("Usage: %s <input.webm> <output.webm>\n", argv[0]);
		return 1;
	}
	FILE* file = fopen(argv[1], "rb");
	if (!file){
		printf("cannot open input file\n");
		return 1;
	}
	FILE* outFile = fopen(argv[2], "wb");
	if (!outFile){
		printf("cannot open output file\n");
		fclose(file);
		return 2;
	}

    auto rootElement = std::make_shared<EBMLElementInfo>();
    readAllElementsWithSegments(file, outFile, rootElement);

    // Печатаем все элементы дерева
    fprintf(outFile, "\n ==============================================================");
    fprintf(outFile, "\nPrinting all elements in the tree:\n");
    rootElement->printInfo(outFile);

    fclose(file);
    fclose(outFile);
    return 0;
}