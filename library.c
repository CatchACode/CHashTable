#include "library.h"
#include "testing.h"

#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define SIZE 16



int main() {
    srand((unsigned int) time(NULL));
    ht* table = ht_create();


    pair_t pairs[SIZE];
    generate_iterative_pairs(&pairs, SIZE);

    for(int i = 0; i < SIZE; ++i) {
        ht_insert(table, pairs[i].key, pairs[i].value, pairs[i].keySize, pairs[i].valueSize);
        // Check if all pairs are still in table
        for(int j = 0; j < i; j++) {
            unsigned char* test = ht_cget(table, pairs[j].key, pairs[j].keySize);
            if(memcmp(pairs[j].value,test, pairs[j].valueSize) == 0) {
                printf("After %d was inserted, %d is still in the table\n", i, j);
            } else {
                printf("\033[1;31mAfter %d was inserted, %d was not in the table\n\033[0m", i, j);
            }
            free(test);
        }

    }

    for(int i = 0; i < SIZE; ++i) {
        unsigned char* test = ht_cget(table, pairs[i].key, pairs[i].keySize);
        if(memcmp(test, pairs[i].value, pairs[i].valueSize) == 0) {
            printf("%d Match! | Key %s:  %s == %s\n", i, pairs[i].key, pairs[i].value, test);
        } else {
            printf("%d No Match! | Key %s:  %s != %s\n", i, pairs[i].key, pairs[i].value, test);
        }
        free(test);
    }
    ht_free(table);
    free_pairs(pairs, SIZE);
    exit(1);
    ht_delete(table, pairs[0].key, pairs[0].keySize);


    ht_free(table);
    free_pairs(&pairs, SIZE);
    exit(1);

}
