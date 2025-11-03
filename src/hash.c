#include "hash.h"
#include "string.h"

typedef struct hash_nodo {
    char *clave;
    void *valor;
    struct hash_nodo *siguiente;
} hash_nodo_t;

struct hash {
    hash_nodo_t **buckets; // Vector de punteros a listas de nodos
    size_t capacidad;
    size_t cantidad;
};


hash_t *hash_crear(size_t capacidad_inicial){
    if(capacidad_inicial < 3) capacidad_inicial =3;

    hash_t *hash = calloc(1,sizeof(hash_t));
    if(!hash) return NULL;

    hash->buckets = calloc(capacidad_inicial, sizeof(hash_nodo_t *));
    if(!hash->buckets){
        free(hash);
        return NULL;
    }

    hash->capacidad = capacidad_inicial;
    hash->cantidad = 0;

    return hash;

}


size_t hash_cantidad(hash_t *hash){
    if(!hash) return 0;
    return hash->cantidad;
}

static size_t funcion_hashing(const char *str, size_t capacidad) {
    size_t hash = 5381;
    int c;
    while (*str) {
        c = *str;
        hash = ((hash << 5) + hash) + (size_t)c; // hash * 33 + c
        str++;
    }
    return hash % capacidad;
}

/**
 *
 * Inserta un elemento asociado a la clave en la tabla de hash.
 *
 * Si la clave ya existe en la tabla, modificamos el valor y ponemos el nuevo.
 * Si encontrado no es NULL, se almacena el elemento reemplazado en el mismo.
 *
 * Esta funcion almacena una copia de la clave.
 *
 * No se admiten claves nulas.
 *
 * Devuelve true si se pudo almacenar el valor.
 **/
bool hash_insertar(hash_t *hash, char *clave, void *valor, void **encontrado){
    if(!hash || !clave) return NULL;

    size_t valor_hashing = funcion_hashing(clave, hash->capacidad);
    hash_nodo_t *actual = hash->buckets[valor_hashing];

    while(actual){
        if(strcmp(actual->clave , clave)== 0){
            if(encontrado) *encontrado = actual->valor;
            actual->valor = valor;
        }
        actual = actual->siguiente;
    }

    hash_nodo_t *nuevo = malloc(sizeof(hash_nodo_t));
    if(!nuevo) return false;

    nuevo->clave = strdup(clave);
    if (!nuevo->clave) {
        free(nuevo);
        return false;
    }

    nuevo->valor = valor;
    nuevo->siguiente = hash->buckets[valor_hashing] ;
    hash->buckets[valor_hashing] =nuevo;
    hash->cantidad++;
    
    if(encontrado) *encontrado = NULL;
    return true;


}