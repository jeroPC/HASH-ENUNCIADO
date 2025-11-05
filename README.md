<div align="right">
<img width="32px" src="img/algo2.svg">
</div>

# TDA HASH

## Repositorio de (Nombre Apellido) - (Padrón) - (Mail)

- Para compilar:

```bash
línea de compilación
```

- Para ejecutar:

```bash
línea de ejecución
```

- Para ejecutar con valgrind:
```bash
línea con valgrind
```
---
##  Funcionamiento

Explicación de cómo funcionan las estructuras desarrolladas en el TP y el funcionamiento general del mismo.

Aclarar en esta parte todas las decisiones que se tomaron al realizar el TP, cosas que no se aclaren en el enunciado, fragmentos de código que necesiten explicación extra, etc.

Incluir **EN TODOS LOS TPS** los diagramas relevantes al problema (mayormente diagramas de memoria para explicar las estructuras, pero se pueden utilizar otros diagramas si es necesario).

### Por ejemplo:

El programa funciona abriendo el archivo pasado como parámetro y leyendolo línea por línea. Por cada línea crea un registro e intenta agregarlo al vector. La función de lectura intenta leer todo el archivo o hasta encontrar el primer error. Devuelve un vector con todos los registros creados.

<div align="center">
<img width="70%" src="img/diagrama1.svg">
</div>

En el archivo `sarasa.c` la función `funcion1` utiliza `realloc` para agrandar la zona de memoria utilizada para conquistar el mundo. El resultado de `realloc` lo guardo en una variable auxiliar para no perder el puntero original en caso de error:

```c
int *vector = realloc(vector_original, (n+1)*sizeof(int));

if(vector == NULL)
    return -1;
vector_original = vector;
```


<div align="center">
<img width="70%" src="img/diagrama2.svg">
</div>

---

## Respuestas a las preguntas teóricas
Incluír acá las respuestas a las preguntas del enunciado (si aplica).


La función de hashing que estás usando es la djb2 (también conocida como Dan Bernstein's hash), una de las funciones hash más populares y eficientes para strings.

Origen:
Fue creada por Daniel J. Bernstein (djb) y publicada en un newsgroup de Unix (comp.lang.c) alrededor de 1991. Es famosa por su simplicidad y excelente distribución

Explicación:
hash = 5381: Número inicial (número primo, da mejor distribución)
hash << 5: Desplaza 5 bits a la izquierda (equivale a multiplicar por 32)
(hash << 5) + hash: Es lo mismo que hash * 33 (32 + 1)
+ (size_t)c: Suma el valor ASCII del carácter
hash % capacidad: Reduce el valor al rango de la tabla
¿Por qué 33?
El número 33 fue elegido empíricamente por Bernstein. Produce pocas colisiones y tiene buena distribución estadística para strings comunes (nombres, palabras, URLs, etc.).



⚠️ Lo que NO pudimos implementar con hash:
Búsqueda por ID - Sin hash

void buscar_por_id(tp1_t *tp, int id)
{
    struct pokemon *p = tp1_buscar_id(tp, id);  // O(n) - búsqueda lineal
    // ...
}

¿Por qué NO usamos hash para búsqueda por ID?
Razón 1: La clave del hash es el NOMBRE, no el ID

// Insertamos con NOMBRE como clave:
hash_insertar(hash, p->nombre, p, NULL);
       //           ^^^^^^^^^ clave = nombre

       
Análisis de la implementación del main.c según el enunciado
 Lo que SÍ usamos del enunciado:
1. Línea de comandos con nombre de archivo
 Implementado correctamente: Recibe el nombre del archivo por línea de comandos.

2. Primitivas de TP1
 Usamos las primitivas del TP1 para cargar y manipular los pokemones.

3. Tabla de Hash con primitivas
 Usamos la tabla de hash implementada con todas sus primitivas.

4. Búsqueda por nombre usando hash
 Implementado con hash: Complejidad O(1) promedio.

 Lo que NO pudimos implementar con hash:
Búsqueda por ID - Sin hash
¿Por qué NO usamos hash para búsqueda por ID?
Razón 1: La clave del hash es el NOMBRE, no el ID

El hash está estructurado con nombre como clave. No podemos buscar por ID porque el ID no es la clave.