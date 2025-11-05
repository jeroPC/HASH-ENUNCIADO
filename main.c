#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "src/tp1.h"
#include "src/hash.h"

// Declaración de función auxiliar de tp1.c
const char *tipo_a_string(enum tipo_pokemon tipo);

void mostrar_pokemon(struct pokemon *p)
{
	if (!p)
		return;
	printf("ID: %d, Nombre: %s, Tipo: %s, Ataque: %d, Defensa: %d, Velocidad: %d\n",
	       p->id, p->nombre, tipo_a_string(p->tipo), p->ataque, p->defensa,
	       p->velocidad);
}

void buscar_por_nombre(hash_t *hash, const char *nombre)
{
	struct pokemon *p = hash_buscar(hash, (char *)nombre);
	if (p) {
		mostrar_pokemon(p);
	} else {
		printf("No se encontró el pokemon con nombre '%s'.\n", nombre);
	}
}

void buscar_por_id(tp1_t *tp, int id)
{
	struct pokemon *p = tp1_buscar_id(tp, id);
	if (p) {
		mostrar_pokemon(p);
	} else {
		printf("No se encontró el pokemon con id %d.\n", id);
	}
}

void mostrar_ayuda(void)
{
	printf("Comandos disponibles:\n");
	printf("  nombre <nombre_pokemon> - Buscar por nombre )\n");
	printf("  id <numero_id>          - Buscar por ID , sin hash)\n");
	printf("  salir                   - Salir del programa\n");
}

bool procesar_comando(hash_t *hash, tp1_t *tp, const char *buffer)
{
	if (strncmp(buffer, "nombre ", 7) == 0) {
		buscar_por_nombre(hash, buffer + 7);
		return true;
	}

	if (strncmp(buffer, "id ", 3) == 0) {
		buscar_por_id(tp, atoi(buffer + 3));
		return true;
	}

	if (strcmp(buffer, "salir") == 0) {
		return false;
	}

	if (strcmp(buffer, "ayuda") == 0) {
		mostrar_ayuda();
		return true;
	}

	printf("Comando no reconocido. Escribi 'ayuda' para ver los comandos.\n");
	return true;
}

typedef struct {
	hash_t *hash;
	bool exito;
} contexto_carga_t;

static bool insertar_en_hash(struct pokemon *p, void *ctx)
{
	contexto_carga_t *contexto = (contexto_carga_t *)ctx;
	if (!p || !hash_insertar(contexto->hash, p->nombre, p, NULL)) {
		fprintf(stderr, "Error al insertar pokemon '%s' en el hash.\n",
			p ? p->nombre : "NULL");
		contexto->exito = false;
		return false;
	}
	return true;
}

bool cargar_pokemones_en_hash(hash_t *hash, tp1_t *tp)
{
	contexto_carga_t contexto = { .hash = hash, .exito = true };
	size_t procesados =
		tp1_con_cada_pokemon(tp, insertar_en_hash, &contexto);

	if (!contexto.exito || procesados != tp1_cantidad(tp)) {
		fprintf(stderr, "Solo se insertaron %zu de %zu pokemones.\n",
			procesados, tp1_cantidad(tp));
		return false;
	}

	return true;
}

void ejecutar_interfaz_interactiva(hash_t *hash, tp1_t *tp)
{
	char buffer[128];
	bool continuar = true;

	mostrar_ayuda();

	while (continuar) {
		printf("> ");
		if (fgets(buffer, sizeof(buffer), stdin)) {
			buffer[strcspn(buffer, "\n")] = 0;
			continuar = procesar_comando(hash, tp, buffer);
		} else {
			continuar = false;
		}
	}
}
int main(int argc, char *argv[])
{
	if (argc < 2) {
		fprintf(stderr, "Uso: %s <archivo_pokemones> [comando]\n",
			argv[0]);
		return 1;
	}

	tp1_t *tp = tp1_leer_archivo(argv[1]);
	if (!tp) {
		fprintf(stderr, "Error al leer el archivo de pokemones.\n");
		return 1;
	}

	hash_t *hash = hash_crear(tp1_cantidad(tp));
	if (!hash) {
		fprintf(stderr, "Error al crear el hash.\n");
		tp1_destruir(tp);
		return 1;
	}

	if (!cargar_pokemones_en_hash(hash, tp)) {
		hash_destruir(hash);
		tp1_destruir(tp);
		return 1;
	}

	if (argc > 2) {
		char comando[128] = { 0 };
		for (int i = 2; i < argc; i++) {
			strcat(comando, argv[i]);
			if (i < argc - 1)
				strcat(comando, " ");
		}
		procesar_comando(hash, tp, comando);
	} else {
		ejecutar_interfaz_interactiva(hash, tp);
	}

	hash_destruir(hash);
	tp1_destruir(tp);
	return 0;
}