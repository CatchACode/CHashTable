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
    generate_pairs(&pairs, SIZE, 128, 128);

    for(int i = 0; i <= SIZE; ++i) {
        ht_insert(table, pairs[i].key, pairs[i].value);
    }

    for(int i = 0; i <= SIZE; ++i) {
        char* test = ht_cget(table, pairs[i].key);
        if(strcmp(pairs[i].value, test) == 0) {
            printf("%d Match! | Key %s:  %s == %s\n", i, pairs[i].key, pairs[i].value, test);
        } else {
            printf("%d No Match! | Key %s:  %s != %s\n", i, pairs[i].key, pairs[i].value, test);
        }
        free(test);
    }
    ht_delete(table, "key0");


    ht_free(table);
    free_pairs(&pairs, SIZE);
    exit(1);

}
