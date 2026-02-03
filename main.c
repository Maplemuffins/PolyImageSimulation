#include <stdio.h>

const char *TARGET_FILE = "/Users/evan/Desktop/cat.png";;

int main(void) {
    FILE *file = fopen(TARGET_FILE, "r");

    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    unsigned char buffer[16];
    size_t offset = 0;

    for (size_t i = 0; i < 16; i++) {
        const size_t bytesRead = fread(buffer, 1, sizeof(buffer), file);
        printf("%08zx  ", offset);

        for (size_t j = 0; j < bytesRead; j++) {
            printf("%02X ", buffer[j]);
        }

        printf("\n");
        offset += bytesRead;
    }

    fclose(file);
    return 0;
}

