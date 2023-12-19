#ifndef HASHTABLE_LIBRARY_H
#define HASHTABLE_LIBRARY_H

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define FNVOFFSETBASIS  0xcbf29ce484222325
#define FNVPRIME        0x00000100000001B3
#define STARTINGSIZE    128


static uint64_t hash_key(const char* buf) {
    uint64_t hash = FNVOFFSETBASIS;
    for(const char* pos = buf; pos != NULL; ++pos) {
        hash = hash * FNVPRIME;
        hash = hash ^ (uint64_t)pos;
    }
    return hash;
}

typedef struct _ht_Entry {
    const char* key;
    void* bucket;
} ht_Entry;

typedef struct _ht{
    ht_Entry* entries;
    size_t capacity;
    size_t length;

    size_t index;
} ht;


ht* create_ht() {
    ht* ht = calloc(1, sizeof(ht));
    if(ht == NULL) {
        return NULL;
    }
    ht->capacity = STARTINGSIZE;
    ht->entries = calloc(STARTINGSIZE, sizeof(ht_Entry));
    if(ht->entries == NULL) {
        return NULL;
    }
    ht->length = 0;
    return ht;
}

void free_ht(ht* table) {
    for(size_t i = 0; i < table->capacity; i++) {
        free(table->entries[i].bucket);
    }
    free(table->entries);
    free(table);
}

ht* ht_realloc(ht* table) {
    ht* new_table = (ht*) calloc(1, sizeof(ht));
    new_table->length = table->length;
    new_table->capacity = table->capacity * 2;
    // Insert old values
    for(size_t i = 0; i < table->length; ++i) {
        uint64_t hash = hash_key()
    }
    return NULL;
}



/* returns a void* pointer to the
 *
 */
void* ht_get(ht* table, const char* key) {
    uint64_t hash = hash_key(key) % table->capacity;
    if(hash >= table->length) {
        return NULL;
    }
    ht_Entry* entry = &table->entries[hash];
    if(entry->key == NULL) {
        return NULL;
    }
    if((hash_key(entry->key) % table->capacity) != hash) {
        // We iterate to the next entry to see if it matches and continue if not, till the reach the length or capacity
    }
    return entry->bucket;
}

ht_Entry* ht_insert(ht* table, const char* key, void* object) {
    if(table == NULL) {
        return NULL;
    }
    if(table->capacity <= table->length) {
        table = ht_realloc(table);
        if (table == NULL) {
            return NULL;
        }
    }
    uint64_t hash = hash_key(key) % table->capacity;

}



#endif //HASHTABLE_LIBRARY_H
