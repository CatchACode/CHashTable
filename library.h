#ifndef HASHTABLE_LIBRARY_H
#define HASHTABLE_LIBRARY_H

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define FNVOFFSETBASIS  0xcbf29ce484222325
#define FNVPRIME        0x00000100000001B3
#define STARTINGSIZE    0x1


static uint64_t hash_key(const char* buf) {
    uint64_t hash = FNVOFFSETBASIS;
    for(const char* pos = buf; *pos != '\0'; ++pos) {
        hash = hash * FNVPRIME;
        hash = hash ^ (uint64_t)pos;
    }
    return hash;
}

typedef struct _ht_Entry {
    const char* key;
    char* bucket;
} ht_Entry;

typedef struct _ht{
    size_t used;
    size_t capacity;
    ht_Entry* entries;
} ht;


ht* ht_create() {
    ht* table = calloc(1, sizeof(ht));
    if(table == NULL) {
        return NULL;
    }
    table->capacity = (size_t) STARTINGSIZE;
    table->entries = (ht_Entry*) calloc(STARTINGSIZE, sizeof(ht_Entry));
    if(table->entries == NULL) {
        return NULL;
    }
    table->used = 0;
    return table;
}

void free_ht(ht* table) {
    for(size_t i = 0; i < table->capacity; i++) {
        if(table->entries[i].key == NULL)
        free(table->entries[i].bucket);
    }
    free(table->entries);
    free(table);
}

ht* ht_realloc(ht* table) {
    ht* new_table = (ht*) calloc(1, sizeof(ht));
    if (new_table == NULL) {
        return NULL;
    }
    new_table->used = table->used;
    new_table->capacity = table->capacity * 2;
    new_table->entries = (ht_Entry*) calloc(table->capacity * 2, sizeof(ht_Entry));
    if (new_table->entries == NULL) {
        return NULL;
    }

    // Insert old values
    for(size_t i = 0; i < table->used; ++i) {
        ht_Entry entry = table->entries[i];
        uint64_t hash = hash_key(entry.key) % new_table->capacity;
        if (new_table->entries[hash].key == NULL) {
            memcpy((void*)new_table->entries[hash].key, table->entries[i].key, strlen(table->entries[i].key));
            memcpy((void*)new_table->entries[hash].bucket, table->entries[i].bucket, strlen(table->entries[i].bucket));
        }
        else {
            hash += 1; // Might save us from one loop
            while(new_table->entries[hash].key != NULL) {
                hash += 1;
            }
            memcpy((void*)new_table->entries[hash].key, table->entries[i].key, strlen(table->entries[i].key));
            memcpy((void*)new_table->entries[hash].bucket, table->entries[i].bucket, strlen(table->entries[i].bucket));
        }
        new_table->used += 1;
    }
    free_ht(table);
    return new_table;
}



/* returns a void* pointer to the
 *
 */
const char* ht_get(ht* table, const char* key) {
    uint64_t hash = hash_key(key) % table->capacity;
    if(hash >= table->used) {
        return NULL;
    }
    ht_Entry* entry = &table->entries[hash];
    if(entry->key == NULL) {
        return NULL;
    }
    if((hash_key(entry->key) % table->capacity) != hash) {
        // We iterate to the next entry to see if it matches and continue if not, till the reach the used or capacity
        while(!strcmp(table->entries[hash].key, key)) {
            hash += 1;
        }
        entry = &table->entries[hash];
    }
    return entry->bucket;
}

ht_Entry* ht_insert(ht* table, const char* key, void* object) {
    if(table == NULL) {
        return NULL;
    }
    if(table->capacity <= table->used) {
        table = ht_realloc(table);
        if (table == NULL) {
            return NULL;
        }
        return ht_insert(table, key, object);
    }

    uint64_t hash = hash_key(key) % table->capacity;
    while(table->entries[hash].key != NULL) {
        hash += 1;
    }
    table->entries[hash].key = (char*) calloc(strlen(key) + 1,sizeof(char));
    table->entries[hash].bucket = (char*) calloc(strlen(object) + 1, sizeof(char));
    table->entries[hash].key = strcpy((char*)table->entries[hash].key, key);
    table->entries[hash].bucket = strcpy((char*)table->entries[hash].bucket, object);
    table->used += 1;
    return &table->entries[hash];
}



#endif //HASHTABLE_LIBRARY_H
