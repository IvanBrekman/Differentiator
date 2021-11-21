//
// Created by IvanBrekman on 21.11.2021
//

#include "../libs/baselib.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        PRINT_WARNING("Cant parse source_file arg");
        return -1;
    }

    printf("Diffirentiator. File path: %s\n", argv[1]);

    return 0;
}
