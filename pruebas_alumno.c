#include "pa2m.h"
#include "src/hash.h"
#include <stdlib.h>
#include <string.h>

// Estructura auxiliar para contar iteraciones
typedef struct {
	size_t contador;
	size_t limite;
} contexto_iteracion_t;

// Función para iterar que siempre devuelve true
bool contar_elementos(char *clave, void *valor, void *ctx)
{
	contexto_iteracion_t *contexto = (contexto_iteracion_t *)ctx;
	contexto->contador++;
	return true;
}

// Función para iterar que corta después de N elementos
bool contar_hasta_limite(char *clave, void *valor, void *ctx)
{
	contexto_iteracion_t *contexto = (contexto_iteracion_t *)ctx;
	contexto->contador++;
	if (contexto->contador >= contexto->limite)
		return false;
	return true;
}

// Destructor simple para pruebas
void destructor_simple(void *elemento)
{
	free(elemento);
}

void prueba_hash_crear()
{
	hash_t *hash_tmp;
	
	hash_tmp = hash_crear(0);
	pa2m_afirmar(hash_tmp != NULL,
		     "Crear hash con capacidad 0 usa capacidad mínima (3)");
	hash_destruir(hash_tmp);
	
	hash_tmp = hash_crear(1);
	pa2m_afirmar(hash_tmp != NULL,
		     "Crear hash con capacidad 1 usa capacidad mínima (3)");
	hash_destruir(hash_tmp);
	
	hash_tmp = hash_crear(2);
	pa2m_afirmar(hash_tmp != NULL,
		     "Crear hash con capacidad 2 usa capacidad mínima (3)");
	hash_destruir(hash_tmp);

	hash_t *hash = hash_crear(10);
	pa2m_afirmar(hash != NULL, "Crear hash con capacidad 10 es exitoso");
	pa2m_afirmar(hash_cantidad(hash) == 0,
		     "Hash recién creado tiene cantidad 0");
	hash_destruir(hash);

	hash = hash_crear(100);
	pa2m_afirmar(hash != NULL, "Crear hash con capacidad 100 es exitoso");
	hash_destruir(hash);
}

void prueba_hash_cantidad()
{
	pa2m_afirmar(hash_cantidad(NULL) == 0,
		     "Hash NULL tiene cantidad 0");

	hash_t *hash = hash_crear(5);
	pa2m_afirmar(hash_cantidad(hash) == 0,
		     "Hash vacío tiene cantidad 0");

	int valor = 42;
	hash_insertar(hash, "clave1", &valor, NULL);
	pa2m_afirmar(hash_cantidad(hash) == 1,
		     "Después de insertar 1 elemento, cantidad es 1");

	hash_insertar(hash, "clave2", &valor, NULL);
	hash_insertar(hash, "clave3", &valor, NULL);
	pa2m_afirmar(hash_cantidad(hash) == 3,
		     "Después de insertar 3 elementos, cantidad es 3");

	hash_destruir(hash);
}

void prueba_hash_insertar()
{
	hash_t *hash = hash_crear(3);
	int v1 = 10, v2 = 20, v3 = 30, v4 = 40;
	void *anterior = NULL;

	// Inserción normal
	pa2m_afirmar(hash_insertar(hash, "a", &v1, &anterior),
		     "Insertar elemento con clave 'a' es exitoso");
	pa2m_afirmar(anterior == NULL,
		     "Al insertar en clave nueva, anterior es NULL");
	pa2m_afirmar(hash_cantidad(hash) == 1, "Cantidad es 1");

	// Insertar varias claves
	hash_insertar(hash, "b", &v2, NULL);
	hash_insertar(hash, "c", &v3, NULL);
	pa2m_afirmar(hash_cantidad(hash) == 3,
		     "Después de insertar 3 elementos, cantidad es 3");

	// Reemplazar elemento existente
	anterior = NULL;
	pa2m_afirmar(hash_insertar(hash, "a", &v4, &anterior),
		     "Reemplazar elemento existente es exitoso");
	pa2m_afirmar(anterior == &v1,
		     "Al reemplazar, se obtiene el valor anterior");
	pa2m_afirmar(hash_cantidad(hash) == 3,
		     "Al reemplazar, la cantidad no cambia");
	pa2m_afirmar(hash_buscar(hash, "a") == &v4,
		     "El nuevo valor está almacenado");

	// Insertar con NULL
	pa2m_afirmar(!hash_insertar(NULL, "clave", &v1, NULL),
		     "Insertar en hash NULL falla");

	hash_destruir(hash);
}

void prueba_hash_buscar()
{
	hash_t *hash = hash_crear(5);
	int v1 = 100, v2 = 200, v3 = 300;

	// Buscar en hash vacío
	pa2m_afirmar(hash_buscar(hash, "noexiste") == NULL,
		     "Buscar en hash vacío devuelve NULL");

	// Insertar y buscar
	hash_insertar(hash, "primero", &v1, NULL);
	hash_insertar(hash, "segundo", &v2, NULL);
	hash_insertar(hash, "tercero", &v3, NULL);

	pa2m_afirmar(hash_buscar(hash, "primero") == &v1,
		     "Buscar 'primero' devuelve el valor correcto");
	pa2m_afirmar(hash_buscar(hash, "segundo") == &v2,
		     "Buscar 'segundo' devuelve el valor correcto");
	pa2m_afirmar(hash_buscar(hash, "tercero") == &v3,
		     "Buscar 'tercero' devuelve el valor correcto");

	// Buscar clave inexistente
	pa2m_afirmar(hash_buscar(hash, "noexiste") == NULL,
		     "Buscar clave inexistente devuelve NULL");

	// Casos borde
	pa2m_afirmar(hash_buscar(NULL, "clave") == NULL,
		     "Buscar en hash NULL devuelve NULL");

	hash_destruir(hash);
}

void prueba_hash_contiene()
{
	hash_t *hash = hash_crear(5);
	int v1 = 1;

	// Hash vacío
	pa2m_afirmar(!hash_contiene(hash, "clave"),
		     "Hash vacío no contiene ninguna clave");

	// Insertar y verificar
	hash_insertar(hash, "existe", &v1, NULL);
	pa2m_afirmar(hash_contiene(hash, "existe"),
		     "Hash contiene la clave insertada");
	pa2m_afirmar(!hash_contiene(hash, "noexiste"),
		     "Hash no contiene clave no insertada");

	// Casos borde
	pa2m_afirmar(!hash_contiene(NULL, "clave"),
		     "Hash NULL no contiene ninguna clave");

	hash_destruir(hash);
}

void prueba_hash_quitar()
{
	hash_t *hash = hash_crear(5);
	int v1 = 10, v2 = 20, v3 = 30;

	// Quitar de hash vacío
	pa2m_afirmar(hash_quitar(hash, "noexiste") == NULL,
		     "Quitar de hash vacío devuelve NULL");

	// Insertar elementos
	hash_insertar(hash, "a", &v1, NULL);
	hash_insertar(hash, "b", &v2, NULL);
	hash_insertar(hash, "c", &v3, NULL);
	pa2m_afirmar(hash_cantidad(hash) == 3, "Cantidad inicial es 3");

	// Quitar elementos
	void *quitado = hash_quitar(hash, "b");
	pa2m_afirmar(quitado == &v2, "Quitar 'b' devuelve el valor correcto");
	pa2m_afirmar(hash_cantidad(hash) == 2,
		     "Después de quitar, cantidad es 2");
	pa2m_afirmar(!hash_contiene(hash, "b"),
		     "La clave quitada ya no está en el hash");

	// Quitar clave inexistente
	pa2m_afirmar(hash_quitar(hash, "noexiste") == NULL,
		     "Quitar clave inexistente devuelve NULL");
	pa2m_afirmar(hash_cantidad(hash) == 2,
		     "Cantidad no cambia al quitar clave inexistente");

	// Quitar todos los elementos
	hash_quitar(hash, "a");
	hash_quitar(hash, "c");
	pa2m_afirmar(hash_cantidad(hash) == 0,
		     "Después de quitar todos, cantidad es 0");

	// Casos borde
	pa2m_afirmar(hash_quitar(NULL, "clave") == NULL,
		     "Quitar de hash NULL devuelve NULL");

	hash_destruir(hash);
}

void prueba_hash_iterar()
{
	hash_t *hash = hash_crear(5);
	int v1 = 1, v2 = 2, v3 = 3, v4 = 4, v5 = 5;
	contexto_iteracion_t ctx;

	// Iterar hash vacío
	ctx.contador = 0;
	pa2m_afirmar(hash_iterar(hash, contar_elementos, &ctx) == 0,
		     "Iterar hash vacío devuelve 0");
	pa2m_afirmar(ctx.contador == 0, "No se aplicó la función");

	// Insertar elementos
	hash_insertar(hash, "uno", &v1, NULL);
	hash_insertar(hash, "dos", &v2, NULL);
	hash_insertar(hash, "tres", &v3, NULL);
	hash_insertar(hash, "cuatro", &v4, NULL);
	hash_insertar(hash, "cinco", &v5, NULL);

	// Iterar todos los elementos
	ctx.contador = 0;
	size_t aplicadas = hash_iterar(hash, contar_elementos, &ctx);
	pa2m_afirmar(aplicadas == 5,
		     "Iterar todos los elementos devuelve 5");
	pa2m_afirmar(ctx.contador == 5, "Se aplicó la función 5 veces");

	// Iterar con corte
	ctx.contador = 0;
	ctx.limite = 3;
	aplicadas = hash_iterar(hash, contar_hasta_limite, &ctx);
	pa2m_afirmar(aplicadas == 2, "Iterar con corte devuelve 2 (la función devolvió false en el 3er elemento)");
	pa2m_afirmar(ctx.contador == 3,
		     "Se aplicó la función 3 veces (incluyendo la que devolvió false)");

	// Casos borde
	pa2m_afirmar(hash_iterar(NULL, contar_elementos, &ctx) == 0,
		     "Iterar hash NULL devuelve 0");
	pa2m_afirmar(hash_iterar(hash, NULL, &ctx) == 0,
		     "Iterar con función NULL devuelve 0");

	hash_destruir(hash);
}

void prueba_hash_destruir()
{
	hash_t *hash = hash_crear(5);
	int v1 = 1, v2 = 2;

	hash_insertar(hash, "a", &v1, NULL);
	hash_insertar(hash, "b", &v2, NULL);

	hash_destruir(hash);
	pa2m_afirmar(true, "Destruir hash no falla");

	hash_destruir(NULL);
	pa2m_afirmar(true, "Destruir hash NULL no falla");
}

void prueba_hash_destruir_todo()
{
	hash_t *hash = hash_crear(5);

	// Crear elementos dinámicos
	int *v1 = malloc(sizeof(int));
	int *v2 = malloc(sizeof(int));
	int *v3 = malloc(sizeof(int));
	*v1 = 10;
	*v2 = 20;
	*v3 = 30;

	hash_insertar(hash, "a", v1, NULL);
	hash_insertar(hash, "b", v2, NULL);
	hash_insertar(hash, "c", v3, NULL);

	hash_destruir_todo(hash, destructor_simple);
	pa2m_afirmar(true, "Destruir hash con destructor no falla");

	hash_destruir_todo(NULL, destructor_simple);
	pa2m_afirmar(true, "Destruir hash NULL con destructor no falla");
}

void prueba_colisiones()
{
	hash_t *hash = hash_crear(3);
	int v1 = 1, v2 = 2, v3 = 3, v4 = 4, v5 = 5;

	// Insertar muchos elementos en un hash pequeño (forzar colisiones)
	hash_insertar(hash, "a", &v1, NULL);
	hash_insertar(hash, "b", &v2, NULL);
	hash_insertar(hash, "c", &v3, NULL);
	hash_insertar(hash, "d", &v4, NULL);
	hash_insertar(hash, "e", &v5, NULL);

	pa2m_afirmar(hash_cantidad(hash) == 5,
		     "Insertar 5 elementos en hash de capacidad 3 funciona");

	pa2m_afirmar(hash_buscar(hash, "a") == &v1,
		     "Buscar 'a' en hash con colisiones funciona");
	pa2m_afirmar(hash_buscar(hash, "b") == &v2,
		     "Buscar 'b' en hash con colisiones funciona");
	pa2m_afirmar(hash_buscar(hash, "e") == &v5,
		     "Buscar 'e' en hash con colisiones funciona");

	void *quitado = hash_quitar(hash, "c");
	pa2m_afirmar(quitado == &v3, "Quitar 'c' en hash con colisiones funciona");
	pa2m_afirmar(hash_cantidad(hash) == 4, "Cantidad es correcta después de quitar");

	hash_destruir(hash);
}

void prueba_claves_largas()
{
	hash_t *hash = hash_crear(10);
	int v1 = 1, v2 = 2;

	char clave_larga1[1000];
	char clave_larga2[1000];
	memset(clave_larga1, 'a', 999);
	memset(clave_larga2, 'b', 999);
	clave_larga1[999] = '\0';
	clave_larga2[999] = '\0';

	hash_insertar(hash, clave_larga1, &v1, NULL);
	hash_insertar(hash, clave_larga2, &v2, NULL);

	pa2m_afirmar(hash_cantidad(hash) == 2,
		     "Insertar claves largas funciona");
	pa2m_afirmar(hash_buscar(hash, clave_larga1) == &v1,
		     "Buscar clave larga 1 funciona");
	pa2m_afirmar(hash_buscar(hash, clave_larga2) == &v2,
		     "Buscar clave larga 2 funciona");

	hash_destruir(hash);
}

void prueba_valores_null()
{
	hash_t *hash = hash_crear(5);

	// Insertar NULL como valor
	pa2m_afirmar(hash_insertar(hash, "null_value", NULL, NULL),
		     "Insertar NULL como valor es exitoso");
	pa2m_afirmar(hash_contiene(hash, "null_value"),
		     "Hash contiene la clave con valor NULL");
	pa2m_afirmar(hash_buscar(hash, "null_value") == NULL,
		     "Buscar clave con valor NULL devuelve NULL");

	void *quitado = hash_quitar(hash, "null_value");
	pa2m_afirmar(quitado == NULL, "Quitar clave con valor NULL devuelve NULL");
	pa2m_afirmar(hash_cantidad(hash) == 0, "Cantidad es 0 después de quitar");

	hash_destruir(hash);
}

int main()
{
	pa2m_nuevo_grupo("============== PRUEBAS HASH_CREAR ==============");
	prueba_hash_crear();

	pa2m_nuevo_grupo("============== PRUEBAS HASH_CANTIDAD ==============");
	prueba_hash_cantidad();

	pa2m_nuevo_grupo("============== PRUEBAS HASH_INSERTAR ==============");
	prueba_hash_insertar();

	pa2m_nuevo_grupo("============== PRUEBAS HASH_BUSCAR ==============");
	prueba_hash_buscar();

	pa2m_nuevo_grupo("============== PRUEBAS HASH_CONTIENE ==============");
	prueba_hash_contiene();

	pa2m_nuevo_grupo("============== PRUEBAS HASH_QUITAR ==============");
	prueba_hash_quitar();

	pa2m_nuevo_grupo("============== PRUEBAS HASH_ITERAR ==============");
	prueba_hash_iterar();

	pa2m_nuevo_grupo("============== PRUEBAS HASH_DESTRUIR ==============");
	prueba_hash_destruir();

	pa2m_nuevo_grupo("============== PRUEBAS HASH_DESTRUIR_TODO ==============");
	prueba_hash_destruir_todo();

	pa2m_nuevo_grupo("============== PRUEBAS COLISIONES ==============");
	prueba_colisiones();

	pa2m_nuevo_grupo("============== PRUEBAS CLAVES LARGAS ==============");
	prueba_claves_largas();

	pa2m_nuevo_grupo("============== PRUEBAS VALORES NULL ==============");
	prueba_valores_null();

	return pa2m_mostrar_reporte();
}
