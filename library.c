#include "library.h"

#include <stdio.h>

int main() {
    ht* table = ht_create();

    char key[] = "key";
    char value[] = "value";

    char key2[] = "key2";
    char value2[] = "value2";

    if(table->entries[0].key != NULL) {
        printf("FAILED table entry key at 0 is %p instead of NULL", &table->entries[0].key);
    }
    else {
        printf("worked lol");
    }
    ht_insert(table, key, value);

    char* retrieved = ht_get(table, key);

    ht_insert(table, key2, value2);

    free_ht(table);
    free(retrieved);


}
