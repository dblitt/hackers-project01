#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdint.h>

// Opaque struct (forward declaration)
typedef struct HashTable HashTable;

// Function prototypes for creating, inserting, searching, deleting, and freeing the hash table
HashTable* hashtable_create(void);
void hashtable_insert(HashTable* table, int key, uint64_t value);
uint64_t hashtable_search(HashTable* table, int key);
void hashtable_delete(HashTable* table, int key);
void hashtable_free(HashTable* table);

#endif // HASHTABLE_H
