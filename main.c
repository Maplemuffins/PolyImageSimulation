#include <stdio.h>
#include <stdint.h>

const char *TARGET_FILE = "/Users/evan/Desktop/cat.png";;
const unsigned char PNG_SIGNATURE[] = {137, 80, 78, 71, 13, 10, 26, 10};
const int PNG_SIGNATURE_SIZE = sizeof(PNG_SIGNATURE);

//Chunking CONST
const unsigned char IEND_CHUNK_SIGNATURE[] = {73, 69, 78, 68};
const int CHUNK_LENGTH_BYTES = 4;
const int CHUNK_TYPE_BYTES = 4;
const int CHUNK_CVC_BYTES = 4;
const int BYTE_SIZE = 8;
const int DATA_DISPLAY_COUNT = 20;

typedef enum {
    STATUS_ERROR,
    STATUS_SUCCESS,
    STATUS_END
} Chunk_Status;

// Detects if a file is a PNG
// - Assumes that this program is run when file is just opened or is at byte 0
// - Assumes that the file is open
// - returns 1 if file is PNG, 0 if file is not a PNG
int is_png(FILE *file) {
    unsigned char buffer[PNG_SIGNATURE_SIZE];
    if (fread(buffer, 1, PNG_SIGNATURE_SIZE, file) != PNG_SIGNATURE_SIZE) {
        return 0;
    }

    for (size_t i = 0; i < PNG_SIGNATURE_SIZE; i++) {
        if (buffer[i] != PNG_SIGNATURE[i]) {
            return 0;
        }
    }
    return 1;
}

// Prints the details of a chunk in a readable format
// - Assumes that the file is at a line which a chunk starts
// - Assumes that the file is open
// - returns 1 if succeeded, returns 0 if failed
Chunk_Status read_chunk(FILE *file) {
    unsigned char length[CHUNK_LENGTH_BYTES];
    unsigned char type[CHUNK_TYPE_BYTES];
    unsigned char cvc[CHUNK_CVC_BYTES];

    // First read the first 4 bytes in the chunk, describing the length of the data
    if (fread(length, 1, CHUNK_LENGTH_BYTES, file) != CHUNK_LENGTH_BYTES) {
        return STATUS_ERROR;
    }
    uint32_t data_length = ((uint32_t)length[0] << 24) |
           ((uint32_t)length[1] << 16) |
           ((uint32_t)length[2] << 8)  |
           ((uint32_t)length[3]);

    // Second read the next 4 bytes in the chunk, describing the type of data
    if (fread(type, 1, CHUNK_TYPE_BYTES, file) != CHUNK_TYPE_BYTES) {
        return STATUS_ERROR;
    }

    // Third read the data in the chunk
    unsigned char data[data_length];
    if (fread(data, 1, data_length, file) != data_length) {
        return STATUS_ERROR;
    }

    // Finally read the cvc in the chunk
    if (fread(cvc, 1, CHUNK_CVC_BYTES, file) != CHUNK_CVC_BYTES) {
        return STATUS_ERROR;
    }

    // Print chunk data in readable format
    printf("Type: ");
    char is_end_chunk = 1;
    for (int i = 0; i < CHUNK_TYPE_BYTES; i++) {
        printf("%c", type[i]);
        // Also checks if chunk is classified as an "end chunk" which appears at the end of a png file
        if (type[i] != IEND_CHUNK_SIGNATURE[i]) is_end_chunk = 0;
    }
    printf(" | Length: %u | CVC: " , data_length);
    for (int i = 0; i < CHUNK_CVC_BYTES; i++) {
        printf("%02X", cvc[i]);
    }
    printf(" | ");
    for (int i = 0; i < data_length && i < DATA_DISPLAY_COUNT; i++) {
        printf("%02X ", data[i]);
    }
    if (data_length > DATA_DISPLAY_COUNT) {
        printf("...");
    }
    printf("\n");

    if (is_end_chunk) return STATUS_END;
    return STATUS_SUCCESS;
}

int main(void) {
    FILE *file = fopen(TARGET_FILE, "r");

    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    if (!is_png(file)) {
        perror("Error in file classification");
        return 1;
    }

    Chunk_Status status = STATUS_SUCCESS;
    while (status == STATUS_SUCCESS) {
        status = read_chunk(file);
        if (status == STATUS_ERROR) {
            perror("Error reading chunk");
            return 1;
        }
    }

    fclose(file);
    return 0;
}

