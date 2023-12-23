#include "library.h"

#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define SIZE 1

const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

char* randomString(ssize_t maxSize) {
    uint64_t length = maxSize;

    char* randomstring = calloc(length + 1, sizeof(char));
    if (randomstring == NULL) {
        return NULL;
    }
    for(size_t pos = 0; pos <= length; ++pos) {
        char c = charset[rand() % strlen(charset)];
        randomstring[pos] = c;
    }
    randomstring[length + 1] = '\0';
    return randomstring;
}

void print_buffer(char* buffer, size_t size) {
    char* hexes = calloc((size * 2) + 1, sizeof(char));

    for(size_t pos = 0; pos < size; ++pos) {
        snprintf(hexes + pos*2, 3, "%02X", buffer[pos]);
    }
    printf("%s\n", hexes);
    free(hexes);
    return;
}



typedef struct _pair {
    char* key;
    size_t key_size;
    char* value;
    size_t value_size;
} pair_t;

pair_t* generate_pairs(pair_t* array,uint64_t num, size_t maxKeyLength, size_t maxValueLength) {
    size_t size = 0;
    for(uint64_t i = 0; i < num; ++i) {
        size = (uint32_t)rand() % maxKeyLength + 1;
        array[i].key = randomString(size);
        array[i].key_size = size + 1;
        size = (uint64_t)rand() % maxValueLength + 1;
        array[i].value = randomString(size);
        array[i].value_size = size + 1;
    }
    return array;
}

pair_t* generate_iterative_pairs(pair_t* array, uint64_t num) {
    const char* key = "key";
    const char* value = "value";
    char* number = calloc(21, sizeof(char));
    for(uint64_t i = 0; i <= num; ++i) {
        snprintf(number, 21, "%u", i);
        size_t numLength = strlen(number);
        array[i].key = calloc(5 + numLength, sizeof(char)); // key = strlen(key) + null terminator + numLength
        strncat(array[i].key, key, 5 + numLength);
        strncat(array[i].key, number, 5 + numLength);
        array[i].key_size = 5 + numLength;

        array[i].value = calloc(6 + numLength, sizeof(char));
        strncat(array[i].value, value, 6 + numLength);
        strncat(array[i].value, number, 6 + numLength);
        array[i].value_size = 6 + numLength;
    }
    free(number);
    return array;
}

void free_pairs(pair_t array[], size_t num) {
    for (uint64_t i = 0; i <= num; ++i) {
        free(array[i].value);
        free(array[i].key);
    }
}




int main() {
    srand((unsigned int) time(NULL));
    ht* table = ht_create();

    pair_t pairs[SIZE];
    generate_iterative_pairs(&pairs, 16);

    for(int i = 0; i <= SIZE; ++i) {
        ht_insert(table, pairs[i].key, pairs[i].value);
    }
    char* test = ht_get(table, pairs[0].key);
    free(test);
    test = ht_get(table, pairs[16].key);
    free(test);
    test = ht_get(table, pairs[8].key);
    free(test);
    test = ht_get(table, pairs[9].key);
    free(test);
    test = ht_get(table, pairs[1].key);
    ht_free(table);
    free_pairs(&pairs, SIZE);
    exit(1);
    for(int i = 0; i <= SIZE; ++i) {
        char* test = ht_get(table, pairs[i].key);
        if(strcmp(pairs[i].value, test) == 0) {
            printf("%d Match! | Key %s:  %s == %s\n", i, pairs[i].key, pairs[i].value, test);
        } else {
            printf("%d No Match! | Key %s:  %s != %s\n", i, pairs[i].key, pairs[i].value, test);
        }
        free(test);
    }

}
