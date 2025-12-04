# Simulación de Partículas - Física Computacional

Simulación de partículas en 3D con colisiones elásticas, implementada en C++ con visualización en Python.

## Características
- Simulación de N partículas en un cubo 3D
- Colisiones elásticas entre partículas
- Rebotes con las paredes del cubo
- Visualización en tiempo real con VisPy
- Histogramas de distribución de partículas
- Paralelización con OpenMP
- Comunicación vía ZeroMQ

## Dependencias

# ZeroMQ
- libzmq5

# Python
- python3
- vispy
- pyzmq
- numpy
- matplotlib

## Compilación
-bash
g++ -O3 -march=native -ffast-math -funroll-loops -flto -fno-trapping-math -fno-math-errno -std=c++17 -fopenmp main.cpp objeto.cpp metodos.cpp -lzmq -o simulador.o

# Dependecias adicionales para compilación
- g++
- libzmq3-dev
- libomp-dev

## Ejecutar
- ./simulador.o 
- Cambiar parametros ./simulador.o -h

# En otra terminal
- Ejecutar "python3 visual.py" para obtener la visualizaión.
- Ejecutar "python3 histograma.py" para obtener el histograma.
