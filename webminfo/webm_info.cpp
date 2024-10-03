#include <iostream>
#include <cstdio>
#include <cstdint>
#include <string>
#include <arpa/inet.h> // для использования функции ntohl

struct webm_info {
    unsigned int EBMLver;
    unsigned int currVer;
    unsigned int MaxID;
    unsigned int MaxLen;
    std::string docType;
    unsigned int typeVer;
    unsigned int currDocVer;
};

#define WEBM_SUCCESS 0
#define READ_ERROR -1
#define NOT_WEBM -10
#define WEBM_UNSUPPORTED -11

int read_webm_info(webm_info *webm, FILE *f) {
    fseek(f, 0, SEEK_SET);

    // Read the magic number, which should be 0x1A45DFA3 for WebM files
    unsigned int mag;
    if (fread(&mag, sizeof(mag), 1, f) != 1) return READ_ERROR;
	mag = ntohl(mag);  // Преобразование из сетевого порядка (big-endian) в порядок хоста
	if (mag != 0x1A45DFA3) return NOT_WEBM;

    if (fread(&webm->EBMLver, sizeof(webm->EBMLver), 1, f) != 1) return READ_ERROR;
    if (fread(&webm->currVer, sizeof(webm->currVer), 1, f) != 1) return READ_ERROR;
    if (fread(&webm->MaxID, sizeof(webm->MaxID), 1, f) != 1) return READ_ERROR;
    if (fread(&webm->MaxLen, sizeof(webm->MaxLen), 1, f) != 1) return READ_ERROR;

    // Reading docType as a string
    char typeBuffer[100];
    if (fread(typeBuffer, sizeof(char), 100, f) != 100) return READ_ERROR;
    webm->docType = std::string(typeBuffer);

    if (fread(&webm->typeVer, sizeof(webm->typeVer), 1, f) != 1) return READ_ERROR;
    if (fread(&webm->currDocVer, sizeof(webm->currDocVer), 1, f) != 1) return READ_ERROR;

    return WEBM_SUCCESS;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <WebM file>" << std::endl;
        return 1;
    }

    FILE *file = fopen(argv[1], "rb");
    if (file == NULL) {
        std::cerr << "Failed to open file: " << argv[1] << std::endl;
        return 1;
    }

    webm_info info;
    int result = read_webm_info(&info, file);
    fclose(file);

    if (result == WEBM_SUCCESS) {
        std::cout << "EBML Version: " << std::hex << info.EBMLver << std::endl;
        std::cout << "EBML Read Version: " << std::hex << info.currVer << std::endl;
        std::cout << "EBML Max ID Length: " <<std::hex << info.MaxID << std::endl;
        std::cout << "EBML Max Size Length: " << std::hex << info.MaxLen << std::endl;
        std::cout << "Document Type: " << std::hex << info.docType << std::endl;
        std::cout << "Document Type Version: "<< std::hex << info.typeVer << std::endl;
        std::cout << "Document Type Read Version: "<< std::hex << info.currDocVer << std::endl;
    } else {
        if (result == READ_ERROR) {
            std::cerr << "Error reading the WebM file." << std::endl;
        } else if (result == NOT_WEBM) {
            std::cerr << "The file is not a valid WebM file." << std::endl;
        } else if (result == WEBM_UNSUPPORTED) {
            std::cerr << "The WebM file version is unsupported." << std::endl;
        }
        return 1;
    }

    return 0;
}
