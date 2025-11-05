#include "pa2m.h"
#include "src/hash.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
	size_t contador;
	size_t limite;
} contexto_iteracion_t;

bool contar_elementos(char *clave, void *valor, void *ctx)
{
	contexto_iteracion_t *contexto = (contexto_iteracion_t *)ctx;
	contexto->contador++;
	return true;
}

bool contar_hasta_limite(char *clave, void *valor, void *ctx)
{
	contexto_iteracion_t *contexto = (contexto_iteracion_t *)ctx;
	contexto->contador++;
	if (contexto->contador >= contexto->limite)
		return false;
	return true;
}

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
	pa2m_afirmar(hash_cantidad(NULL) == 0, "Hash NULL tiene cantidad 0");

	hash_t *hash = hash_crear(5);
	pa2m_afirmar(hash_cantidad(hash) == 0, "Hash vacío tiene cantidad 0");

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

	pa2m_afirmar(hash_insertar(hash, "a", &v1, &anterior),
		     "Insertar elemento con clave 'a' es exitoso");
	pa2m_afirmar(anterior == NULL,
		     "Al insertar en clave nueva, anterior es NULL");
	pa2m_afirmar(hash_cantidad(hash) == 1, "Cantidad es 1");

	hash_insertar(hash, "b", &v2, NULL);
	hash_insertar(hash, "c", &v3, NULL);
	pa2m_afirmar(hash_cantidad(hash) == 3,
		     "Después de insertar 3 elementos, cantidad es 3");

	anterior = NULL;
	pa2m_afirmar(hash_insertar(hash, "a", &v4, &anterior),
		     "Reemplazar elemento existente es exitoso");
	pa2m_afirmar(anterior == &v1,
		     "Al reemplazar, se obtiene el valor anterior");
	pa2m_afirmar(hash_cantidad(hash) == 3,
		     "Al reemplazar, la cantidad no cambia");
	pa2m_afirmar(hash_buscar(hash, "a") == &v4,
		     "El nuevo valor está almacenado");

	pa2m_afirmar(!hash_insertar(NULL, "clave", &v1, NULL),
		     "Insertar en hash NULL falla");

	hash_destruir(hash);
}

void prueba_hash_buscar()
{
	hash_t *hash = hash_crear(5);
	int v1 = 100, v2 = 200, v3 = 300;

	pa2m_afirmar(hash_buscar(hash, "noexiste") == NULL,
		     "Buscar en hash vacío devuelve NULL");

	hash_insertar(hash, "primero", &v1, NULL);
	hash_insertar(hash, "segundo", &v2, NULL);
	hash_insertar(hash, "tercero", &v3, NULL);

	pa2m_afirmar(hash_buscar(hash, "primero") == &v1,
		     "Buscar 'primero' devuelve el valor correcto");
	pa2m_afirmar(hash_buscar(hash, "segundo") == &v2,
		     "Buscar 'segundo' devuelve el valor correcto");
	pa2m_afirmar(hash_buscar(hash, "tercero") == &v3,
		     "Buscar 'tercero' devuelve el valor correcto");

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

	pa2m_afirmar(!hash_contiene(hash, "clave"),
		     "Hash vacío no contiene ninguna clave");

	hash_insertar(hash, "existe", &v1, NULL);
	pa2m_afirmar(hash_contiene(hash, "existe"),
		     "Hash contiene la clave insertada");
	pa2m_afirmar(!hash_contiene(hash, "noexiste"),
		     "Hash no contiene clave no insertada");

	pa2m_afirmar(!hash_contiene(NULL, "clave"),
		     "Hash NULL no contiene ninguna clave");

	hash_destruir(hash);
}

void prueba_hash_quitar()
{
	hash_t *hash = hash_crear(5);
	int v1 = 10, v2 = 20, v3 = 30;

	pa2m_afirmar(hash_quitar(hash, "noexiste") == NULL,
		     "Quitar de hash vacío devuelve NULL");

	hash_insertar(hash, "a", &v1, NULL);
	hash_insertar(hash, "b", &v2, NULL);
	hash_insertar(hash, "c", &v3, NULL);
	pa2m_afirmar(hash_cantidad(hash) == 3, "Cantidad inicial es 3");

	void *quitado = hash_quitar(hash, "b");
	pa2m_afirmar(quitado == &v2, "Quitar 'b' devuelve el valor correcto");
	pa2m_afirmar(hash_cantidad(hash) == 2,
		     "Después de quitar, cantidad es 2");
	pa2m_afirmar(!hash_contiene(hash, "b"),
		     "La clave quitada ya no está en el hash");

	pa2m_afirmar(hash_quitar(hash, "noexiste") == NULL,
		     "Quitar clave inexistente devuelve NULL");
	pa2m_afirmar(hash_cantidad(hash) == 2,
		     "Cantidad no cambia al quitar clave inexistente");

	hash_quitar(hash, "a");
	hash_quitar(hash, "c");
	pa2m_afirmar(hash_cantidad(hash) == 0,
		     "Después de quitar todos, cantidad es 0");

	pa2m_afirmar(hash_quitar(NULL, "clave") == NULL,
		     "Quitar de hash NULL devuelve NULL");

	hash_destruir(hash);
}

void prueba_hash_iterar()
{
	hash_t *hash = hash_crear(5);
	int v1 = 1, v2 = 2, v3 = 3, v4 = 4, v5 = 5;
	contexto_iteracion_t ctx;

	ctx.contador = 0;
	pa2m_afirmar(hash_iterar(hash, contar_elementos, &ctx) == 0,
		     "Iterar hash vacío devuelve 0");
	pa2m_afirmar(ctx.contador == 0, "No se aplicó la función");

	hash_insertar(hash, "uno", &v1, NULL);
	hash_insertar(hash, "dos", &v2, NULL);
	hash_insertar(hash, "tres", &v3, NULL);
	hash_insertar(hash, "cuatro", &v4, NULL);
	hash_insertar(hash, "cinco", &v5, NULL);

	ctx.contador = 0;
	size_t aplicadas = hash_iterar(hash, contar_elementos, &ctx);
	pa2m_afirmar(aplicadas == 5, "Iterar todos los elementos devuelve 5");
	pa2m_afirmar(ctx.contador == 5, "Se aplicó la función 5 veces");

	ctx.contador = 0;
	ctx.limite = 3;
	aplicadas = hash_iterar(hash, contar_hasta_limite, &ctx);
	pa2m_afirmar(
		aplicadas == 3,
		"Iterar con corte devuelve 3 (la función se invocó 3 veces incluyendo la que devolvió false)");
	pa2m_afirmar(
		ctx.contador == 3,
		"Se aplicó la función 3 veces (incluyendo la que devolvió false)");

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
	pa2m_afirmar(quitado == &v3,
		     "Quitar 'c' en hash con colisiones funciona");
	pa2m_afirmar(hash_cantidad(hash) == 4,
		     "Cantidad es correcta después de quitar");

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

	pa2m_afirmar(hash_insertar(hash, "null_value", NULL, NULL),
		     "Insertar NULL como valor es exitoso");
	pa2m_afirmar(hash_contiene(hash, "null_value"),
		     "Hash contiene la clave con valor NULL");
	pa2m_afirmar(hash_buscar(hash, "null_value") == NULL,
		     "Buscar clave con valor NULL devuelve NULL");

	void *quitado = hash_quitar(hash, "null_value");
	pa2m_afirmar(quitado == NULL,
		     "Quitar clave con valor NULL devuelve NULL");
	pa2m_afirmar(hash_cantidad(hash) == 0,
		     "Cantidad es 0 después de quitar");

	hash_destruir(hash);
}

void prueba_clave_vacia()
{
	hash_t *hash = hash_crear(5);
	int v1 = 100;

	pa2m_afirmar(hash_insertar(hash, "", &v1, NULL),
		     "Insertar con clave vacía \"\" es exitoso");
	pa2m_afirmar(hash_contiene(hash, ""), "Hash contiene la clave vacía");
	pa2m_afirmar(hash_buscar(hash, "") == &v1,
		     "Buscar clave vacía devuelve el valor correcto");
	pa2m_afirmar(hash_cantidad(hash) == 1, "Cantidad es 1");

	void *quitado = hash_quitar(hash, "");
	pa2m_afirmar(quitado == &v1,
		     "Quitar clave vacía devuelve el valor correcto");
	pa2m_afirmar(hash_cantidad(hash) == 0,
		     "Cantidad es 0 después de quitar");

	hash_destruir(hash);
}

void prueba_muchas_colisiones_mismo_bucket()
{
	hash_t *hash = hash_crear(3);
	int valores[20];

	for (int i = 0; i < 20; i++) {
		valores[i] = i * 10;
	}

	for (int i = 0; i < 20; i++) {
		char clave[20];
		sprintf(clave, "clave_%d", i);
		pa2m_afirmar(
			hash_insertar(hash, clave, &valores[i], NULL),
			"Insertar elemento con muchas colisiones es exitoso");
	}

	pa2m_afirmar(hash_cantidad(hash) == 20,
		     "Insertar 20 elementos en hash de capacidad 3 funciona");

	bool todos_presentes = true;
	for (int i = 0; i < 20; i++) {
		char clave[20];
		sprintf(clave, "clave_%d", i);
		if (hash_buscar(hash, clave) != &valores[i]) {
			todos_presentes = false;
			break;
		}
	}
	pa2m_afirmar(
		todos_presentes,
		"Todos los elementos con colisiones se pueden buscar correctamente");

	for (int i = 0; i < 10; i++) {
		char clave[20];
		sprintf(clave, "clave_%d", i);
		hash_quitar(hash, clave);
	}
	pa2m_afirmar(hash_cantidad(hash) == 10,
		     "Después de quitar 10 elementos, cantidad es 10");

	hash_destruir(hash);
}

void prueba_iteracion_con_colisiones()
{
	hash_t *hash = hash_crear(3);
	int v1 = 1, v2 = 2, v3 = 3, v4 = 4, v5 = 5;
	contexto_iteracion_t ctx;

	hash_insertar(hash, "a", &v1, NULL);
	hash_insertar(hash, "b", &v2, NULL);
	hash_insertar(hash, "c", &v3, NULL);
	hash_insertar(hash, "d", &v4, NULL);
	hash_insertar(hash, "e", &v5, NULL);

	ctx.contador = 0;
	size_t aplicadas = hash_iterar(hash, contar_elementos, &ctx);
	pa2m_afirmar(aplicadas == 5,
		     "Iteración con colisiones visita todos los elementos");
	pa2m_afirmar(
		ctx.contador == 5,
		"Se aplicó la función a todos los elementos con colisiones");

	hash_destruir(hash);
}

void prueba_redimensionamiento()
{
	hash_t *hash = hash_crear(3);
	int valores[50];

	for (int i = 0; i < 50; i++) {
		valores[i] = i * 100;
	}

	// El factor de carga es 0.75, entonces:
	// - Con capacidad 3: se redimensiona al insertar el 3er elemento (3/3 > 0.75 es falso, pero 3/3 = 1.0)
	// - Realmente se redimensiona cuando cantidad/capacidad > 0.75
	// - Capacidad 3: redimensiona después de 2 elementos (3/3 > 0.75)
	// - Capacidad 6: redimensiona después de 4 elementos (5/6 > 0.75)
	// - Capacidad 12: redimensiona después de 9 elementos (10/12 > 0.75)
	// etc.

	// Insertar muchos elementos para forzar múltiples redimensionamientos
	for (int i = 0; i < 50; i++) {
		char clave[20];
		sprintf(clave, "elem_%d", i);
		pa2m_afirmar(hash_insertar(hash, clave, &valores[i], NULL),
			     "Inserción exitosa durante redimensionamiento");
	}

	pa2m_afirmar(hash_cantidad(hash) == 50,
		     "Después de 50 inserciones, la cantidad es correcta");

	bool todos_accesibles = true;
	for (int i = 0; i < 50; i++) {
		char clave[20];
		sprintf(clave, "elem_%d", i);
		void *encontrado = hash_buscar(hash, clave);
		if (encontrado != &valores[i]) {
			todos_accesibles = false;
			break;
		}
	}
	pa2m_afirmar(
		todos_accesibles,
		"Todos los elementos son accesibles después del redimensionamiento");

	for (int i = 0; i < 25; i++) {
		char clave[20];
		sprintf(clave, "elem_%d", i);
		void *quitado = hash_quitar(hash, clave);
		pa2m_afirmar(
			quitado == &valores[i],
			"Quitar elementos después del redimensionamiento funciona");
	}

	pa2m_afirmar(hash_cantidad(hash) == 25,
		     "Después de quitar 25 elementos, quedan 25");

	bool restantes_ok = true;
	for (int i = 25; i < 50; i++) {
		char clave[20];
		sprintf(clave, "elem_%d", i);
		if (hash_buscar(hash, clave) != &valores[i]) {
			restantes_ok = false;
			break;
		}
	}
	pa2m_afirmar(restantes_ok,
		     "Los elementos no eliminados siguen accesibles");

	hash_destruir(hash);
}

void prueba_redimensionamiento_con_actualizaciones()
{
	hash_t *hash = hash_crear(3);
	int valores_originales[20];
	int valores_nuevos[20];

	for (int i = 0; i < 20; i++) {
		valores_originales[i] = i;
		valores_nuevos[i] = i * 1000;
	}

	for (int i = 0; i < 20; i++) {
		char clave[20];
		sprintf(clave, "key_%d", i);
		hash_insertar(hash, clave, &valores_originales[i], NULL);
	}

	void *anterior;
	for (int i = 0; i < 20; i++) {
		char clave[20];
		sprintf(clave, "key_%d", i);
		hash_insertar(hash, clave, &valores_nuevos[i], &anterior);
		pa2m_afirmar(
			anterior == &valores_originales[i],
			"La actualización devuelve el valor anterior correcto");
	}

	pa2m_afirmar(hash_cantidad(hash) == 20,
		     "La cantidad no cambia al actualizar claves existentes");

	bool valores_actualizados = true;
	for (int i = 0; i < 20; i++) {
		char clave[20];
		sprintf(clave, "key_%d", i);
		if (hash_buscar(hash, clave) != &valores_nuevos[i]) {
			valores_actualizados = false;
			break;
		}
	}
	pa2m_afirmar(valores_actualizados,
		     "Todos los valores fueron actualizados correctamente");

	hash_destruir(hash);
}

void prueba_redimensionamiento_con_iteracion()
{
	hash_t *hash = hash_crear(3);
	int valores[100];
	contexto_iteracion_t ctx;

	for (int i = 0; i < 100; i++) {
		valores[i] = i;
		char clave[20];
		sprintf(clave, "item_%d", i);
		hash_insertar(hash, clave, &valores[i], NULL);
	}

	pa2m_afirmar(hash_cantidad(hash) == 100,
		     "Se insertaron 100 elementos correctamente");

	ctx.contador = 0;
	size_t aplicadas = hash_iterar(hash, contar_elementos, &ctx);
	pa2m_afirmar(aplicadas == 100,
		     "La iteración visita todos los 100 elementos");
	pa2m_afirmar(ctx.contador == 100, "Se aplicó la función 100 veces");

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

	pa2m_nuevo_grupo(
		"============== PRUEBAS HASH_DESTRUIR_TODO ==============");
	prueba_hash_destruir_todo();

	pa2m_nuevo_grupo("============== PRUEBAS COLISIONES ==============");
	prueba_colisiones();

	pa2m_nuevo_grupo("============== PRUEBAS CLAVES LARGAS ==============");
	prueba_claves_largas();

	pa2m_nuevo_grupo("============== PRUEBAS VALORES NULL ==============");
	prueba_valores_null();

	pa2m_nuevo_grupo("============== PRUEBAS CLAVE VACÍA ==============");
	prueba_clave_vacia();

	pa2m_nuevo_grupo(
		"============== PRUEBAS MUCHAS COLISIONES ==============");
	prueba_muchas_colisiones_mismo_bucket();

	pa2m_nuevo_grupo(
		"============== PRUEBAS ITERACIÓN CON COLISIONES ==============");
	prueba_iteracion_con_colisiones();

	pa2m_nuevo_grupo(
		"============== PRUEBAS REDIMENSIONAMIENTO ==============");
	prueba_redimensionamiento();

	pa2m_nuevo_grupo(
		"============== PRUEBAS REDIMENSIONAMIENTO CON ACTUALIZACIONES ==============");
	prueba_redimensionamiento_con_actualizaciones();

	pa2m_nuevo_grupo(
		"============== PRUEBAS REDIMENSIONAMIENTO CON ITERACIÓN ==============");
	prueba_redimensionamiento_con_iteracion();

	return pa2m_mostrar_reporte();
}
