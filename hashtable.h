#ifndef HASHTABLE_HASHTABLE_H
#define HASHTABLE_HASHTABLE_H

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#define FNVOFFSETBASIS  0xcbf29ce484222325
#define FNVPRIME        0x00000100000001B3
#define STARTINGSIZE    1

typedef struct ht_Entry {
    void* key;
    size_t keySize;
    void* bucket;
    size_t bucketSize;
} ht_Entry;

typedef struct ht{
    size_t used;
    size_t capacity;
    ht_Entry* entries;
} ht;


static uint64_t hash_key(const void* buffer, size_t bufferSize) {
    uint64_t hash = FNVOFFSETBASIS;
    for (const char* p = buffer; p != buffer + bufferSize; p++) {
        hash ^= (uint64_t)(unsigned char)(*p);
        hash *= FNVPRIME;
    }
    return hash;
}


__attribute__((unused)) static uint64_t test_hash(const char* buf, size_t size, bool set, uint64_t settee) {
    if (set == true) {
        return hash_key(buf, size);
    } else {
        return settee;
    }
}


// frees all data in an entry, but not the entry itself
void ht_entry_free(ht_Entry* entry) {
    if(entry == NULL) {
        return;
    }
    if(entry->key != NULL) {
        free(entry->key);
        entry->key = NULL;
        entry->keySize = 0;
    }
    if(entry->bucket != NULL) {
        free(entry->bucket);
        entry->bucket = NULL;
        entry->bucketSize = 0;
    }
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
        ht_entry_free(&table->entries[i]);
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
        if(entry.key != NULL) {
            uint64_t starting_hash = hash_key(entry.key, entry.keySize) % (table->capacity * 2);
            uint64_t hash = starting_hash;
            if (new_entries[hash].key != NULL) { // Need to find next empty slot
                do {
                    hash += 1;
                    if (hash >= (table->capacity * 2)) {
                        hash = 0;
                    }
                } while (new_entries[hash].key != NULL &&
                         hash != starting_hash); // Use do while loop to detect looping in ht
            }
            new_entries[hash].key = malloc(entry.keySize);
            new_entries[hash].bucket = malloc(entry.bucketSize);
            new_entries[hash].keySize = entry.keySize;
            new_entries[hash].bucketSize = entry.bucketSize;
            memcpy(new_entries[hash].key, entry.key, entry.keySize);
            memcpy(new_entries[hash].bucket, entry.bucket, entry.bucketSize);
        }
    }
    for(size_t i = 0; i < table->capacity; ++i) { // free table
        if(table->entries[i].key != NULL) {
            free(table->entries[i].key);
        }
        if(table->entries[i].bucket != NULL) { // Should not happen separately from key == NULL
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

// returns a copy of the value with the key, null if not found
void* ht_cget(ht* table, const void* key, size_t keySize) {
    uint64_t hash = hash_key(key, keySize) % table->capacity;

    while(table->entries[hash].key != NULL) {
        if((table->entries[hash].keySize == keySize) && (memcmp(table->entries[hash].key, key, keySize) == 0)) {
            //Found entry
            void* rv = malloc(table->entries[hash].bucketSize);
            memcpy(rv, table->entries[hash].bucket, table->entries[hash].bucketSize);
            return rv;
        }
        hash += 1;
        if(hash >= table->capacity) { // At end of table wraparound
            hash = 0;
        }
    }
    return NULL;

}

// returns a pointer to the bucket assign to key, null if not found
const char* ht_get(ht* table, const void* key, size_t keySize) {
    uint64_t hash = hash_key(key, keySize) % table->capacity;

    while(table->entries[hash].key != NULL) {
        if(strcmp(table->entries[hash].key, key)) {
            return table->entries->bucket;
        }
    }
}
/* Inserts a key-value pair into the table. If the key already exists, the value is updated. Returns NULL on error
 *
 */
ht_Entry* ht_insert(ht* table, const void* key, const void* object, const size_t keySize, const size_t objectSize) {
    if((table == NULL) || (key == NULL )) {
        return NULL;
    }
    if(table->capacity <= table->used) {
        table = ht_realloc(table);
        if (table == NULL) {
            return NULL;
        }
    }
    uint64_t hash = hash_key(key, keySize) % table->capacity;
    while(table->entries[hash].key != NULL) { // don't need a loop check, as capacity > used, therefore there must be a free slot before we reach the starting hash
        if((table->entries[hash].keySize == keySize) && (memcmp(table->entries[hash].key, key, keySize) == 0)) {
            // keySizes matches => Keys might match => might need to update entry instead of inserting
            free(table->entries[hash].bucket);
            table->entries[hash].bucket = malloc(objectSize);
            table->entries[hash].bucketSize = objectSize;
            memcpy(table->entries[hash].bucket, object, objectSize);
            return &table->entries[hash];
        }
        hash += 1;
        if(hash >= table->capacity) { // Wrap around
            hash = 0;
        }
    }
    table->entries[hash].key = malloc(keySize);
    table->entries[hash].bucket = malloc(objectSize);
    table->entries[hash].keySize = keySize;
    table->entries[hash].bucketSize = objectSize;
    memcpy(table->entries[hash].key, key, keySize);
    memcpy(table->entries[hash].bucket, object, objectSize);
    table->used += 1;

    return &table->entries[hash];
}

// deletes an entry by key, returns a copy of the value or NULL if not found
char* ht_cdelete(ht* table, const char* key, size_t keySize) {
    uint64_t starting_hash = hash_key(key, keySize) % table->capacity;
    uint64_t hash = starting_hash;
    do {
        if (table->entries[hash].key == NULL) { // needed as do while always executes the do once
            return NULL;
        }
        if((table->entries[hash].keySize == keySize) && (memcmp(table->entries[hash].key, key, keySize) == 0)) {
            const char* rv = malloc(table->entries[hash].bucketSize);
            memcpy(rv, table->entries[hash].bucket, table->entries[hash].bucketSize);
            ht_entry_free(&table->entries[hash]);
            return rv;
        }
        hash += 1;
        if(hash >= table->capacity) {
            hash = 0;
        }
    } while(table->entries[hash].key != NULL && hash != starting_hash);

    return NULL;
}
// deletes an entry
void ht_delete(ht* table, unsigned char* key, size_t keySize) {
    uint64_t starting_hash = hash_key(key, keySize) % table->capacity;
    uint64_t hash = starting_hash;
    do {
        if(table->entries[hash].key == NULL) {
            return;
        }
        if((table->entries[hash].keySize == keySize) && (memcmp(table->entries[hash].key, key, keySize) == 0)) {
            ht_entry_free(&table->entries[hash]);
            return;
        }
        hash += 1;
        if(hash >= table->capacity) {
            hash = 0;
        }
    } while((table->entries[hash].key != NULL) && (hash != starting_hash));

    return;
}


#endif //HASHTABLE_HASHTABLE_H
