#define _POSIX_C_SOURCE 200809L
#include "hash.h"
#include <stdlib.h>
#include <string.h>

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

hash_t *hash_crear(size_t capacidad_inicial)
{
	if (capacidad_inicial < 3)
		capacidad_inicial = 3;

	hash_t *hash = calloc(1, sizeof(hash_t));
	if (!hash)
		return NULL;

	hash->buckets = calloc(capacidad_inicial, sizeof(hash_nodo_t *));
	if (!hash->buckets) {
		free(hash);
		return NULL;
	}

	hash->capacidad = capacidad_inicial;
	hash->cantidad = 0;

	return hash;
}

size_t hash_cantidad(hash_t *hash)
{
	if (!hash)
		return 0;
	return hash->cantidad;
}

static size_t funcion_hashing(const char *str, size_t capacidad)
{
	size_t hash = 5381;
	while (*str) {
		unsigned char c = (unsigned char)(*str);
		hash = ((hash << 5) + hash) + (size_t)c; // hash * 33 + c
		str++;
	}
	return hash % capacidad;
}

static hash_nodo_t *hash_buscar_nodo(hash_t *hash, const char *clave,
				     size_t *pos_hash)
{
	if (!hash || !clave)
		return NULL;
	size_t i = funcion_hashing(clave, hash->capacidad);
	if (pos_hash)
		*pos_hash = i;
	hash_nodo_t *actual = hash->buckets[i];
	while (actual) {
		if (strcmp(actual->clave, clave) == 0)
			return actual;
		actual = actual->siguiente;
	}
	return NULL;
}

static bool hash_redimensionar(hash_t *hash)
{
	size_t nueva_capacidad = hash->capacidad * 2;
	hash_nodo_t **nuevos_buckets =
		calloc(nueva_capacidad, sizeof(hash_nodo_t *));
	if (!nuevos_buckets)
		return false;

	for (size_t i = 0; i < hash->capacidad; i++) {
		hash_nodo_t *actual = hash->buckets[i];
		while (actual) {
			hash_nodo_t *siguiente = actual->siguiente;

			size_t nueva_pos =
				funcion_hashing(actual->clave, nueva_capacidad);

			actual->siguiente = nuevos_buckets[nueva_pos];
			nuevos_buckets[nueva_pos] = actual;

			actual = siguiente;
		}
	}

	free(hash->buckets);
	hash->buckets = nuevos_buckets;
	hash->capacidad = nueva_capacidad;
	return true;
}

bool hash_insertar(hash_t *hash, char *clave, void *valor, void **encontrado)
{
	if (!hash || !clave)
		return false;

	if ((double)hash->cantidad / (double)hash->capacidad > 0.75) {
		if (!hash_redimensionar(hash))
			return false;
	}

	size_t valor_hashing;
	hash_nodo_t *actual = hash_buscar_nodo(hash, clave, &valor_hashing);
	if (actual) {
		if (encontrado)
			*encontrado = actual->valor;
		actual->valor = valor;
		return true;
	}

	hash_nodo_t *nuevo = malloc(sizeof(hash_nodo_t));
	if (!nuevo)
		return false;

	nuevo->clave = strdup(clave);
	if (!nuevo->clave) {
		free(nuevo);
		return false;
	}

	nuevo->valor = valor;
	nuevo->siguiente = hash->buckets[valor_hashing];
	hash->buckets[valor_hashing] = nuevo;
	hash->cantidad++;

	if (encontrado)
		*encontrado = NULL;
	return true;
}

void *hash_buscar(hash_t *hash, char *clave)
{
	hash_nodo_t *nodo = hash_buscar_nodo(hash, clave, NULL);
	if (nodo) {
		return nodo->valor;
	} else {
		return NULL;
	}
}

bool hash_contiene(hash_t *hash, char *clave)
{
	return hash_buscar_nodo(hash, clave, NULL) != NULL;
}

static void *hash_remover_nodo(hash_t *hash, size_t posicion_hashing,
			       const char *clave)
{
	hash_nodo_t *actual = hash->buckets[posicion_hashing];
	hash_nodo_t *anterior = NULL;
	while (actual) {
		if (strcmp(actual->clave, clave) == 0) {
			void *valor = actual->valor;
			if (anterior)
				anterior->siguiente = actual->siguiente;
			else
				hash->buckets[posicion_hashing] =
					actual->siguiente;
			free(actual->clave);
			free(actual);
			hash->cantidad--;
			return valor;
		}
		anterior = actual;
		actual = actual->siguiente;
	}
	return NULL;
}

void *hash_quitar(hash_t *hash, char *clave)
{
	if (!hash || !clave)
		return NULL;
	size_t posicion_hashing = funcion_hashing(clave, hash->capacidad);
	return hash_remover_nodo(hash, posicion_hashing, clave);
}

size_t hash_iterar(hash_t *hash, bool (*f)(char *, void *, void *), void *ctx)
{
	if (!hash || !f)
		return 0;

	size_t aplicadas = 0;
	for (size_t i = 0; i < hash->capacidad; i++) {
		hash_nodo_t *nodo = hash->buckets[i];
		while (nodo) {
			aplicadas++;
			if (!f(nodo->clave, nodo->valor, ctx))
				return aplicadas;
			nodo = nodo->siguiente;
		}
	}

	return aplicadas;
}

static void hash_destruir_interno(hash_t *hash, void (*destructor)(void *))
{
	if (!hash)
		return;

	for (size_t i = 0; i < hash->capacidad; i++) {
		hash_nodo_t *actual = hash->buckets[i];
		while (actual) {
			hash_nodo_t *siguiente = actual->siguiente;
			if (destructor && actual->valor)
				destructor(actual->valor);
			free(actual->clave);
			free(actual);
			actual = siguiente;
		}
	}

	free(hash->buckets);
	free(hash);
}

void hash_destruir(hash_t *hash)
{
	hash_destruir_interno(hash, NULL);
}

void hash_destruir_todo(hash_t *hash, void (*destructor)(void *))
{
	hash_destruir_interno(hash, destructor);
}
