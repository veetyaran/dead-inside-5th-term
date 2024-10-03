#include <iostream>
#include <cstdio>
#include <cstdint>
#include <string>

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

    // Считывание магии, 1A 45 DF A3 у webm
    unsigned int mag;
    if (fread(&mag, sizeof(mag), 1, f) != 1) return READ_ERROR;
    if (mag != 0x1A45DFA3) return NOT_WEBM;

    // Пример чтения дополнительных данных
    // Для простоты пример не полный и предполагает, что все числа в файле имеют фиксированную длину
    if (fread(&webm->EBMLver, sizeof(webm->EBMLver), 1, f) != 1) return READ_ERROR;
    if (fread(&webm->currVer, sizeof(webm->currVer), 1, f) != 1) return READ_ERROR;
    if (fread(&webm->MaxID, sizeof(webm->MaxID), 1, f) != 1) return READ_ERROR;
    if (fread(&webm->MaxLen, sizeof(webm->MaxLen), 1, f) != 1) return READ_ERROR;

    // Чтение docType как строки
    char typeBuffer[100];
    if (fread(typeBuffer, sizeof(char), 100, f) != 100) return READ_ERROR;
    webm->docType = std::string(typeBuffer);

    if (fread(&webm->typeVer, sizeof(webm->typeVer), 1, f) != 1) return READ_ERROR;
    if (fread(&webm->currDocVer, sizeof(webm->currDocVer), 1, f) != 1) return READ_ERROR;

    return WEBM_SUCCESS;
}


