#ifndef METODOS_HPP
#define METODOS_HPP

#include "objeto.hpp"
#include "parametros.hpp"
#include <vector>

/// @brief Genera N partículas aleatorias
/// @param particulas Vector de partículas
/// @param params Parámetros
void generar_particulas(std::vector<Particula>& particulas, const ParametrosSimulacion& params);

/// @brief Actualiza todas las partículas un paso
/// @param particulas Vector de partículas
void actualizar_simulacion(std::vector<Particula>& particulas);

/// @brief Convierte datos a buffer para ZMQ
/// @param particulas Vector de partículas
/// @param buffer Buffer de salida
void empaquetar_datos(const std::vector<Particula>& particulas, std::vector<double>& buffer);

#endif
