#!/bin/bash

# Script de prueba para el main

echo "=== Prueba 1: Buscar pokemon existente por nombre ==="
echo "nombre Pikachu" | ./main pokemones_test.txt

echo ""
echo "=== Prueba 2: Buscar pokemon existente por ID ==="
echo "id 25" | ./main pokemones_test.txt

echo ""
echo "=== Prueba 3: Buscar pokemon inexistente por nombre ==="
echo "nombre Mewtwo" | ./main pokemones_test.txt

echo ""
echo "=== Prueba 4: Buscar pokemon inexistente por ID ==="
echo "id 999" | ./main pokemones_test.txt

echo ""
echo "=== Prueba 5: Comando inválido ==="
echo "comando_invalido" | ./main pokemones_test.txt

echo ""
echo "=== Prueba 6: Múltiples búsquedas ==="
echo -e "nombre Charizard\nid 7\nnombre Blastoise\nsalir" | ./main pokemones_test.txt

echo ""
echo "=== Prueba 7: Sin argumentos (debe fallar) ==="
./main 2>&1 | head -1

echo ""
echo "=== Prueba 8: Archivo inexistente ==="
./main archivo_no_existe.txt 2>&1 | head -1

echo ""
echo "=== Todas las pruebas completadas ==="
