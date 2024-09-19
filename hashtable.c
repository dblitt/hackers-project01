#include "hashtable.h"
#include <stdio.h>
#include <stdlib.h>

#define INITIAL_TABLE_SIZE 8
#define LOAD_FACTOR_THRESHOLD 0.75

typedef struct Entry {
    int key;
    uint64_t value;
    struct Entry* next;
} Entry;

struct HashTable {
    Entry** entries;
    int size;  // The current size of the table (number of buckets)
    int count; // The number of entries currently in the table
};

// Hash function
static unsigned int hash(int key, int table_size) {
    return key % table_size;
}

// Create a new entry
static Entry* create_entry(int key, uint64_t value) {
    Entry* entry = (Entry*) malloc(sizeof(Entry));
    entry->key = key;
    entry->value = value;
    entry->next = NULL;
    return entry;
}

// Resize the hash table when the load factor exceeds the threshold
static void resize_table(HashTable* table) {
    int new_size = table->size * 2;
    Entry** new_entries = (Entry**) malloc(new_size * sizeof(Entry*));
    for (int i = 0; i < new_size; i++) {
        new_entries[i] = NULL;
    }

    // Rehash all the old entries into the new table
    for (int i = 0; i < table->size; i++) {
        Entry* entry = table->entries[i];
        while (entry != NULL) {
            Entry* next = entry->next;
            unsigned int new_index = hash(entry->key, new_size);

            // Insert into the new table
            entry->next = new_entries[new_index];
            new_entries[new_index] = entry;
            entry = next;
        }
    }

    // Replace the old entries with the new resized entries
    free(table->entries);
    table->entries = new_entries;
    table->size = new_size;
}

// Create a new hash table
HashTable* hashtable_create(void) {
    HashTable* table = (HashTable*) malloc(sizeof(HashTable));
    table->size = INITIAL_TABLE_SIZE;
    table->count = 0;
    table->entries = (Entry**) malloc(table->size * sizeof(Entry*));
    for (int i = 0; i < table->size; i++) {
        table->entries[i] = NULL;
    }
    return table;
}

// Insert key-value pair into the hash table
void hashtable_insert(HashTable* table, int key, uint64_t value) {
    // Check if we need to resize the table
    if ((float) table->count / table->size > LOAD_FACTOR_THRESHOLD) {
        resize_table(table);
    }

    unsigned int index = hash(key, table->size);
    Entry* entry = table->entries[index];

    // Check if key already exists and update its value
    while (entry != NULL) {
        if (entry->key == key) {
            entry->value = value;
            return;
        }
        entry = entry->next;
    }

    // If key doesn't exist, create a new entry
    Entry* new_entry = create_entry(key, value);
    new_entry->next = table->entries[index];
    table->entries[index] = new_entry;
    table->count++;
}

// Search for a value by key in the hash table
uint64_t hashtable_search(HashTable* table, int key) {
    unsigned int index = hash(key, table->size);
    Entry* entry = table->entries[index];

    while (entry != NULL) {
        if (entry->key == key) {
            return entry->value;
        }
        entry = entry->next;
    }

    // If key not found
    return 0;  // Return 0 or any invalid value to represent not found
}

// Delete an entry by key
void hashtable_delete(HashTable* table, int key) {
    unsigned int index = hash(key, table->size);
    Entry* entry = table->entries[index];
    Entry* prev = NULL;

    while (entry != NULL) {
        if (entry->key == key) {
            if (prev == NULL) {
                // Head of the list
                table->entries[index] = entry->next;
            } else {
                // Middle or end of the list
                prev->next = entry->next;
            }
            free(entry);
            table->count--;
            return;
        }
        prev = entry;
        entry = entry->next;
    }
}

// Free the memory allocated for the hash table
void hashtable_free(HashTable* table) {
    for (int i = 0; i < table->size; i++) {
        Entry* entry = table->entries[i];
        while (entry != NULL) {
            Entry* tmp = entry;
            entry = entry->next;
            free(tmp);
        }
    }
    free(table->entries);
    free(table);
}
