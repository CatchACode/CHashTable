#ifndef HASHTABLE_LIBRARY_H
#define HASHTABLE_LIBRARY_H

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#define FNVOFFSETBASIS  0xcbf29ce484222325
#define FNVPRIME        0x00000100000001B3
#define STARTINGSIZE    1

typedef struct ht_Entry {
    const char* key;
    char* bucket;
} ht_Entry;

typedef struct ht{
    size_t used;
    size_t capacity;
    ht_Entry* entries;
} ht;


static uint64_t hash_key(const char* key) {
    uint64_t hash = FNVOFFSETBASIS;
    for (const char* p = key; *p; p++) {
        hash ^= (uint64_t)(unsigned char)(*p);
        hash *= FNVPRIME;
    }
    return hash;
}


__attribute__((unused)) static uint64_t test_hash(const char* buf, size_t size, bool set, uint64_t settee) {
    if (set == true) {
        return hash_key(buf);
    } else {
        return settee;
    }
}



void ht_entry_free(ht_Entry* entry) {
    if(entry == NULL) {
        return;
    }
    if(entry->key != NULL) {
        free(entry->key);
    }
    if(entry->bucket != NULL) {
        free(entry->bucket);
    }
    free(entry);
    return;
}

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

void ht_free(ht* table) {
    for(size_t i = 0; i < table->capacity; i++) {
        if(table->entries[i].key == NULL) {
            continue;
        }
        free(table->entries[i].key);
        table->entries[i].key = NULL;
        if(table->entries[i].bucket == NULL) {
            continue;
        }
        free(table->entries[i].bucket);
        table->entries[i].bucket = NULL;
    }
    free(table->entries);
    table->entries = NULL;
    free(table);
    table = NULL;
    return;
}
/*
 *
 *
 */
ht* ht_realloc(ht* table) {
    ht_Entry* new_entries = (ht_Entry*) calloc(table->capacity * 2, sizeof(ht_Entry));
    if (new_entries == NULL) {
        return NULL;
    }

    // Insert old values
    for(size_t i = 0; i < table->capacity; ++i) {
        ht_Entry entry = table->entries[i];
        if(entry.key == NULL) { // no entry in slot
            continue;
        }
        uint64_t hash = hash_key(entry.key) % (table->capacity * 2);
        size_t entry_key_length = strlen(entry.key) + 1;
        size_t entry_bucket_length = strlen(entry.bucket) + 1;

        while(new_entries[hash].key != NULL) {
            hash += 1;
            if (hash == table->capacity) { // Reached end of table without finding a spot for separate chaining
                for(uint64_t j = 0; j < table->capacity * 2; ++j) { // free allocated new entries
                    ht_entry_free(&new_entries[j]);
                }
                free(new_entries);
                table->capacity *= 2;
                return ht_realloc(table); // as table->entries have not been modified we can double the capacity and call realloc again
            }
        }
        new_entries[hash].key = calloc(entry_key_length, sizeof(char));
        new_entries[hash].bucket = (char*) calloc(entry_bucket_length, sizeof(char));
        strncpy(new_entries[hash].key, table->entries[i].key, entry_key_length);
        strncpy(new_entries[hash].bucket, table->entries[i].bucket, entry_bucket_length);
    }

    for(size_t i = 0; i < table->capacity; ++i) { //
        if(table->entries[i].key != NULL) {
            free(table->entries[i].key);
        }
        if(table->entries[i].bucket != NULL) {
            free(table->entries[i].bucket);
        }
        table->entries[i].key = NULL;
        table->entries[i].bucket = NULL;
    }
    free(table->entries);
    table->entries = NULL;
    table->entries = new_entries;
    table->capacity *= 2;

    float load = (float)table->used / (float) table->capacity;
    if(load >= 0.75f) {
        return ht_realloc(table);
    }
    return table;
}



/* returns a copy of the value with the key
 *
 */
const char* ht_get(ht* table, const char* key) {
    uint64_t hash = hash_key(key) % table->capacity;

    while(table->entries[hash].key != NULL) {
        if(strcmp(table->entries[hash].key, key) == 0) {
            //Found entry
            const char* rv = calloc(strlen(table->entries[hash].bucket), sizeof(char));
            return rv;
        }
        hash += 1;
        if(hash >= table->capacity) { // At end of table wraparound
            hash = 0;
        }
    }
    return NULL;

}



ht_Entry* ht_insert(ht* table, const char* key, const char* object) {
    if(table == NULL) {
        return NULL;
    }
    if(table->capacity <= table->used) {
        table = ht_realloc(table);
        if (table == NULL) {
            return NULL;
        }
    }

    uint64_t hash = hash_key(key) % table->capacity;
    while(table->entries[hash].key != NULL) {
        hash += 1;
        if(hash>= table->capacity) { // Realloc and hope hash does not collide again
            table = ht_realloc(table);
            hash = hash_key(key) % table->capacity;
        }
    }
    table->entries[hash].key = (char*) calloc(strlen(key) + 1,sizeof(char));
    table->entries[hash].bucket = (char*) calloc(strlen(object) + 1, sizeof(char));
    table->entries[hash].key = strcpy((char*)table->entries[hash].key, key);
    table->entries[hash].bucket = strcpy((char*)table->entries[hash].bucket, object);
    table->used += 1;

    return &table->entries[hash];
}


__attribute__((unused)) const char* ht_delete(ht* table, const char* key) {
    uint64_t hash = hash_key(key) % table->capacity;
    ht_Entry* found = &table->entries[hash];
    if(found == NULL) {
        return NULL;
    }
    size_t sizeBucket = strlen(found->bucket) + 1;
    const char* bucket = calloc(sizeBucket, sizeof(char));
    bucket = strncpy(bucket, found->bucket, sizeBucket);
    free(found->key);
    free(found->bucket);
    found->key = NULL;
    found->bucket = NULL;
    return bucket;
}



#endif //HASHTABLE_LIBRARY_H
