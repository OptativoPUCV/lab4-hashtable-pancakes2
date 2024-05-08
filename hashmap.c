#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "hashmap.h"


typedef struct HashMap HashMap;
int enlarge_called=0;

struct HashMap {
    Pair ** buckets;
    long size; //cantidad de datos/pairs en la tabla
    long capacity; //capacidad de la tabla
    long current; //indice del ultimo dato accedido
};

Pair * createPair( char * key,  void * value) {
    Pair * new = (Pair *)malloc(sizeof(Pair));
    new->key = key;
    new->value = value;
    return new;
}

long hash( char * key, long capacity) {
    unsigned long hash = 0;
     char * ptr;
    for (ptr = key; *ptr != '\0'; ptr++) {
        hash += hash*32 + tolower(*ptr);
    }
    return hash%capacity;
}

int is_equal(void* key1, void* key2){
    if(key1==NULL || key2==NULL) return 0;
    if(strcmp((char*)key1,(char*)key2) == 0) return 1;
    return 0;
}


void insertMap(HashMap * map, char * key, void * value) {
    long idx = hash(key, map->capacity);
    while (map->buckets[idx] != NULL) {
        if (is_equal(map->buckets[idx]->key, key)) {
            // If the key already exists in the map, update its value
            map->buckets[idx]->value = value;
            return;
        }
        idx = (idx + 1) % map->capacity; // Linear probing
    }
    map->buckets[idx] = createPair(key, value);
    map->size++;

    // If the load factor exceeds 0.7, enlarge the map
    if ((double)map->size / map->capacity > 0.7) {
        enlarge(map);
    }
}

void enlarge(HashMap * map) {
    enlarge_called = 1; //no borrar (testing purposes)

    long new_capacity = map->capacity * 2;
    HashMap *new_map = createMap(new_capacity);

    for (long i = 0; i < map->capacity; i++) {
        Pair *pair = map->buckets[i];
        if (pair != NULL) {
            insertMap(new_map, pair->key, pair->value);
        }
    }

    // Free the old map's buckets array, but not the pairs themselves
    free(map->buckets);

    // Copy the new map's data over to the old map
    map->buckets = new_map->buckets;
    map->capacity = new_map->capacity;
    map->size = new_map->size;

    // Free the new map structure (but not the buckets array, which we're still using)
    free(new_map);
}


HashMap * createMap(long capacity) {
    HashMap *map = (HashMap *)malloc(sizeof(HashMap));
    if (map == NULL) {
        return NULL; // memory allocation failed
    }

    map->buckets = (Pair **)calloc(capacity, sizeof(Pair *));
    if (map->buckets == NULL) {
        free(map); // free the previously allocated memory
        return NULL; // memory allocation failed
    }

    map->size = 0;
    map->capacity = capacity;
    map->current = -1; // set to -1 as no data has been accessed yet

    return map;
}

void eraseMap(HashMap * map, char * key) {
    long idx = hash(key, map->capacity);
    while (map->buckets[idx] != NULL) {
        if (is_equal(map->buckets[idx]->key, key)) {
            // If the key exists in the map, erase its pair
            free(map->buckets[idx]);
            map->buckets[idx] = NULL;
            map->size--;
        }
    }
}

Pair * searchMap(HashMap * map,  char * key) {   
    long idx = hash(key, map->capacity);
    while (map->buckets[idx] != NULL) {
        if (is_equal(map->buckets[idx]->key, key)) {
            // If the key exists in the map, return its pair
            map->current = idx;
            return map->buckets[idx];
        }
        idx = (idx + 1) % map->capacity; // Linear probing
    }
    return NULL; // Key not found
}

Pair * firstMap(HashMap * map) {
    for (long i = map->current; i < map->capacity; i++) {
        if (map->buckets[i] != NULL) {
            map->current = i;
            return map->buckets[i];
        }
    }
    return NULL; // No non-NULL pairs found
}

Pair * nextMap(HashMap * map) {
    for (long i = map->current + 1; i < map->capacity; i++) {
        if (map->buckets[i] != NULL) {
            map->current = i;
            return map->buckets[i];
        }
    }
    return NULL; // No more non-NULL pairs found
}
