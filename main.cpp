//
// Created by IvanBrekman on 21.11.2021
//

#include <cstdio>

#include "libs/baselib.h"
#include "libs/tree.h"

int main(int argc, char** argv) {
    char* source_file = (char*) calloc_s(MAX_FILEPATH_SIZE, sizeof(char));

    if (argc > 1) {
        source_file = argv[1];
    } else {
        printf("Введите путь к файлу с функцией:\n");
        scanf("%s", source_file);
    }

        system("gcc differentiator/differentiator.cpp libs/baselib.cpp libs/tree.cpp -lstdc++ -lm -o differentiator/differentiator.out");
    SPR_SYSTEM("./differentiator/differentiator.out %s", source_file);

    return 0;
}
