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

    // Se obtiene el indice de la llave con la función hash
    long idx = hash(key, map->capacity);

    // Se itera sobre el mapa hasta encontrar un bucket nulo
    while (map->buckets[idx] != NULL) {

        // Si la llave ya existe en el mapa, se actualiza su valor
        if (is_equal(map->buckets[idx]->key, key)) {
            map->buckets[idx]->value = value;
            return;
        }
        idx = (idx + 1) % map->capacity; // Se realiza linear probing para avanzar el mapa
    }

    // Se crea un nuevo par y se inserta en el mapa
    map->buckets[idx] = createPair(key, value);
    map->size++;

    // Si supera el factor de carga, se duplica la capacidad del mapa
    if ((double)map->size / map->capacity > 0.7) {
        enlarge(map);
    }
}

void enlarge(HashMap * map) {

    // Lo dejaré como esta esto
    enlarge_called = 1; //no borrar (testing purposes)

    // Se duplica la capacidad del mapa para evitar colisiones
    long new_capacity = map->capacity * 2;

    // Se crea un nuevo mapa con la nueva capacidad
    HashMap *new_map = createMap(new_capacity);

    // Se itera sobre el mapa original y se insertan los pares en el nuevo mapa
    for (long i = 0; i < map->capacity; i++) {
        Pair *pair = map->buckets[i];
        if (pair != NULL) {
            insertMap(new_map, pair->key, pair->value);
        }
    }

    // Se liberan los buckets del mapa original
    free(map->buckets);

    // Copiamos los valores del nuevo mapa al original
    map->buckets = new_map->buckets;
    map->capacity = new_map->capacity;
    map->size = new_map->size;

    // Liberamos la estructura del nuevo mapa pero no los buckets ya que los usamos en el mapa original
    free(new_map);
}


HashMap * createMap(long capacity) {

    // Se asigna memoria al mapa
    HashMap *map = (HashMap *)malloc(sizeof(HashMap));
    if (map == NULL) {
        return NULL; 
    }

    // Se asigna memoria a los buckets
    map->buckets = (Pair **)calloc(capacity, sizeof(Pair *));
    if (map->buckets == NULL) {
        free(map); 
        return NULL;
    }

    // Se inicializan los valores del mapa
    map->size = 0;
    map->capacity = capacity;

    // Se inicializa en -1 para indicar que no se ha accedido a ningún dato
    map->current = -1; 

    return map;
}

void eraseMap(HashMap * map, char * key) {
    // El indice es lo que retorna la función hash con la llave
    long idx = hash(key, map->capacity);

    // Se itera todo el mapa hasta que el indice sea nulo
    while (map->buckets[idx] != NULL) {

        // Si la llave existe en el mapa, se marca su par para ser eliminado
        if (is_equal(map->buckets[idx]->key, key)) {
            // Se libera la memoria de la llave y el valor
            free(map->buckets[idx]->key);
            map->buckets[idx]->key = NULL;
            map->size--;
            return;
        }
        idx = (idx + 1) % map->capacity; // Linear probing
    }
}

Pair * searchMap(HashMap * map,  char * key) {

    // El indice es lo que retorna la función hash con la llave  
    long idx = hash(key, map->capacity);

    // Se itera todo el mapa hasta que el indice sea nulo
    while (map->buckets[idx] != NULL) {

        // Si la llave existe en el mapa, se retorna su par
        if (is_equal(map->buckets[idx]->key, key)) {
            map->current = idx;
            return map->buckets[idx];
        }
        idx = (idx + 1) % map->capacity; // Se realiza linear probing

    }
    return NULL; // No se encuentra la llave
}

Pair * firstMap(HashMap * map) {

    // Caso donde el mapa es nulo o no tiene buckets
    if (map == NULL || map->buckets == NULL) {
        return NULL;
    }

    // Se iteran los elementos del mapa
    for (long i = 0; i < map->capacity; i++) {

        // Si se encuentra un par válido, se actualiza el índice actual y se retorna el par
        if (map->buckets[i] != NULL && map->buckets[i]->key != NULL) {
            map->current = i;
            return map->buckets[i];
        }

    }
    return NULL; // Ningun par válido fue encontrado
}

Pair * nextMap(HashMap * map) {

    // Se iteran los elementos del mapa hasta encontrar uno no nulo
    for (long i = map->current + 1; i < map->capacity; i++) {

        if (map->buckets[i] != NULL) {

            // Al encontrar un elemento no nulo, se actualiza el índice actual y se retorna el par
            map->current = i;
            return map->buckets[i];

        }

    }
    return NULL; // Ningun par válido fue encontrado
}