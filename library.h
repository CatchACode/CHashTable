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
 * TODO: Change to new allocate a new table, but only new entries. This will hopefully fix the pointer changing
 *
 */
ht* ht_realloc(ht* table) {
    ht_Entry* entries = (ht_Entry*) calloc(table->capacity * 2, sizeof(ht_Entry));
    if (entries == NULL) {
        return NULL;
    }
    table->capacity *= 2;
    // Insert old values
    for(size_t i = 0; i < table->used; ++i) {
        ht_Entry entry = table->entries[i];
        uint64_t hash = hash_key(entry.key) % table->capacity;
        size_t entry_key_length = strlen(table->entries[i].key) + 1;
        size_t entry_bucket_length = strlen(table->entries[i].bucket) + 1;
        if (entries[hash].key == NULL) {
            entries[hash].key = (char*) calloc(entry_key_length, sizeof(char));
            entries[hash].bucket = (char*) calloc(entry_bucket_length, sizeof(char));
            strncpy(entries[hash].key, table->entries[i].key, entry_key_length);
            strncpy(entries[hash].bucket, table->entries[i].bucket, entry_bucket_length);
        }
        else {
            //TODO: Make sure we are accessing into the allocated array and not outside it
            // If we double the capacity, we should be able to move the old array to the were the new entries start
            // meaning we don't need to check for bounds for this issue.
            // But it is possible for multiple hashes to map to the end of the the array. Wrap around?
            hash += 1; // Might save us from one loop
            while((entries[hash].key != NULL) && (hash < table->capacity)) {
                hash += 1;
            }
            entries[hash].key = (char*) calloc(entry_key_length, sizeof(char));
            entries[hash].bucket = (char*) calloc(entry_bucket_length, sizeof(char));
            strncpy(entries[hash].key, table->entries[i].key, entry_key_length);
            strncpy(entries[hash].bucket, table->entries[i].bucket, entry_bucket_length);
        }
    }
    for(size_t i = 0; i < table->used; ++i) {
        free(table->entries[i].key);
        free(table->entries[i].bucket);
        table->entries[i].key = NULL;
        table->entries[i].bucket = NULL;
    }
    free(table->entries);
    table->entries = entries;
    return table;
}



/* returns a copy of the value with the key
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
    const char* rv = calloc(strlen(entry->bucket) + 1, sizeof(char));
    strcpy(rv, entry->bucket);
    return rv;
}

ht_Entry* ht_insert(ht* table, const char* key, const char* object) {
    if(table == NULL) {
        return NULL;
    }
    if(table->capacity <= table->used) {
        ht* temp = ht_realloc(table);
        table->entries = temp->entries;
        table->capacity = temp->capacity;
        table->used = temp->used;
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

ht_Entry* ht_delete(ht* table, const char* key) {
    ht_Entry* entry = calloc(1, sizeof(ht_Entry));
    uint64_t hash = hash_key(key) % table->capacity;
    ht_Entry* found = &table->entries[hash];
    if(found == NULL) {
        return NULL;
    }
    size_t sizeKey = strlen(found->key) + 1;
    size_t sizeBucket = strlen(found->bucket) + 1;
    entry->key = calloc(sizeKey, sizeof(char));
    entry->bucket = calloc(sizeBucket, sizeof(char));
    entry->key = strncpy(entry->key, found->key, sizeKey);
    entry->bucket = strncpy(entry->bucket, found->bucket, sizeBucket);
    free(found->key);
    free(found->bucket);
    found->key = NULL;
    found->bucket = NULL;
    return entry;
}



#endif //HASHTABLE_LIBRARY_H
