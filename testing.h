//
// Created by CatchACode on 23.12.23.
//

#ifndef HASHTABLE_TESTING_H
#define HASHTABLE_TESTING_H

const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";


/* generates a randomstring with the given length. Total length of the string is length - 1 as a null terminator is
 * added. Returns NULL if length is under 1
 */
char* randomString(ssize_t length) {
    if(length == 0) {
        return NULL;
    }
    char* randomstring = calloc(length, sizeof(char));
    if (randomstring == NULL) {
        return NULL;
    }
    for(size_t pos = 0; pos < length - 1; ++pos) {
        char c = charset[rand() % strlen(charset)];
        randomstring[pos] = c;
    }
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
    for(uint64_t i = 0; i <= num; ++i) {
        size = (uint64_t)rand() % maxKeyLength;
        array[i].key = randomString(size);
        array[i].key_size = size + 1;
        size = (uint64_t)rand() % maxValueLength;
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


#endif //HASHTABLE_TESTING_H
